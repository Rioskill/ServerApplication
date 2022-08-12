#ifndef ROUTEHANDLERS_H
#define ROUTEHANDLERS_H

#include <fstream>
#include <sstream>

#include "HttpResponse.hpp"

inline void setFileBody (HttpResponse &response, const std::string &file_name) {
    std::ifstream file_stream(file_name);
    std::stringstream buffer;
    buffer << file_stream.rdbuf();

    response.setBody(buffer.str());
}

inline HttpResponse root() {
    HttpResponse response;
    response.addHeader("Content-Type", "text/html");
    setFileBody(response, "pages/index.html");

    return response;
}

inline HttpResponse js() {
    HttpResponse response;
    response.addHeader("Content-Type", "application/javascript");
    setFileBody(response, "pages/main.js");

    return response;
}

#endif