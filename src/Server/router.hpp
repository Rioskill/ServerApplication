#ifndef ROUTER_H
#define ROUTER_H

#include <iostream>
#include <functional>
#include <typeinfo>
#include <type_traits>
#include <array>
#include <tuple>
#include <variant>
#include <string>
#include <iomanip>
#include <exception>

#include "HttpResponse.hpp"

using RouteFunctionReturnType = HttpResponse;

template<size_t N>
struct StringLiteral {
    constexpr StringLiteral (const char (&str)[N]) {
        std::copy_n(str, N, value);
    }

    constexpr char operator[] (unsigned int n) const {
        return value[n];
    }

    constexpr int size() const {
        return sizeof(value);
    }
    
    char value[N];
};

template <typename... Ts>
struct TypeHolder {};

template <typename T1, typename T2>
struct TypeAdder {
    using type = TypeHolder<T1, T2>;
};

template <typename T, typename... Ts>
struct TypeAdder<TypeHolder<Ts...>, T> {
    using type = TypeHolder<Ts..., T>;
};


template <char C, bool TYPE, StringLiteral STR, unsigned int POS, typename ...Ts>
struct Format {
    using type = typename Format<STR[POS + 1], false, STR, POS + 1, Ts...>::type;
};

template <StringLiteral STR, unsigned int POS, typename... Ts>
struct Format<'\0', false, STR, POS, Ts...> {
    using type = TypeHolder<Ts...>;
};

template <StringLiteral STR, unsigned int POS, typename... Ts>
struct Format<'%', false, STR, POS, Ts...> {
    using type = typename Format<STR[POS + 1], true, STR, POS + 1, Ts...>::type;
};

template <StringLiteral STR, unsigned int POS, typename... Ts>
struct Format<'d', true, STR, POS, Ts...> {
    using type = typename Format<STR[POS + 1], false, STR, POS + 1, Ts..., int>::type;
};

template <StringLiteral STR, unsigned int POS, typename... Ts>
struct Format<'f', true, STR, POS, Ts...> {
    using type = typename Format<STR[POS + 1], false, STR, POS + 1, Ts..., float>::type;
};

template <StringLiteral STR, unsigned int POS, typename... Ts>
struct Format<'s', true, STR, POS, Ts...> {
    using type = typename Format<STR[POS + 1], false, STR, POS + 1, Ts..., std::string>::type;
};

template <StringLiteral STR>
struct GetTypeFromStringLiteral {
    using type = typename Format<STR[0], false, STR, 0>::type;
};

template <typename>
struct Func {};

template <typename... Ts>
struct Func<TypeHolder<Ts...>> {
    using type = RouteFunctionReturnType(*)(Ts...);
};

template <typename>
struct FuncObj {};

template <typename... Ts>
struct FuncObj<TypeHolder<Ts...>> {
    using type = std::function<RouteFunctionReturnType(Ts...)>;
};

template <typename>
struct ArgTuple {};

template <typename... Ts>
struct ArgTuple<TypeHolder<Ts...>> {
    using type = std::tuple<Ts...>;
};

template <typename>
struct Var {};

template <typename... Ts>
struct Var<TypeHolder<Ts...>> {
    using type = std::variant<Ts...>;
};

template <typename... ArgTs>
class PathParser {
private:
    bool success;
    std::tuple <ArgTs...> res;
    std::string path_template;
    std::string::iterator templ_it, path_it;

    void prep_checks() {
        if (*templ_it == '\0')
            return;

        if (*templ_it == '/') {
            templ_it++;
            path_it++;
        }
    }

    template <int id>
    void parse_subfolder() {
        while (*templ_it != '\0' && *path_it != '\0') {
            if (*templ_it != *path_it) {
                success = false;
                return;
            }
            templ_it++;
            path_it++;
        }
        if (*templ_it != *path_it)
            success = false;
    }

    template <int id, typename T>
    void parse_subfolder() {

        prep_checks();

        if (*templ_it == '%') {
            get_argument<id, T>();

            return;
        }

        while (*templ_it != '/') {
            if (*templ_it != *path_it) {
                success = false;
                return;
            }
            templ_it++;
            path_it++;
        }

        parse_subfolder<id, T>();
    }

