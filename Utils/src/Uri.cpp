#include <Utils/String.hpp>
#include <Utils/Uri.hpp>

Uri::Uri() : absolute(false) {}
Uri::Uri(const std::string &rawPath)
{
	absolute = (rawPath[0] == '/');
	path_ = Utils::String::split(rawPath, '/');
}
Uri::Uri(const Uri &x) : path_(x.path_), absolute(x.absolute) {}
std::vector<std::string> &Uri::path()
{
	return path_;
}
std::string Uri::str() const
{
	std::string str;
	if (absolute && path_.size() == 0)
		return "/";
	for (size_t i = 0; i < path_.size(); ++i)
	{
		if (!str.empty() || absolute)
			str += '/';
		str += path_[i];
	}
	return str;
}
Uri &Uri::operator+=(const Uri &x)
{
	path_.insert(path_.end(), x.path_.begin(), x.path_.end());
	return *this;
}
Uri Uri::operator+(const Uri &x) const
{
	Uri ret(*this);
	return ret += x;
}
int Uri::findExt(const std::string &ext)
{
	for (size_t i = 0; i < path_.size(); ++i)
	{
		std::vector<std::string> exts = getExtensions(path_[i]);
		if (!exts.empty() && std::count(exts.begin(), exts.end(), ext))
			return i;
	}
	return -1;
}
std::pair<int, std::string> Uri::findExt(const std::vector<std::string> &exts)
{
	for (size_t i = 0; i < path_.size(); ++i)
	{
		std::vector<std::string> ext = getExtensions(path_[i]);
		for (size_t j = 0; ext.size() && j < exts.size(); ++j)
		{
			if (std::count(ext.begin(), ext.end(), exts[j]))
				return std::make_pair(i, exts[j]);
		}
	}
	return std::make_pair(-1, "");
}
std::string Uri::decode(const std::string &url)
{
	const std::string hexa = "0123456789ABCDEF";
	std::string decoded;
	decoded.reserve(url.size());
	for (size_t i = 0; i < url.size(); ++i)
	{
		if (url[i] == '%')
		{
			char a = toupper(url[++i]), b = toupper(url[++i]);
			uint8_t val = hexa.find(a) * 16 + hexa.find(b);	 // todo error checking
			decoded.push_back(val);
		}
		else if (url[i] == '+')
			decoded.push_back(' ');
		else
			decoded.push_back(url[i]);
	}
	return decoded;
}
std::vector<std::string> Uri::getExtensions(const std::string &subpath)
{
	std::vector<std::string> ext;
	size_t first = subpath.find('.');
	if (first != std::string::npos)
		ext = Utils::String::split(subpath.substr(first + 1), '.');
	return ext;
}
