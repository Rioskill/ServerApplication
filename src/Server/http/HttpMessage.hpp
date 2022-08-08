#ifndef HTTPMESSAGE_H
#define HTTPMESSAGE_H

#include <map>
#include <set>
#include <string>

class HttpMessage {
protected:
    std::map<std::string, std::set<std::string>> headers;

    std::string renderHeaders() const {
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

        return headers_str;
    }

public:
    const std::map<std::string, std::set<std::string>> &getHeaders() {
        return headers;
    }

    void addHeader (const std::string &key, const std::string &value) {
        headers[key].insert(value);
    }
};

#endif