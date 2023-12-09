#pragma once

#include "board.hpp"
#include "moves.hpp"

namespace chess {

    enum class MoveGenType : uint8_t { ALL, QUIET, CAPTURE };

    class MoveGen {
    private:
        template <Color c>
        static inline Bitboard getLeaperAttacks(const Board& board) {
            Bitboard attacks;
            Bitboard pawns = board.bitboard<c, PieceType::PAWN>();

            attacks |= Attacks::pawnLeftAttacks<c>(pawns) | Attacks::pawnRightAttacks<c>(pawns);

            Bitboard knights = board.bitboard<c, PieceType::KNIGHT>();
            BitboardIterator(knights) {
                Square index = knights.poplsb();
                attacks |= Attacks::knightAttacks(index);
            }

            attacks |= Attacks::kingAttacks(board.kingSq<c>());

            return attacks;
        }

        template <Color c>
        static inline Bitboard getSliderAttacks(const Board& board, Bitboard all) {
            Bitboard attacks;
            Bitboard bishops = board.bitboard<c, PieceType::BISHOP>() | board.bitboard<c, PieceType::QUEEN>();
            Bitboard rooks   = board.bitboard<c, PieceType::ROOK>() | board.bitboard<c, PieceType::QUEEN>();

            BitboardIterator(bishops) {
                Square index = bishops.poplsb();
                attacks |= Attacks::bishopAttacks(index, all);
            }

            BitboardIterator(rooks) {
                Square index = rooks.poplsb();
                attacks |= Attacks::rookAttacks(index, all);
            }

            return attacks;
        }

        template <Color c>
        static inline Bitboard generateSeenSquares(const Board& board, Bitboard all, Bitboard enemy_or_empty) {
            Square theirKing = board.kingSq<~c>();

            if ((Attacks::kingAttacks(theirKing) & enemy_or_empty).empty()) {
                return Bitboard(0);
            }

            all &= ~Bitboard(theirKing);

            Bitboard seen;

            seen |= getLeaperAttacks<c>(board);
            seen |= getSliderAttacks<c>(board, all);

            return seen;
        }

        template <Color c>
        static inline int generateCheckers(const Board& board, Square kingSq, Bitboard& leaperCheckers,
                                           Bitboard& sliderCheckers) {
            Bitboard knight_attacks = Attacks::knightAttacks(kingSq) & board.bitboard<~c, PieceType::KNIGHT>();
            Bitboard pawn_attacks   = Attacks::pawnAttacks<c>(kingSq) & board.bitboard<~c, PieceType::PAWN>();

            Bitboard bishop_attacks =
                Attacks::bishopAttacks(kingSq, board.occupied()) &
                (board.bitboard<~c, PieceType::BISHOP>() | board.bitboard<~c, PieceType::QUEEN>());

            Bitboard rook_attacks =
                Attacks::rookAttacks(kingSq, board.occupied()) &
                (board.bitboard<~c, PieceType::ROOK>() | board.bitboard<~c, PieceType::QUEEN>());

            leaperCheckers = (knight_attacks | pawn_attacks);
            sliderCheckers = (rook_attacks | bishop_attacks);

            return (leaperCheckers | sliderCheckers).popcount();
        }

        template <Color c>
        static inline Bitboard generateCheckMask(Square kingSq, int check_count, Bitboard leaperCheckers,
                                                 Bitboard sliderCheckers) {
            if (check_count == 2) {
                return Bitboard(0);
            }

            Bitboard checkmask;
            checkmask |= leaperCheckers;

            if (!sliderCheckers.empty()) {
                checkmask |= Attacks::squaresBetween(kingSq, sliderCheckers.lsb());
            }

            return checkmask;
        }

        template <Color c>
        static inline Bitboard pinMaskHorizontalVertical(const Board& board, Square kingSq, Bitboard them,
                                                         Bitboard us) {
            Bitboard pinHV;

            Bitboard rook_attacks =
                Attacks::rookAttacks(kingSq, them) &
                (board.bitboard<~c, PieceType::ROOK>() | board.bitboard<~c, PieceType::QUEEN>());

            BitboardIterator(rook_attacks) {
                const Square sq = rook_attacks.poplsb();

                const Bitboard possible_pin = Attacks::squaresBetween(kingSq, sq) | Bitboard(sq);
                pinHV |= possible_pin * (possible_pin & us).single();
            }

            return pinHV;
        }

