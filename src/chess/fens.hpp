#pragma once

#include <string>

namespace chess {
    // Some FEN strings for testing
    namespace FENS {
        static constexpr std::string_view STARTPOS = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
        static constexpr std::string_view KIWIPETE = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1";
    } // namespace FENS
} // namespace chess