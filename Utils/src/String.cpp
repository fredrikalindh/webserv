#include <Utils/String.hpp>
#include <algorithm>
#include <limits>
#include <stdexcept>
namespace Utils
{
std::string String::toupper(const std::string &str)
{
	std::string ret(str);
	for (size_t i = 0; i < str.length(); ++i)
		ret[i] = std::toupper(str[i]);
	return ret;
}
std::string String::tolower(const std::string &str)
{
	std::string ret(str);
	for (size_t i = 0; i < str.length(); ++i)
		ret[i] = std::tolower(str[i]);
	return ret;
}
std::vector<std::string> String::split(const std::string &str, const char c)
{
	std::vector<std::string> ret;
	size_t first = str.find_first_not_of(c), last = 0;
	while (first < str.length() && last != std::string::npos)
	{
		last = str.find(c, first + 1);
		std::string tmp = str.substr(first, last - first);
		if (!tmp.empty())
			ret.push_back(tmp);
		first = str.find_first_not_of(c, last + 1);
	}
	return ret;
}
std::string String::to_string(int n)
{
	std::string ret;
	long nbr = n;
	if (n < 0)
		nbr *= -1;
	if (!n)
		return "0";
	while (nbr)
	{
		ret.push_back(nbr % 10 + '0');
		nbr /= 10;
	}
	if (n < 0)
		ret.push_back('-');
	std::reverse(ret.begin(), ret.end());
	return ret;
}
int String::to_int(const std::string &str)
{
	int n = 0, c = (str[0] == '-') ? -1 : 1;
	if (str.substr(0, 12) == "-2147483648" && (str.size() == 12 || !isdigit(str[12])))
		return -2147483648;
	for (size_t i = (str[0] == '-' || str[0] == '+') ? 1 : 0;
		 i < str.length(); ++i)
	{
		if (!isdigit(str[i]))
			break;
		int decimalValue = str[i] - '0';
		if (((std::numeric_limits<int>::max() - decimalValue) / 10 < n))
			throw std::overflow_error("int");
		n = n * 10 + decimalValue;
	}
	return n * c;
}

}  // namespace Utils