        template <Color c>
        static inline Bitboard pinMaskDiagonal(const Board& board, Square kingSq, Bitboard them, Bitboard us) {
            Bitboard pinD;

            Bitboard bishop_attacks =
                Attacks::bishopAttacks(kingSq, them) &
                (board.bitboard<~c, PieceType::BISHOP>() | board.bitboard<~c, PieceType::QUEEN>());

            BitboardIterator(bishop_attacks) {
                const Square sq = bishop_attacks.poplsb();

                const Bitboard possible_pin = Attacks::squaresBetween(kingSq, sq) | Bitboard(sq);
                pinD |= possible_pin * (possible_pin & us).single();
            }

            return pinD;
        }

        template <Color c, bool double_ep_possible>
        static inline void generateEnpassantMoves(const Board& board, Movelist& movelist, Square ep,
                                                  Bitboard ep_bb, Bitboard all, Bitboard pinD) {
            if constexpr (double_ep_possible) {
                BitboardIterator(ep_bb) {
                    Square from = ep_bb.poplsb();
                    if (!(pinD & Bitboard(from)) || !(pinD & Bitboard(ep))) {
                        movelist.add(Move::makeEnpassant(from, ep));
                    }
                }
            } else {
                const Square ep_pawn = ep + relativeDirection<c, Direction::SOUTH>();
                const Square from    = ep_bb.lsb();

                const Square   kingSq = board.kingSq<c>();
                const Bitboard rook_queen =
                    board.bitboard<~c, PieceType::ROOK>() | board.bitboard<~c, PieceType::QUEEN>();

                const bool possible_pin =
                    (board.bitboard<c, PieceType::KING>() & Bitboard(ep_pawn.rank())) && rook_queen;

                const Bitboard connectingPawns = Bitboard(ep_pawn) | Bitboard(from);

                if (possible_pin &&
                    !(Attacks::rookAttacks(kingSq, all & ~connectingPawns) & rook_queen).empty()) {
                    return;
                }

                movelist.add(Move::makeEnpassant(from, ep));
            }
        }

        template <Color c, MoveGenType mt>
        static inline void generatePawnMoves(const Board& board, Movelist& movelist, Bitboard them,
                                             Bitboard all, Bitboard pinD, Bitboard pinHV, Bitboard checkmask) {
            constexpr Bitboard rank_before_promo  = Bitboard(Square::relativeRank<c, Rank::RANK_7>());
            constexpr Bitboard rank_promo         = Bitboard(Square::relativeRank<c, Rank::RANK_8>());
            constexpr Bitboard single_pushed_rank = Bitboard(Square::relativeRank<c, Rank::RANK_3>());

            constexpr Direction DOWN       = relativeDirection<c, Direction::SOUTH>();
            constexpr Direction UP         = relativeDirection<c, Direction::NORTH>();
            constexpr Direction DOWN_LEFT  = relativeDirection<c, Direction::SOUTH_WEST>();
            constexpr Direction DOWN_RIGHT = relativeDirection<c, Direction::SOUTH_EAST>();

            const Bitboard pawns = board.bitboard<c, PieceType::PAWN>();

            const bool promo_possible = !(pawns & rank_before_promo).empty();

            if constexpr (mt != MoveGenType::CAPTURE) {
                const Bitboard moveable_square = ~all & checkmask;

                const Bitboard pawns_hv             = pawns & ~pinD;
                const Bitboard unpinned_single_push = (pawns_hv & ~pinHV).shift<UP>() & ~all;
                const Bitboard pinned_single_push   = (pawns_hv & pinHV).shift<UP>() & pinHV & ~all;

                Bitboard single_push = (unpinned_single_push | pinned_single_push) & moveable_square;

                if constexpr (mt != MoveGenType::QUIET) {
                    Bitboard promotions = (single_push & rank_promo) * promo_possible;

                    BitboardIterator(promotions) {
                        Square to = promotions.poplsb();
                        movelist.addPromotions(to + DOWN, to);
                    }
                }

                Bitboard double_push = (unpinned_single_push & single_pushed_rank).shift<UP>() |
                                       (pinned_single_push & single_pushed_rank).shift<UP>();

                single_push &= ~rank_promo;
                double_push &= moveable_square;

                BitboardIterator(single_push) {
                    Square to = single_push.poplsb();

                    movelist.add(Move::makeNormal(to + DOWN, to));
                }

                BitboardIterator(double_push) {
                    Square to = double_push.poplsb();

                    movelist.add(Move::makeNormal(to + DOWN + DOWN, to));
                }
            }

            if constexpr (mt != MoveGenType::QUIET) {
                const Bitboard moveable_square = them & checkmask;

                const Bitboard pawns_d          = pawns & ~pinHV;
                const Bitboard unpinned_pawns_d = pawns_d & ~pinD;
                const Bitboard pinned_pawns_d   = pawns_d & pinD;

                Bitboard left_attacks = (Attacks::pawnLeftAttacks<c>(unpinned_pawns_d)) |
                                        (Attacks::pawnLeftAttacks<c>(pinned_pawns_d) & pinD);
                Bitboard right_attacks = (Attacks::pawnRightAttacks<c>(unpinned_pawns_d)) |
                                         (Attacks::pawnRightAttacks<c>(pinned_pawns_d) & pinD);

                left_attacks &= moveable_square;
                right_attacks &= moveable_square;

                Bitboard promotions_left  = (left_attacks & rank_promo) * promo_possible;
                Bitboard promotions_right = (right_attacks & rank_promo) * promo_possible;

                BitboardIterator(promotions_left) {
                    Square to = promotions_left.poplsb();
                    movelist.addPromotions(to + DOWN_RIGHT, to);
                }

                BitboardIterator(promotions_right) {
                    Square to = promotions_right.poplsb();
                    movelist.addPromotions(to + DOWN_LEFT, to);
                }

                left_attacks &= ~rank_promo;
                right_attacks &= ~rank_promo;

                BitboardIterator(left_attacks) {
                    Square to = left_attacks.poplsb();
                    movelist.add(Move::makeNormal(to + DOWN_RIGHT, to));
                }

                BitboardIterator(right_attacks) {
                    Square to = right_attacks.poplsb();
                    movelist.add(Move::makeNormal(to + DOWN_LEFT, to));
                }

                Square ep = board.enPassant();

                if (!ep.isValid()) {
                    return;
                }

                if (((Bitboard(ep) | Bitboard(Square(ep + DOWN))) & checkmask).empty()) {
                    return;
                }

                Bitboard ep_bb = Attacks::pawnAttacks<~c>(ep) & pawns_d;

                if (ep_bb.single()) {
                    generateEnpassantMoves<c, false>(board, movelist, ep, ep_bb, all, pinD);
                } else if (ep_bb) {
                    generateEnpassantMoves<c, true>(board, movelist, ep, ep_bb, all, pinD);
                }
            }
        }

