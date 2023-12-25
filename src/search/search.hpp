#pragma once

#include "tt.hpp"
#include "types.hpp"

namespace jet {
    namespace search {
        extern TT::Table TranspositionTable;

        class SearchThread;
        class SearchInfo;
        struct SearchStack;

        enum class NodeType : uint8_t { ROOT, PV, NONPV };

        void search(SearchThread&, SearchInfo&);
        void search(SearchThread&, types::Depth);

        template <NodeType nt>
        types::Value negamax(types::Value a, types::Value b, types::Depth, SearchThread&, SearchStack*);
    } // namespace search
} // namespace jet