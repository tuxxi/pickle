#include "pickle.h"

class Test {
public:
    virtual bool run() = 0;
    virtual std::string_view name() = 0;
};

std::vector<Test*>& all_tests() {
    static std::vector<Test*> s_tests;
    return s_tests;
}

void add_test(Test* t) {
    all_tests().push_back(t);
}

#define TEST(tname)                                         \
class Test_ ## tname : public Test {                        \
public:                                                     \
    Test_ ## tname() { add_test(this); }                    \
    std::string_view name() override { return #tname; }     \
    bool run() override;                                    \
};                                                          \
static Test_ ## tname s_ ## tname ## _inst;                 \
bool Test_ ## tname::run()                                  \


// ====================================

TEST(PickleParse) {
    return true;
}

TEST(ParseFoo) {
    return false;
}
// run pickle tests
int main() {
    const auto& tests = all_tests();
    if (tests.empty()) {
        println("No tests to run.");
        return 0;
    }
    println("Running {} tests...", tests.size());

    int passed = 0;
    for (auto* t : tests) {
        println("Running: {}", t->name());
        if (!t->run()) {
            println("! FAILED : {}", t->name());
            continue;
        }
        // println("+ PASSED : {}", t->name());
        passed++;
    }
    println("====================");
    println("PASSED: {} / {}", passed, tests.size());
    if (passed != tests.size()) {
        exit(1);
    }
}
