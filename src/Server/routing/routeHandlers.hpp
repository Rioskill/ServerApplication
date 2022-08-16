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

inline HttpResponse room (int room_id) {
    HttpResponse response;

    std::ifstream file_stream("pages/room.html");
    std::stringstream buffer;
    buffer << file_stream.rdbuf();

    std::string file_str = buffer.str();

    int pos = file_str.find("{}");

    std::string res_str = file_str.substr(0, pos);
    res_str += std::to_string(room_id);
    res_str += file_str.substr(pos + 2);

    response.setBody(res_str);

    return response;
}

#endif