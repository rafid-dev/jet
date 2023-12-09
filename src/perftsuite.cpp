#include "../headers/board.hpp"
#include "../headers/misc.hpp"
#include "../headers/movegen.hpp"
#include "../headers/perfsuite.hpp"

#include <fstream>
#include <iostream>
#include <sstream>

namespace perft {

    template <bool print = false>
    uint64_t bulkPerft(chess::Board& board, int depth) {
        chess::Movelist moves;
        chess::MoveGen::legalmoves<chess::MoveGenType::ALL>(board, moves);

        if (depth == 1) {
            return moves.size();
        }

        uint64_t nodes = 0;

        for (const auto& move : moves) {
            if constexpr (print) {
                board.makeMove(move);
                auto child = bulkPerft<false>(board, depth - 1);
                board.unmakeMove(move);
                std::cout << move << ": " << child << std::endl;
                nodes += child;
            } else {
                board.makeMove(move);
                nodes += bulkPerft<false>(board, depth - 1);
                board.unmakeMove(move);
            }
        }

        return nodes;
    }

    template <bool print = false>
    void testPositionBulk(chess::Board& board, int depth, uint64_t& nodes) {
        if constexpr (print) {
            if (depth == 1) {
                chess::Movelist moves;
                chess::MoveGen::legalmoves<chess::MoveGenType::ALL>(board, moves);

                for (const auto& move : moves) {
                    std::cout << move << ": 1" << std::endl;
                }

                nodes = moves.size();
            } else {
                nodes = bulkPerft<true>(board, depth);
            }
        } else {
            nodes = bulkPerft<false>(board, depth);
        }
    }

    void bulkSuite(const std::string& name, const uint64_t max) {
        std::cout << "(BULK) Starting perft suite: " << name << std::endl;

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

        uint64_t totalNodes = 0;

        while (std::getline(file, line)) {
            if (count > max) {
                break;
            }

            EpdInfo info(line);

            for (const auto& depthnodes : info.fetch()) {
                std::cout << "\033[0m" << std::endl;
                std::cout << "testing: " << info.fen();
                count++;

                chess::Board board(info.fen());

                uint64_t nodes = 0;

                auto depth    = depthnodes.depth();
                auto required = depthnodes.nodes();

                auto start = misc::tick();
                testPositionBulk(board, depth, nodes);

                auto     time_elapsed = misc::tick() - start;
                uint64_t speed        = static_cast<uint64_t>(1000.0f * nodes / (time_elapsed + 1));

                std::string color = (nodes == required) ? "\033[32m" : "\033[31m";
                if (nodes != required) {
                    std::cout << "\r" << color << "#" << count << " D" << depth << " Failed: [" << info.fen()
                              << "] Expected: " << required << " Got: " << nodes << " Speed: " << speed
                              << " NPS" << std::endl;

                    testPositionBulk<true>(board, depth, nodes);
                    std::cout << board << std::endl;

                    fails++;
                } else {
                    std::cout << "\r" << color << "#" << count << " D" << depth << " Passed: [" << info.fen()
                              << "] Expected: " << required << " Got: " << nodes << " Speed: " << speed
                              << " NPS" << std::endl;
                    passes++;
                }

                totalTime += time_elapsed;
                totalNodes += nodes;

                std::cout << "\033[0m" << std::endl;
            }
        }

        std::cout << "Finished perft suite: " << name << std::endl;
        std::cout << "Total tests: " << count << std::endl;
        std::cout << "Total passes: " << passes << std::endl;
        std::cout << "Total fails: " << fails << std::endl;
        std::cout << "Total time: " << totalTime << "ms" << std::endl;
        std::cout << "Average speed: " << static_cast<uint64_t>(1000.0f * totalNodes / (totalTime + 1))
                  << " NPS" << std::endl;
    }

    void startBulk(const std::string& fen, const int depth) {
        chess::Board board(fen);

        uint64_t nodes = 0;

        testPositionBulk<true>(board, depth, nodes);

        std::cout << "Nodes: " << nodes << std::endl;
    }

    void startBulk(const chess::Board& brd, const int depth) {
        chess::Board board = brd;

        uint64_t nodes = 0;

        testPositionBulk<true>(board, depth, nodes);

        std::cout << "Nodes: " << nodes << std::endl;
    }

    void bulkSpeedTest(const std::string_view& fen, const int depth) {
        std::cout << "(BULK) Starting speed test: " << fen << " D" << depth << std::endl;
        chess::Board board(fen);

        uint64_t nodes = 0;

        auto start_time = misc::tick();

        testPositionBulk<false>(board, depth, nodes);

        auto time_elapsed = misc::tick() - start_time;

        std::cout << "Nodes: " << nodes << std::endl;
        std::cout << "Time: " << time_elapsed << "ms" << std::endl;
        std::cout << "Speed: " << static_cast<uint64_t>(1000.0f * nodes / (time_elapsed + 1)) << " NPS"
                  << std::endl;
    }

    void bulkSpeedTest(const chess::Board& brd, const int depth) {
        std::cout << "(BULK) Starting speed test: "
                  << " D" << depth << std::endl;
        chess::Board board = brd;

        uint64_t nodes = 0;

        auto start_time = misc::tick();

        testPositionBulk<false>(board, depth, nodes);

        auto time_elapsed = misc::tick() - start_time;

        std::cout << "Nodes: " << nodes << std::endl;
        std::cout << "Time: " << time_elapsed << "ms" << std::endl;
        std::cout << "Speed: " << static_cast<uint64_t>(1000.0f * nodes / (time_elapsed + 1)) << " NPS"
                  << std::endl;
    }

} // namespace perft