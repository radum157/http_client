#ifndef __HTTP_PARSER_HPP
#define __HTTP_PARSER_HPP 1

#include <string>
#include <unordered_map>

/* Convention for adding the body and first line of the header */
#define BODY_KEY "Response-Body"
#define HEADER_KEY "Response-Header"

class HttpParser {
private:
	/**
	 * Reads and returns exactly one line (every character until '\n') from @a sockfd
	*/
	static std::string read_line(int sockfd);

public:
	/**
	 * Parses an HTTP message sent on @a sockfd and returns the content as a @a hashmap
	*/
	static std::unordered_map<std::string, std::string>
		parse(int sockfd);

};

/**
 * Ensures the correct and "complete" transfer of @a buf on @a fd
*/
size_t transfer_all(int fd, char *buf, size_t len, void *func);

/**
 * Attempts to reconnect upon unexpected EOFs
*/
void await_connect(int fd);

#endif
