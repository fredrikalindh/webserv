#ifndef __PARSER_H__
#define __PARSER_H__

#include <Utils/String.hpp>
#include <fstream>
#include <stack>
#include <string>
#include <vector>

/**
 * This class is used to parse a config file in a nginx style into a
 * collection of Tokens. 
 */
template <class Token>
class Parser
{
public:
	typedef typename std::vector<Token>::iterator iterator;

	Parser(std::string &path)
	{
		std::ifstream conf_file(path.c_str());

		if (!conf_file.good())
			throw std::runtime_error("error accessing config file on path: " + path);
		tokenize(conf_file);
		conf_file.close();
	}
	~Parser() {}
	iterator begin()
	{
		return tokens_.begin();
	}
	iterator end()
	{
		return tokens_.end();
	}

private:
	/**
     * This method is a helper method for the constructor that will go through the 
     * file and add the tokens into the collection. 
     * 
     * @throws
     *      Throws a std::exception on failure.
     */
	void tokenize(std::ifstream &file_)
	{
		std::string line;
		Token tmp;
		std::string::size_type first, last;
		std::stack<bool> brackets;
		int line_idx = 1;

		while (std::getline(file_, line))
		{
			last = 0;
			while ((first = line.find_first_not_of(" \t", last)) != std::string::npos)
			{
				if (line[first] == '#')
					break;
				last = line.find_first_of("; \t", first);
				if (line[first] == ';' && first == last++)
				{
					tokens_.push_back(Token(";", line_idx));
					break;
				}
				tmp = Token(line.substr(first, last - first), line_idx);
				if (tmp.value_ == "{")
					brackets.push(true);
				else if (tmp.value_ == "}")
				{
					if (brackets.empty())
						throw std::runtime_error("extra closing '}' on line " + Utils::String::to_string(line_idx));
					brackets.pop();
				}
				if (tmp.isDirective() && line[line.find_last_not_of(" \t", line.length())] != ';')
					throw std::runtime_error("missing ';' on line " + Utils::String::to_string(line_idx));
				tokens_.push_back(tmp);
			}
			line_idx++;
		}
	}

	std::vector<Token> tokens_;

	Parser();
	Parser(Parser const &x);
	Parser &operator=(Parser const &x);
};

#endif	// __PARSER_H__
