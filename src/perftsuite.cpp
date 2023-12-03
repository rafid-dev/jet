#include "../headers/board.hpp"
#include "../headers/misc.hpp"
#include "../headers/movegen.hpp"
#include "../headers/perfsuite.hpp"

#include <fstream>
#include <iostream>
#include <sstream>

namespace perft {

    void listMoves(chess::Board& board) {
        chess::Movelist moves;
        chess::movegen::generate<chess::movegen::MoveGenType::ALL>(board, moves);

        for (const auto& move : moves) {
            if (!board.makeMove(move)) {
                continue;
            }
            std::cout << move << ": 1" << std::endl;
            board.unmakeMove(move);
        }
    }

    void perft(chess::Board& board, int depth, uint64_t& nodes) {
        if (depth == 0) {
            nodes++;
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

    template <bool print = false>
    void testPosition(chess::Board& board, int depth, uint64_t& nodes) {
        chess::Movelist moves;
        chess::movegen::generate<chess::movegen::MoveGenType::ALL>(board, moves);

        for (const auto& move : moves) {
            if (!board.makeMove(move)) {
                continue;
            }

            uint64_t child = 0;

            perft(board, depth - 1, child);

            board.unmakeMove(move);

            if constexpr (print) {
                std::cout << move << ": " << child << std::endl;
            }

            nodes += child;
        }
    }

    void startSuite(const std::string& name, const uint64_t max) {
        std::cout << "Starting perft suite: " << name << std::endl;

        std::ifstream file(name, std::ios::in);

        if (!file.is_open()) {
            std::cout << "Failed to open file: " << name << std::endl;
            return;
        }

        std::string line;

        uint64_t count = 0;

        uint64_t fails     = 0;
        uint64_t passes    = 0;
        uint64_t totalTime = 0;

        auto start_time = misc::tick();

        while (std::getline(file, line)) {
            if (count > max) {
                break;
            }
            count++;

            EpdInfo info(line);

            for (const auto& depthnodes : info.fetch()) {
                std::cout << "\033[0m" << std::endl;
                std::cout << "testing: " << info.fen();

                chess::Board board(info.fen());

                uint64_t nodes = 0;

                auto depth    = depthnodes.depth();
                auto required = depthnodes.nodes();

                auto start = misc::tick();
                testPosition(board, depth, nodes);

                auto     time_elapsed = misc::tick() - start;
                uint64_t speed        = static_cast<uint64_t>(1000.0f * nodes / (time_elapsed + 1));

                std::string color = (nodes == required) ? "\033[32m" : "\033[31m";
                if (nodes != required) {
                    std::cout << "\r" << color << "#" << count << " D" << depth << " Failed: [" << info.fen() << "] Expected: " << required << " Got: " << nodes << " Speed: " << speed << " NPS" << std::endl;

                    testPosition<true>(board, depth, nodes);
                    std::cout << board << std::endl;

                    fails++;
                } else {
                    std::cout << "\r" << color << "#" << count << " D" << depth << " Passed: [" << info.fen() << "] Expected: " << required << " Got: " << nodes << " Speed: " << speed << " NPS" << std::endl;
                    passes++;
                }

                totalTime += time_elapsed;

                std::cout << "\033[0m" << std::endl;
            }
        }

        std::cout << "Finished perft suite: " << name << std::endl;
        std::cout << "Total tests: " << count << std::endl;
        std::cout << "Total passes: " << passes << std::endl;
        std::cout << "Total fails: " << fails << std::endl;
        std::cout << "Total time: " << totalTime << "ms" << std::endl;
        std::cout << "Average time: " << totalTime / count << "ms" << std::endl;
    }

    void start(const std::string& fen, const int depth) {
        chess::Board board(fen);

        uint64_t nodes = 0;

        testPosition<true>(board, depth, nodes);

        std::cout << "Nodes: " << nodes << std::endl;
    }

    void speedTest(const std::string_view& fen, const int depth) {
        std::cout << "Starting speed test: " << fen << " D" << depth << std::endl;
        auto     start_time = misc::tick();
        uint64_t nodes      = 0;

        chess::Board    board(fen);
        chess::Movelist moves;
        chess::movegen::generate<chess::movegen::MoveGenType::ALL>(board, moves);

        for (const auto& move : moves) {
            if (!board.makeMove(move)) {
                continue;
            }

            perft(board, depth - 1, nodes);

            board.unmakeMove(move);
        }

        auto time_elapsed = misc::tick() - start_time;

        std::cout << "Nodes: " << nodes << std::endl;
        std::cout << "Time: " << time_elapsed << "ms" << std::endl;
        std::cout << "Speed: " << static_cast<uint64_t>(1000.0f * nodes / (time_elapsed + 1)) << " NPS" << std::endl;
    }
} // namespace perft