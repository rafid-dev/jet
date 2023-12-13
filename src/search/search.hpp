#pragma once

#include "types.hpp"

namespace jet {
    namespace search {
        class SearchThread;
        class SearchInfo;
        struct SearchStack;

        void         search(SearchThread&, SearchInfo&);
        types::Value negamax(types::Value a, types::Value b, types::Depth, SearchThread&, SearchStack*);
    } // namespace search
} // namespace jet