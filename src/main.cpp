#include "chess/movegen.hpp"
#include "chess/zobrist.hpp"
#include "evaluation/evaluate.hpp"
#include "perfsuite.hpp"
#include "search/search.hpp"
#include "search/searchinfo.hpp"
#include "search/searchthread.hpp"

#include <iostream>
#include <istream>
#include <sstream>

using namespace chess;

static constexpr std::string_view NAME   = "Jet";
static constexpr std::string_view AUTHOR = "Rafid Ahsan";

int main() {
    Attacks::init();

    std::cout << NAME << std::endl;
    std::cout << AUTHOR << std::endl;

    jet::search::SearchThread st;
    Board&                    board = st.board();

    std::string line;
    std::string token;

    std::vector<Move> moves;

    jet::search::SearchInfo info;

    while (std::getline(std::cin, line)) {
        token.clear();

        std::istringstream iss(line);

        iss >> token;

        if (token == "uci") {
            std::cout << "id name " << NAME << '\n';
            std::cout << "id author " << AUTHOR << '\n';
            std::cout << "uciok\n";
        } else if (token == "isready") {
            std::cout << "readyok\n";
        } else if (token == "ucinewgame") {
            board.setFen(FENS::STARTPOS);
        } else if (token == "movegen") {
            Movelist list;
            MoveGen::legalmoves<MoveGenType::ALL>(board, list);

            std::cout << list << std::endl;
        } else if (token == "position") {
            iss >> token;

            if (token == "startpos") {
                board.setFen(FENS::STARTPOS);
            } else if (token == "kiwipete") {
                board.setFen(FENS::KIWIPETE);
            } else if (token == "fen") {
                std::string fen;

                while (iss >> token && token != "moves") {
                    fen += token + ' ';
                }

                board.setFen(fen);
            }

            while (iss >> token) {
                Move move = board.uciToMove(token);
                board.makeMove(move);
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
            while (iss >> token) {
                if (token == "depth") {
                    iss >> token;
                    info.setDepth(std::stoi(token));
                }
            }

            jet::search::search(st, info);
        } else if (token == "print") {
            std::cout << board << std::endl;
            std::cout << "Eval: " << jet::evaluation::evaluate(board) << std::endl;
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