#pragma once

#include "attacks.hpp"
#include "board.hpp"
#include "move.hpp"

namespace chess {
    namespace movegen {
        enum class MoveGenType : uint8_t { ALL, QUIET, CAPTURE };

        // Helper function to handle attacks of a specific piece type and update the double check count
        template <Color side, PieceType pieceType>
        inline U64 handlePieceAttacks(const Board& board, Square square, int& doubleCheckCount) {
            // Get attacks for the specified piece type on the given square
            U64 pieceAttacks = attacks::getAttacks<pieceType>(square) & board.bitboard(~side, pieceType);

            // Update double check count if there are attacks from the piece
            doubleCheckCount += bool(pieceAttacks);

            return pieceAttacks;
        }

        // Helper function to handle slider attacks and update the double check count
        inline U64 handleSliderAttacks(Square square, U64 sliderAttacks, int& doubleCheckCount) {
            U64 mask = 0ULL;

            // Process each slider attack
            while (sliderAttacks) {
                // Get the target square of the slider attack
                const Square targetSquare = poplsb(sliderAttacks);

                // Include the target square and squares between in the check mask
                mask |= squaresBetween(square, targetSquare) | targetSquare.bb();

                // Increment double check count
                doubleCheckCount++;
            }

            return mask;
        }

        // Generates the check mask for a given square
        template <Color side>
        inline U64 generateCheckMask(const Board& board, Square square, int& doubleCheckCount) {
            U64 mask    = 0ULL;
            doubleCheckCount = 0;

            // Handle knight attacks
            mask |= handlePieceAttacks<side, PieceType::KNIGHT>(board, square, doubleCheckCount);

            // Handle pawn attacks
            mask |= handlePieceAttacks<side, PieceType::PAWN>(board, square, doubleCheckCount);

            // Handle bishop and queen attacks
            U64 bishopAndQueenAttacks = attacks::bishop(square, board.occupied()) & (board.bitboard(~side, PieceType::BISHOP) | board.bitboard(~side, PieceType::QUEEN));
            mask |= handleSliderAttacks(square, bishopAndQueenAttacks, doubleCheckCount);

            // Handle rook and queen attacks
            U64 rookAndQueenAttacks = attacks::rook(square, board.occupied()) & (board.bitboard(~side, PieceType::ROOK) | board.bitboard(~side, PieceType::QUEEN));
            mask |= handleSliderAttacks(square, rookAndQueenAttacks, doubleCheckCount);

            // Return the final check mask or a default mask if no checks are found
            return (mask != 0ULL) ? mask : DEFAULT_CHECKMASK;
        }

        template <Color c, PieceType pt>
        inline U64 generatePinMask(const Board& board, Square kingSq, U64 them, U64 us) {
            U64 pinMask = 0;

            const U64 oppositeSlider = board.bitboard<~c, PieceType::QUEEN>() | board.bitboard<~c, pt>();

            U64 sliderAttacks = attacks::getAttacks<pt>(kingSq) & oppositeSlider;

            while (sliderAttacks) {
                const auto index = poplsb(sliderAttacks);

                const U64 possible_pin = squaresBetween(kingSq, index) | index.bb();

                if (popcount(possible_pin & us) == 1) {
                    pinMask |= possible_pin;
                }
            }

            return pinMask;
        }

        template <Color c>
        inline U64 pinMaskRooks(const Board& board, Square sq, U64 occ_enemy, U64 occ_us) {
            U64 pin_hv = 0;

            const U64 oppRook  = board.bitboard<~c, PieceType::ROOK>();
            const U64 oppQueen = board.bitboard<~c, PieceType::QUEEN>();

            U64 rookAttacks = attacks::rook(sq, occ_enemy) & (oppRook | oppQueen);

            while (rookAttacks) {
                const auto index = poplsb(rookAttacks);

                const U64 possible_pin = squaresBetween(sq, index) | index.bb();

                if (popcount(possible_pin & occ_us) == 1) {
                    pin_hv |= possible_pin;
                }
            }

            return pin_hv;
        }

