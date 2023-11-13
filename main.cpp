#include <cstdio>
#include <cwchar>
#include <format>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include <stdexcept>


template<typename ...Args>
void print(std::format_string<Args...> fmt, Args&&... args) {
    std::cout << std::format(fmt, std::forward<Args>(args)...);
}

template<typename ...Args>
void println(std::format_string<Args...> fmt, Args&&... args) {
    std::cout << std::format(fmt, std::forward<Args>(args)...) << std::endl;
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

    bool contains(Binding b) const {
        return bindings.find(b) != bindings.end();
    }

    Val get(Binding b) const {
        return bindings.at(b);
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

struct BinaryOp : public Op {
    BinaryOp(decltype(Op::type) type, Binding ret, Arg lhs, Arg rhs) : Op(type), ret(ret), lhs(lhs), rhs(rhs) {}

    Val eval(const Environment &env) {
        auto [lhs, rhs] = resolve_bindings(env);
        return op(lhs, rhs);
    }

    Binding ret;
    Arg lhs;
    Arg rhs;

protected:
    // Each BinaryOp will override this
    virtual Val op(Val lhs, Val rhs) = 0;

    std::tuple<Val, Val> resolve_bindings(const Environment& env) {
        Val lhs, rhs;

        if (Binding* lhs_binding = std::get_if<Binding>(&this->lhs)) {
            if (!env.contains(*lhs_binding)) {
                // TODO return a completion or failure here
                throw std::runtime_error(std::format("unresolved binding: {}", *lhs_binding));
            }
            lhs = env.get(*lhs_binding);
        } else {
            lhs = std::get<Val>(this->lhs);
        }

        // Resolve bindings
        if (Binding* rhs_binding = std::get_if<Binding>(&this->rhs)) {
            if (!env.contains(*rhs_binding)) {
                // TODO return a completion or failure here
                throw std::runtime_error(std::format("unresolved binding: {}", *rhs_binding));
            }
            rhs = env.get(*rhs_binding);
        } else {
            rhs = std::get<Val>(this->rhs);
        }
        return {lhs, rhs};
    }

};

struct CallOp : public Op {
    CallOp(const Vec<Arg>& args, Func callable, Binding ret) : Op(Op::CALL), args(args), callable(callable), ret(ret) {};

    Vec<Arg> args;
    Func callable;
    Binding ret;

    void dump() const override {
        println("CALL: {} = ", ret);
    }
};

struct AssignOp : public Op {
    AssignOp(Binding lhs, Arg rhs) : Op(Op::ASSIGN), lhs(lhs), rhs(rhs) {};

    Binding lhs;
    Arg rhs;

    void dump() const override {
        print("ASSIGN: {} = ", lhs);
        //rhs.dump();
        println("");
    }
};

struct AddOp : public BinaryOp {
    AddOp(Binding ret, Arg lhs, Arg rhs) : BinaryOp(Op::ADD, ret, lhs, rhs) {}

    void dump() const override {
        println("ADD {} = ", ret);
    }

private:
    Val op(Val lhs, Val rhs) override {
        Val new_value;

        // TODO clean up this mess
        if (i64* ilhs = std::get_if<i64>(&lhs.val)) {
            if (i64* irhs = std::get_if<i64>(&rhs.val)) {
                new_value = Val{*ilhs + *irhs};
            } else if (f64* irhs = std::get_if<f64>(&rhs.val)) {
                new_value = Val{*ilhs + *irhs};
            }
        } else if (f64* flhs = std::get_if<f64>(&lhs.val)) {
            if (i64* irhs = std::get_if<i64>(&rhs.val)) {
                new_value = Val{*flhs + *irhs};
            } else if (f64* frhs = std::get_if<f64>(&rhs.val)) {
                new_value = Val{*flhs + *frhs};
            }
        } else {
            throw std::runtime_error(std::format("unsupported types for op Add (lhs: {}, rhs: {})", lhs.val.index(), rhs.val.index()));
        }
        return new_value;
    }
};

struct MulOp : public BinaryOp {
    MulOp(Binding ret, Arg lhs, Arg rhs) : BinaryOp(Op::MUL, ret, lhs, rhs) {}

    void dump() const override {
        println("MUL {} = ", ret);
    }

private:
    Val op(Val lhs, Val rhs) override {
        Val new_value;

        // TODO clean up this mess
        if (i64* ilhs = std::get_if<i64>(&lhs.val)) {
            if (i64* irhs = std::get_if<i64>(&rhs.val)) {
                new_value = Val{*ilhs * *irhs};
            } else if (f64* irhs = std::get_if<f64>(&rhs.val)) {
                new_value = Val{*ilhs * *irhs};
            }
        } else if (f64* flhs = std::get_if<f64>(&lhs.val)) {
            if (i64* irhs = std::get_if<i64>(&rhs.val)) {
                new_value = Val{*flhs * *irhs};
            } else if (f64* frhs = std::get_if<f64>(&rhs.val)) {
                new_value = Val{*flhs * *frhs};
            }
        } else {
            throw std::runtime_error(std::format("unsupported types for op Add (lhs: {}, rhs: {})", lhs.val.index(), rhs.val.index()));
        }
        return new_value;
    }
};

/////////

namespace builtins {
};

/////////

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

auto parse(std::string_view line) -> RefPtr<Program> {
    return {};
}

auto eval(Op& op, const Environment& env) -> Environment {
    Environment new_env = env;
    switch (op.type) {
    case Op::ASSIGN:
    {
        AssignOp& assign = static_cast<AssignOp&>(op);

        Val new_val;
        if (Binding* binding = std::get_if<Binding>(&assign.rhs)) {
            if (!env.contains(*binding)) {
                // TODO return a completion or failure here
                throw std::runtime_error(std::format("unresolved binding: {}", *binding));
            }
            new_val = env.get(*binding);
        } else {
            new_val = std::get<Val>(assign.rhs);
        }
        new_env.bindings[assign.lhs] = new_val;
        break;
    }
    case Op::CALL:
    {
        // CallOp& cop = static_cast<CallOp&>(op);
        // new_env = cop.func(env);
        break;
    }
    case Op::ADD:
    {
        auto add = static_cast<AddOp&>(op);
        new_env.bindings.insert_or_assign(add.ret, add.eval(new_env));
        break;
    }
    case Op::MUL:
    {
        auto mul = static_cast<MulOp&>(op);
        new_env.bindings.insert_or_assign(mul.ret, mul.eval(new_env));
        break;
    }
    // case Op::SUB:
    // {
    //     new_env.bindings.insert_or_assign(add.ret, static_cast<SubOp&>(op).eval(new_env));
    //     break;
    // }
    // case Op::DIV:
    // {
    //     new_env.bindings.insert_or_assign(add.ret, static_cast<DivOp&>(op).eval(new_env));
    //     break;
    // }
    default:
        print("Unknown op type: {}", (int)op.type);
        exit(1);
    }
    return new_env;
}

auto eval(BasicBlock& block, const Environment& env) -> Environment {
    Environment new_env = env;
    for (auto&& op : block.ops) {
        new_env = eval(*op, new_env);
        new_env.dump();
    }
    return new_env;
}

auto main() -> int {

    // read input from stdin
    // auto program = read(std::cin);
    // std::string_view prog(program);
    //
    // auto tree = parse(prog);


    Program p = {
        Vec<BasicBlock>{
            {
                Vec<RefPtr<Op>>{
                    std::make_shared<AssignOp>(Binding("x"), Val{i64(1)}),
                    std::make_shared<AssignOp>(Binding("y"), Val{i64(2)}),
                    std::make_shared<AddOp>(Binding("z"), Binding("x"), Binding("y")),
                    std::make_shared<MulOp>(Binding("zz"), Binding("z"), Binding("z")),
                    std::make_shared<AssignOp>(Binding("x"), Binding("zz")),
                    // reference error
                    std::make_shared<AssignOp>(Binding("x"), Binding("bad")),
                }
            },
        },
    };

    p.dump();

    Environment env;
    for (auto block : p.blocks) {
        env = eval(block, env);
        env.dump();
    }
    return 0;
}
