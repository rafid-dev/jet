#pragma once

#include "tt.hpp"
#include "types.hpp"

namespace jet {
    namespace search {
        namespace search_params {
            extern float lmr_base;
            extern float lmr_division;
            extern int   lmr_see_margin;

            extern int qs_see_ordering_threshold;

            extern int nmp_base;
            extern int nmp_depth_divisor;
            extern int nmp_max_scaled_depth;
            extern int nmp_divisor;

            extern int rfp_margin;
            extern int rfp_depth;

            extern int lmp_depth;
            extern int lmp_base;
            extern int lmp_scalar;

            extern int se_depth;
            extern int se_depth_offset;
            extern int singular_scalar;
            extern int singular_divisor;
            extern int singular_depth_divisor;
            extern int singular_depth_intercept;
            
            extern int asp_delta;
        } // namespace search_params

        extern TT::Table TranspositionTable;

        class SearchThread;
        class SearchInfo;
        struct SearchStack;

        enum class NodeType : uint8_t { ROOT, PV, NONPV };

        void init();
        void search(SearchThread&, SearchInfo&);
        void search(SearchThread&, types::Depth);

        template <NodeType nt>
        types::Value negamax(types::Value a, types::Value b, types::Depth, SearchThread&, SearchStack*);
    } // namespace search
} // namespace jet