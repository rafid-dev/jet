#include "search/constants.hpp"

#include "search/moveorder.hpp"
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

        TT::Table TranspositionTable;

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

            if (standing_pat >= beta) {
                return beta;
            }

            if (standing_pat > alpha) {
                alpha = standing_pat;
            }

            Board&   board = st.board();
            Movelist movelist;
            MoveGen::legalmoves<MoveGenType::CAPTURE>(board, movelist);
            MoveOrdering::capturesWithSee(board, movelist);

            Value score = -constants::VALUE_INFINITY;

            for (int i = 0; i < movelist.size(); i++) {
                movelist.nextmove(i);

                const auto& move = movelist[i];

                if (move.score() < MoveOrdering::SEE_SCORE) {
                    break;
                }

                board.makeMove(move);
                st.nodes++;

                score = -qsearch<NodeType::PV>(-beta, -alpha, st);

                board.unmakeMove(move);

                if (st.stop()) {
                    return 0;
                }

                if (score > alpha) {
                    alpha = score;

                    if (score >= beta) {
                        break;
                    }
                }
            }

            return alpha;
        }

        template <NodeType nt>
        Value negamax(Value alpha, Value beta, Depth depth, SearchThread& st, SearchStack* ss) {
            if constexpr (nt == NodeType::PV || nt == NodeType::ROOT) {
                ss->pv_length = ss->ply;
            }

            st.checkup(); // check for time/nodes

            if (depth <= 0) {
                return qsearch<nt>(alpha, beta, st);
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

            constexpr bool isPvNode = (nt == NodeType::PV || nt == NodeType::ROOT);

            bool        ttHit = false;
            const auto& entry = TranspositionTable.probe(board.hash(), ttHit);

            const bool inCheck = board.isCheck();

            if (inCheck) {
                depth++;
                ss->static_eval = 0;
            } else {
                ss->static_eval = evaluation::evaluate(st);
            }

            Value oldAlpha  = alpha;
            Value score     = 0;
            Value bestscore = -constants::VALUE_INFINITY;

            Movelist movelist;
            MoveGen::legalmoves<MoveGenType::ALL>(board, movelist);
            MoveOrdering::all(board, movelist, ss, entry.move());

            Move bestmove  = Move::none();
            int  movecount = 0;

            for (int i = 0; i < movelist.size(); i++) {
                movelist.nextmove(i);

                const auto& move    = movelist[i];
                const bool  isQuiet = board.isQuiet(move);

                board.makeMove(move);
                (ss + 1)->ply = ss->ply + 1;
                ss->move      = move;
                st.nodes++;
                movecount++;

                if constexpr (nt == NodeType::ROOT) {
                    if (movecount == 1 && depth == 1) {
                        ss->updatePV(move, ss + 1);
                    }
                }

                bool do_fullsearch = !isPvNode || movecount > 1;

                if (!inCheck && isQuiet && movecount > 4 && depth >= 3) {
                    Depth reduction = 2;            

                    score = -negamax<NodeType::NONPV>(-alpha - 1, -alpha, depth - reduction, st, ss + 1);

                    do_fullsearch = score > alpha;
                }

                if (do_fullsearch) {
                    score = -negamax<NodeType::NONPV>(-alpha - 1, -alpha, depth - 1, st, ss + 1);
                }

                if constexpr (isPvNode) {
                    if (movecount == 1 || (score > alpha && score < beta)) {
                        score = -negamax<NodeType::PV>(-beta, -alpha, depth - 1, st, ss + 1);
                    }
                }

                board.unmakeMove(move);

                // if we stop, return 0
                if (st.stop()) {
                    return 0;
                }

                if (score > bestscore) {
                    bestscore = score;

                    if (score > alpha) {
                        bestmove = move;
                        alpha    = score;

                        if constexpr (isPvNode) {
                            ss->updatePV(move, ss + 1);
                        }

                        if (score >= beta) {
                            if (isQuiet) {
                                // update killer moves
                                ss->updateKiller(move);
                            }

                            break;
                        }
                    }
                }
            }

            if (!movecount) {
                bestscore = inCheck ? -constants::IS_MATE + ss->ply : 0;
            }

            TT::Flag flag = bestscore >= beta ? TT::Flag::LOWER : (alpha != oldAlpha) ? TT::Flag::EXACT : TT::Flag::UPPER;

            TranspositionTable.store(board.hash(), bestmove, bestscore, depth, flag);

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

        void search(SearchThread& st, Depth depth) {
            st.start();

            SearchStack::List stack;
            SearchStack*      ss = SearchStack::init(stack);

            auto start = misc::tick();

            for (Depth d = 1; d <= depth; d++) {
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
