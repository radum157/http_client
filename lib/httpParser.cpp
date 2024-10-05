#include <string>
#include <unistd.h>
#include <unordered_map>
#include <arpa/inet.h>

#include "httpParser.hpp"
#include "utils.h"
#include "string_ext.hpp"

#define CRLF "\r\n"

using std::string;

string HttpParser::read_line(int sockfd)
{
	string line;
	char c;

	/* Parse byte-by-byte */
	while (true) {
		ssize_t rc = read(sockfd, &c, 1);
		if (rc < 0) {
			DIE(errno != EINTR, "read");
			continue;
		}

		/* Nothing left to read */
		if (rc == 0) {
			break;
		}

		line += c;

		if (c == '\n') {
			break;
		}
	}

	return line;
}

void await_connect(int fd)
{
	static constexpr size_t connect_attempts = 10000;
	constexpr struct timespec timeout = {
		.tv_sec = 0,
		.tv_nsec = 1000
	};

	static const sockaddr_in saddr = {
		.sin_family = AF_INET,
		.sin_port = htons(SERVER_PORT),
		.sin_addr = { .s_addr = inet_addr(SERVER_HOST) },
		.sin_zero = { 0 }
	};

	for (size_t i = 0; i < connect_attempts; i++) {
		int rc = connect(fd, (sockaddr *)&saddr, sizeof(saddr));
		if (rc == 0) {
			return;
		}

		nanosleep(&timeout, NULL);
	}

	DIE(true, "connect");
}

size_t transfer_all(int fd, char *buf, size_t len, void *func)
{
	/* Should be either read or write */
	ssize_t(*transfer_func)(int, void *, size_t) = decltype(transfer_func)(func);

	ssize_t remain = len;
	ssize_t handled = 0;

	/* Handle remaining bytes */
	while (remain != 0) {
		ssize_t transfered = transfer_func(fd, buf + handled, remain);
		if (transfered < 0) {
			DIE(errno != EINTR, "write");
			continue;
		}

		if (transfered == 0) {
			await_connect(fd);
			continue;
		}

		remain -= transfered;
		handled += transfered;
	}

	return handled;
}

std::unordered_map<string, string>
HttpParser::parse(int sockfd)
{
	std::unordered_map<string, string> res;
	res[HEADER_KEY] = std::trim(read_line(sockfd));

	/* Parse headers line by line */
	while (true) {
		string line = read_line(sockfd);
		if (line == CRLF) {
			break;
		}

		/* Split line */
		size_t colon = line.find(':');
		string key = line.substr(0, colon);

		/* Ignore whitespace and CRLF */
		string value = line.substr(colon + 2);
		value.resize(value.size() + 1 - sizeof(CRLF));

		res.insert({ key, std::trim(value) });
	}

	/* If body exists, add it */
	if (res.find("Content-Length") != res.end()) {
		size_t len = std::stoul(res["Content-Length"]);

		char buf[len + 1];
		transfer_all(sockfd, buf, len, (void *)read);
		buf[len] = '\0';

		res[BODY_KEY] = buf;
	}

	return res;
}
