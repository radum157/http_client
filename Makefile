# Copyright Radu Marin 2024

CC := g++
CCFLAGS := -std=c++17 -Wall -Wextra -I./include -fPIC

LDFLAGS :=

CLIENT := client

CLIENT_SRCS := client.cpp lib/httpParser.cpp lib/string_ext.cpp
CLIENT_OBJS := $(CLIENT_SRCS:.cpp=.o)

CLIENT_HEADERS := client.hpp

build: $(CLIENT)

$(CLIENT): $(CLIENT_OBJS)
	$(CC) $(CLIENT_OBJS) -o $(CLIENT) $(LDFLAGS)

%.o: %.cpp
	$(CC) $(CCFLAGS) -c $< -o $@

clean:
	rm -f $(CLIENT) $(CLIENT_OBJS)

run:
	./client

pack:
	zip -FSr 323CA_Marin_Radu_Tema4PC.zip client.* lib/ include/ Makefile README.md

.PHONY: build clean run pack $(CLIENT)
