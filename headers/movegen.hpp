#pragma once

#include "attacks.hpp"
#include "board.hpp"
#include "move.hpp"

namespace chess {
    namespace movegen {
        enum class MoveGenType : uint8_t { ALL, QUIET, CAPTURE };

        template <Color c>
        inline Bitboard seenSquares(const Board& board, Bitboard enemy_empty) {
            Square kingSq = board.kingSq(~c);

            Bitboard queens  = board.bitboard(c, PieceType::QUEEN);
            Bitboard bishops = board.bitboard(c, PieceType::BISHOP) | queens;
            Bitboard rooks   = board.bitboard(c, PieceType::ROOK) | queens;

            Bitboard knights = board.bitboard(c, PieceType::KNIGHT);
            Bitboard pawns   = board.bitboard(c, PieceType::PAWN);

            Bitboard occ = board.occupied();

            Bitboard mapKingAtk = attacks::king(kingSq) & enemy_empty;

            if (mapKingAtk == 0ull) {
                return 0ull;
            }

            occ &= ~(1ULL << kingSq);

            Bitboard seen = attacks::pawnLeftAttacks<c>(pawns) | attacks::pawnRightAttacks<c>(pawns);

            while (knights) {
                Square sq = poplsb(knights);
                seen |= attacks::knight(sq);
            }

            while (rooks) {
                Square sq = poplsb(rooks);
                seen |= attacks::rook(sq, occ);
            }

            while (bishops) {
                Square sq = poplsb(bishops);
                seen |= attacks::bishop(sq, occ);
            }

            seen |= attacks::king(board.kingSq(c));

            return seen;
        }