        template <typename Function>
        static inline void enumerateMoves(Movelist& movelist, Bitboard mask, Function function) {
            BitboardIterator(mask) {
                const Square from  = mask.poplsb();
                auto         moves = function(from);
                BitboardIterator(moves) {
                    const Square to = moves.poplsb();
                    movelist.add(Move::makeNormal(from, to));
                }
            }
        }

        template <Color c, CastlingSide castleSide>
        static inline void generateCastlingMove(const Board& board, Movelist& movelist, Square kingSq,
                                                Bitboard seen, Bitboard all, Bitboard pinHV) {
            const CastlingRights castlingRights = board.castlingRights();

            if (!castlingRights.hasCastlingRights<c, castleSide>()) {
                return;
            }

            constexpr Square   king_to   = CastlingRights::kingTo<c, castleSide>();
            constexpr Square   rook_to   = CastlingRights::rookTo<c, castleSide>();
            constexpr Square   rook_from = CastlingRights::rookFrom<c, castleSide>();
            constexpr Bitboard rook_from_bb(rook_from);
            constexpr Bitboard not_rook_from_bb = ~rook_from_bb;
            constexpr Bitboard king_to_bb(king_to);
            constexpr Bitboard rook_to_bb(rook_to);

            const Bitboard not_occupied_mask       = Attacks::SQUARES_BETWEEN[kingSq][rook_from];
            const Bitboard not_attacked_mask       = Attacks::SQUARES_BETWEEN[kingSq][king_to];
            const Bitboard empty_not_attacked_mask = ~seen & ~(all & not_rook_from_bb);

            const bool path_not_attacked = (not_attacked_mask & empty_not_attacked_mask) == not_attacked_mask;
            const bool path_not_occupied = (not_occupied_mask & ~all) == not_occupied_mask;

            Bitboard king(kingSq);
            Bitboard no_rook(all & ~not_rook_from_bb);
            Bitboard no_king(all & ~king);

            const bool king_to_safe  = (king_to_bb & (seen | (no_rook & ~king))).empty();
            const bool rook_to_clear = (rook_to_bb & (no_rook & no_king)).empty();
            const bool rook_unpinned = (rook_from_bb & pinHV & Bitboard(kingSq.rank())).empty();

            Bitboard moves = rook_from_bb * (path_not_attacked && path_not_occupied && king_to_safe &&
                                             rook_to_clear && rook_unpinned);

            BitboardIterator(moves) {
                Square to = moves.poplsb();
                movelist.add(Move::makeCastling(kingSq, to));
            }
        }

