#pragma once

#include "board.hpp"
#include "moves.hpp"

namespace chess {

    enum class MoveGenType : uint8_t { ALL, QUIET, CAPTURE };

    enum class PawnMoveType : uint8_t {
        SinglePush,
        DoublePush,
        LeftCapture,
        RightCapture,
        Promotion,
        LeftPromotion,
        RightPromotion,
    };

    namespace movegen_detail {

        struct MoveListSink {
            Movelist& movelist;

            inline void add(Move move) {
                movelist.add(move);
            }

            inline void addNormalMoves(Square from, Bitboard destinations) {
                BitboardIterator(destinations) {
                    movelist.add(Move::makeNormal(from, destinations.poplsb()));
                }
            }

            inline void addPromotions(Square from, Square to) {
                movelist.addPromotions(from, to);
            }
        };

        struct MoveCountSink {
            int count = 0;

            inline void add(Move) {
                ++count;
            }

            inline void addNormalMoves(Square, Bitboard destinations) {
                count += destinations.popcount();
            }

            inline void addPromotions(Square, Square) {
                count += 4;
            }
        };

    } // namespace movegen_detail

    template <Color c, MoveGenType mt>
    class PawnMovesHandler {
    private:
        static inline constexpr Bitboard rank_before_promo  = Bitboard(Square::relativeRank<c, Rank::RANK_7>());
        static inline constexpr Bitboard rank_promo         = Bitboard(Square::relativeRank<c, Rank::RANK_8>());
        static inline constexpr Bitboard single_pushed_rank = Bitboard(Square::relativeRank<c, Rank::RANK_3>());

        static inline constexpr Direction down       = relativeDirection<c, Direction::SOUTH>();
        static inline constexpr Direction up         = relativeDirection<c, Direction::NORTH>();
        static inline constexpr Direction down_left  = relativeDirection<c, Direction::SOUTH_WEST>();
        static inline constexpr Direction down_right = relativeDirection<c, Direction::SOUTH_EAST>();

        template <typename Sink>
        static inline void generateEnpassantMoves(const Board& board, Sink& sink, Square ep, Bitboard candidates, Bitboard all,
                                                  Bitboard pinD) {
            if constexpr (mt == MoveGenType::QUIET) {
                return;
            }

            const Square capturedPawnSquare = ep + down;
            const Square king               = board.kingSq<c>();

            const Bitboard enemyRooksQueens = board.bitboard<~c, PieceType::ROOK>() | board.bitboard<~c, PieceType::QUEEN>();

            const Bitboard enemyBishopsQueens =
                board.bitboard<~c, PieceType::BISHOP>() | board.bitboard<~c, PieceType::QUEEN>();

            BitboardIterator(candidates) {
                const Square from = candidates.poplsb();

                // A diagonally pinned pawn may capture only along its pin ray.
                if ((Bitboard(from) & pinD) && !(Bitboard(ep) & pinD)) {
                    continue;
                }

                // En passant removes two pieces from their original squares, so
                // legality must be checked using the resulting occupancy.
                Bitboard occupancyAfter = all;
                occupancyAfter &= ~Bitboard(from);
                occupancyAfter &= ~Bitboard(capturedPawnSquare);
                occupancyAfter |= Bitboard(ep);

                if (Attacks::rookAttacks(king, occupancyAfter) & enemyRooksQueens) {
                    continue;
                }

                if (Attacks::bishopAttacks(king, occupancyAfter) & enemyBishopsQueens) {
                    continue;
                }

                sink.add(Move::makeEnpassant(from, ep));
            }
        }

        template <PawnMoveType pmt, typename Sink>
        static inline void enumerateMoves(Sink& sink, Bitboard moves) {
            BitboardIterator(moves) {
                const Square to = moves.poplsb();

                if constexpr (pmt == PawnMoveType::SinglePush) {
                    sink.add(Move::makeNormal(to + down, to));
                } else if constexpr (pmt == PawnMoveType::DoublePush) {
                    sink.add(Move::makeNormal(to + down + down, to));
                } else if constexpr (pmt == PawnMoveType::LeftCapture) {
                    sink.add(Move::makeNormal(to + down_right, to));
                } else if constexpr (pmt == PawnMoveType::RightCapture) {
                    sink.add(Move::makeNormal(to + down_left, to));
                } else if constexpr (pmt == PawnMoveType::Promotion) {
                    sink.addPromotions(to + down, to);
                } else if constexpr (pmt == PawnMoveType::LeftPromotion) {
                    sink.addPromotions(to + down_right, to);
                } else if constexpr (pmt == PawnMoveType::RightPromotion) {
                    sink.addPromotions(to + down_left, to);
                }
            }
        }

