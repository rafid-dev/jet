#include "../headers/attacks.hpp"
#include "../headers/movegen.hpp"

#include <iostream>

using chess::Square;

void perft(chess::Board& board, int depth, uint64_t& nodes) {
    if (depth == 0) {
        ++nodes;
        return;
    }

    chess::Movelist moves;
    chess::movegen::generate<chess::movegen::MoveGenType::ALL>(board, moves);

    for (const auto& move : moves) {
        if (!board.makeMove(move)) {
            continue;
        }

        perft(board, depth - 1, nodes);
        board.unmakeMove(move);
    }
}

void perftDriver(chess::Board& board, int depth) {
    chess::Movelist moves;
    chess::movegen::generate<chess::movegen::MoveGenType::ALL>(board, moves);

    uint64_t nodes = 0;

    for (const auto& move : moves) {
        if (!board.makeMove(move)) {
            continue;
        }

        uint64_t nodes_reached = 0;

        perft(board, depth, nodes_reached);
        board.unmakeMove(move);

        if (move.from() == Square(Square::SQ_D1) && move.to() == Square(Square::SQ_E2))
            std::cout << board << std::endl;

        nodes += nodes_reached;

        std::cout << move << ":" << nodes_reached << std::endl;
    }

    std::cout << "Total nodes: " << nodes << std::endl;
}

int main() {
    chess::attacks::init();

    chess::Board board("rnbqkbnr/pppp1ppp/8/4p3/4P3/8/PPPP1PPP/RNBQKBNR w KQkq e6 0 2");

    perftDriver(board, 1);

    return 0;
}