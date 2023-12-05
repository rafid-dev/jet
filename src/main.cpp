#include "../headers/board.hpp"

#include <iostream>

using namespace chess;

int main() {
    Board board{};

    std::cout << board << '\n';

    return 0;
}