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

    while (std::getline(std::cin, line)) {
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
                Move move = Move::fromUCI(token);
                board.makeMove(move);
            }
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