    public:
        template <bool pinned>
        static constexpr inline Bitboard pawnPinMaskHV(Bitboard pawns, Bitboard all, Bitboard pinD, Bitboard pinHV) {
            const Bitboard pawns_hv = pawns & ~pinD;

            if constexpr (pinned) {
                return (pawns_hv & ~pinHV).shift<up>() & ~all;
            } else {
                return (pawns_hv & pinHV).shift<up>() & pinHV & ~all;
            }
        }

        template <bool pinned>
        static constexpr inline Bitboard pawnPinMaskD(Bitboard pawns_d, Bitboard pinD) {
            if constexpr (pinned) {
                return pawns_d & pinD;
            } else {
                return pawns_d & ~pinD;
            }
        }

        static constexpr inline Bitboard legalPawnPush(Bitboard pinned_pushes, Bitboard unpinned_pushes,
                                                       Bitboard moveable_square) {
            return (pinned_pushes | unpinned_pushes) & moveable_square;
        }

        static constexpr inline Bitboard promotionPushes(Bitboard legal_pushes, bool promo_possible) {
            return (legal_pushes & rank_promo) * promo_possible;
        }

        static constexpr inline Bitboard singlePushes(Bitboard legal_pushes) {
            return legal_pushes & ~rank_promo;
        }

        static constexpr inline Bitboard doublePushes(Bitboard pinned_pushes, Bitboard unpinned_pushes,
                                                      Bitboard moveable_square) {
            Bitboard double_push =
                (pinned_pushes & single_pushed_rank).shift<up>() | (unpinned_pushes & single_pushed_rank).shift<up>();

            return double_push & moveable_square;
        }

        template <Direction d>
        static constexpr inline Bitboard legalCaptures(Bitboard pinned_pawns_d, Bitboard unpinned_pawns_d, Bitboard pinD,
                                                       Bitboard moveable_squares) {
            static_assert(d == Direction::WEST || d == Direction::EAST, "Pawn captures can only be either west or east");

            if constexpr (d == Direction::WEST) {
                return ((Attacks::pawnLeftAttacks<c>(unpinned_pawns_d)) |
                        (Attacks::pawnLeftAttacks<c>(pinned_pawns_d) & pinD)) &
                       moveable_squares;
            } else {
                return ((Attacks::pawnRightAttacks<c>(unpinned_pawns_d)) |
                        (Attacks::pawnRightAttacks<c>(pinned_pawns_d) & pinD)) &
                       moveable_squares;
            }
        }

        static constexpr inline Bitboard captures(Bitboard attacks) {
            return attacks & ~rank_promo;
        }

        static constexpr inline Bitboard promotionCaptures(Bitboard attacks, bool promo_possible) {
            return (attacks & rank_promo) * promo_possible;
        }

        template <typename Sink>
        static inline void generatePawnMoves(const Board& board, Sink& sink, Bitboard them, Bitboard all, Bitboard pinD,
                                             Bitboard pinHV, Bitboard checkmask) {
            const Bitboard pawns          = board.bitboard<c, PieceType::PAWN>();
            const bool     promo_possible = !(pawns & rank_before_promo).empty();

            if constexpr (mt != MoveGenType::CAPTURE) {
                const Bitboard moveable_square = ~all & checkmask;

                const Bitboard pinned_pushes   = pawnPinMaskHV<true>(pawns, all, pinD, pinHV);
                const Bitboard unpinned_pushes = pawnPinMaskHV<false>(pawns, all, pinD, pinHV);
                const Bitboard legal_push      = legalPawnPush(pinned_pushes, unpinned_pushes, moveable_square);
                const Bitboard double_push     = doublePushes(pinned_pushes, unpinned_pushes, moveable_square);
                const Bitboard single_push     = singlePushes(legal_push);

                if constexpr (mt != MoveGenType::QUIET) {
                    enumerateMoves<PawnMoveType::Promotion>(sink, promotionPushes(legal_push, promo_possible));
                }

                enumerateMoves<PawnMoveType::SinglePush>(sink, single_push);
                enumerateMoves<PawnMoveType::DoublePush>(sink, double_push);
            }

            if constexpr (mt != MoveGenType::QUIET) {
                const Bitboard moveable_square  = them & checkmask;
                const Bitboard pawns_d          = pawns & ~pinHV;
                const Bitboard pinned_pawns_d   = pawnPinMaskD<true>(pawns_d, pinD);
                const Bitboard unpinned_pawns_d = pawnPinMaskD<false>(pawns_d, pinD);

                const Bitboard legal_left =
                    legalCaptures<Direction::WEST>(pinned_pawns_d, unpinned_pawns_d, pinD, moveable_square);
                const Bitboard legal_right =
                    legalCaptures<Direction::EAST>(pinned_pawns_d, unpinned_pawns_d, pinD, moveable_square);

                enumerateMoves<PawnMoveType::LeftPromotion>(sink, promotionCaptures(legal_left, promo_possible));
                enumerateMoves<PawnMoveType::RightPromotion>(sink, promotionCaptures(legal_right, promo_possible));
                enumerateMoves<PawnMoveType::LeftCapture>(sink, captures(legal_left));
                enumerateMoves<PawnMoveType::RightCapture>(sink, captures(legal_right));

                const Square ep = board.enPassant();

                if (!ep.isValid()) {
                    return;
                }

                if (((Bitboard(ep) | Bitboard(ep + down)) & checkmask).empty()) {
                    return;
                }

                const Bitboard candidates = Attacks::pawnAttacks<~c>(ep) & pawns_d;
                generateEnpassantMoves(board, sink, ep, candidates, all, pinD);
            }
        }
    };

