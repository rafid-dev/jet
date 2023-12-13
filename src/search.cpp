#include "search/constants.hpp"

#include "search/search.hpp"
#include "search/searchinfo.hpp"
#include "search/searchstack.hpp"
#include "search/searchthread.hpp"

#include "evaluation/evaluate.hpp"

#include "chess/movegen.hpp"

using namespace chess;
using jet::types::Depth;
using jet::types::Value;

namespace jet {
    namespace search {
        Value negamax(Value alpha, Value beta, Depth depth, SearchThread& st, SearchStack* ss) {
            if (depth == 0) {
                return evaluation::evaluate(st);
            }

            Value score     = 0;
            Value bestscore = -constants::VALUE_INFINITY;

            Board&   board = st.board();
            Movelist movelist;
            MoveGen::legalmoves<MoveGenType::ALL>(board, movelist);

            for (const auto& move : movelist) {
                board.makeMove(move);

                (ss + 1)->ply = ss->ply + 1;
                st.nodes++;

                score = -negamax(-beta, -alpha, depth - 1, st, ss + 1);
                board.unmakeMove(move);

                if (score > bestscore) {
                    bestscore = score;

                    if (score > alpha) {
                        alpha = score;

                        if (score >= beta) {
                            break;
                        }
                    }
                }
            }

            if (movelist.empty()) {
                bestscore = board.isCheck() ? -constants::IS_MATE + ss->ply : 0;
            }

            return bestscore;
        }

        void search(SearchThread& st, SearchInfo& info) {
            SearchStack stack[constants::MAX_PLY + 10], *ss = stack + 7;

            auto start = misc::tick();

            for (Depth d = 1; d <= info.depth(); d++) {
                Value score = negamax(-constants::VALUE_INFINITY, constants::VALUE_INFINITY, d, st, ss);

                auto time_elapsed = misc::tick() - start;

                std::cout << "info depth " << d << " score cp " << score;
                std::cout << " time " << time_elapsed;
                std::cout << " nodes " << st.nodes;
                std::cout << " nps " << static_cast<int>(1000.0f * st.nodes / (time_elapsed + 1));
                std::cout << std::endl;
            }
        }

    } // namespace search

} // namespace jet