        template <Color c, MoveGenType mt>
        void generatePawnMoves(const Board& board, Movelist& moves, Bitboard enemy_occ) {
            Bitboard pawns = board.bitboard(c, PieceType::PAWN);

            constexpr Direction UP         = c == Color::WHITE ? Direction::NORTH : Direction::SOUTH;
            constexpr Direction DOWN       = c == Color::WHITE ? Direction::SOUTH : Direction::NORTH;
            constexpr Direction DOWN_LEFT  = c == Color::WHITE ? Direction::SOUTH_WEST : Direction::NORTH_EAST;
            constexpr Direction DOWN_RIGHT = c == Color::WHITE ? Direction::SOUTH_EAST : Direction::NORTH_WEST;

            constexpr Bitboard RANK_7 = c == Color::WHITE ? attacks::MASK_RANK[int(Rank::RANK_7)] : attacks::MASK_RANK[int(Rank::RANK_2)];

            constexpr Bitboard RANK_8 = c == Color::WHITE ? attacks::MASK_RANK[int(Rank::RANK_8)] : attacks::MASK_RANK[int(Rank::RANK_1)];

            constexpr Bitboard RANK_3 = c == Color::WHITE ? attacks::MASK_RANK[int(Rank::RANK_3)] : attacks::MASK_RANK[int(Rank::RANK_6)];

            Bitboard left_attacks  = attacks::pawnLeftAttacks<c>(pawns) & enemy_occ;
            Bitboard right_attacks = attacks::pawnRightAttacks<c>(pawns) & enemy_occ;
            Bitboard single_pushes = attacks::shift<UP>(pawns) & ~board.occupied();

            // promotion moves
            if (pawns & RANK_7) {
                Bitboard promotions       = single_pushes & RANK_8;
                Bitboard promotions_left  = left_attacks & RANK_8;
                Bitboard promotions_right = right_attacks & RANK_8;

                while (mt != MoveGenType::QUIET && promotions) {
                    Square to = poplsb(promotions);
                    moves.add(Move::make<Move::PROMOTION>(static_cast<Square>(to + DOWN), to, PieceType::QUEEN));
                    moves.add(Move::make<Move::PROMOTION>(static_cast<Square>(to + DOWN), to, PieceType::ROOK));
                    moves.add(Move::make<Move::PROMOTION>(static_cast<Square>(to + DOWN), to, PieceType::BISHOP));
                    moves.add(Move::make<Move::PROMOTION>(static_cast<Square>(to + DOWN), to, PieceType::KNIGHT));
                }

                while (mt != MoveGenType::CAPTURE && promotions_right) {
                    Square to = poplsb(promotions_right);
                    moves.add(Move::make<Move::PROMOTION>(static_cast<Square>(to + DOWN_LEFT), to, PieceType::QUEEN));
                    moves.add(Move::make<Move::PROMOTION>(static_cast<Square>(to + DOWN_LEFT), to, PieceType::ROOK));
                    moves.add(Move::make<Move::PROMOTION>(static_cast<Square>(to + DOWN_LEFT), to, PieceType::BISHOP));
                    moves.add(Move::make<Move::PROMOTION>(static_cast<Square>(to + DOWN_LEFT), to, PieceType::KNIGHT));
                }

                while (mt != MoveGenType::CAPTURE && promotions_left) {
                    Square to = poplsb(promotions_left);
                    moves.add(Move::make<Move::PROMOTION>(static_cast<Square>(to + DOWN_RIGHT), to, PieceType::QUEEN));
                    moves.add(Move::make<Move::PROMOTION>(static_cast<Square>(to + DOWN_RIGHT), to, PieceType::ROOK));
                    moves.add(Move::make<Move::PROMOTION>(static_cast<Square>(to + DOWN_RIGHT), to, PieceType::BISHOP));
                    moves.add(Move::make<Move::PROMOTION>(static_cast<Square>(to + DOWN_RIGHT), to, PieceType::KNIGHT));
                }
            }

            // rule out to-promoted moves
            single_pushes &= ~RANK_8;
            left_attacks &= ~RANK_8;
            right_attacks &= ~RANK_8;

            if constexpr (mt != MoveGenType::CAPTURE) {
                Bitboard double_pushes = attacks::shift<UP>(single_pushes & RANK_3) & ~board.occupied();

                // single push
                while (single_pushes) {
                    Square to = poplsb(single_pushes);
                    moves.add(Move::make<Move::NORMAL>(static_cast<Square>(to + DOWN), to));
                }

                // double push

                while (double_pushes) {
                    Square to = poplsb(double_pushes);
                    moves.add(Move::make<Move::NORMAL>(static_cast<Square>(to + DOWN + DOWN), to));
                }
            }

            if constexpr (mt != MoveGenType::QUIET) {
                // left attacks
                while (left_attacks) {
                    Square to = poplsb(left_attacks);

                    moves.add(Move::make<Move::NORMAL>(static_cast<Square>(to + DOWN_RIGHT), to));
                }

                // right attacks
                while (right_attacks) {
                    Square to = poplsb(right_attacks);
                    moves.add(Move::make<Move::NORMAL>(static_cast<Square>(to + DOWN_LEFT), to));
                }

                // en passant
                Square ep = board.enPassant();
                if (ep != Square(Square::NO_SQ)) {
                    Bitboard ep_bb = attacks::pawn(ep, ~c) & pawns;

                    while (ep_bb) {
                        Square from = poplsb(ep_bb);
                        moves.add(Move::make<Move::ENPASSANT>(from, ep));
                    }
                }
            }
        }

        template <typename T>
        inline void generateAndAddMoves(Movelist& movelist, Bitboard mask, T func) {
            while (mask) {
                const Square from  = poplsb(mask);
                auto         moves = func(from);
                while (moves) {
                    const Square to = poplsb(moves);
                    movelist.add(Move::make(from, to));
                }
            }
        }

