#pragma once

#include "../chess/moves.hpp"
#include <cstdint>

namespace jet {

    namespace types {
        using PvTable   = std::array<chess::Move, 64>;
        using Value     = int;
        using Depth     = int;
        using Value_i16 = int16_t;
        using Depth_u8  = uint8_t;
        using Time      = double;
    } // namespace types

} // namespace jet