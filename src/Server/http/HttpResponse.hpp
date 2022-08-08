#ifndef HTTPRESPONSE_H
#define HTTPRESPONSE_H

#include <map>
#include <set>
#include <string>

#include "HttpMessage.hpp"

class HttpResponse: public HttpMessage {
private:
    int status_code;
    std::string reason_phrase;
    std::string body_string;

public:
    HttpResponse (int status_code, std::string reason_phrase): status_code(status_code), reason_phrase(reason_phrase) {}

    std::string render() const;

    void setBody (const std::string &body);
};

#endif