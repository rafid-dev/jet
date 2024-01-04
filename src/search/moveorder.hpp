#pragma once

#include "../chess/board.hpp"
#include "../chess/moves.hpp"
#include "searchstack.hpp"
#include "searchthread.hpp"

namespace jet {

    namespace search {

        class MoveOrdering {
        private:
            using Bitboard  = chess::Bitboard;
            using Color     = chess::Color;
            using PieceType = chess::PieceType;

            static int16_t _mvvlva(PieceType target, PieceType attacker) {
                return static_cast<int16_t>(target) * static_cast<int16_t>(10) - static_cast<int16_t>(attacker);
            }

            static int16_t _updateHistoryScore(int16_t& history, int16_t depth) {
                return history + depth * depth;
            }

        public:
            static constexpr inline int TT_MOVE_SCORE  = 300000;
            static constexpr inline int SEE_SCORE      = 100000;
            static constexpr inline int KILLER_1_SCORE = 80000;
            static constexpr inline int KILLER_2_SCORE = 70000;

            static void updateHistory(SearchThread& st, chess::Movelist& quiets, const chess::Move& move, int depth) {
                st.history.update(st.board(), move, History::bonus(depth));

                for (auto& quiet : quiets) {
                    if (quiet != move) {
                        st.history.update(st.board(), quiet, -History::bonus(depth));
                    }
                }
            }

            static void captures(const chess::Board& board, chess::Movelist& movelist) {
                for (auto& move : movelist) {
                    const auto attacker = board.pieceTypeAt(move.from());
                    const auto target   = board.pieceTypeAt(move.to());

                    if (target != PieceType::NONE) {
                        move.setScore(_mvvlva(target, attacker));
                    }
                }
            }

            static void capturesWithSee(const chess::Board& board, chess::Movelist& movelist) {
                for (auto& move : movelist) {
                    const auto attacker = board.pieceTypeAt(move.from());
                    const auto target   = board.pieceTypeAt(move.to());

                    if (target != PieceType::NONE) {
                        move.setScore(see(board, move, 0) * SEE_SCORE + _mvvlva(target, attacker));
                    }
                }
            }

            static void all(chess::Movelist& movelist, const SearchThread& st, const SearchStack* ss,
                            const chess::Move& ttMove) {
                const auto& board = st.board();
                for (auto& move : movelist) {
                    const auto attacker = board.pieceTypeAt(move.from());
                    const auto target   = board.pieceTypeAt(move.to());

                    if (move == ttMove) {
                        move.setScore(TT_MOVE_SCORE);
                    } else if (target != PieceType::NONE) {
                        move.setScore(see(board, move, 0) * SEE_SCORE + _mvvlva(target, attacker));
                    } else if (move == ss->killers[0]) {
                        move.setScore(KILLER_1_SCORE);
                    } else if (move == ss->killers[1]) {
                        move.setScore(KILLER_2_SCORE);
                    } else {
                        move.setScore(st.history.index(board, move));
                    }
                }
            }

            static bool see(const chess::Board& board, const chess::Move& move, const int threshold) {
                constexpr std::array<int, 7> values = {100, 320, 330, 500, 900, 20000, 0};

                const auto from = move.from();
                const auto to   = move.to();

                const auto target = board.pieceTypeAt(to);

                int score = values[static_cast<int>(target)] - threshold;

                if (score < 0) {
                    return false;
                }

                const auto attacker = board.pieceTypeAt(from);

                score -= values[static_cast<int>(attacker)];

                if (score >= threshold) {
                    return true;
                }

                Bitboard occupied  = board.occupied() ^ Bitboard(from) | Bitboard(to);
                Bitboard attackers = board.attackers(to, occupied) & occupied;

                Bitboard queens  = board.bitboard<PieceType::QUEEN>();
                Bitboard rooks   = board.bitboard<PieceType::ROOK>() | queens;
                Bitboard bishops = board.bitboard<PieceType::BISHOP>() | queens;

                Color st = ~board.colorOf(from);

                while (true) {
                    attackers &= occupied;

                    Bitboard ourAttackers = attackers & board.us(st);

                    if (ourAttackers.empty()) {
                        break;
                    }

                    int pt;
                    for (pt = 0; pt < 6; pt++) {
                        if (ourAttackers & board.bitboard(static_cast<PieceType>(pt))) {
                            break;
                        }
                    }

                    st = ~st;

                    score = -score - 1 - values[pt];

                    if (score >= 0) {
                        if (static_cast<PieceType>(pt) == PieceType::KING && (attackers & board.us(st))) {
                            st = ~st;
                            break;
                        }
                    }

                    PieceType _pt = static_cast<PieceType>(pt);

                    occupied ^= Bitboard((ourAttackers & board.bitboard(_pt)).lsb());

                    if (_pt == PieceType::PAWN || _pt == PieceType::BISHOP || _pt == PieceType::QUEEN) {
                        attackers |= chess::Attacks::bishopAttacks(to, occupied) & bishops;
                    }

                    if (_pt == PieceType::ROOK || _pt == PieceType::QUEEN) {
                        attackers |= chess::Attacks::rookAttacks(to, occupied) & rooks;
                    }
                }

                return st != board.colorOf(from);
            }
        };

    } // namespace search

} // namespace jet
