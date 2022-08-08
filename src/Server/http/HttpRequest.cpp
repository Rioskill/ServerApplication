#include "HttpRequest.hpp"

HttpRequest::HttpRequest (std::string header) {
    size_t line_end = header.find("\r\n");
    std::string_view request_line(header.begin(), header.begin() + line_end);

    parse_request_line(request_line);

    size_t line_start = line_end + 2;

    std::string_view headers_str(header.begin() + line_start, header.end());

    parse_headers(headers_str);
}

std::set<std::string> HttpRequest::split_by_comma(std::string_view &line) {
    std::set<std::string> values;

    size_t value_start = 0;
    size_t comma_pos = line.find(',');

    while (comma_pos != std::string::npos) {
        values.insert(std::string(line, value_start, comma_pos - value_start));

        value_start = comma_pos + 1;
        comma_pos = line.find(',',  value_start);
    }

    values.insert(std::string(line.begin() + value_start, line.end()));

    return values;
}

void HttpRequest::parse_request_line (const std::string_view &request_line) {
    size_t whitespace = request_line.find(' ');
    method = std::string(request_line.begin(), whitespace);

    size_t start_path = whitespace;
    whitespace = request_line.find(' ', whitespace + 1);

    path = std::string(request_line.begin() + start_path, request_line.begin() + whitespace);
    protocol = std::string(request_line.begin() + whitespace + 1, request_line.end());
}

void HttpRequest::parse_headers (const std::string_view &headers_str) {
    size_t colon_pos = headers_str.find(": ");

    size_t line_start = 0;
    size_t line_end = headers_str.find("\r\n");
    while(line_end != std::string::npos) {
        std::string key(headers_str.begin() + line_start, headers_str.begin() + colon_pos);
        std::string_view value(headers_str.begin() + colon_pos + 2, headers_str.begin() + line_end);

        auto values = split_by_comma(value);

        headers.emplace(key, values);

        line_start = line_end + 2;
        colon_pos = headers_str.find(": ", line_start);
        line_end = headers_str.find("\r\n", line_start);
    }

    std::string_view key(headers_str.begin() + line_start, headers_str.begin() + colon_pos);
    std::string_view value(headers_str.begin() + colon_pos + 2, headers_str.end());

    auto values = split_by_comma(value);

    headers.emplace(key, values);
}