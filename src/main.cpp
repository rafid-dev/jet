#include "../headers/argparse.hpp"
#include "../headers/attacks.hpp"
#include "../headers/board.hpp"
#include "../headers/movegen.hpp"
#include "../headers/perfsuite.hpp"

#include <iostream>

using chess::Square;

int main(int argc, char** argv) {
    chess::attacks::init();

    // perft::bulkSpeedTest();

    perft::bulkSuite("./standard.epd", 10000);

    // chess::Board board("rnbqkb1r/ppppp1pp/7n/4Pp2/8/8/PPPP1PPP/RNBQKBNR w KQkq f6 0 3");

    return 0;
}