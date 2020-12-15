#include <arpa/inet.h>

#include <Config/MimeTypes.hpp>
#include <Config/Server.hpp>
#include <Utils/String.hpp>
#include <cstring>
#include <limits>

namespace Config
{
std::vector<Server> Server::configs_;
int Server::WORKER_PROCESSES = 0;

void set_workers(Parser<Token>::iterator &it)
{
	if (it->tag_ == SEMI || (it + 1)->tag_ != SEMI)
		throw runtime_error("unexpected value in workers on line " + Utils::String::to_string(it->line_idx_));
	if (!isdigit(it->value_[0]))
		throw runtime_error("bad value in workers on line " + Utils::String::to_string(it->line_idx_));
	int workers = Utils::String::to_int(it++->value_);
	if (workers >= 0)
		Server::WORKER_PROCESSES = (workers) ? workers : 1;
	else
		throw runtime_error("bad value in workers on line " + Utils::String::to_string(it->line_idx_));
}
bool set_plugins(Parser<Token>::iterator &it)
{
	std::string plugin = it++->value_;
	if (it->tag_ == SEMI || (it + 1)->tag_ != SEMI || it++->value_ != "on")
		return false;
	return Server::set_plugin(plugin);
}
u_int16_t Server::PLUGINS = 0;
const std::string SUPPORTED_PLUGINS[] = {
	"utf-8",
	"gzip",
	"deflate",
};
int Server::get_plugin(const std::string &plugin)
{
	if (plugin == "utf-8")
		return UTF8;
	else if (plugin == "gzip")
		return GZIP;
	else if (plugin == "deflate")
		return DEFLATE;
	else if (plugin == "unicode")
		return UNICODE;
	return 0;
}

int Server::set_plugin(const std::string &plugin)
{
	if (plugin == "utf-8")
	{
		PLUGINS |= UTF8;
		return UTF8;
	}
	else if (plugin == "gzip")
	{
		PLUGINS |= GZIP;
		return GZIP;
	}
	else if (plugin == "deflate")
	{
		PLUGINS |= DEFLATE;
		return DEFLATE;
	}
	else if (plugin == "unicode")
	{
		PLUGINS |= UNICODE;
		return UNICODE;
	}

	return 0;
}
void Server::set_plugin(const u_int16_t plugins)
{
	PLUGINS ^= plugins;
}

void Server::load(std::string &path)
{
	Mime::type("");
	static bool first = true;
	if (first)
	{
		Parser<Token> parser(path);
		Server base;

		for (Parser<Token>::iterator it = parser.begin(); it != parser.end(); ++it)
		{
			if (it->tag_ == SERVER)
				add(Server(base))
					.server(++it);
			else if (it->tag_ == WORKERS)
				set_workers(++it);
			else if (it->isServerDirective())
				base.dir(it);
			else if (set_plugins(it))
				;
			else
				throw std::runtime_error("invalid directive '" + it->value_ + "' in main block on line " + Utils::String::to_string(it->line_idx_));
		}
		if (configs_.empty())
			throw std::runtime_error("missing server block");  // servers_.push_back(base_);
		first = false;
	}
}
Server &Server::add(Server toAdd)
{
	configs_.push_back(toAdd);
	return configs_.back();
}

std::vector<uint16_t> Server::uniqueListens()
{
	std::vector<Listen> listens;

	for (std::vector<Server>::const_iterator it = configs_.begin(); it != configs_.end(); ++it)
	{
		uint16_t port = htons(it->listen());
		if (!std::count(listens.begin(), listens.end(), port))
			listens.push_back(port);
	}
	return listens;
}

/**
 * matchServerBlock will match the ip:port mapped to clientSocket to a Config
 * a) one match -> done
 * b) matches > 1 -> it will use the server_names to choose
 * c) no exact server_name match -> match by 1. leading * 2. trailing * -> choose longest match
 * TODO d) (regular expressions indicated by ~)
 * e) choosing default server
 */
Server const &Server::match(Listen toMatch, std::string const &hostField)
{
	size_t ret = 0;
	std::vector<int> matches;
	for (size_t i = 0; i < configs_.size(); ++i)
	{
		if (configs_[i].listen() == toMatch)
		{
			matches.push_back(i);
			ret = i;
		}
	}
	if (matches.size() > 1)
		ret = matchByHost(matches, hostField);
	return configs_[ret];
}

int Server::matchByHost(std::vector<int> matches, std::string hostName)
{
	size_t semi;
	std::pair<int, int> match(0, matches.front());
	int ret = 0;

	if ((semi = hostName.find(':')) != std::string::npos)  // if Port in hostName, ex: localhost:8080
		hostName.erase(semi);
	for (std::vector<int>::iterator it = matches.begin(); it != matches.end(); ++it)
	{
		if (configs_[*it].exactMatch(hostName))
			return *it;
		/**
         * wildcardMatch will return the size of the match,
         * positive for a leading and negative for a trailing.
         * so localhost.com:8080 with server_name localhost.*
         * will return -10.
        */
		ret = configs_[*it].wildcardMatch(hostName);
		if (ret &&
			(ret > match.first ||
			 (match.first <= 0 && ret < match.first)))
			match = std::make_pair(ret, *it);
	}
	return match.second;
}

Server::Server() : listen_(80) {}

Server::Server(Server const &x) : location_(x.location_),
								  listen_(x.listen_),
								  server_name_(x.server_name_),
								  error_page_(x.error_page_),
								  directives_(x.directives_)
{
}

Server &Server::operator=(Server const &x)
{
	location_ = x.location_;
	listen_ = x.listen_;
	server_name_ = x.server_name_;
	error_page_ = x.error_page_;
	directives_ = x.directives_;
	return *this;
}
Server::~Server() {}

bool Server::errorPageExists(int code) const { return error_page_.count(code); }
string const &Server::errorPage(int code) const
{
	// return error_page_[code];
	map<int, string>::const_iterator it = error_page_.find(code);
	if (it != error_page_.end())
		return it->second;
	throw runtime_error("code not registered");
	// return (error_page_.find(code))->second;
}
Listen const &Server::listen() const { return listen_; }
string Server::directive(const string &name) const
{
	if (directives_.count(name))
		return (directives_.find(name))->second;
	return "";
}

bool Server::exactMatch(string const &hostName) const
{
	for (size_t i = 0; i < server_name_.size(); ++i)
		if (server_name_[i] == hostName)
			return true;
	return false;
}

int Server::wildcardMatch(string const &hostName) const
{
	int match = 0;
	string substr;
	for (size_t i = 0; i < server_name_.size(); ++i)
	{
		if (server_name_[i][0] == '*')	// leading
		{
			substr = server_name_[i].substr(1);
			if (match < int(substr.size()) && hostName.find(substr, hostName.size() - substr.size()) != string::npos)
				match = substr.size();
		}
		else if (match <= 0 && !server_name_.empty() && server_name_[i][server_name_[i].size() - 1] == '*')	 //trailing
		{
			substr = server_name_[i].substr(0, server_name_[i].size() - 1);
			if (match < int(substr.size()) && hostName.find(substr) == 0)
				match = -substr.size();
		}
	}
	return match;
}

bool Server::dir(Parser<Token>::iterator &it)
{
	switch (it->tag_)
	{
	case LISTEN:
		set_listen(++it);
		break;
	case SERVER_NAME:
		set_server_name(++it);
		break;
	case ERROR_PAGE:
		set_error_page(++it);
		break;
	default:
		set_directive(it);
	}
	return true;
}

void Server::server(Parser<Token>::iterator &it)
{
	Location baseLocation;

	if (it->tag_ != O_BRACKET)
		throw runtime_error("missing opening bracket in server block on line " + Utils::String::to_string(it->line_idx_));
	while ((++it)->tag_ != C_BRACKET)
	{
		if (it->tag_ == LOCATION)
			location_.add(Location(baseLocation)).location(++it);
		else if (it->isServerDirective())
			dir(it);
		else if (it->isLocationDirective())
			baseLocation.dir(it);
		else
			throw runtime_error("invalid directive '" + it->value_ + "' in server block on line " + Utils::String::to_string(it->line_idx_));
	}
	if (location_.empty())
		location_.add(Location(baseLocation));
}

void Server::set_listen(Parser<Token>::iterator &it)
{
	if (it->value_.find_first_not_of("0123456789") != std::string::npos)
		throw runtime_error("unexpected symbol in listen on line " + Utils::String::to_string(it->line_idx_));
	int val = Utils::String::to_int(it->value_);
	if (val < 0 || val > std::numeric_limits<uint16_t>::max())
		throw runtime_error("invalid port in listen on line " + Utils::String::to_string(it->line_idx_));
	listen_ = static_cast<uint16_t>(val);
	if ((++it)->tag_ != SEMI)
		throw runtime_error("unexpected extra value '" + it->value_ + "' in listen on line " + Utils::String::to_string(it->line_idx_));
}

void Server::set_server_name(Parser<Token>::iterator &it)
{
	while (it->tag_ != SEMI)
		server_name_.push_back((it++)->value_);
}

void Server::set_error_page(Parser<Token>::iterator &it)
{
	if (it->tag_ == SEMI || it[1].tag_ == SEMI)
		throw runtime_error("unexpected number of values in error_page on line " + Utils::String::to_string(it->line_idx_) +
							"\nusage: 400 /data/404.html;");
	Parser<Token>::iterator it2(it);
	while (it[1].tag_ != SEMI)
		++it;
	string path = (it++)->value_;
	while (it2->value_ != path)
	{
		int code;
		if (it2->value_.size() != 3 || it2->value_.find_first_not_of("0123456789") != string::npos || (code = Utils::String::to_int((it2++)->value_)) < 100 || code > 599)
			throw runtime_error("invalid code for error page '" + it2->value_ + "' on line " + Utils::String::to_string(it2->line_idx_));
		error_page_[code] = path;
	}
}

void Server::set_directive(Parser<Token>::iterator &it)
{
	string name = it++->value_;
	directives_.insert(make_pair(name, it++->value_));
	if (it->tag_ != SEMI)
		throw runtime_error("unexpected number of values in " + name + " on line " + Utils::String::to_string(it->line_idx_));
}

void Server::print()
{
	cout << "server {\n";
	cout << "listen " << listen_ << "\n";
	cout << "server_name ";
	for (vector<string>::iterator it = server_name_.begin(); it != server_name_.end(); ++it)
		cout << *it << " ";
	cout << "\n";
	location_.print();
	cout << "error_page ";
	for (map<int, string>::iterator it = error_page_.begin(); it != error_page_.end(); ++it)
		cout << it->first << "=>" << it->second << " ";
	cout << "\n";
	cout << "\n}\n";
}
}  // namespace Config