        template <Color c>
        inline U64 pinMaskBishops(const Board& board, Square sq, U64 occ_enemy, U64 occ_us) {
            U64 pin_d = 0;

            const U64 oppBishop = board.bitboard<~c, PieceType::BISHOP>();
            const U64 oppQueen  = board.bitboard<~c, PieceType::QUEEN>();

            U64 bishopAttacks = attacks::bishop(sq, occ_enemy) & (oppBishop | oppQueen);

            while (bishopAttacks) {
                const auto index = poplsb(bishopAttacks);

                const U64 possible_pin = squaresBetween(sq, index) | index.bb();

                if (popcount(possible_pin & occ_us) == 1) {
                    pin_d |= possible_pin;
                }
            }

            return pin_d;
        }

        template <Color c>
        inline U64 seenSquares(const Board& board, U64 enemy_empty, U64 occ) {
            Square theirKingSq = board.kingSq<~c>();

            U64 mapKingAtk = attacks::king(theirKingSq) & enemy_empty;

            if (mapKingAtk == 0ull) {
                return 0ull;
            }

            occ &= ~theirKingSq.bb();

            U64 queens  = board.bitboard<c, PieceType::QUEEN>();
            U64 bishops = board.bitboard<c, PieceType::BISHOP>() | queens;
            U64 rooks   = board.bitboard<c, PieceType::ROOK>() | queens;

            U64 knights = board.bitboard<c, PieceType::KNIGHT>();
            U64 pawns   = board.bitboard<c, PieceType::PAWN>();

            U64 seen = attacks::pawnLeftAttacks<c>(pawns) | attacks::pawnRightAttacks<c>(pawns);

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
        inline void generatePromotionMoves(const U64 pawns, const U64 single_push, const U64 left_attacks, const U64 right_attacks, Movelist& moves) {
            constexpr U64 RANK_7 = toBitboard<relativeRank<c, Rank::RANK_7>()>();
            constexpr U64 RANK_8 = toBitboard<relativeRank<c, Rank::RANK_8>()>();

            constexpr Direction DOWN       = relativeDirection<c, Direction::SOUTH>();
            constexpr Direction DOWN_LEFT  = relativeDirection<c, Direction::SOUTH_WEST>();
            constexpr Direction DOWN_RIGHT = relativeDirection<c, Direction::SOUTH_EAST>();

            if (pawns & RANK_7) {
                U64 promotions       = single_push & RANK_8;
                U64 promotions_left  = left_attacks & RANK_8;
                U64 promotions_right = right_attacks & RANK_8;

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
        void generatePawnMoves(const Board& board, Movelist& moves, U64 them, U64 occAll, U64 pinD = 0ULL, U64 pinHV = 0ULL, U64 checkMask = 0ULL) {
            U64 pawns = board.bitboard(c, PieceType::PAWN);

            constexpr Direction UP         = relativeDirection<c, Direction::NORTH>();
            constexpr Direction DOWN       = relativeDirection<c, Direction::SOUTH>();
            constexpr Direction DOWN_LEFT  = relativeDirection<c, Direction::SOUTH_WEST>();
            constexpr Direction DOWN_RIGHT = relativeDirection<c, Direction::SOUTH_EAST>();

            constexpr U64 RANK_8 = toBitboard<relativeRank<c, Rank::RANK_8>()>();
            constexpr U64 RANK_3 = toBitboard<relativeRank<c, Rank::RANK_3>()>();

            if constexpr (Legal) {
                const U64 pawns_lr = pawns & ~pinHV;

                const U64 unpinnedpawns_lr = pawns_lr & ~pinD;
                const U64 pinnedpawns_lr   = pawns_lr & pinD;

                // clang-format off
                U64 left_attacks = (attacks::pawnLeftAttacks<c>(unpinnedpawns_lr)) | 
                                         (attacks::pawnLeftAttacks<c>(pinnedpawns_lr) & pinD);
                U64 right_attacks = (attacks::pawnRightAttacks<c>(unpinnedpawns_lr)) | 
                                        (attacks::pawnRightAttacks<c>(pinnedpawns_lr) & pinD);

                left_attacks &= them & checkMask;
                right_attacks &= them & checkMask;
                
                const U64 pawns_hv = pawns & ~pinD;

                const U64 unpinnedpawns_hv = pawns_hv & ~pinHV;
                const U64 pinnedpawns_hv   = pawns_hv & pinHV;

                const U64 single_push_unpinned = attacks::shift<UP>(unpinnedpawns_hv) & ~occAll;
                const U64 single_push_pinned   = attacks::shift<UP>(pinnedpawns_hv) & pinHV & ~occAll;

                U64 single_push = (single_push_unpinned | single_push_pinned) & checkMask;

                // clang-format on
                generatePromotionMoves<c, mt>(pawns, single_push, left_attacks, right_attacks, moves);

                // rule out to-promoted moves
                single_push &= ~RANK_8;
                left_attacks &= ~RANK_8;
                right_attacks &= ~RANK_8;

                if constexpr (mt != MoveGenType::CAPTURE) {
                    U64 double_push = ((attacks::shift<UP>(single_push_unpinned & RANK_3) & ~occAll) | (attacks::shift<UP>(single_push_pinned & RANK_3) & ~occAll)) & checkMask;

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
                        const U64 epSqBB   = ep.bb();
                        const U64 epPawnBB = ep_pawn.bb();

                        const U64 ep_mask = epPawnBB | epSqBB;

                        if ((checkMask & ep_mask) == 0)
                            return;

                        const Square   kingSq         = board.kingSq<c>();
                        const U64 kingMask       = kingSq.bb() & toBitboard(ep_pawn.rank());
                        const U64 enemyQueenRook = board.bitboard<~c, PieceType::QUEEN>() | board.bitboard<~c, PieceType::ROOK>();

                        const bool possiblePin = kingMask && enemyQueenRook;
                        U64   ep_bb       = attacks::pawn(ep, ~c) & pawns_lr;

                        while (ep_bb) {
                            Square from = poplsb(ep_bb);
                            Square to   = ep;

                            if ((1ULL << from) & pinD && !(pinD & epSqBB)) {
                                continue;
                            }

                            const U64 connectingPawns = epPawnBB | (1ULL << from);

                            if (possiblePin && (attacks::rook(kingSq, occAll & ~connectingPawns) & enemyQueenRook) != 0)
                                break;

                            moves.add(Move::make<Move::ENPASSANT>(from, to));
                        }
                    }
                }

            } else {
                U64 left_attacks  = attacks::pawnLeftAttacks<c>(pawns) & them;
                U64 right_attacks = attacks::pawnRightAttacks<c>(pawns) & them;
                U64 single_pushes = attacks::shift<UP>(pawns) & ~occAll;

                generatePromotionMoves<c, mt>(pawns, single_pushes, left_attacks, right_attacks, moves);

                // rule out to-promoted moves
                single_pushes &= ~RANK_8;
                left_attacks &= ~RANK_8;
                right_attacks &= ~RANK_8;

                if constexpr (mt != MoveGenType::CAPTURE) {
                    U64 double_pushes = attacks::shift<UP>(single_pushes & RANK_3) & ~occAll;

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
                        U64 ep_bb = attacks::pawn(ep, ~c) & pawns;

                        while (ep_bb) {
                            Square from = poplsb(ep_bb);
                            moves.add(Move::make<Move::ENPASSANT>(from, ep));
                        }
                    }
                }
            }
        }

        inline U64 generateBishopMoves(Square sq, U64 pinD, U64 occ) {
            if (pinD & sq.bb())
                return attacks::bishop(sq, occ) & pinD;

            return attacks::bishop(sq, occ);
        }

        inline U64 generateRookMoves(Square sq, U64 pinHV, U64 occ) {
            if (pinHV & sq.bb())
                return attacks::rook(sq, occ) & pinHV;

            return attacks::rook(sq, occ);
        }

        inline U64 generateQueenMoves(Square sq, U64 pinHV, U64 pinD, U64 occ) {
            U64 moves = 0ull;

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
        inline void generateAndAddMoves(Movelist& movelist, U64 mask, T func) {
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
        inline U64 generateCastlingMoves(const Board& board, Square kingSq, U64 seen, U64 occAll, U64 pinHV = 0ULL) {
            if constexpr (mt == MoveGenType::CAPTURE) {
                return 0ULL;
            }
            if (seen & kingSq.bb()) {
                return 0ULL;
            }

            U64 moves = 0ULL;

            for (const CastlingSide castleSide : {CastlingSide::KING_SIDE, CastlingSide::QUEEN_SIDE}) {
                if (!board.hasCastlingRights(c, castleSide)) {
                    continue;
                }

                const Square kingToSq   = CastlingRights::kingTo<c>(castleSide);
                const Square rookToSq   = CastlingRights::rookTo<c>(castleSide);
                const Square rookFromSq = CastlingRights::rookFrom<c>(castleSide);

                const U64 rookFromSqBB = rookFromSq.bb();

                const U64 NOT_OCCUPIED_PATH  = squaresBetween(kingSq, rookFromSq);
                const U64 NOT_ATTACKED_PATH  = squaresBetween(kingSq, kingToSq);
                const U64 EMPTY_NOT_ATTACKED = ~seen & ~(occAll & ~rookFromSqBB);

                // Check if the castling path is not attacked
                const bool pathNotAttacked = (NOT_ATTACKED_PATH & EMPTY_NOT_ATTACKED) == NOT_ATTACKED_PATH;

                // Check if the castling path is not occupied
                const bool pathNotOccupied = (NOT_OCCUPIED_PATH & ~occAll) == NOT_OCCUPIED_PATH;

                // Check if the king's destination is not occupied or attacked.
                const U64 kingToSqBB   = kingToSq.bb();
                const U64 kingBB       = kingSq.bb();
                const U64 withoutRook  = occAll & ~rookFromSqBB;
                const bool     isKingToSafe = !(kingToSqBB & (seen | (withoutRook & ~kingBB)));

                // Check if the rook's destination is not occupied by the king
                const U64 rookToSqBB         = rookToSq.bb();
                const U64 withoutKing        = occAll & ~kingBB;
                const bool     isRookToUnoccupied = !(rookToSqBB & (withoutRook & withoutKing));

                if constexpr (Legal) {
                    // Check if the rook is not pinned
                    const U64 kingRank      = toBitboard(kingSq.rank());
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

        // generate legal moves
        template <Color c, MoveGenType mt>
        void generateLegalMovess(const Board& board, Movelist& moves) {
            U64 us          = board.us(c);
            U64 them        = board.them(c);
            U64 all         = us | them;
            U64 enemy_empty = ~us;

            int double_check = 0;

            const auto kingSq = board.kingSq(c);

            U64 checkMask = generateCheckMask<c>(board, kingSq, double_check);
            U64 pinHV     = pinMaskRooks<c>(board, kingSq, them, us);
            U64 pinD      = pinMaskBishops<c>(board, kingSq, them, us);

            assert(double_check <= 2);

            U64 moveableSquare;

            if constexpr (mt == MoveGenType::ALL) {
                moveableSquare = ~us;
            } else if constexpr (mt == MoveGenType::CAPTURE) {
                moveableSquare = them;
            } else {
                moveableSquare = ~all;
            }

            U64 seen = seenSquares<~c>(board, enemy_empty, all);

            // clang-format off

            // Generate king moves and castling moves
            generateAndAddMoves(moves, makeBitboard(kingSq), [&](Square from) {
                 return attacks::king(from) & moveableSquare & ~seen; 
            });

            constexpr Rank RANK_1 = c == Color::WHITE ? Rank::RANK_1 : Rank::RANK_8;

            if (board.hasCastlingRights(c) && (kingSq.rank() == RANK_1) && checkMask == DEFAULT_CHECKMASK) {
                U64 castlingMoves = generateCastlingMoves<c, mt, true>(board, kingSq, seen, all, pinHV);

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
            U64 knight_mask = board.bitboard<c, PieceType::KNIGHT>() & ~(pinD | pinHV);
            
            generateAndAddMoves(moves, knight_mask, [&](Square from) { 
                return attacks::knight(from) & moveableSquare; 
            });

            // Generate bishop moves
            U64 bishop_mask = board.bitboard<c, PieceType::BISHOP>() & ~pinHV;

            generateAndAddMoves(moves, bishop_mask, [&](Square from) {
                return generateBishopMoves(from, pinD, all) & moveableSquare;
            });

            // Generate rook moves
            U64 rook_mask = board.bitboard<c, PieceType::ROOK>() & ~pinD;

            generateAndAddMoves(moves, rook_mask, [&](Square from) {
                return generateRookMoves(from, pinHV, all) & moveableSquare;
            });

            // Generate queen moves
            U64 queen_mask = board.bitboard<c, PieceType::QUEEN>() & ~(pinD & pinHV);
            
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