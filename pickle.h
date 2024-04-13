#pragma once

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

#include "fmt.h"

// Definitions for pickle language implementation.

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

// A name such as `x` which resolve to a value
using Binding = std::string;

// A concrete value
struct Val {
    std::variant<std::string, f64, i64> val;

    void dump() const {
        std::visit([](auto&& value) { print("{}", value); }, val);
    }
};

// An argument to an Op. If a binding, it will need to be resolved.
using Arg = std::variant<Val, Binding>;

using Func = std::function<void()>;

// Run-time environment of the execution
struct Environment {
    Map<Binding, Val> bindings;

    std::optional<Val> get(Binding b) const {
        auto it = bindings.find(b);
        if (it == bindings.end()) {
            return {};
        }
        return it->second;
    }

    void dump() const {
        println("env: {{");
        for (auto [k, v] : bindings) {
            print("\t`{}`: ", k);
            v.dump();
            println(",");
        }
        println("}}");
    }
};

// A single operation
struct Op {
    enum {
       CALL,
       ASSIGN,
       ADD,
       SUB,
       MUL,
       DIV,

       OP_COUNT
    } type;

    Op(decltype(type) type) : type(type) {}

    virtual void dump() const = 0;
};


// Block of program.
struct BasicBlock {
    Vec<RefPtr<Op>> ops;
};

struct Program {
    Vec<BasicBlock> blocks;

    void dump() const {
        for (size_t i = 0; i < blocks.size(); i++) {
            println("Block [{}]: {{", i);
            auto ops = blocks[i].ops;
            for (size_t j = 0; j < ops.size(); j++) {
                print("\t{}: ", j);
                ops[j]->dump();
            }
        }
    }
};

