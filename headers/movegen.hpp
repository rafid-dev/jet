#pragma once

#include "attacks.hpp"
#include "board.hpp"
#include "move.hpp"

namespace chess {
    namespace movegen {
        enum class MoveGenType : uint8_t { ALL, QUIET, CAPTURE };

        template <Color c>
        inline Bitboard checkMask(const Board& board, Square sq, int& double_check) {
            Bitboard mask = 0ULL;
            double_check  = 0;

            const Bitboard oppKnight = board.bitboard(~c, PieceType::KNIGHT);
            const Bitboard oppBishop = board.bitboard(~c, PieceType::BISHOP);
            const Bitboard oppRook   = board.bitboard(~c, PieceType::ROOK);
            const Bitboard oppQueen  = board.bitboard(~c, PieceType::QUEEN);
            const Bitboard oppPawns  = board.bitboard(~c, PieceType::PAWN);

            Bitboard knightAttacks = attacks::knight(sq) & oppKnight;

            double_check += bool(knightAttacks);
            mask |= knightAttacks;

            Bitboard pawnAttacks = attacks::pawn(sq, c) & oppPawns;

            double_check += bool(pawnAttacks);
            mask |= pawnAttacks;

            Bitboard bishopAttacks = attacks::bishop(sq, board.occupied()) & (oppBishop | oppQueen);

            if (bishopAttacks) {
                const auto index = poplsb(bishopAttacks);

                mask |= attacks::SQUARES_BETWEEN_BB[sq][index] | (1ULL << index);
                double_check++;
            }

            Bitboard rookAttacks = attacks::rook(sq, board.occupied()) & (oppRook | oppQueen);

            if (rookAttacks) {
                const auto index = poplsb(rookAttacks);

                mask |= attacks::SQUARES_BETWEEN_BB[sq][index] | (1ULL << index);
                double_check++;
            }

            if (!mask) {
                return DEFAULT_CHECKMASK;
            }

            return mask;
        }

        template <Color c>
        inline Bitboard pinMaskRooks(const Board& board, Square sq, Bitboard occ_enemy, Bitboard occ_us) {
            Bitboard pin_hv = 0;

            const Bitboard oppRook  = board.bitboard<~c, PieceType::ROOK>();
            const Bitboard oppQueen = board.bitboard<~c, PieceType::QUEEN>();

            Bitboard rookAttacks = attacks::rook(sq, occ_enemy) & (oppRook | oppQueen);

            while (rookAttacks) {
                const auto index = poplsb(rookAttacks);

                const Bitboard possible_pin = squaresBetween(sq, index) | index.bb();

                if (popcount(possible_pin & occ_us) == 1) {
                    pin_hv |= possible_pin;
                }
            }

            return pin_hv;
        }

        template <Color c>
        inline Bitboard pinMaskBishops(const Board& board, Square sq, Bitboard occ_enemy, Bitboard occ_us) {
            Bitboard pin_d = 0;

            const Bitboard oppBishop = board.bitboard<~c, PieceType::BISHOP>();
            const Bitboard oppQueen  = board.bitboard<~c, PieceType::QUEEN>();

            Bitboard bishopAttacks = attacks::bishop(sq, occ_enemy) & (oppBishop | oppQueen);

            while (bishopAttacks) {
                const auto index = poplsb(bishopAttacks);

                const Bitboard possible_pin = squaresBetween(sq, index) | index.bb();

                if (popcount(possible_pin & occ_us) == 1) {
                    pin_d |= possible_pin;
                }
            }

            return pin_d;
        }

        template <Color c>
        inline Bitboard seenSquares(const Board& board, Bitboard enemy_empty, Bitboard occ) {
            Square theirKingSq = board.kingSq<~c>();

            Bitboard mapKingAtk = attacks::king(theirKingSq) & enemy_empty;

            if (mapKingAtk == 0ull) {
                return 0ull;
            }

            occ &= ~theirKingSq.bb();

            Bitboard queens  = board.bitboard<c, PieceType::QUEEN>();
            Bitboard bishops = board.bitboard<c, PieceType::BISHOP>() | queens;
            Bitboard rooks   = board.bitboard<c, PieceType::ROOK>() | queens;

            Bitboard knights = board.bitboard<c, PieceType::KNIGHT>();
            Bitboard pawns   = board.bitboard<c, PieceType::PAWN>();

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

            seen |= attacks::king(board.kingSq<c>());

            return seen;
        }

