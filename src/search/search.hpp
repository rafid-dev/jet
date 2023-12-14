#pragma once

#include "types.hpp"

namespace jet {
    namespace search {
        class SearchThread;
        class SearchInfo;
        struct SearchStack;

        enum class NodeType : uint8_t { ROOT, PV, NONPV };

        void search(SearchThread&, SearchInfo&);

        template <NodeType nt>
        types::Value negamax(types::Value a, types::Value b, types::Depth, SearchThread&, SearchStack*);
    } // namespace search
} // namespace jet