        template <Color c, MoveGenType mt>
        static inline void generateKingMoves(const Board& board, Movelist& movelist, Bitboard moveable_squares,
                                             Bitboard seen, Bitboard all, Bitboard pinHV) {
            Square   kingSq = board.kingSq<c>();
            Bitboard king   = Bitboard(kingSq);

            enumerateMoves(movelist, king,
                           [&](Square sq) { return Attacks::kingAttacks(sq) & moveable_squares & ~seen; });

            if constexpr (mt != MoveGenType::CAPTURE) {
                if (seen & king) {
                    return;
                }
                generateCastlingMove<c, CastlingSide::KING_SIDE>(board, movelist, kingSq, seen, all, pinHV);
                generateCastlingMove<c, CastlingSide::QUEEN_SIDE>(board, movelist, kingSq, seen, all, pinHV);
            }
        }

        // All bits set to 1
        static constexpr inline Bitboard default_checkmask = 0xffffffffffffffffull;

        template <Color c, MoveGenType mt>
        static inline void generateLegalMoves(const Board& board, Movelist& movelist) {
            Bitboard us               = board.us<c>();
            Bitboard them             = board.them<c>();
            Bitboard enemy_or_empty   = ~us;
            Bitboard all              = board.occupied();
            Bitboard moveable_squares = enemy_or_empty;

            if constexpr (mt == MoveGenType::CAPTURE) {
                moveable_squares = them;
            } else if constexpr (mt == MoveGenType::QUIET) {
                moveable_squares = ~all;
            }

            Square kingSq = board.kingSq<c>();

            Bitboard leaper_checkers;
            Bitboard slider_checkers;

            int check_count = generateCheckers<c>(board, kingSq, leaper_checkers, slider_checkers);

            Bitboard checkmask = default_checkmask;

            if (check_count) {
                checkmask = generateCheckMask<c>(kingSq, check_count, leaper_checkers, slider_checkers);
            }

            Bitboard pinD  = pinMaskDiagonal<c>(board, kingSq, them, us);
            Bitboard pinHV = pinMaskHorizontalVertical<c>(board, kingSq, them, us);

            Bitboard seen = generateSeenSquares<~c>(board, all, enemy_or_empty);

            generateKingMoves<c, mt>(board, movelist, moveable_squares, seen, all, pinHV);

            moveable_squares &= checkmask;

            moveable_squares = moveable_squares * !(check_count == 2);

            if (moveable_squares.empty()) {
                return;
            }

            generatePawnMoves<c, mt>(board, movelist, them, all, pinD, pinHV, checkmask);

            Bitboard knights = board.bitboard<c, PieceType::KNIGHT>() & ~(pinD | pinHV);

            enumerateMoves(movelist, knights,
                           [&](Square sq) { return Attacks::knightAttacks(sq) & moveable_squares; });

            Bitboard bishops = board.bitboard<c, PieceType::BISHOP>() & ~pinHV;

            // clang-format off
            enumerateMoves(movelist, bishops,
            [&](Square sq) {
                const bool is_pinned = pinD & Bitboard(sq);

                Bitboard attack = Attacks::bishopAttacks(sq, all);

                if (is_pinned){
                    attack &= pinD;
                }

                return attack & moveable_squares;
             });
            // clang-format on

            Bitboard rooks = board.bitboard<c, PieceType::ROOK>() & ~pinD;

            // clang-format off
            enumerateMoves(movelist, rooks,
            [&](Square sq) {
                const bool is_pinned = pinHV & Bitboard(sq);

                Bitboard attack = Attacks::rookAttacks(sq, all);

                if (is_pinned){
                    attack &= pinHV;
                }

                return attack & moveable_squares;
             });
            // clang-format on

            Bitboard queens = board.bitboard<c, PieceType::QUEEN>() & ~(pinD & pinHV);

            // clang-format off
            enumerateMoves(movelist, queens,
            [&](Square sq) {
                const bool pinned_hv = pinHV & Bitboard(sq);
                const bool pinned_d = pinD & Bitboard(sq);

                Bitboard attacks_d = Attacks::bishopAttacks(sq, all);
                Bitboard attacks_hv = Attacks::rookAttacks(sq, all);
                Bitboard attacks = attacks_d | attacks_hv;

                if (pinned_hv){
                    attacks &= attacks_hv & pinHV;
                } else if (pinned_d) {
                    attacks &= attacks_d & pinD;
                }

                return attacks & moveable_squares;
             });
            // clang-format on
        }

    public:
        template <MoveGenType mt>
        static inline void legalmoves(const Board& board, Movelist& movelist) {
            if (board.sideToMove() == Color::WHITE) {
                generateLegalMoves<Color::WHITE, mt>(board, movelist);
            } else {
                generateLegalMoves<Color::BLACK, mt>(board, movelist);
            }
        }
    };

} // namespace chess