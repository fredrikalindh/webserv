#include <limits.h>

#include <HTTP/Headers.hpp>
// #include <sstream>
namespace
{
static bool validHeaderName(const std::string &name)
{
	for (size_t i = 0; i < name.size(); ++i)
		if (name[i] < 33 || name[i] > 126)
			return false;
	return true;
}

std::string timeToString(const time_t &time)
{
	char *buf = new char[LINE_MAX];
	strftime(buf, LINE_MAX, "%a, %d %b %Y %T GMT",
			 gmtime(&time));
	std::string ret(buf);
	delete[] buf;
	return ret;
}
void makeLowerCase(std::string &s)
{
	for (std::string::iterator it = s.begin(); it != s.end(); ++it)
		*it = std::tolower(*it);
}
}  // namespace

namespace Http
{
Headers::HeaderName::HeaderName() {}
Headers::HeaderName::HeaderName(const std::string &name) : name_(name) {}
Headers::HeaderName::HeaderName(const char *name) : name_(name) {}
Headers::HeaderName &Headers::HeaderName::operator=(const std::string &name)
{
	name_ = name;
	return *this;
}
Headers::HeaderName::~HeaderName() {}

bool Headers::HeaderName::operator==(const HeaderName &rhs) const
{
	if (name_.size() != rhs.name_.size())
		return false;
	for (size_t i = 0; i < name_.size(); ++i)
		if (std::toupper(name_[i]) != std::toupper(rhs.name_[i]))
			return false;
	return true;
}

Headers::HeaderName::operator const std::string &() const
{
	return name_;
}

Headers::Header::Header(const HeaderName &name,
						const HeaderValue &value) : name(name),
													value(value) {}

Headers::Headers() : state_(Incomplete) {}
Headers::Headers(const Headers &x) : headers_(x.headers_), state_(x.state_) {}
Headers &Headers::operator=(const Headers &x)
{
	state_ = x.state_;
	headers_ = x.headers_;
	return *this;
}
Headers::~Headers() {}

bool Headers::IsValid() const
{
	return true;
}

size_t Headers::parseRawMessage(const std::string &rawMessage, size_t offset)
{
	state_ = Incomplete;
	size_t delimiter, initialOffset = offset;
	bool LineFold = false;

	std::string line, name, value;	//= rawMessage.substr(0, delimiter + CRLF.length());
	while (offset < rawMessage.length() && state_ == Incomplete)
	{
		if ((delimiter = rawMessage.find(CRLF, offset)) == std::string::npos || delimiter > 10000)
		{
			if (rawMessage.length() > 10000)
				state_ = Error;
			break;
		}
		line = rawMessage.substr(offset, delimiter - offset);
		offset = delimiter + CRLF.length();
		if (!LineFold)
		{
			if (line.empty())
			{
				state_ = Complete;
				break;
			}
			if ((delimiter = line.find(":")) == std::string::npos || delimiter == 0)
			{
				state_ = Error;
				break;
			}
			name = line.substr(0, delimiter);
			if (name == "Host" && (hasHeader("Host") || line.find('@') != std::string::npos))
			{
				state_ = Error;
				break;
			}
			delimiter = line.find_first_not_of(WSP, delimiter + 1);
			value = (delimiter != std::string::npos) ? line.substr(delimiter) : "";	 // can contain all US-ASCII except CR and LF unless line fold
		}
		else
		{
			value += " " + line.substr(line.find_first_not_of(WSP));
			LineFold = false;
		}
		if (offset < rawMessage.length() &&
			(rawMessage[offset] == ' ' || rawMessage[offset] == '\t'))
		{
			LineFold = true;
			continue;
		}
		if (!addHeader(name, value))
		{
			state_ = Error;
			break;
		}
	}
	return offset - initialOffset;
}

Headers::State Headers::state() const
{
	return state_;
}

Headers::HeaderCollection Headers::getAll() const
{
	return headers_;
}

bool Headers::hasHeader(const HeaderName &name) const
{
	for (HeaderCollection::const_iterator it = headers_.begin(); it != headers_.end(); ++it)
		if (it->name == name)
			return true;
	return false;
}

Headers::HeaderValue Headers::value(const HeaderName &name,
									std::string const &delimiter) const
{
	std::string value;
	for (HeaderCollection::const_iterator it = headers_.begin(); it != headers_.end(); ++it)
	{
		if (it->name == name)
		{
			if (!value.empty())
				value += delimiter;
			value += it->value;
		}
	}
	return value;
}

std::vector<Headers::HeaderValue> Headers::multiValue(const HeaderName &name) const
{
	std::vector<HeaderValue> values;
	for (HeaderCollection::const_iterator it = headers_.begin(); it != headers_.end(); ++it)
	{
		if (it->name == name)
			values.push_back(it->value);
	}
	return values;
}

std::vector<Headers::HeaderValue> Headers::tokens(const HeaderName &name,
												  const char splitBy) const
{
	std::vector<HeaderValue> tokens;
	size_t start, end;
	std::string nextToken;
	for (HeaderCollection::const_iterator it = headers_.begin(); it != headers_.end(); ++it)
	{
		if (it->name == name)
		{
			start = it->value.find_first_not_of(WSP);
			while (start != std::string::npos)
			{
				end = it->value.find_first_of(WSP + splitBy, start);
				nextToken = it->value.substr(start, end - start);
				makeLowerCase(nextToken);
				tokens.push_back(nextToken);
				start = it->value.find_first_not_of(WSP + splitBy, end);
			}
		}
	}
	return tokens;
}

bool Headers::hasToken(const HeaderName &name, const HeaderValue &toFind) const
{
	size_t start, end;
	std::string nextToken, token(toFind);
	makeLowerCase(token);
	for (HeaderCollection::const_iterator it = headers_.begin(); it != headers_.end(); ++it)
	{
		if (it->name == name)
		{
			start = it->value.find_first_not_of(WSP);
			while (start != std::string::npos)
			{
				end = it->value.find_first_of(" \t,", start);
				nextToken = it->value.substr(start, end - start);
				makeLowerCase(nextToken);
				if (nextToken == token)
					return true;
				start = it->value.find_first_not_of(" \t,", end);
			}
		}
	}
	return false;
}

bool Headers::setHeader(const HeaderName &name, const HeaderValue &value)
{
	if (!validHeaderName(name) ||
		value.find('\r') != std::string::npos ||
		value.find('\n') != std::string::npos)
		return false;
	for (HeaderCollection::iterator it = headers_.begin(); it != headers_.end(); ++it)
	{
		if (it->name == name)
		{
			it->value = value;
			return true;  // erase if there are more?
		}
	}
	headers_.push_back(Headers::Header(name, value));
	return true;
}

bool Headers::addHeader(const HeaderName &name,
						const HeaderValue &value)
{
	if (validHeaderName(name) &&
		value.find('\r') == std::string::npos &&
		value.find('\n') == std::string::npos)
	{
		headers_.push_back(Headers::Header(name, value));
		return true;
	}
	return false;
}

bool Headers::addHeader(const HeaderName &name,
						const time_t &time)
{
	if (validHeaderName(name))
	{
		headers_.push_back(Headers::Header(name, timeToString(time)));
		return true;
	}
	return false;
}

bool Headers::addHeader(const HeaderName &name,
						const std::vector<HeaderValue> &values,
						const std::string delimiter)
{
	std::string value;
	for (size_t i = 0; i < values.size(); ++i)
	{
		value += values[i];
		if (i + 1 < values.size())
			value += delimiter;
	}
	if (!validHeaderName(name) ||
		value.find('\r') != std::string::npos ||
		value.find('\n') != std::string::npos)
		return false;
	return addHeader(name, value);
}

void Headers::removeHeader(const HeaderName &name)
{
	HeaderCollection::iterator it = headers_.begin();
	while (it != headers_.end())
	{
		if (it->name == name)
			it = headers_.erase(it);
		else
			++it;
	}
}

void Headers::print() const
{
	HeaderCollection::const_iterator it = headers_.begin();
	while (it != headers_.end())
	{
		std::cout << (std::string)it->name << ":" << (std::string)it->value << std::endl;
		++it;
	}
}

std::string Headers::generateRawHeaders() const
{
	std::string ret;
	for (HeaderCollection::const_iterator it = headers_.begin(); it != headers_.end(); ++it)
		ret += (std::string)it->name + ": " + it->value + "\r\n";
	ret += "\r\n";
	return ret;
	// std::ostringstream ret;
	// for (HeaderCollection::const_iterator it = headers_.begin(); it != headers_.end(); ++it)
	// 	ret << (std::string)it->name << ": " << it->value << "\r\n";
	// ret << "\r\n";
	// return ret.str();
}

bool operator==(const std::string &lhs, const Headers::HeaderName &rhs)
{
	Headers::HeaderName tmp(lhs);
	return tmp == rhs;
}

}  // namespace Http
