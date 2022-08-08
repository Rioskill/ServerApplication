#include "HttpResponse.hpp"

std::string HttpResponse::render() const {
    std::string status_line = "HTTP/1.1 " + std::to_string(status_code) + ' ' + reason_phrase + "\r\n";

    std::string headers_str = renderHeaders();

    if (body_string.empty())
        return status_line + headers_str + "\r\n";

    return status_line + headers_str + "\r\n" + body_string;
}

void HttpResponse::setBody (const std::string &body) {
    size_t length = body.size();

    addHeader("Content-Length", std::to_string(length));

    body_string = body;
}