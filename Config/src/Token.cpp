#include <Config/Token.hpp>
namespace Config
{
Token::Token() : tag_(WORD) {}
Token::Token(std::string value,
			 int line) : tag_(tag(value)),
						 value_(value),
						 line_idx_(line) {}
Token::Token(const std::string &value) : tag_(tag(value)),
										 value_(value),
										 line_idx_(0) {}
Token::Token(e_tag tag, int line) : tag_(tag), line_idx_(line) {}

bool Token::isDirective() const
{
	return tag_ >= directivesStart;
}
bool Token::isServerDirective() const
{
	return tag_ > directivesStart && tag_ <= serverDirectivesEnd;
}
bool Token::isLocationDirective() const
{
	return tag_ > serverDirectivesEnd;
}

e_tag Token::tag(std::string value)
{
	size_t i;
	for (i = 1; i < directivesSize; ++i)
		if (value == validDirectives[i])
			return e_tag(i);
	return WORD;
}
}  // namespace Config
