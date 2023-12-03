#include "../headers/argparse.hpp"
#include "../headers/attacks.hpp"
#include "../headers/board.hpp"
#include "../headers/movegen.hpp"
#include "../headers/perfsuite.hpp"

#include <iostream>

using chess::Square;

int main(int argc, char** argv) {
    chess::attacks::init();

    ArgumentParser parser;
    parser.addArgument("--perft", "Run perft suite", true);
    parser.addArgument("--depth", "Depth to run perft suite to", true);
    parser.addArgument("--path", "Path to perft suite", true);
    parser.addArgument("--fen", "FEN to run perft on", true);
    parser.addArgument("--count", "Number of perft tests to run", true);
    parser.addArgument("--speed", "Run speed test", true);

    if (!parser.parse(argc, argv)) {
        parser.printHelp();
        return 1;
    }

    if (parser.argumentExists("--perft")) {
        if (parser.argumentExists("--path")) {
            std::string path  = parser.getArgumentValue("--path");
            uint64_t    count = std::stoi(parser.getArgumentValue("--count"));

            perft::startSuite(path, count);
        } else if (parser.argumentExists("--fen")) {
            std::string fen   = std::string{misc::split(parser.getArgumentValue("--fen"), '"')[0]};
            int         depth = std::stoi(parser.getArgumentValue("--depth"));

            perft::start(fen, depth);
        }
    }

    if (parser.argumentExists("--speed")) {
        if (parser.argumentExists("--fen")) {
            std::string fen   = std::string{misc::split(parser.getArgumentValue("--fen"), '"')[0]};
            int         depth = std::stoi(parser.getArgumentValue("--depth"));

            perft::speedTest(fen, depth);
        } else {
            perft::speedTest(chess::START_FEN, 6);
        }
    }

    // chess::Board board;

    // board.setFen("r3k2r/p1p1qpb1/bn1ppnp1/1B1PN3/1p2P3/2N2Q1p/PPPB1PPP/R2K3R b kq - 1 2");
    // // board.makeMove(chess::Move::make<chess::Move::NORMAL>(Square(Square::SQ_C7), Square(Square::SQ_C5)));
    // std::cout << board << std::endl;

    // chess::Movelist moves;
    // chess::movegen::generate<chess::movegen::MoveGenType::ALL>(board, moves);

    // // for (const auto& move : moves) {
    // //     std::cout << move << ": 1" << std::endl;
    // // }
    // std::cout << "Total moves: " << moves.size() << std::endl;

    // std::cout << "testing legal moves: " << std::endl;

    // int legal = 0;
    // for (const auto& move : moves) {
    //     if (!board.makeMove(move)) {
    //         continue;
    //     }
    //     legal++;
    //     board.unmakeMove(move);
    //     std::cout << move << ": 1" << std::endl;
    // }

    // std::cout << "Total legal moves: " << legal << std::endl;

    return 0;
}