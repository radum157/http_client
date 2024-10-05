#ifndef __STRING_EXT_HPP
#define __STRING_EXT_HPP 1

#include <string>

namespace std {

	/**
	 * @return If a string contains whitespaces
	*/
	inline bool haswhitespace(const string &s)
	{
		return s.find_first_of(" \r\t\v\f\n") != string::npos;
	}

	/**
	 * Removes any leading "whitespace" characters
	 * from a @a std::string
	 *
	 * @return The resulted string
	*/
	string ltrim(const string &s);

	/**
	 * Removes any trailing "whitespace" characters
	 * from a @a string
	 *
	 * @return The resulted string
	*/
	string rtrim(const string &s);

	/**
	 * Removes any leading or trailing "whitespace" characters
	 * from a @a string
	 *
	 * @return The resulted string
	*/
	inline string trim(const string &s)
	{
		return rtrim(ltrim(s));
	}

};

#endif
