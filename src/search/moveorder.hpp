#pragma once

#include "../chess/board.hpp"
#include "../chess/moves.hpp"
#include "searchstack.hpp"

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

        public:
            static constexpr inline int16_t SEE_SCORE = 30000;

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

            static void all(const chess::Board& board, chess::Movelist& movelist, SearchStack* ss) {
                for (auto& move : movelist) {
                    const auto attacker = board.pieceTypeAt(move.from());
                    const auto target   = board.pieceTypeAt(move.to());

                    if (target != PieceType::NONE) {
                        move.setScore(_mvvlva(target, attacker));
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
