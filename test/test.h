#include <string_view>
#include <vector>

class Test {
public:
    virtual bool run() = 0;
    virtual std::string_view name() = 0;

    std::string_view fail_reason() const { return fail_reason_; }
protected:
    bool fail(std::string_view reason) {
        fail_reason_ = reason;
        return false;
    };

    std::string_view fail_reason_;
};

inline std::vector<Test*>& all_tests() {
    static std::vector<Test*> s_tests;
    return s_tests;
}

inline void add_test(Test* t) {
    all_tests().push_back(t);
}

// Define a test.
// The program body will return true if the test passed, or false if it failed.
#define TEST(tname)                                         \
class Test_ ## tname : public Test {                        \
public:                                                     \
    Test_ ## tname() { add_test(this); }                    \
    std::string_view name() override { return #tname; }     \
    bool run() override;                                    \
};                                                          \
static Test_ ## tname s_ ## tname ## _inst;                 \
bool Test_ ## tname::run()                                  \

