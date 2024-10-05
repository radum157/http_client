#include <string>

#include "string_ext.hpp"

std::string std::ltrim(const std::string &s)
{
	size_t start = 0;
	while (start < s.length() && std::isspace(s[start])) {
		start++;
	}

	return s.substr(start);
}

std::string std::rtrim(const std::string &s)
{
	size_t end = s.length();
	while (end > 0 && std::isspace(s[end - 1])) {
		end--;
	}

	return s.substr(0, end);
}
