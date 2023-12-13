#pragma once

#include "../search/searchthread.hpp"
#include "../search/types.hpp"

namespace jet {
    namespace evaluation {
        types::Value evaluate(chess::Board&);
        types::Value evaluate(search::SearchThread&);
    } // namespace evaluation
} // namespace jet