    class MoveGen {
    public:
        static constexpr inline Bitboard default_checkmask = 0xffffffffffffffffULL;

        template <MoveGenType mt>
        static inline void legalmoves(const Board& board, Movelist& movelist) {
            movegen_detail::MoveListSink sink{movelist};
            dispatch<mt>(board, sink);
        }

        template <MoveGenType mt>
        static inline int countLegalMoves(const Board& board) {
            movegen_detail::MoveCountSink sink;
            dispatch<mt>(board, sink);
            return sink.count;
        }

    private:
        template <Color c>
        static inline Bitboard getLeaperAttacks(const Board& board) {
            Bitboard attacks;
            Bitboard pawns = board.bitboard<c, PieceType::PAWN>();

            attacks |= Attacks::pawnLeftAttacks<c>(pawns) | Attacks::pawnRightAttacks<c>(pawns);

            Bitboard knights = board.bitboard<c, PieceType::KNIGHT>();
            BitboardIterator(knights) {
                attacks |= Attacks::knightAttacks(knights.poplsb());
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
                attacks |= Attacks::bishopAttacks(bishops.poplsb(), all);
            }

            BitboardIterator(rooks) {
                attacks |= Attacks::rookAttacks(rooks.poplsb(), all);
            }

            return attacks;
        }

        template <Color c>
        static inline Bitboard generateSeenSquares(const Board& board, Bitboard all) {
            const Square opposingKing = board.kingSq<~c>();

            all &= ~Bitboard(opposingKing);

            return getLeaperAttacks<c>(board) | getSliderAttacks<c>(board, all);
        }

        template <Color c>
        static inline int generateCheckers(const Board& board, Square kingSq, Bitboard& leaperCheckers,
                                           Bitboard& sliderCheckers) {
            const Bitboard knight_attacks = Attacks::knightAttacks(kingSq) & board.bitboard<~c, PieceType::KNIGHT>();
            const Bitboard pawn_attacks   = Attacks::pawnAttacks<c>(kingSq) & board.bitboard<~c, PieceType::PAWN>();

            const Bitboard bishop_attacks = Attacks::bishopAttacks(kingSq, board.occupied()) &
                                            (board.bitboard<~c, PieceType::BISHOP>() | board.bitboard<~c, PieceType::QUEEN>());

            const Bitboard rook_attacks = Attacks::rookAttacks(kingSq, board.occupied()) &
                                          (board.bitboard<~c, PieceType::ROOK>() | board.bitboard<~c, PieceType::QUEEN>());

            leaperCheckers = knight_attacks | pawn_attacks;
            sliderCheckers = rook_attacks | bishop_attacks;

            return (leaperCheckers | sliderCheckers).popcount();
        }

        template <Color c>
        static inline Bitboard generateCheckMask(Square kingSq, int check_count, Bitboard leaperCheckers,
                                                 Bitboard sliderCheckers) {
            if (check_count >= 2) {
                return Bitboard(0);
            }

            Bitboard checkmask = leaperCheckers;

            if (sliderCheckers.nonEmpty()) {
                const Square checker = sliderCheckers.lsb();

                checkmask |= Attacks::squaresBetween(kingSq, checker);

                // Required if squaresBetween excludes its endpoints.
                checkmask |= Bitboard(checker);
            }

            return checkmask;
        }