    template <int id, typename T1, typename T2, typename... Ts>
    void parse_subfolder() {

        prep_checks();

        if (*templ_it == '%') {
            get_argument<id, T1>();
            if (success)
                parse_subfolder<id + 1, T2, Ts...>();
        } else {
            while (*templ_it != '/') {
                if (*templ_it != *path_it) {
                    success = false;
                    return;
                }
                templ_it++;
                path_it++;
            }
            parse_subfolder<id, T1, T2, Ts...>();
        }
    }

    std::string read_argument () {
        templ_it++;
        char type = *templ_it;
        templ_it++;

        std::string value;

        while (*path_it != '\0' && *path_it != '/') {
            value += *path_it;
            path_it++;
        }

        return value;
    }

    class NotImplementedTypeError: std::exception {
    public:
        virtual const char* what() const throw() {
            return "get_argument for this type is not implemented yet!";
        }
    };

    bool check_d (const std::string &str) {
        return str.find_first_not_of("0123456789") == std::string::npos;
    }

    template <int id, typename T>
    void get_argument() {

        std::string value = read_argument();

        if constexpr (std::is_same<T, int>::value) {
            if (check_d(value)) {
                std::get<id>(res) = std::stoi(value);
            } else
                success = false;
            return;
        }
    
        if constexpr (std::is_same<T, std::string>::value) {
            std::get<id>(res) = value;
            return;
        }

        throw NotImplementedTypeError();
    }

public:
    PathParser () {}

    PathParser (const std::string &path_template): path_template(path_template) {}

    void setPathTemplate(const std::string &path_template) {
        this->path_template = path_template;
    }

    void parse(std::string path) {
        success = true;

        templ_it = path_template.begin();
        path_it = path.begin();

        parse_subfolder<0, ArgTs...>();
    }

    bool succeeded() {
        return success;
    }

    const auto &getParsingResult() {
        return res;
    }
};

template <typename>
struct PathParserCreator {};

template <typename... Ts>
struct PathParserCreator<TypeHolder<Ts...>> {
    using parser = PathParser<Ts...>;
};

template <StringLiteral lit>
class Route {
private:
    using func_args_types = GetTypeFromStringLiteral<lit>::type;

    using func_type = Func<func_args_types>::type;
    using func_obj = FuncObj<func_args_types>::type;
    using args_tuple = ArgTuple<func_args_types>::type;

    using parser_type = PathParserCreator<func_args_types>::parser;
    parser_type path_parser;

    std::string path;
    func_obj function;

    // using nothing = func_type::nothing;

public:
    template <typename FUNC>
    Route (FUNC function): function(function) {
        path = lit.value;
        path_parser.setPathTemplate(path);
    }

    RouteFunctionReturnType exec() {
        return std::apply(function, path_parser.getParsingResult());
    }

    bool exec_path (const std::string &path) {
        path_parser.parse(path);

        return path_parser.succeeded();
    }

    const std::string &get_path() {
        return path;
    }
};

template <typename... Ts>
class Router {
private:
    std::tuple<Ts...> routes;
    std::function<RouteFunctionReturnType()> defaultFunction;

    template <int id>
    typename std::enable_if<id == sizeof...(Ts), RouteFunctionReturnType>::type check_route (const std::string &path) {
        return defaultFunction();
    }   

    template <int id>
    typename std::enable_if<id < sizeof...(Ts), RouteFunctionReturnType>::type check_route (const std::string &path) {
        auto route = std::get<id>(routes);
        bool success = route.exec_path(path);

        if (success)
            return route.exec();
    
        return check_route<id+1>(path);
    }

public:
    Router (std::tuple<Ts...> routes, std::function<RouteFunctionReturnType()> defaultFunction): routes(routes), defaultFunction(defaultFunction) {}

    RouteFunctionReturnType check_routes (const std::string &path) {
        return check_route<0>(path);
    }
};

#endif