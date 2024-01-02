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

        std::array<std::array<Depth, 64>, 64> LmrTable;

        void init() {
            for (int depth = 1; depth < 64; depth++) {
                for (int played = 1; played < 64; played++) {
                    LmrTable[depth][played] = 0.75 + std::log(depth) * std::log(played) / 2.5;
                }
            }
        }

        template Value negamax<NodeType::ROOT>(Value, Value, Depth, SearchThread&, SearchStack*);
        template Value negamax<NodeType::PV>(Value, Value, Depth, SearchThread&, SearchStack*);
        template Value negamax<NodeType::NONPV>(Value, Value, Depth, SearchThread&, SearchStack*);

        template <NodeType nt>
        Value qsearch(Value alpha, Value beta, SearchThread& st) {
            st.checkup();

            Value bestscore = evaluation::evaluate(st);

            if (bestscore >= beta) {
                return bestscore;
            }

            if (bestscore > alpha) {
                alpha = bestscore;
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

                st.makeMove<true>(move);
                st.nodes++;

                score = -qsearch<NodeType::PV>(-beta, -alpha, st);

                st.unmakeMove<true>(move);

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

            if constexpr (!isPvNode) {
                if (ttHit && entry.depth() >= depth) {
                    if ((entry.flag() == TT::Flag::UPPER && entry.score() <= alpha) ||
                        (entry.flag() == TT::Flag::LOWER && entry.score() >= beta) || (entry.flag() == TT::Flag::EXACT)) {
                        return entry.score();
                    }
                }
            }

            const bool inCheck   = board.isCheck();
            const bool improving = !inCheck && ss->static_eval > (ss - 2)->static_eval;

            if (inCheck) {
                depth++;
                ss->static_eval = 0;
            } else {
                ss->static_eval = evaluation::evaluate(st);
            }

            Value eval = ss->static_eval;

            if constexpr (!isPvNode) {
                if (ttHit) {
                    eval = entry.score();
                }

                if (!inCheck && depth < 9 && ss->static_eval - depth * 77 >= beta) {
                    return ss->static_eval;
                }

                if (!inCheck && depth >= 3 && (ss - 1)->move != Move::nullmove() && board.hasNonPawnMat() &&
                    ss->static_eval >= beta && (!ttHit || entry.flag() != TT::Flag::UPPER || eval >= beta)) {
                    Depth reduction = 3 + depth / 3 + std::min(3, (eval - beta) / 180);

                    board.makeNullMove();
                    ss->move      = Move::nullmove();
                    (ss + 1)->ply = ss->ply + 1;

                    Value score = -negamax<NodeType::NONPV>(-beta, -beta + 1, depth - reduction, st, ss + 1);

                    board.unmakeNullMove();

                    if (st.stop()) {
                        return 0;
                    }

                    if (score >= beta) {
                        if (score > constants::IS_MATE) {
                            score = beta;
                        }

                        return score;
                    }
                }
            }

            Value oldAlpha  = alpha;
            Value score     = 0;
            Value bestscore = -constants::VALUE_INFINITY;

            Movelist movelist;
            MoveGen::legalmoves<MoveGenType::ALL>(board, movelist);
            MoveOrdering::all(movelist, st, ss, entry.move());

            Move bestmove  = Move::none();
            int  movecount = 0;

            for (int i = 0; i < movelist.size(); i++) {
                movelist.nextmove(i);

                const auto& move    = movelist[i];
                const bool  isQuiet = board.isQuiet(move);

                st.makeMove<true>(move);
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

                if (!inCheck && isQuiet && movecount > 2 && depth >= 3) {
                    Depth reduction = LmrTable[std::min(63, depth)][std::min(63, movecount)];

                    reduction += !isPvNode;
                    reduction += !improving;

                    reduction = std::clamp(reduction, 0, depth - 1);

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

                st.unmakeMove<true>(move);

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
                                MoveOrdering::updateHistory(st, move, depth);
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

            TranspositionTable.store(board.hash(), bestmove, depth, bestscore, flag);

            return bestscore;
        }

        Value aspiration_window(SearchThread& st, SearchStack* ss, Depth depth, Value prevEval) {
            Value score = 0;

            Value delta = 10;

            Value alpha = -constants::VALUE_INFINITY;
            Value beta  = constants::VALUE_INFINITY;

            Depth initial_depth = depth;

            if (depth > 3) {
                alpha = std::max(prevEval - delta, -constants::VALUE_INFINITY);
                beta  = std::min(prevEval + delta, constants::VALUE_INFINITY);
            }

            while (true) {
                score = negamax<NodeType::ROOT>(alpha, beta, depth, st, ss);

                if (st.stop()) {
                    break;
                }

                if (score <= alpha) {
                    beta  = (alpha + beta) / 2;
                    alpha = std::max(score - delta, -constants::VALUE_INFINITY);

                    depth = initial_depth;
                } else if (score >= beta) {
                    beta = std::min(score + delta, constants::VALUE_INFINITY);

                    if (std::abs(beta) <= constants::IS_MATE / 2 && depth > 1) {
                        depth--;
                    }

                    ss->updatePV(ss->pv[0], ss + 1);

                } else {
                    break;
                }

                delta += delta / 2;
            }

            return score;
        }

        void search(SearchThread& st, SearchInfo& info) {
            st.start();

            SearchStack::List stack;
            SearchStack*      ss = SearchStack::init(stack);

            auto start = misc::tick();

            Value score = 0;

            for (Depth d = 1; d <= info.depth(); d++) {
                if (st.stop()) {
                    break;
                }

                score = aspiration_window(st, ss, d, score);

                if (info.shouldPrintInfo()) {
                    auto time_elapsed = misc::tick() - start;

                    std::cout << "info depth " << d << " score cp " << score;
                    std::cout << " time " << time_elapsed;
                    std::cout << " nodes " << st.nodes;
                    std::cout << " nps " << static_cast<int>(1000.0f * st.nodes / (time_elapsed + 1));
                    std::cout << " pv";
                    SearchStack::printPVs(ss);

                    std::cout << std::endl;
                }
            }

            if (info.shouldPrintInfo()) {
                std::cout << "bestmove " << ss->pv[0] << std::endl;
            }
        }

        void search(SearchThread& st, Depth depth) {
            st.start();

            SearchInfo info;
            info.setDepth(depth);
            info.setPrintInfo(false);

            search(st, info);
        }

    } // namespace search

} // namespace jet