        inline void addPromotionMoves(Square from, Square to, Movelist& moves) {
            moves.add(Move::make<Move::PROMOTION>(from, to, PieceType::QUEEN));
            moves.add(Move::make<Move::PROMOTION>(from, to, PieceType::ROOK));
            moves.add(Move::make<Move::PROMOTION>(from, to, PieceType::BISHOP));
            moves.add(Move::make<Move::PROMOTION>(from, to, PieceType::KNIGHT));
        }

        template <Color c, MoveGenType mt>
        inline void generatePromotionMoves(const Bitboard pawns, const Bitboard single_push, const Bitboard left_attacks, const Bitboard right_attacks, Movelist& moves) {
            constexpr Bitboard RANK_7 = toBitboard<relativeRank<c, Rank::RANK_7>()>();
            constexpr Bitboard RANK_8 = toBitboard<relativeRank<c, Rank::RANK_8>()>();

            constexpr Direction DOWN       = relativeDirection<c, Direction::SOUTH>();
            constexpr Direction DOWN_LEFT  = relativeDirection<c, Direction::SOUTH_WEST>();
            constexpr Direction DOWN_RIGHT = relativeDirection<c, Direction::SOUTH_EAST>();

            if (pawns & RANK_7) {
                Bitboard promotions       = single_push & RANK_8;
                Bitboard promotions_left  = left_attacks & RANK_8;
                Bitboard promotions_right = right_attacks & RANK_8;

                while (mt != MoveGenType::QUIET && promotions) {
                    Square to = poplsb(promotions);
                    addPromotionMoves(to + DOWN, to, moves);
                }

                while (mt != MoveGenType::CAPTURE && promotions_right) {
                    Square to = poplsb(promotions_right);
                    addPromotionMoves(to + DOWN_LEFT, to, moves);
                }

                while (mt != MoveGenType::CAPTURE && promotions_left) {
                    Square to = poplsb(promotions_left);
                    addPromotionMoves(to + DOWN_RIGHT, to, moves);
                }
            }
        }