        template <Color c>
        static inline Bitboard pinMaskHorizontalVertical(const Board& board, Square kingSq, Bitboard them, Bitboard us) {
            Bitboard pinHV;

            Bitboard pinners = Attacks::rookAttacks(kingSq, them) &
                               (board.bitboard<~c, PieceType::ROOK>() | board.bitboard<~c, PieceType::QUEEN>());

            BitboardIterator(pinners) {
                const Square   pinner       = pinners.poplsb();
                const Bitboard possible_pin = Attacks::squaresBetween(kingSq, pinner);
                pinHV |= possible_pin * (possible_pin & us).single();
            }

            return pinHV;
        }

        template <Color c>
        static inline Bitboard pinMaskDiagonal(const Board& board, Square kingSq, Bitboard them, Bitboard us) {
            Bitboard pinD;

            Bitboard pinners = Attacks::bishopAttacks(kingSq, them) &
                               (board.bitboard<~c, PieceType::BISHOP>() | board.bitboard<~c, PieceType::QUEEN>());

            BitboardIterator(pinners) {
                const Square   pinner       = pinners.poplsb();
                const Bitboard possible_pin = Attacks::squaresBetween(kingSq, pinner);
                pinD |= possible_pin * (possible_pin & us).single();
            }

            return pinD;
        }

        template <typename Sink, typename Function>
        static inline void enumerateMoves(Sink& sink, Bitboard pieces, Function&& attacks) {
            BitboardIterator(pieces) {
                const Square from = pieces.poplsb();
                sink.addNormalMoves(from, attacks(from));
            }
        }

        template <Color c, CastlingSide castleSide>
        static inline Bitboard generateCastlingMove(const Board& board, Square kingSq, Bitboard seen, Bitboard all,
                                                    Bitboard pinHV) {
            const CastlingRights castlingRights = board.castlingRights();

            if (!castlingRights.hasCastlingRights<c, castleSide>()) {
                return Bitboard(0);
            }

            constexpr Square   king_to          = CastlingRights::kingTo<c, castleSide>();
            constexpr Square   rook_to          = CastlingRights::rookTo<c, castleSide>();
            constexpr Square   rook_from        = CastlingRights::rookFrom<c, castleSide>();
            constexpr Bitboard rook_from_bb     = Bitboard(rook_from);
            constexpr Bitboard not_rook_from_bb = ~rook_from_bb;
            constexpr Bitboard king_to_bb       = Bitboard(king_to);
            constexpr Bitboard rook_to_bb       = Bitboard(rook_to);

            const Bitboard not_occupied_mask       = Attacks::SQUARES_BETWEEN[kingSq][rook_from];
            const Bitboard not_attacked_mask       = Attacks::SQUARES_BETWEEN[kingSq][king_to];
            const Bitboard empty_not_attacked_mask = ~seen & ~(all & not_rook_from_bb);

            const bool path_not_attacked = (not_attacked_mask & empty_not_attacked_mask) == not_attacked_mask;
            const bool path_not_occupied = (not_occupied_mask & ~all) == not_occupied_mask;

            const Bitboard king    = Bitboard(kingSq);
            const Bitboard no_rook = all & ~not_rook_from_bb;
            const Bitboard no_king = all & ~king;

            const bool king_to_safe  = (king_to_bb & (seen | (no_rook & ~king))).empty();
            const bool rook_to_clear = (rook_to_bb & (no_rook & no_king)).empty();
            const bool rook_unpinned = (rook_from_bb & pinHV & Bitboard(kingSq.rank())).empty();

            return rook_from_bb * (path_not_attacked && path_not_occupied && king_to_safe && rook_to_clear && rook_unpinned);
        }

        template <Color c, CastlingSide castleSide, typename Sink>
        static inline void emitCastlingMove(const Board& board, Sink& sink, Square kingSq, Bitboard seen, Bitboard all,
                                            Bitboard pinHV) {
            Bitboard destinations = generateCastlingMove<c, castleSide>(board, kingSq, seen, all, pinHV);

            BitboardIterator(destinations) {
                sink.add(Move::makeCastling(kingSq, destinations.poplsb()));
            }
        }

