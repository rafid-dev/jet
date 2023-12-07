#include "../headers/attacks.hpp"
#include "../headers/board.hpp"
#include "../headers/misc.hpp"

#include <iostream>
#include <istream>
#include <sstream>

using namespace chess;

static constexpr std::string_view NAME   = "Jet";
static constexpr std::string_view AUTHOR = "Rafid Ahsan";

int main() {
    Attacks::init();

    Board board;

    std::string line;
    std::string token;

    std::vector<Move> moves;

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
        } else if (token == "position") {
            iss >> token;

            if (token == "startpos") {
                board.setFen(FENS::STARTPOS);
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

        } else if (token == "quit" || token == "exit") {
            break;
        } else if (token == "print") {
            std::cout << board << std::endl;
        } else {
            std::cout << "Unknown command: " << token << '\n';
        }
    }
    return 0;
}