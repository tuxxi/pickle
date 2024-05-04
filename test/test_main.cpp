#include "test.h"
#include "fmt.h"

int main() {
    const auto& tests = all_tests();
    if (tests.empty()) {
        println("No tests to run.");
        return 0;
    }
    println("Running {} tests...", tests.size());

    int passed = 0;
    for (int i = 0; i < tests.size(); ++i) {
        auto& t = tests[i];
        println("[{}] Running: {}", i+1, t->name());
        if (!t->run()) {
            println("! FAILED - {}", t->name());
            auto reason = t->fail_reason();
            if (!reason.empty()) {
                println("  {}", reason);
            }
            continue;
        }
        passed++;
    }
    println("====================");
    println("PASSED: {} of {}", passed, tests.size());
    if (passed != tests.size()) {
        exit(1);
    }
}

