#ifndef URLS_H
#define URLS_H

#include "router.hpp"
#include "routeHandlers.hpp"

inline HttpResponse defaultResponse() {
    HttpResponse response(404, "Not Found");
    return response;
}

inline std::tuple routes = {
    Route<"/">(root),
    Route<"/main.js">(js)
};

inline Router router(routes, defaultResponse);

#endif