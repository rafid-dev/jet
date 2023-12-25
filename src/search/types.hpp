#pragma once

#include "../chess/moves.hpp"
#include "constants.hpp"
#include <cstdint>

namespace jet {

    namespace types {
        using Value     = int;
        using Depth     = int;
        using Value_i16 = int16_t;
        using Depth_u8  = uint8_t;
        using Time      = double;
        using RawMove   = uint16_t;

    } // namespace types

} // namespace jet