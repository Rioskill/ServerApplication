#ifndef HTTPREQUEST
#define HTTPREQUEST

#include <iostream>
#include <iomanip>
#include <set>
#include <string>
#include <string_view>
#include <map>

class HttpRequest {
private:
    std::string header_src;
    // std::map<std::string_view, std::string_view> headers;

public:

    // std::map<std::string_view, std::string_view> headers;
    std::map<std::string_view, std::set<std::string_view>> headers;

    std::string method;
    std::string path;
    std::string protocol;

    std::set<std::string_view> split_by_comma(std::string_view &line) {
        std::set<std::string_view> values;

        size_t value_start = 0;
        size_t comma_pos = line.find(',');

        while (comma_pos != std::string::npos) {
            values.insert(std::string_view(line.begin() + value_start, line.begin() + comma_pos));

            value_start = comma_pos + 1;
            comma_pos = line.find(',',  value_start);
        }

        values.insert(std::string_view(line.begin() + value_start, line.end()));

        return values;
    }

private:

    void parse_request_line (const std::string_view &request_line) {
        size_t whitespace = request_line.find(' ');
        method = std::string(request_line.begin(), whitespace);

        size_t start_path = whitespace;
        whitespace = request_line.find(' ', whitespace + 1);

        path = std::string(request_line.begin() + start_path, request_line.begin() + whitespace);
        protocol = std::string(request_line.begin() + whitespace + 1, request_line.end());
    }

public:
    HttpRequest (std::string header): header_src(header) {
        size_t line_end = header_src.find("\r\n");
        std::string_view request_line(header_src.begin(), header_src.begin() + line_end);

        parse_request_line(request_line);

        size_t line_start = line_end + 2;

        size_t colon = header_src.find(": ", line_start);

        line_end = header_src.find("\r\n", line_start);
        while(line_end != std::string::npos) {

            std::string_view key(header_src.begin() + line_start, header_src.begin() + colon);
            std::string_view value(header_src.begin() + colon + 2, header_src.begin() + line_end);

            auto values = split_by_comma(value);

            headers.emplace(key, values);

            line_start = line_end + 2;
            colon = header_src.find(": ", line_start);
            line_end = header_src.find("\r\n", line_start);
        }

        std::string_view key(header_src.begin() + line_start, header_src.begin() + colon);
        std::string_view value(header_src.begin() + colon + 2, header_src.end());

        auto values = split_by_comma(value);

        headers.emplace(key, values);

    }

};

#endif