#include <Config/Location.hpp>
#include <Utils/Encoding.hpp>
#include <Utils/String.hpp>
#include <iostream>
namespace Config
{
Location::Location() : matchBy_(Prefix),
					   uri_("/"),
					   autoindex_(false),
					   client_max_body_size_(-1)
{
	limit_except_["GET"] = true;
	limit_except_["POST"] = true;
	limit_except_["HEAD"] = true;
	limit_except_["DELETE"] = true;
	limit_except_["OPTIONS"] = false;
	limit_except_["PATCH"] = false;
	limit_except_["TRACE"] = false;
	limit_except_["CONNECT"] = false;
	limit_except_["PUT"] = true;
}

Location::Location(Location const &x) : matchBy_(x.matchBy_),
										uri_(x.uri_),
										autoindex_(x.autoindex_),
										root_(x.root_),
										index_(x.index_),
										limit_except_(x.limit_except_),
										upload_(x.upload_),
										cgi_(x.cgi_),
										auth_basic_(x.auth_basic_),
										client_max_body_size_(x.client_max_body_size_),
										directives_(x.directives_)
{
}

Location &Location::operator=(Location const &x)
{
	matchBy_ = x.matchBy_;
	uri_ = x.uri_;
	autoindex_ = x.autoindex_;
	client_max_body_size_ = x.client_max_body_size_;
	root_ = x.root_;
	index_ = x.index_;
	limit_except_ = x.limit_except_;
	upload_ = x.upload_;
	cgi_ = x.cgi_;
	auth_basic_ = x.auth_basic_;
	directives_ = x.directives_;
	return *this;
}

Location::~Location() {}

string const &Location::uri() const { return uri_; }
bool Location::autoindex() const { return autoindex_; }
string const &Location::root() const { return root_; }
vector<string> const &Location::index() const { return index_; }
string const &Location::upload() const { return upload_; }
string const &Location::auth_basic() const { return auth_basic_; }

string Location::directive(const string &name) const
{
	if (directives_.count(name))
		return (directives_.find(name))->second;
	return "";
}

bool Location::exactMatch(string const &target) const
{
	return (matchBy_ == Exact && uri_ == target) ||
		   (matchBy_ == Prefix_Prefered && !target.compare(0, uri_.size(), uri_));
}
#ifdef BONUS
bool Location::regexMatch(string const &target) const
{
	int flags = 0;
	flags |= REG_EXTENDED;
	if (matchBy_ == Regex_CaseInsensitive)
		flags |= REG_ICASE;
	else if (matchBy_ != Regex)
		return false;
	regex_t regex;
	regcomp(&regex, uri_.c_str(), flags);
	int ret = regexec(&regex, target.c_str(), 0, NULL, 0);
	regfree(&regex);
	return ret == 0;
}
#endif
size_t Location::matches(string const &target) const
{
	size_t n(uri_.size());
	if (matchBy_ == Prefix && !target.compare(0, n, uri_))
		return n;
	return 0;
}
bool Location::hasCGI(string const &path) const
{
	size_t dot = path.find(".");
	if (dot == std::string::npos)
		return cgi_.count(path);
	for (size_t next = path.find(".", dot + 1);
		 dot != std::string::npos;
		 next = path.find(".", dot + 1))
	{
		std::string ext = path.substr(dot + 1, next - (dot + 1));
		if (cgi_.count(ext))
			return true;
		dot = next;
	}
	return false;
}

vector<string> Location::getCGI() const
{
	vector<string> cgi;

	for (map<string, string>::const_iterator it = cgi_.begin(); it != cgi_.end(); ++it)
		cgi.push_back(it->first);

	return cgi;
}

string const &Location::CGIpath(string const &ext) const
{
	return (cgi_.find(ext))->second;
}

bool Location::allow(string const &method) const
{
	return (limit_except_.find(method))->second;
}

vector<string> Location::allowedMethods() const
{
	vector<string> ret;
	for (map<string, bool>::const_iterator it = limit_except_.begin(); it != limit_except_.end(); ++it)
		if (it->second)
			ret.push_back(it->first);
	return ret;
}

int Location::maxBodySize() const
{
	return client_max_body_size_;
}

bool Location::authenticate(string const &clientInput) const
{
	string::size_type realm;

	if (auth_basic_.empty())
		return true;
	if ((realm = clientInput.find("Basic")) != string::npos)
	{
		if ((realm = clientInput.find_first_not_of("\t ", realm + 6)) != string::npos)
		{
			string givenAuthentification = clientInput.substr(realm, clientInput.find_first_of("\t ", realm) - realm);
			string decodedAuthentification = Encoding::Base64_Decode(givenAuthentification);
			return decodedAuthentification == auth_basic_;
		}
	}
	return false;
}

bool Location::dir(Parser<Token>::iterator &it)
{
	switch (it->tag_)
	{
	case INDEX:
		set_index(++it);
		break;
	case AUTOINDEX:
		set_autoindex(++it);
		break;
	case ROOT:
		set_root(++it);
		break;
	case LIMIT_EXCEPT:
		set_limit_except(++it);
		break;
	case CLIENT_MAX_BODY_SIZE:
		set_client_max_body_size(++it);
		break;
	case UPLOAD:
		set_upload(++it);
		break;
	case CGI:
		set_cgi(++it);
		break;
	case AUTH_BASIC:
		set_auth_basic(++it);
		break;
	default:
		set_directive(it);
		break;
	}
	return true;
}

void Location::location(Parser<Token>::iterator &it)
{
#ifdef BONUS
	if (it->value_ == "~" || it->value_ == "~*")
	{
		matchBy_ = it->value_ == "~" ? Regex : Regex_CaseInsensitive;
		++it;
		regex_t expression;
		if (regcomp(&expression, it->value_.c_str(), REG_EXTENDED))
			throw runtime_error("invalid regex '" + it->value_ + "' in location block on line " + Utils::String::to_string(it->line_idx_));
		regfree(&expression);
	}
	else if (it->value_ == "^~")
	{
		matchBy_ = Prefix_Prefered;
		++it;
	}
#endif
	if (it->value_ == "=")
	{
		matchBy_ = Exact;
		++it;
	}
#ifdef BONUS
	if (matchBy_ != Regex && matchBy_ != Regex_CaseInsensitive && (it->value_)[0] != '/')
		throw runtime_error("invalid uri '" + it->value_ + "' in location block on line " + Utils::String::to_string(it->line_idx_));
#else
	if (it->value_[0] != '/')
		throw runtime_error("bad uri in location definition on line " + Utils::String::to_string(it->line_idx_));
#endif
	uri_ = it++->value_;
	if (it->tag_ != O_BRACKET)
		throw runtime_error("location definition on line " + Utils::String::to_string(it->line_idx_));
	while ((++it)->tag_ != C_BRACKET)
	{
		if (it->isLocationDirective())
			dir(it);
		else
			throw runtime_error("invalid directive '" + it->value_ + "' in location block on line " + Utils::String::to_string(it->line_idx_));
	}
}

void Location::set_autoindex(Parser<Token>::iterator &it)
{
	if (it->tag_ != SEMI)
		autoindex_ = (it++->value_ == "on");
	if (it->tag_ != SEMI)
		throw runtime_error("unexpected number of values in autoindex on line " + Utils::String::to_string(it->line_idx_) +
							"\nusage: autoindex on;");
}

void Location::set_root(Parser<Token>::iterator &it)
{
	if (it->tag_ != SEMI)
		root_ = it++->value_;
	if (it->tag_ != SEMI)
		throw runtime_error("unexpected number of values in root on line " + Utils::String::to_string(it->line_idx_) +
							"\nusage: root /www-root/data;");
}

void Location::set_index(Parser<Token>::iterator &it)
{
	index_.clear();
	while (it->tag_ != SEMI)
		index_.push_back(it++->value_);
}

void Location::set_limit_except(Parser<Token>::iterator &it)
{
	for (map<string, bool>::iterator setFalse = limit_except_.begin();
		 setFalse != limit_except_.end(); ++setFalse)
		setFalse->second = false;
	while (it->tag_ != SEMI)
	{
		if (it->value_ == "TRACE" || it->value_ == "CONNECT" ||
			it->value_ == "OPTIONS" || it->value_ == "PATCH")
			continue;
		if (!limit_except_.count(it->value_))
			throw runtime_error("invalid HTTP method '" + it->value_ + "' in limit_except on line " + Utils::String::to_string(it->line_idx_));

		limit_except_[it++->value_] = true;
	}
}

void Location::set_upload(Parser<Token>::iterator &it)
{
	upload_ = (it->value_[0] == '/' ? "" : "/") + it++->value_;
	if (it->tag_ != SEMI)
		throw runtime_error("unexpected number of values in upload on line " + Utils::String::to_string(it->line_idx_) +
							"\nusage: upload /data/uploads;");
}

void Location::set_cgi(Parser<Token>::iterator &it)
{
	std::string name(it++->value_),
		val = (it->value_[0] == '/' ? "" : "/") + it->value_;
	cgi_[name] = val;
	if ((++it)->tag_ != SEMI)
		throw runtime_error("unexpected number of values in cgi on line " + Utils::String::to_string(it->line_idx_) +
							"\nusage: cgi php /cgi-bin/cgi-php;");
}

void Location::set_auth_basic(Parser<Token>::iterator &it)
{
	auth_basic_ = it++->value_;
	if (it->tag_ != SEMI)
		throw runtime_error("unexpected number of values in upload on line " + Utils::String::to_string(it->line_idx_) +
							"\nusage: auth_basic \"You need to login\"");
}

void Location::set_directive(Parser<Token>::iterator &it)
{
	string name = it++->value_;
	directives_.insert(make_pair(name, it++->value_));
	if (it->tag_ != SEMI)
		throw runtime_error("unexpected number of values in " + name + " on line " + Utils::String::to_string(it->line_idx_));
}

void Location::set_client_max_body_size(Parser<Token>::iterator &it)
{
	if (it->value_.find_first_not_of("0123456789") != string::npos)
		throw runtime_error("unexpected symbol '" + it->value_ + "' in client_max_body_size on line " + Utils::String::to_string(it->line_idx_));
	client_max_body_size_ = Utils::String::to_int((it++)->value_);
}

void Location::print()
{
	cout << "location ";
	// cout << (exactMatch_ ? "=" : "") << " ";
	cout << uri_ << "{\n";
	cout << "  autoindex " << (autoindex_ ? "on" : "off") << "\n";
	cout << "  root " << root_ << "\n";
	cout << "  client_max_body_size " << client_max_body_size_ << "\n";
	cout << "  index ";
	for (vector<string>::iterator it = index_.begin(); it != index_.end(); ++it)
		cout << *it << " ";
	cout << "\n";
	cout << "  limit_except ";
	for (map<string, bool>::iterator it = limit_except_.begin(); it != limit_except_.end(); ++it)
		if (it->second)
			cout << it->first << " ";
	cout << "\n";
	cout << "  upload " << upload_ << "\n}\n";
	cout << "client_max_body_size_ " << client_max_body_size_ << "\n";
	cout << "  cgi ";
	for (map<string, string>::iterator it = cgi_.begin(); it != cgi_.end(); ++it)
		cout << it->first << " => " << it->second << ", ";
	cout << "\n";
}
}  // namespace Config
