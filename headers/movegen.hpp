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

            const Bitboard oppRook  = board.bitboard(~c, PieceType::ROOK);
            const Bitboard oppQueen = board.bitboard(~c, PieceType::QUEEN);

            Bitboard rookAttacks = attacks::rook(sq, occ_enemy) & (oppRook | oppQueen);

            while (rookAttacks) {
                const auto index = poplsb(rookAttacks);

                const Bitboard possible_pin = attacks::SQUARES_BETWEEN_BB[sq][index] | (1ULL << index);

                if (popcount(possible_pin & occ_us) == 1) {
                    pin_hv |= possible_pin;
                }
            }

            return pin_hv;
        }

        template <Color c>
        inline Bitboard pinMaskBishops(const Board& board, Square sq, Bitboard occ_enemy, Bitboard occ_us) {
            Bitboard pin_d = 0;

            const Bitboard oppBishop = board.bitboard(~c, PieceType::BISHOP);
            const Bitboard oppQueen  = board.bitboard(~c, PieceType::QUEEN);

            Bitboard bishopAttacks = attacks::bishop(sq, occ_enemy) & (oppBishop | oppQueen);

            while (bishopAttacks) {
                const auto index = poplsb(bishopAttacks);

                const Bitboard possible_pin = attacks::SQUARES_BETWEEN_BB[sq][index] | (1ULL << index);

                if (popcount(possible_pin & occ_us) == 1) {
                    pin_d |= possible_pin;
                }
            }

            return pin_d;
        }

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

        template <Color c, MoveGenType mt, bool Legal = false>
        void generatePawnMoves(const Board& board, Movelist& moves, Bitboard enemy_occ, Bitboard pinD = 0ULL, Bitboard pinHV = 0ULL, Bitboard checkMask = 0ULL) {
            Bitboard pawns = board.bitboard(c, PieceType::PAWN);

            constexpr Direction UP         = c == Color::WHITE ? Direction::NORTH : Direction::SOUTH;
            constexpr Direction DOWN       = c == Color::WHITE ? Direction::SOUTH : Direction::NORTH;
            constexpr Direction DOWN_LEFT  = c == Color::WHITE ? Direction::SOUTH_WEST : Direction::NORTH_EAST;
            constexpr Direction DOWN_RIGHT = c == Color::WHITE ? Direction::SOUTH_EAST : Direction::NORTH_WEST;

            constexpr Bitboard RANK_7 = c == Color::WHITE ? attacks::MASK_RANK[int(Rank::RANK_7)] : attacks::MASK_RANK[int(Rank::RANK_2)];

            constexpr Bitboard RANK_8 = c == Color::WHITE ? attacks::MASK_RANK[int(Rank::RANK_8)] : attacks::MASK_RANK[int(Rank::RANK_1)];

            constexpr Bitboard RANK_3 = c == Color::WHITE ? attacks::MASK_RANK[int(Rank::RANK_3)] : attacks::MASK_RANK[int(Rank::RANK_6)];

            if constexpr (Legal) {
                const Bitboard pawns_lr = pawns & ~pinHV;

                const Bitboard unpinnedpawns_lr = pawns_lr & ~pinD;
                const Bitboard pinnedpawns_lr   = pawns_lr & pinD;

                // clang-format off
                Bitboard left_attacks = (attacks::pawnLeftAttacks<c>(unpinnedpawns_lr)) | (attacks::pawnLeftAttacks<c>(pinnedpawns_lr) & pinD);
                Bitboard right_attacks = (attacks::pawnRightAttacks<c>(unpinnedpawns_lr)) | (attacks::pawnRightAttacks<c>(pinnedpawns_lr) & pinD);

                left_attacks &= enemy_occ & checkMask;
                right_attacks &= enemy_occ & checkMask;
                
                const Bitboard pawns_hv = pawns & ~pinD;

                const Bitboard unpinnedpawns_hv = pawns_hv & ~pinHV;
                const Bitboard pinnedpawns_hv   = pawns_hv & pinHV;

                const Bitboard single_push_unpinned = attacks::shift<UP>(unpinnedpawns_hv) & ~board.occupied();
                const Bitboard single_push_pinned   = attacks::shift<UP>(pinnedpawns_hv) & pinHV & ~board.occupied();

                Bitboard single_push = (single_push_unpinned | single_push_pinned) & checkMask;

                Bitboard double_push =((attacks::shift<UP>(single_push_unpinned & RANK_3) & ~board.occupied()) |
                                    (attacks::shift<UP>(single_push_pinned & RANK_3) & ~board.occupied())) &
                                    checkMask;

                if (pawns & RANK_7){
                    Bitboard promotions = single_push & RANK_8;
                    Bitboard promotions_left = left_attacks & RANK_8;
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
                // clang-format on

                // rule out to-promoted moves
                single_push &= ~RANK_8;
                left_attacks &= ~RANK_8;
                right_attacks &= ~RANK_8;

                if constexpr (mt != MoveGenType::CAPTURE) {
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
                        const Square ep_pawn = ep + DOWN;

                        const Bitboard ep_mask = (1ULL << ep_pawn) | (1ULL << ep);

                        if ((checkMask & ep_mask) == 0)
                            return;

                        // clang-format off
                        const Square   kingSq         = board.kingSq(c);

                        const Bitboard kingMask       = 
                        (1ULL << kingSq) & attacks::MASK_RANK[static_cast<int>(ep_pawn.rank())];

                        const Bitboard enemyQueenRook = 
                        board.bitboard(~c, PieceType::QUEEN) | board.bitboard(~c, PieceType::ROOK);

                        const bool isPossiblePin = kingMask && enemyQueenRook;
                        Bitboard ep_bb = attacks::pawn(ep, ~c) & pawns_lr;

                        while(ep_bb){
                            Square from = poplsb(ep_bb);
                            Square to = ep;

                            if ((1ULL << from) & pinD && !(pinD & (1ULL << ep)))
                                continue;
                                
                            const Bitboard connectingPawns = (1ULL << ep_pawn) | (1ULL << from);

                            if (isPossiblePin && 
                                (attacks::rook(kingSq, board.occupied() & ~connectingPawns) & enemyQueenRook) != 0)
                                break;
                            
                            moves.add(Move::make<Move::ENPASSANT>(from, to));
                        }
                        // clang-format on
                    }
                }

            } else {
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
        }

        inline Bitboard generateBishopMoves(Square sq, Bitboard pinD, Bitboard occ) {
            if (pinD & (1ull << sq))
                return attacks::bishop(sq, occ) & pinD;

            return attacks::bishop(sq, occ);
        }

        inline Bitboard generateRookMoves(Square sq, Bitboard pinHV, Bitboard occ) {
            if (pinHV & (1ull << sq))
                return attacks::rook(sq, occ) & pinHV;

            return attacks::rook(sq, occ);
        }

        inline Bitboard generateQueenMoves(Square sq, Bitboard pinHV, Bitboard pinD, Bitboard occ) {
            Bitboard moves = 0ull;

            if (pinD & (1ULL << sq)) {
                moves |= attacks::bishop(sq, occ) & pinD;
            } else if (pinHV & (1ULL << sq)) {
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
        inline Bitboard generateCastlingMoves(const Board& board, Square kingSq, Bitboard seen, Bitboard pinHV = 0ULL) {
            if constexpr (mt == MoveGenType::CAPTURE) {
                return 0ULL;
            }
            if (seen & (1ULL << kingSq)) {
                return 0ULL;
            }

            Bitboard moves = 0ULL;

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

                if constexpr (Legal) {
                    if ((NOT_ATTACKED_PATH & EMPTY_NOT_ATTACKED) == NOT_ATTACKED_PATH && ((NOT_OCCUPIED_PATH & ~board.occupied()) == NOT_OCCUPIED_PATH) &&
                        !((1ull << rookFromSq) & pinHV & attacks::MASK_RANK[static_cast<int>(kingSq.rank())]) && !((1ull << rookToSq) & (withoutRook & withoutKing)) &&
                        !((1ull << kingToSq) & (seen | (withoutRook & ~(1ull << kingSq))))) {
                        moves |= (1ull << rookFromSq);
                    }
                } else {
                    if ((NOT_ATTACKED_PATH & EMPTY_NOT_ATTACKED) == NOT_ATTACKED_PATH && ((NOT_OCCUPIED_PATH & ~board.occupied()) == NOT_OCCUPIED_PATH) && !((1ull << rookToSq) & (withoutRook & withoutKing)) &&
                        !((1ull << kingToSq) & (seen | (withoutRook & ~(1ull << kingSq))))) {
                        moves |= (1ull << rookFromSq);
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

            Bitboard seen   = seenSquares<~c>(board, enemy_empty);
            Square   kingSq = board.kingSq(c);

            // clang-format off

            constexpr Rank RANK_1 = c == Color::WHITE ? Rank::RANK_1 : Rank::RANK_8;

            // Generate king moves and castling moves
            generateAndAddMoves(moves, makeBitboard(kingSq), [&](Square from) {
                 return attacks::king(from) & moveableSquare & ~seen; 
            });

            if (board.hasCastlingRights(c) && (kingSq.rank() == RANK_1)) {
                Bitboard castlingMoves = generateCastlingMoves<c, mt>(board, kingSq, seen);

                while (castlingMoves) {
                    Square to = poplsb(castlingMoves);
                    moves.add(Move::make<Move::CASTLING>(kingSq, to));
                }
            }

            // Generate pawn moves
            generatePawnMoves<c, mt>(board, moves, them);

            // Generate knight moves
            Bitboard knights = board.bitboard(c, PieceType::KNIGHT);
            generateAndAddMoves(moves, knights, [&](Square from) { 
                return attacks::knight(from) & moveableSquare; 
            });

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

            Bitboard seen = seenSquares<~c>(board, enemy_empty);

            // clang-format off

            // Generate king moves and castling moves
            generateAndAddMoves(moves, makeBitboard(kingSq), [&](Square from) {
                 return attacks::king(from) & moveableSquare & ~seen; 
            });

            constexpr Rank RANK_1 = c == Color::WHITE ? Rank::RANK_1 : Rank::RANK_8;

            if (board.hasCastlingRights(c) && (kingSq.rank() == RANK_1) && checkMask == DEFAULT_CHECKMASK) {
                Bitboard castlingMoves = generateCastlingMoves<c, mt, true>(board, kingSq, seen);

                while (castlingMoves) {
                    Square to = poplsb(castlingMoves);
                    moves.add(Move::make<Move::CASTLING>(kingSq, to));
                }
            }

            moveableSquare &= checkMask;

            if (double_check == 2) return;

            // Generate pawn moves
            generatePawnMoves<c, mt, true>(board, moves, them, pinD, pinHV, checkMask);

            // Generate knight moves
            Bitboard knight_mask = board.bitboard(c, PieceType::KNIGHT) & ~(pinD | pinHV);

            
            generateAndAddMoves(moves, knight_mask, [&](Square from) { 
                return attacks::knight(from) & moveableSquare; 
            });

            // Generate bishop moves
            Bitboard bishop_mask = board.bitboard(c, PieceType::BISHOP) & ~pinHV;

            generateAndAddMoves(moves, bishop_mask, [&](Square from) {
                return generateBishopMoves(from, pinD, all) & moveableSquare;
            });

            // Generate rook moves
            Bitboard rook_mask = board.bitboard(c, PieceType::ROOK) & ~pinD;

            generateAndAddMoves(moves, rook_mask, [&](Square from) {
                return generateRookMoves(from, pinHV, all) & moveableSquare;
            });

            // Generate queen moves
            Bitboard queen_mask = board.bitboard(c, PieceType::QUEEN) & ~(pinD & pinHV);
            
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