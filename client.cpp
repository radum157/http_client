#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <sstream>
#include <unordered_map>

#include "utils.h"
#include "client.hpp"
#include "string_ext.hpp"
#include "httpParser.hpp"

#define CRLF "\r\n"

/* File path prefix used by server */
#define PATH_PREFIX "/api/v1/tema/"

#define COOKIE_KEY "Set-Cookie"
#define HEADER_COMMON " HTTP/1.1" CRLF								\
						"Host: " SERVER_HOST CRLF					\
						"Content-type: application/json" CRLF		\
						"Connection: keep-alive" CRLF

using std::string;

class Client {
private:
	string cookie;
	string token;
	string id;

	/* Used for communicating with the server; needs to be connected! */
	const int sockfd;

	/* Used in HTTP request creation */
	string createBody(const string &type);
	string createRoute(const string &type);

	/**
	 * Prints a message for successful operations
	*/
	void generateOutput(const string &type, const string &body);

	/**
	 * Check if server returned error
	*/
	inline bool is_error(const string &header)
	{
		size_t start = header.find(' ') + 1;
		/* Error codes (4xx or 5xx) */
		return header[start] == '4' || header[start] == '5';
	}

	/**
	 * @return A string representing the cookie, if any
	*/
	std::unordered_map<string, string>
		sendToServer(int sockfd, const string &body, const string &route);

	/**
	 * Sets the token field, adding the value from the JSON located in @a json
	*/
	void extractToken(const string &json);

public:
	Client(int fd) : sockfd(fd) {};

	/**
	 * Handles all communication with the server
	*/
	void handleCommand(const string &cmd);

	/**
	 * Clears all client data (except for @a sockfd)
	*/
	inline void clear()
	{
		cookie.clear();
		token.clear();
		id.clear();
	}
};

void Client::extractToken(const string &json)
{
	/* Simplistic JSON parsing */
	size_t start = json.find("\"token\":\"");
	if (start == string::npos) {
		return;
	}

	start += sizeof("\"token\":\"") - 1;
	size_t end = json.find('"', start);

	if (end < start + 1) {
		return;
	}

	token = json.substr(start, end - start);
}

string Client::createBody(const string &type)
{
	if (type == "add_book") {
		Book book;
		std::cin >> book;

		/* Error checking */
		if (book.validateInput()) {
			return book.serialise();
		}
		return "";
	}

	if (type == "login" || type == "register") {
		User user;
		std::cin >> user;

		if (!user.validateInput()) {
			return "";
		}

		std::ostringstream json_stream;
		json_stream << "{\"username\":\"" << user.getUsername()
			<< "\",\"password\":\"" << user.getPassword() << "\"}";
		return json_stream.str();
	}

	return "";
}

string Client::createRoute(const string &type)
{
	if (type == "add_book") {
		return "POST " PATH_PREFIX "library/books";
	}
	if (type == "delete_book") {
		return "DELETE " PATH_PREFIX "library/books";
	}
	if (type == "get_books" || type == "get_book") {
		return "GET " PATH_PREFIX "library/books";
	}
	if (type == "login" || type == "register") {
		return "POST " PATH_PREFIX "auth/" + type;
	}
	if (type == "logout") {
		return "GET " PATH_PREFIX "auth/logout";
	}
	if (type == "enter_library") {
		return "GET " PATH_PREFIX "library/access";
	}
	return "";
}

void Client::generateOutput(const string &type, const string &body)
{
	std::cout << "SUCCESS: ";

	if (type == "add_book") {
		std::cout << "Book added successfully";
	} else if (type == "login") {
		std::cout << "User logged in successfully";
	} else if (type == "delete_book") {
		std::cout << "Book with id " << id << " deleted successfully!";
		id.clear();
	} else if (type == "enter_library") {
		std::cout << "User has access to the library";
	} else if (type == "logout") {
		std::cout << "User disconnected successfully!";
	} else if (type == "register") {
		std::cout << "User registered successfully!";
	} else {
		std::cout << body;
	}

	std::cout << '\n';
}

std::unordered_map<string, string>
Client::sendToServer(int sockfd, const string &body, const string &route)
{
	/* Construct request */
	string request = route + HEADER_COMMON;

	if (!cookie.empty()) {
		request += "Cookie: " + cookie + CRLF;
	}
	if (!token.empty()) {
		request += "Authorization: Bearer " + token + CRLF;
	}

	if (!body.empty()) {
		request += "Content-Length: " + std::to_string(body.length())
			+ CRLF CRLF + body;
	} else {
		request += CRLF;
	}

	/* Send request */
	size_t sent = transfer_all(sockfd, request.data(),
		request.length(), (void *)write);

	if (sent == 0) {
		std::cout << "ERROR: connection closed\n";
		return std::unordered_map<string, string>();
	}

	/* Parse response and return */
	return HttpParser::parse(sockfd);
}

void Client::handleCommand(const string &cmd)
{
	/* Send request */
	string route = createRoute(cmd);

	if (cmd == "get_book" || cmd == "delete_book") {
		std::cout << "id=";
		std::getline(std::cin, id);
		std::trim(id);

		/* Error checking */
		try {
			(void)std::stoi(id);
		} catch (const std::invalid_argument &e) {
			std::cout << "ERROR: Invalid ID\n";
			return;
		}

		route += "/" + id;
	}

	string body = createBody(cmd);
	if (body.empty()) {
		if (cmd == "add_book") {
			std::cout << "ERROR: Invalid book field(s)\n";
			return;
		} else if (cmd == "login" || cmd == "register") {
			std::cout << "ERROR: Invalid username or password\n";
			return;
		}
	}

	/* Handle response */
	auto response = sendToServer(sockfd, body, route);

	/* Clear data when switching user */
	if (cmd == "login" || cmd == "register" || cmd == "logout") {
		clear();
	}

	if (response.find(COOKIE_KEY) != response.end()) {
		/* Only store the actual sid */
		size_t pos = response[COOKIE_KEY].find(';');
		cookie = response[COOKIE_KEY].substr(0, pos);
	}

	/* Error checking */
	if (is_error(response[HEADER_KEY])) {
		std::cout << "ERROR: " << response[HEADER_KEY] << ' '
			<< response[BODY_KEY] << '\n';
		return;
	}

	if (cmd == "enter_library") {
		extractToken(response[BODY_KEY]);
	}
	generateOutput(cmd, response[BODY_KEY]);
}

int main()
{
	/* Create socket and connect */
	int sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	DIE(sockfd < 0, "Socket creation");

	await_connect(sockfd);

	/* Parse commands and execute them */
	Client client(sockfd);
	string cmd;

	while (std::getline(std::cin, cmd)) {
		std::trim(cmd);
		if (cmd == "exit") {
			break;
		}

		client.handleCommand(cmd);
	}

	close(sockfd);
	return 0;
}
