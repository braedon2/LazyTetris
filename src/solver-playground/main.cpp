#include <iostream>
#include <stdio.h>
#include "../tetris.h"
#include "../solver.h"

void test_0();

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << "Takes one test id (integer) as input" << std::endl;
        return 1;
    }

    int testId = atoi(argv[1]);

    std::vector<void (*)()> tests;
    tests.push_back(test_0);

    tests.at(testId)();
    return 0;
}

void test_0() {
    Searcher::search();
}  