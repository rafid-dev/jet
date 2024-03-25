#include "bench.hpp"
#include "chess/movegen.hpp"
#include "chess/zobrist.hpp"
#include "evaluation/evaluate.hpp"
#include "nnue/nnue.hpp"
#include "perfsuite.hpp"
#include "search/search.hpp"
#include "search/searchinfo.hpp"
#include "search/searchthread.hpp"

#include <iostream>
#include <istream>
#include <memory>
#include <sstream>

using namespace chess;
using namespace jet;
using namespace jet::search;

static constexpr std::string_view NAME    = "Jet";
static constexpr std::string_view VERSION = "1.2";
static constexpr std::string_view AUTHOR  = "Rafid Ahsan";

int main(int argc, char** argv) {
    Attacks::init();
    nnue::init();
    search::init();

    std::cout << NAME << " " << VERSION << std::endl;
    std::cout << "Copyright (C) 2023  " << AUTHOR << std::endl;
    search::TranspositionTable.initialize<false>(16);

    auto   heapSt = std::make_unique<SearchThread>();
    auto&  st     = *heapSt;
    Board& board  = st.board();

    std::string line;
    std::string token;

    std::vector<Move> moves;

    jet::search::SearchInfo info;

    if (argc > 1 && std::string(argv[1]) == "bench") {
        StartBenchmark(st);
        return 0;
    }

    while (std::getline(std::cin, line)) {
        token.clear();

        std::istringstream iss(line);

        iss >> token;

        if (token == "uci") {
            std::cout << "id name " << NAME << " " << VERSION << std::endl;
            std::cout << "id author " << AUTHOR << std::endl;
            std::cout << "option name Hash type spin default 8 min 8 max 32768" << std::endl;
            std::cout << "option name Threads type spin default 1 min 1 max 1" << std::endl;
            std::cout << "uciok" << std::endl;
        } else if (token == "isready") {
            std::cout << "readyok\n";
        } else if (token == "ucinewgame") {
            // Re initialize the transposition table upon a new game
            search::TranspositionTable.initialize<false>(16);
            st.setFen(FENS::STARTPOS);
        } else if (token == "movegen") {
            Movelist list;
            MoveGen::legalmoves<MoveGenType::ALL>(board, list);

            std::cout << list << std::endl;

            list.clear();
            MoveGen::legalmoves<MoveGenType::QUIET>(board, list);
            std::cout << "Quiet moves: " << list.size() << '\n';
            std::cout << list << std::endl;

            list.clear();
            MoveGen::legalmoves<MoveGenType::CAPTURE>(board, list);
            std::cout << "Capture moves: " << list.size() << '\n';
            std::cout << list << std::endl;

        } else if (token == "position") {
            iss >> token;

            if (token == "startpos") {
                st.setFen(FENS::STARTPOS);
                iss >> token;

            } else if (token == "kiwipete") {
                st.setFen(FENS::KIWIPETE);
                iss >> token;

            } else if (token == "fen") {
                std::string fen;

                while (iss >> token && token != "moves") {
                    fen += token + ' ';
                }

                st.setFen(fen);
            }

            if (token == "moves") {
                while (iss >> token) {
                    Move move = board.uciToMove(token);
                    // std::cout << "Parsed move: " << move << '\n';
                    board.makeMove(move);
                }
            }

            continue;
        } else if (token == "makemove" || token == "make") {
            while (iss >> token) {
                Move move = board.uciToMove(token);
                board.makeMove(move);
                moves.push_back(move);
            }

            std::cout << board << std::endl;
        } else if (token == "unmakemove" || token == "unmake") {
            board.unmakeMove(moves.back());
            moves.pop_back();

            std::cout << board << std::endl;

        } else if (token == "perftsuite") {
            iss >> token;

            perft::bulkSuite(token, 1000);
        } else if (token == "perft") {
            iss >> token;

            int depth = 6;
            if (token == "depth") {
                iss >> token;
                depth = std::stoi(token);
            }

            iss >> token;
            if (token == "speed") {
                perft::bulkSpeedTest(board, depth);
            } else {
                perft::startBulk(board, depth);
            }

            // Search things
        } else if (token == "go") {
            jet::search::TimeManager& tm = st.timeManager();

            tm.setNodes(0);
            tm.setMTG(0);

            while (iss >> token) {
                if (token == "depth") {
                    iss >> token;
                    info.setDepth(std::stoi(token));
                } else if (token == "wtime") {
                    iss >> token;
                    tm.setTime<TimeType::WTIME>(std::stod(token));
                } else if (token == "btime") {
                    iss >> token;
                    tm.setTime<TimeType::BTIME>(std::stod(token));
                } else if (token == "winc") {
                    iss >> token;
                    tm.setTime<TimeType::WINC>(std::stod(token));
                } else if (token == "binc") {
                    iss >> token;
                    tm.setTime<TimeType::BINC>(std::stod(token));
                } else if (token == "movetime") {
                    iss >> token;
                    tm.setTime<TimeType::MOVETIME>(std::stod(token));
                } else if (token == "movestogo") {
                    iss >> token;
                    tm.setMTG(std::stoi(token));
                } else if (token == "nodes") {
                    iss >> token;
                    tm.setNodes(std::stoull(token));
                }
            }

            jet::search::search(st, info);
        } else if (token == "setoption") {
            iss >> token;

            if (token == "name") {
                iss >> token;
                if (token == "Hash") {
                    iss >> token;
                    iss >> token;
                    search::TranspositionTable.initialize<true>(std::clamp(std::stoi(token), 8, 32768));
                }
            }
        } else if (token == "print") {
            std::cout << board << std::endl;
            st.refresh();
            std::cout << "Eval: " << jet::evaluation::evaluate(st) << std::endl;
        } else if (token == "quit" || token == "exit") {
            break;
        } else if (token == "\n") {
            continue;
        } else {
            std::cout << "Unknown command: " << token << '\n';
        }
    }
    return 0;
}