#include <cstdio>
#include <iostream>
#include <string>
#include <format>
#include <unordered_map>
#include <memory>
#include <vector>

template<typename ...Args>
void print(std::format_string<Args...> fmt, Args&&... args) {
    std::cout << std::format(fmt, std::forward(args)...);
}

template<typename ...Args>
void println(std::format_string<Args...> fmt, Args&&... args) {
    print(fmt, std::forward(args)...);
    std::cout << std::endl;
}

template<typename T>
using OwnPtr = std::unique_ptr<T>;

template<typename T>
using RefPtr = std::shared_ptr<T>;

template<typename T>
using Vec = std::vector<T>;

template<typename K, typename V>
using Map = std::unordered_map<K,V>;

using f64 = double;
using u64 = uint64_t;
using i64 = int64_t;

///
///

using Val = std::variant<std::string, f64, i64>;

struct Environment {
    Map<std::string, OwnPtr<Val>> bindings;
};

struct ParseTree {
    enum { Node, Null } tag;
    union {
        Vec<RefPtr<ParseTree>> children;
    };
};

auto read(std::istream& stream) -> std::string {
    std::string buf;
    for (std::string line; std::getline(stream, line);) {
        if (stream.bad()) {
            print("IO error");
            exit(1);
        }
        buf += line;
    }
    return buf;
}

auto parse(std::string_view line) -> RefPtr<ParseTree> {
    return {};
}

auto eval(RefPtr<ParseTree> tree, const Environment& env) -> Environment {
    return {};
}

auto main() -> int {

    // read input from stdin
    auto program = read(std::cin);
    std::string_view prog(program);

    auto tree = parse(prog);

    Environment env;
    for (auto node : tree->children) {
        env = eval(node, env);
    }
    return 0;
}
