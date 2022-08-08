#ifndef HTTPRESPONSE
#define HTTPRESPONSE

#include <map>
#include <set>
#include <string>

class HttpResponse {
private:
    // std::map<std::string_view, std::set<std::string_view>> headers;
    int status_code;
    std::string reason_phrase;
    std::string body_string;

public:

    std::map<std::string, std::set<std::string>> headers;


    HttpResponse (int status_code, std::string reason_phrase): status_code(status_code), reason_phrase(reason_phrase) {}

    void addHeader (const std::string &key, const std::string &value) {
        headers[key].insert(value);
    }

    std::string render() const {
        std::string status_line = "HTTP/1.1 " + std::to_string(status_code) + ' ' + reason_phrase + "\r\n";

        std::string headers_str;

        for (const auto& [key, values]: headers) {
            headers_str += key;
            headers_str += ": ";
            size_t size = values.size();

            auto it = values.begin();

            for (int i = 0; i < size - 1; ++i) {
                headers_str += *it;
                headers_str += ", ";

                it++;
            }

            headers_str += *it;
            headers_str += "\r\n";
        }

        if (body_string.empty())
            return status_line + headers_str + "\r\n";

        return status_line + headers_str + "\r\n" + body_string;
    }

    void setBody (const std::string &body) {
        size_t length = body.size();

        addHeader("Content-Length", std::to_string(length));

        body_string = body;
    }
};

#endif