        template <Color c, MoveGenType mt>
        inline Bitboard generateCastlingMoves(const Board& board, Square kingSq, Bitboard seen) {
            if constexpr (mt == MoveGenType::CAPTURE) {
                return 0ULL;
            }
            if (seen & (1ULL << kingSq)) {
                return 0ULL;
            }

            constexpr Rank RANK_1 = c == Color::WHITE ? Rank::RANK_1 : Rank::RANK_8;

            Bitboard moves = 0ULL;

            if (kingSq.rank() == RANK_1) {
                for (const CastlingSide castleSide : {CastlingSide::KING_SIDE, CastlingSide::QUEEN_SIDE}) {
                    if (!board.hasCastlingRights(c, castleSide)) {
                        continue;
                    }

                    const Square kingToSq = relativeSquare(castleSide == CastlingSide::KING_SIDE ? Square::SQ_G1 : Square::SQ_C1, c);

                    const Square rookToSq = relativeSquare(castleSide == CastlingSide::KING_SIDE ? Square::SQ_F1 : Square::SQ_D1, c);

                    const Square rookFromSq = relativeSquare(castleSide == CastlingSide::KING_SIDE ? Square::SQ_H1 : Square::SQ_A1, c);

                    const Bitboard NOT_OCCUPIED_PATH  = attacks::SQUARES_BETWEEN_BB[int(kingSq)][int(rookFromSq)];
                    const Bitboard NOT_ATTACKED_PATH  = attacks::SQUARES_BETWEEN_BB[int(kingSq)][int(kingToSq)];
                    const Bitboard EMPTY_NOT_ATTACKED = ~seen & ~(board.occupied() & ~(1ull << rookFromSq));
                    const Bitboard withoutRook        = board.occupied() & ~(1ull << rookFromSq);
                    const Bitboard withoutKing        = board.occupied() & ~(1ull << kingSq);

                    if ((NOT_ATTACKED_PATH & EMPTY_NOT_ATTACKED) == NOT_ATTACKED_PATH && ((NOT_OCCUPIED_PATH & ~board.occupied()) == NOT_OCCUPIED_PATH) && !((1ull << rookToSq) & (withoutRook & withoutKing)) &&
                        !((1ull << kingToSq) & (seen | (withoutRook & ~(1ull << kingSq))))) {
                        moves |= (1ull << rookFromSq);
                    }
                }
            }

            return moves;
        }

        // generate moves for knights and king
        template <Color c, MoveGenType mt>
        void generateMoves(const Board& board, Movelist& moves) {
            Bitboard us          = board.us(c);
            Bitboard them        = board.them(c);
            Bitboard all         = us | them;
            Bitboard enemy_empty = ~us;

            Bitboard moveableSquare;

            if constexpr (mt == MoveGenType::ALL) {
                moveableSquare = ~us;
            } else if constexpr (mt == MoveGenType::CAPTURE) {
                moveableSquare = them;
            } else if constexpr (mt == MoveGenType::QUIET) {
                moveableSquare = ~all;
            }

            Bitboard seen   = seenSquares<~c>(board, enemy_empty);
            Square   kingSq = board.kingSq(c);

            // clang-format off

            // Generate king moves and castling moves
            generateAndAddMoves(moves, makeBitboard(kingSq), [&](Square from) {
                 return attacks::king(from) & moveableSquare & ~seen; 
            });

            if (board.hasCastlingRights(c)) {
                Bitboard castlingMoves = generateCastlingMoves<c, mt>(board, kingSq, seen);

                while (castlingMoves) {
                    Square to = poplsb(castlingMoves);
                    moves.add(Move::make<Move::CASTLING>(kingSq, to));
                }
            }

            // Generate knight moves
            Bitboard knights = board.bitboard(c, PieceType::KNIGHT);
            generateAndAddMoves(moves, knights, [&](Square from) { 
                return attacks::knight(from) & moveableSquare; 
            });

            // Generate pawn moves
            generatePawnMoves<c, mt>(board, moves, them);

            // Generate bishop moves
            Bitboard bishops = board.bitboard(c, PieceType::BISHOP);
            generateAndAddMoves(moves, bishops, [&](Square from) {
                return attacks::bishop(from, all) & moveableSquare;
            });

            // Generate rook moves
            Bitboard rooks = board.bitboard(c, PieceType::ROOK);
            generateAndAddMoves(moves, rooks, [&](Square from) {
                return attacks::rook(from, all) & moveableSquare;
            });

            // Generate queen moves
            Bitboard queens = board.bitboard(c, PieceType::QUEEN);
            generateAndAddMoves(moves, queens, [&](Square from) {
                return attacks::queen(from, all) & moveableSquare;
            });

            // clang-format on
        }

        template <MoveGenType mt>
        void generate(const Board& board, Movelist& moves) {
            if (board.sideToMove() == Color::WHITE) {
                generateMoves<Color::WHITE, mt>(board, moves);
            } else {
                generateMoves<Color::BLACK, mt>(board, moves);
            }
        }

    } // namespace movegen
} // namespace chess