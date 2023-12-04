#include "../headers/argparse.hpp"
#include "../headers/attacks.hpp"
#include "../headers/board.hpp"
#include "../headers/movegen.hpp"
#include "../headers/perfsuite.hpp"

#include <iostream>

using chess::Square;

int main(int argc, char** argv) {
    chess::attacks::init();

    perft::bulkSpeedTest();

    return 0;
}