        template <Color c, MoveGenType mt, typename Sink>
        static inline void generateKingMoves(const Board& board, Sink& sink, Bitboard moveable_squares, Bitboard seen,
                                             Bitboard all, Bitboard pinHV) {
            const Square   kingSq = board.kingSq<c>();
            const Bitboard king   = Bitboard(kingSq);

            enumerateMoves(sink, king, [&](Square sq) { return Attacks::kingAttacks(sq) & moveable_squares & ~seen; });

            if constexpr (mt != MoveGenType::CAPTURE) {
                if (seen & king) {
                    return;
                }

                emitCastlingMove<c, CastlingSide::KING_SIDE>(board, sink, kingSq, seen, all, pinHV);
                emitCastlingMove<c, CastlingSide::QUEEN_SIDE>(board, sink, kingSq, seen, all, pinHV);
            }
        }

        template <Color c, MoveGenType mt, typename Sink>
        static inline void generateLegalMoves(const Board& board, Sink& sink) {
            const Bitboard us             = board.us<c>();
            const Bitboard them           = board.them<c>();
            const Bitboard enemy_or_empty = ~us;
            const Bitboard all            = board.occupied();

            Bitboard moveable_squares = enemy_or_empty;

            if constexpr (mt == MoveGenType::CAPTURE) {
                moveable_squares = them;
            } else if constexpr (mt == MoveGenType::QUIET) {
                moveable_squares = ~all;
            }

            const Square kingSq = board.kingSq<c>();

            Bitboard leaper_checkers;
            Bitboard slider_checkers;

            const int check_count = generateCheckers<c>(board, kingSq, leaper_checkers, slider_checkers);

            Bitboard checkmask = default_checkmask;

            if (check_count != 0) {
                checkmask = generateCheckMask<c>(kingSq, check_count, leaper_checkers, slider_checkers);
            }

            const Bitboard pinD  = pinMaskDiagonal<c>(board, kingSq, them, us);
            const Bitboard pinHV = pinMaskHorizontalVertical<c>(board, kingSq, them, us);
            const Bitboard seen  = generateSeenSquares<~c>(board, all);

            generateKingMoves<c, mt>(board, sink, moveable_squares, seen, all, pinHV);

            if (check_count >= 2) {
                return;
            }

            moveable_squares &= checkmask;

            PawnMovesHandler<c, mt>::generatePawnMoves(board, sink, them, all, pinD, pinHV, checkmask);

            const Bitboard knights = board.bitboard<c, PieceType::KNIGHT>() & ~(pinD | pinHV);

            enumerateMoves(sink, knights, [&](Square sq) { return Attacks::knightAttacks(sq) & moveable_squares; });

            const Bitboard bishops = board.bitboard<c, PieceType::BISHOP>() & ~pinHV;

            enumerateMoves(sink, bishops, [&](Square sq) {
                Bitboard attacks = Attacks::bishopAttacks(sq, all);

                if (pinD & Bitboard(sq)) {
                    attacks &= pinD;
                }

                return attacks & moveable_squares;
            });

            const Bitboard rooks = board.bitboard<c, PieceType::ROOK>() & ~pinD;

            enumerateMoves(sink, rooks, [&](Square sq) {
                Bitboard attacks = Attacks::rookAttacks(sq, all);

                if (pinHV & Bitboard(sq)) {
                    attacks &= pinHV;
                }

                return attacks & moveable_squares;
            });

            const Bitboard queens = board.bitboard<c, PieceType::QUEEN>() & ~(pinD & pinHV);

            enumerateMoves(sink, queens, [&](Square sq) {
                const bool pinned_hv = pinHV & Bitboard(sq);
                const bool pinned_d  = pinD & Bitboard(sq);

                const Bitboard attacks_d  = Attacks::bishopAttacks(sq, all);
                const Bitboard attacks_hv = Attacks::rookAttacks(sq, all);
                Bitboard       attacks    = attacks_d | attacks_hv;

                if (pinned_hv) {
                    attacks &= attacks_hv & pinHV;
                } else if (pinned_d) {
                    attacks &= attacks_d & pinD;
                }

                return attacks & moveable_squares;
            });
        }

        template <MoveGenType mt, typename Sink>
        static inline void dispatch(const Board& board, Sink& sink) {
            if (board.sideToMove() == Color::WHITE) {
                generateLegalMoves<Color::WHITE, mt>(board, sink);
            } else {
                generateLegalMoves<Color::BLACK, mt>(board, sink);
            }
        }
    };

} // namespace chess