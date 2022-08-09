#ifndef HTTPREQUEST_H
#define HTTPREQUEST_H

#include <iostream>
#include <iomanip>
#include <set>
#include <string>
#include <string_view>
#include <map>

#include "HttpMessage.hpp"

class HttpRequest: public HttpMessage {
public:
    std::string method;
    std::string path;
    std::string protocol;

    std::string body_string;

private:

    std::set<std::string> split_by_comma(std::string_view &line);
    void parse_request_line (const std::string_view &request_line);
    void parse_headers (const std::string_view &headers_str);
    void set_body (const std::string &body_string);

public:
    HttpRequest (std::string header);

};

#endif