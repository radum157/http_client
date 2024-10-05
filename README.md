# README

## View as webpage

```sh
sudo pip3 install grip
grip  README.md
# open http://localhost:6419/
```

# Content

1. [Overview](#overview)
2. [Client](#the-client)
3. [HTTP requests](#http)
4. [JSON formatting](#json)

## Overview

This is a client **API** implementation for communicating with a server. The client receives various commands as input, then converts them into **HTTP requests** using the server API, sends them and awaits a response.

## The client

The client app performs basic operations on an online library, such as registering, logging in / out, viewing all books or a specific one, as well as adding / deleting books, if authorised.

It handles all conversions to / from HTTP, as to improve user experience. After a response from the server is received, a message is printed to `stdout`. The message is prefixed with *SUCCESS* or *ERROR*, depending on the **return code**. The prefix is then followed by either a message describing the result, or the body of the HTTP response.

All server communication is done on a socket, using the host *34.246.184.49*, running on port *8080*. Sent requests specify that the connection should be kept alive. In case **EOF** is reached, an error message is printed, after which the client waits for a termination signal. Any further commands at that point would yield the same result.

Every value is read from `stdin` as strings, using `std::getline`, in order to correctly handle erroneous input from the user, preventing unwanted crashes etc. Any failing **system calls** WILL result in the client failing.

## HTTP

Both the client and the server follow a strict format when communicating over HTTP:

> {HTTP header}<br>
> {header}: {value}CRLF<br>
> ...<br>
> CRLF<br>
> {body (optional)}

Because of this, an HTTP parser was briefly implemented to separate each line by the ':' character and extract the content of the HTTP request into an `unordered map`. **Note** that any changes in the server message convention should be reflected in the client.

## JSON

The body of an HTTP response is usually in **JSON** format, as indicated by the `Content-Type` header. However, the client interacts with only two JSON fields: "error" and "token". This is needed for gaining authorisation to perform operations on the library.

Considering the limited use of JSON parsing, similarly to the HTTP parser, `std::string` searches have been used for a simplistic extraction of the value of "token".

The server also expects any requests to contain JSON-formatted bodies. Class serialisation was used to return the required objects as strings.

#### Note that a more scalable but less lightweight approach would be to use a JSON and HTTP parser.

#### Further details can be found in the source files and headers (comments, variable, function and class names etc.).