        template <Color c, MoveGenType mt, bool Legal = false>
        void generatePawnMoves(const Board& board, Movelist& moves, Bitboard them, Bitboard occAll, Bitboard pinD = 0ULL, Bitboard pinHV = 0ULL, Bitboard checkMask = 0ULL) {
            Bitboard pawns = board.bitboard(c, PieceType::PAWN);

            constexpr Direction UP         = relativeDirection<c, Direction::NORTH>();
            constexpr Direction DOWN       = relativeDirection<c, Direction::SOUTH>();
            constexpr Direction DOWN_LEFT  = relativeDirection<c, Direction::SOUTH_WEST>();
            constexpr Direction DOWN_RIGHT = relativeDirection<c, Direction::SOUTH_EAST>();

            constexpr Bitboard RANK_8 = toBitboard<relativeRank<c, Rank::RANK_8>()>();
            constexpr Bitboard RANK_3 = toBitboard<relativeRank<c, Rank::RANK_3>()>();

            if constexpr (Legal) {
                const Bitboard pawns_lr = pawns & ~pinHV;

                const Bitboard unpinnedpawns_lr = pawns_lr & ~pinD;
                const Bitboard pinnedpawns_lr   = pawns_lr & pinD;

                // clang-format off
                Bitboard left_attacks = (attacks::pawnLeftAttacks<c>(unpinnedpawns_lr)) | 
                                         (attacks::pawnLeftAttacks<c>(pinnedpawns_lr) & pinD);
                Bitboard right_attacks = (attacks::pawnRightAttacks<c>(unpinnedpawns_lr)) | 
                                        (attacks::pawnRightAttacks<c>(pinnedpawns_lr) & pinD);

                left_attacks &= them & checkMask;
                right_attacks &= them & checkMask;
                
                const Bitboard pawns_hv = pawns & ~pinD;

                const Bitboard unpinnedpawns_hv = pawns_hv & ~pinHV;
                const Bitboard pinnedpawns_hv   = pawns_hv & pinHV;

                const Bitboard single_push_unpinned = attacks::shift<UP>(unpinnedpawns_hv) & ~occAll;
                const Bitboard single_push_pinned   = attacks::shift<UP>(pinnedpawns_hv) & pinHV & ~occAll;

                Bitboard single_push = (single_push_unpinned | single_push_pinned) & checkMask;

                // clang-format on
                generatePromotionMoves<c, mt>(pawns, single_push, left_attacks, right_attacks, moves);

                // rule out to-promoted moves
                single_push &= ~RANK_8;
                left_attacks &= ~RANK_8;
                right_attacks &= ~RANK_8;

                if constexpr (mt != MoveGenType::CAPTURE) {
                    Bitboard double_push = ((attacks::shift<UP>(single_push_unpinned & RANK_3) & ~occAll) | (attacks::shift<UP>(single_push_pinned & RANK_3) & ~occAll)) & checkMask;

                    // single push
                    while (single_push) {
                        Square to = poplsb(single_push);
                        moves.add(Move::make<Move::NORMAL>(static_cast<Square>(to + DOWN), to));
                    }

                    // double push
                    while (double_push) {
                        Square to = poplsb(double_push);
                        moves.add(Move::make<Move::NORMAL>(static_cast<Square>(to + DOWN + DOWN), to));
                    }
                }

                if constexpr (mt != MoveGenType::QUIET) {
                    while (left_attacks) {
                        Square to = poplsb(left_attacks);
                        moves.add(Move::make<Move::NORMAL>(static_cast<Square>(to + DOWN_RIGHT), to));
                    }

                    while (right_attacks) {
                        Square to = poplsb(right_attacks);
                        moves.add(Move::make<Move::NORMAL>(static_cast<Square>(to + DOWN_LEFT), to));
                    }

                    // en passant
                    Square ep = board.enPassant();
                    if (ep != Square(Square::NO_SQ)) {
                        const Square   ep_pawn  = ep + DOWN;
                        const Bitboard epSqBB   = ep.bb();
                        const Bitboard epPawnBB = ep_pawn.bb();

                        const Bitboard ep_mask = epPawnBB | epSqBB;

                        if ((checkMask & ep_mask) == 0)
                            return;

                        const Square   kingSq         = board.kingSq<c>();
                        const Bitboard kingMask       = kingSq.bb() & toBitboard(ep_pawn.rank());
                        const Bitboard enemyQueenRook = board.bitboard<~c, PieceType::QUEEN>() | board.bitboard<~c, PieceType::ROOK>();

                        const bool possiblePin = kingMask && enemyQueenRook;
                        Bitboard   ep_bb       = attacks::pawn(ep, ~c) & pawns_lr;

                        while (ep_bb) {
                            Square from = poplsb(ep_bb);
                            Square to   = ep;

                            if ((1ULL << from) & pinD && !(pinD & epSqBB)) {
                                continue;
                            }

                            const Bitboard connectingPawns = epPawnBB | (1ULL << from);

                            if (possiblePin && (attacks::rook(kingSq, occAll & ~connectingPawns) & enemyQueenRook) != 0)
                                break;

                            moves.add(Move::make<Move::ENPASSANT>(from, to));
                        }
                    }
                }

            } else {
                Bitboard left_attacks  = attacks::pawnLeftAttacks<c>(pawns) & them;
                Bitboard right_attacks = attacks::pawnRightAttacks<c>(pawns) & them;
                Bitboard single_pushes = attacks::shift<UP>(pawns) & ~occAll;

                generatePromotionMoves<c, mt>(pawns, single_pushes, left_attacks, right_attacks, moves);

                // rule out to-promoted moves
                single_pushes &= ~RANK_8;
                left_attacks &= ~RANK_8;
                right_attacks &= ~RANK_8;

                if constexpr (mt != MoveGenType::CAPTURE) {
                    Bitboard double_pushes = attacks::shift<UP>(single_pushes & RANK_3) & ~occAll;

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
        }

        inline Bitboard generateBishopMoves(Square sq, Bitboard pinD, Bitboard occ) {
            if (pinD & sq.bb())
                return attacks::bishop(sq, occ) & pinD;

            return attacks::bishop(sq, occ);
        }

        inline Bitboard generateRookMoves(Square sq, Bitboard pinHV, Bitboard occ) {
            if (pinHV & sq.bb())
                return attacks::rook(sq, occ) & pinHV;

            return attacks::rook(sq, occ);
        }

        inline Bitboard generateQueenMoves(Square sq, Bitboard pinHV, Bitboard pinD, Bitboard occ) {
            Bitboard moves = 0ull;

            if (pinD & sq.bb()) {
                moves |= attacks::bishop(sq, occ) & pinD;
            } else if (pinHV & sq.bb()) {
                moves |= attacks::rook(sq, occ) & pinHV;
            } else {
                moves |= attacks::queen(sq, occ);
            }

            return moves;
        }

        template <typename T>
        inline void generateAndAddMoves(Movelist& movelist, Bitboard mask, T func) {
            while (mask) {
                const Square from  = poplsb(mask);
                auto         moves = func(from);
                while (moves) {
                    const Square to = poplsb(moves);
                    movelist.add(Move::make<Move::NORMAL>(from, to));
                }
            }
        }

        template <Color c, MoveGenType mt, bool Legal = false>
        inline Bitboard generateCastlingMoves(const Board& board, Square kingSq, Bitboard seen, Bitboard occAll, Bitboard pinHV = 0ULL) {
            if constexpr (mt == MoveGenType::CAPTURE) {
                return 0ULL;
            }
            if (seen & kingSq.bb()) {
                return 0ULL;
            }

            Bitboard moves = 0ULL;

            for (const CastlingSide castleSide : {CastlingSide::KING_SIDE, CastlingSide::QUEEN_SIDE}) {
                if (!board.hasCastlingRights(c, castleSide)) {
                    continue;
                }

                const Square kingToSq   = CastlingRights::kingTo<c>(castleSide);
                const Square rookToSq   = CastlingRights::rookTo<c>(castleSide);
                const Square rookFromSq = CastlingRights::rookFrom<c>(castleSide);

                const Bitboard rookFromSqBB = rookFromSq.bb();

                const Bitboard NOT_OCCUPIED_PATH  = squaresBetween(kingSq, rookFromSq);
                const Bitboard NOT_ATTACKED_PATH  = squaresBetween(kingSq, kingToSq);
                const Bitboard EMPTY_NOT_ATTACKED = ~seen & ~(occAll & ~rookFromSqBB);

                // Check if the castling path is not attacked
                const bool pathNotAttacked = (NOT_ATTACKED_PATH & EMPTY_NOT_ATTACKED) == NOT_ATTACKED_PATH;

                // Check if the castling path is not occupied
                const bool pathNotOccupied = (NOT_OCCUPIED_PATH & ~occAll) == NOT_OCCUPIED_PATH;

                // Check if the king's destination is not occupied or attacked.
                const Bitboard kingToSqBB   = kingToSq.bb();
                const Bitboard kingBB       = kingSq.bb();
                const Bitboard withoutRook  = occAll & ~rookFromSqBB;
                const bool     isKingToSafe = !(kingToSqBB & (seen | (withoutRook & ~kingBB)));

                // Check if the rook's destination is not occupied by the king
                const Bitboard rookToSqBB         = rookToSq.bb();
                const Bitboard withoutKing        = occAll & ~kingBB;
                const bool     isRookToUnoccupied = !(rookToSqBB & (withoutRook & withoutKing));

                if constexpr (Legal) {
                    // Check if the rook is not pinned
                    const Bitboard kingRank      = toBitboard(kingSq.rank());
                    const bool     rookNotPinned = !(rookFromSqBB & pinHV & kingRank);

                    if (pathNotAttacked && pathNotOccupied && rookNotPinned && isRookToUnoccupied && isKingToSafe) {
                        moves |= rookFromSqBB;
                    }
                } else {
                    if (pathNotAttacked && pathNotOccupied && isRookToUnoccupied && isKingToSafe) {
                        moves |= rookFromSqBB;
                    }
                }
            }

            return moves;
        }

        // generate psuedo legal moves
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

            Bitboard seen   = seenSquares<~c>(board, enemy_empty, all);
            Square   kingSq = board.kingSq(c);

            // clang-format off

            constexpr Rank RANK_1 = c == Color::WHITE ? Rank::RANK_1 : Rank::RANK_8;

            // Generate king moves and castling moves
            generateAndAddMoves(moves, makeBitboard(kingSq), [&](Square from) {
                 return attacks::king(from) & moveableSquare & ~seen; 
            });

            if (board.hasCastlingRights(c) && (kingSq.rank() == RANK_1)) {
                Bitboard castlingMoves = generateCastlingMoves<c, mt>(board, kingSq, seen, all);

                while (castlingMoves) {
                    Square to = poplsb(castlingMoves);
                    moves.add(Move::make<Move::CASTLING>(kingSq, to));
                }
            }

            // Generate pawn moves
            generatePawnMoves<c, mt>(board, moves, them, all);

            // Generate knight moves
            Bitboard knights = board.bitboard<c, PieceType::KNIGHT>();
            generateAndAddMoves(moves, knights, [&](Square from) { 
                return attacks::knight(from) & moveableSquare; 
            });

            // Generate bishop moves
            Bitboard bishops = board.bitboard<c, PieceType::BISHOP>();
            generateAndAddMoves(moves, bishops, [&](Square from) {
                return attacks::bishop(from, all) & moveableSquare;
            });

            // Generate rook moves
            Bitboard rooks = board.bitboard<c, PieceType::ROOK>();
            generateAndAddMoves(moves, rooks, [&](Square from) {
                return attacks::rook(from, all) & moveableSquare;
            });

            // Generate queen moves
            Bitboard queens = board.bitboard<c, PieceType::QUEEN>();
            generateAndAddMoves(moves, queens, [&](Square from) {
                return attacks::queen(from, all) & moveableSquare;
            });

            // clang-format on
        }

        // generate legal moves
        template <Color c, MoveGenType mt>
        void generateLegalMovess(const Board& board, Movelist& moves) {
            Bitboard us          = board.us(c);
            Bitboard them        = board.them(c);
            Bitboard all         = us | them;
            Bitboard enemy_empty = ~us;

            int double_check = 0;

            const auto kingSq = board.kingSq(c);

            Bitboard checkMask = movegen::checkMask<c>(board, kingSq, double_check);
            Bitboard pinHV     = movegen::pinMaskRooks<c>(board, kingSq, them, us);
            Bitboard pinD      = movegen::pinMaskBishops<c>(board, kingSq, them, us);

            assert(double_check <= 2);

            Bitboard moveableSquare;

            if constexpr (mt == MoveGenType::ALL) {
                moveableSquare = ~us;
            } else if constexpr (mt == MoveGenType::CAPTURE) {
                moveableSquare = them;
            } else {
                moveableSquare = ~all;
            }

            Bitboard seen = seenSquares<~c>(board, enemy_empty, all);

            // clang-format off

            // Generate king moves and castling moves
            generateAndAddMoves(moves, makeBitboard(kingSq), [&](Square from) {
                 return attacks::king(from) & moveableSquare & ~seen; 
            });

            constexpr Rank RANK_1 = c == Color::WHITE ? Rank::RANK_1 : Rank::RANK_8;

            if (board.hasCastlingRights(c) && (kingSq.rank() == RANK_1) && checkMask == DEFAULT_CHECKMASK) {
                Bitboard castlingMoves = generateCastlingMoves<c, mt, true>(board, kingSq, seen, all, pinHV);

                while (castlingMoves) {
                    Square to = poplsb(castlingMoves);
                    moves.add(Move::make<Move::CASTLING>(kingSq, to));
                }
            }

            moveableSquare &= checkMask;

            if (double_check == 2) return;
            if (moveableSquare == 0ULL) return;

            // Generate pawn moves
            generatePawnMoves<c, mt, true>(board, moves, them, all, pinD, pinHV, checkMask);

            // Generate knight moves
            Bitboard knight_mask = board.bitboard<c, PieceType::KNIGHT>() & ~(pinD | pinHV);
            
            generateAndAddMoves(moves, knight_mask, [&](Square from) { 
                return attacks::knight(from) & moveableSquare; 
            });

            // Generate bishop moves
            Bitboard bishop_mask = board.bitboard<c, PieceType::BISHOP>() & ~pinHV;

            generateAndAddMoves(moves, bishop_mask, [&](Square from) {
                return generateBishopMoves(from, pinD, all) & moveableSquare;
            });

            // Generate rook moves
            Bitboard rook_mask = board.bitboard<c, PieceType::ROOK>() & ~pinD;

            generateAndAddMoves(moves, rook_mask, [&](Square from) {
                return generateRookMoves(from, pinHV, all) & moveableSquare;
            });

            // Generate queen moves
            Bitboard queen_mask = board.bitboard<c, PieceType::QUEEN>() & ~(pinD & pinHV);
            
            generateAndAddMoves(moves, queen_mask, [&](Square from) {
                return generateQueenMoves(from, pinHV, pinD, all) & moveableSquare;
            });
        }

        template <MoveGenType mt>
        void psuedoLegal(const Board& board, Movelist& moves) {
            if (board.sideToMove() == Color::WHITE) {
                generateMoves<Color::WHITE, mt>(board, moves);
            } else {
                generateMoves<Color::BLACK, mt>(board, moves);
            }
        }

        template <MoveGenType mt>
        void legalMoves(const Board& board, Movelist& moves) {
            if (board.sideToMove() == Color::WHITE) {
                generateLegalMovess<Color::WHITE, mt>(board, moves);
            } else {
                generateLegalMovess<Color::BLACK, mt>(board, moves);
            }
        }

    } // namespace movegen
} // namespace chess