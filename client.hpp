#ifndef __CLIENT_HPP
#define __CLIENT_HPP 1

#include <iostream>

#include "string_ext.hpp"

class User {
	std::string username;
	std::string password;

public:
	/**
	 * @return If the user data is valid (or empty)
	*/
	inline bool validateInput()
	{
		return !username.empty() && !password.empty() &&
			!std::haswhitespace(username) && !std::haswhitespace(password);
	}

	/**
	 * Prints a prompt to @a stdout and reads all fields from @a in
	*/
	friend std::istream &operator>>(std::istream &in, User &user)
	{
		std::cout << "username=";
		std::getline(in, user.username);
		std::trim(user.username);

		std::cout << "password=";
		std::getline(in, user.password);
		std::trim(user.password);

		return in;
	}

	inline std::string getUsername() const
	{
		return username;
	}

	inline std::string getPassword() const
	{
		return password;
	}

};

class Book {
private:
	std::string title;
	std::string author;
	std::string genre;
	std::string publisher;
	int pageCount;

	/**
	 * @return If @a author is a valid name
	*/
	bool validateAuthor()
	{
		for (char c : author) {
			if (!std::isalpha(c) && !std::isspace(c)
				&& c != '.' && c != '-') {
				return false;
			}
		}
		return !author.empty();
	}

public:
	/**
	 * @return If the book data is valid (or empty)
	*/
	inline bool validateInput()
	{
		return pageCount > 0 && validateAuthor() && !title.empty()
			&& !genre.empty() && !publisher.empty();
	}

	/**
	 * Prints a prompt to @a stdout and reads all fields from @a in
	*/
	friend std::istream &operator>>(std::istream &in, Book &book)
	{
		std::cout << "title=";
		std::getline(in, book.title);
		std::trim(book.title);

		std::cout << "author=";
		std::getline(in, book.author);
		std::trim(book.author);

		std::cout << "genre=";
		std::getline(in, book.genre);
		std::trim(book.genre);

		std::cout << "publisher=";
		std::getline(in, book.publisher);
		std::trim(book.publisher);

		std::cout << "page_count=";

		/* Check for invalid inputs */
		std::string tmp;
		std::getline(in, tmp);
		std::trim(tmp);

		try {
			book.pageCount = std::stoi(tmp);
		} catch (const std::invalid_argument &e) {
			book.pageCount = INT32_MIN;
		}

		return in;
	}

	/**
	 * @return
	 * The book object in @a JSON format
	*/
	inline std::string serialise()
	{
		return "{\"title\":\"" + title + "\","
			+ "\"author\":\"" + author + "\","
			+ "\"genre\":\"" + genre + "\","
			+ "\"page_count\":" + std::to_string(pageCount) + ","
			+ "\"publisher\":\"" + publisher + "\"}";
	}

};

#endif
