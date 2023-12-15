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

        template Value negamax<NodeType::ROOT>(Value, Value, Depth, SearchThread&, SearchStack*);
        template Value negamax<NodeType::PV>(Value, Value, Depth, SearchThread&, SearchStack*);
        template Value negamax<NodeType::NONPV>(Value, Value, Depth, SearchThread&, SearchStack*);

        template <NodeType nt>
        Value qsearch(Value alpha, Value beta, SearchThread& st) {
            st.checkup();

            if (st.board().isRepetition()) {
                return 0;
            }

            Value standing_pat = evaluation::evaluate(st);
            Value bestscore    = standing_pat;
            Value score        = -constants::VALUE_INFINITY;

            if (standing_pat >= beta) {
                return beta;
            }

            if (standing_pat > alpha) {
                alpha = standing_pat;
            }

            Board&   board = st.board();
            Movelist movelist;
            MoveGen::legalmoves<MoveGenType::CAPTURE>(board, movelist);

            for (const auto& move : movelist) {
                board.makeMove(move);
                st.nodes++;

                score = -qsearch<NodeType::NONPV>(-beta, -alpha, st);

                board.unmakeMove(move);

                if (st.stop()) {
                    return 0;
                }

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

            return bestscore;
        }

        template <NodeType nt>
        Value negamax(Value alpha, Value beta, Depth depth, SearchThread& st, SearchStack* ss) {
            if constexpr (nt == NodeType::PV || nt == NodeType::ROOT) {
                ss->pvLength = ss->ply;
            }

            st.checkup(); // check for time/nodes

            if (depth == 0) {
                return evaluation::evaluate(st);
            }

            Board& board = st.board();
            if constexpr (nt != NodeType::ROOT) {
                if (board.isRepetition()) {
                    return 0;
                }

                if (ss->ply >= constants::PLY_MAX) {
                    return evaluation::evaluate(st);
                }
            }

            Value score     = 0;
            Value bestscore = -constants::VALUE_INFINITY;

            Movelist movelist;
            MoveGen::legalmoves<MoveGenType::ALL>(board, movelist);

            Move bestmove = Move::none();

            int movecount = 0;

            for (const auto& move : movelist) {
                board.makeMove(move);
                (ss + 1)->ply = ss->ply + 1;
                st.nodes++;
                movecount++;

                score = -negamax<NodeType::PV>(-beta, -alpha, depth - 1, st, ss + 1);

                board.unmakeMove(move);

                // if we stop, return 0
                if (st.stop()) {
                    return 0;
                }

                // if constexpr (nt == NodeType::ROOT) {
                //     if (movecount == 1) {
                //         ss->pv[0] = move;
                //     }
                // }

                if (score > bestscore) {
                    bestscore = score;

                    if (score > alpha) {
                        bestmove = move;

                        ss->updatePV(bestmove, ss + 1);

                        if (score >= beta) {
                            break;
                        }
                    }
                }
            }

            if (!movecount) {
                bestscore = board.isCheck() ? -constants::IS_MATE + ss->ply : 0;
            }

            return bestscore;
        }

        void search(SearchThread& st, SearchInfo& info) {
            st.start();

            SearchStack::List stack;
            SearchStack*      ss = SearchStack::init(stack);

            auto start = misc::tick();

            for (Depth d = 1; d <= info.depth(); d++) {
                if (st.stop()) {
                    break;
                }

                Value score = negamax<NodeType::ROOT>(-constants::VALUE_INFINITY, constants::VALUE_INFINITY, d, st, ss);

                auto time_elapsed = misc::tick() - start;

                std::cout << "info depth " << d << " score cp " << score;
                std::cout << " time " << time_elapsed;
                std::cout << " nodes " << st.nodes;
                std::cout << " nps " << static_cast<int>(1000.0f * st.nodes / (time_elapsed + 1));
                std::cout << " pv";
                SearchStack::printPVs(ss);

                std::cout << std::endl;
            }

            std::cout << "bestmove " << ss->pv[0] << std::endl;
        }

    } // namespace search

} // namespace jet
