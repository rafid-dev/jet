#pragma once

#include "board.hpp"
#include "movegen.hpp"

namespace chess {
    class MoveGenCountOnly : private MoveGen {
    private:
        template <typename Function>
        static inline void enumerateMoves(Bitboard mask, int& count, Function function) {
            BitboardIterator(mask) {
                const Square from  = mask.poplsb();
                auto         moves = function(from);
                count += moves.popcount();
            }
        }

        template <Color c, bool double_ep_possible>
        static inline void generateEnpassantMoves(const Board& board, Square ep, Bitboard ep_bb, Bitboard all, Bitboard pinD,
                                                  int& count) {
            if constexpr (double_ep_possible) {
                BitboardIterator(ep_bb) {
                    Square from = ep_bb.poplsb();
                    if (!(pinD & Bitboard(from)) && !(pinD & Bitboard(ep))) {
                        count++;
                    }
                }
            } else {
                constexpr Direction down    = relativeDirection<c, Direction::SOUTH>();
                const Square        ep_pawn = ep + down;
                const Square        from    = ep_bb.lsb();

                const Square   kingSq     = board.kingSq<c>();
                const Bitboard rook_queen = board.bitboard<~c, PieceType::ROOK>() | board.bitboard<~c, PieceType::QUEEN>();

                const bool possible_pin = (board.bitboard<c, PieceType::KING>() & Bitboard(ep_pawn.rank())) && rook_queen;

                const Bitboard connectingPawns = Bitboard(ep_pawn) | Bitboard(from);

                if (possible_pin && !(Attacks::rookAttacks(kingSq, all & ~connectingPawns) & rook_queen).empty()) {
                    return;
                }

                count++;
            }
        }

        template <PawnMoveType pmt>
        static constexpr inline void enumerateMoves(Bitboard moves, int& count) {
            if constexpr (pmt == PawnMoveType::Promotion || pmt == PawnMoveType::LeftPromotion ||
                          pmt == PawnMoveType::RightPromotion) {
                count += moves.popcount() * 4;
            } else {
                count += moves.popcount();
            }
        }

        template <Color c, MoveGenType mt>
        static inline void generatePawnMoves(const Board& board, Bitboard them, Bitboard all, Bitboard pinD, Bitboard pinHV,
                                             Bitboard checkmask, int& count) {
            constexpr Bitboard  rank_before_promo = Bitboard(Square::relativeRank<c, Rank::RANK_7>());
            constexpr Direction down              = relativeDirection<c, Direction::SOUTH>();
            const Bitboard      pawns             = board.bitboard<c, PieceType::PAWN>();
            const bool          promo_possible    = !(pawns & rank_before_promo).empty();

            if constexpr (mt != MoveGenType::CAPTURE) {
                const Bitboard moveable_square = ~all & checkmask;

                Bitboard pinned_pushes   = PawnMovesHandler<c, mt>::template pawnPinMaskHV<true>(pawns, all, pinD, pinHV);
                Bitboard unpinned_pushes = PawnMovesHandler<c, mt>::template pawnPinMaskHV<false>(pawns, all, pinD, pinHV);
                Bitboard legal_push  = PawnMovesHandler<c, mt>::legalPawnPush(pinned_pushes, unpinned_pushes, moveable_square);
                Bitboard double_push = PawnMovesHandler<c, mt>::doublePushes(pinned_pushes, unpinned_pushes, moveable_square);
                Bitboard single_push = PawnMovesHandler<c, mt>::singlePushes(legal_push);

                if constexpr (mt != MoveGenType::QUIET) {
                    Bitboard promotions = PawnMovesHandler<c, mt>::promotionPushes(legal_push, promo_possible);
                    enumerateMoves<PawnMoveType::Promotion>(promotions, count);
                }

                enumerateMoves<PawnMoveType::SinglePush>(single_push, count);
                enumerateMoves<PawnMoveType::DoublePush>(double_push, count);
            }

            if constexpr (mt != MoveGenType::QUIET) {
                const Bitboard moveable_square  = them & checkmask;
                const Bitboard pawns_d          = pawns & ~pinHV;
                const Bitboard pinned_pawns_d   = PawnMovesHandler<c, mt>::template pawnPinMaskD<true>(pawns_d, pinD);
                const Bitboard unpinned_pawns_d = PawnMovesHandler<c, mt>::template pawnPinMaskD<false>(pawns_d, pinD);

                Bitboard legal_left = PawnMovesHandler<c, mt>::template legalCaptures<Direction::WEST>(
                    pinned_pawns_d, unpinned_pawns_d, pinD, moveable_square);
                Bitboard legal_right = PawnMovesHandler<c, mt>::template legalCaptures<Direction::EAST>(
                    pinned_pawns_d, unpinned_pawns_d, pinD, moveable_square);

                Bitboard promotions_left  = PawnMovesHandler<c, mt>::promotionCaptures(legal_left, promo_possible);
                Bitboard promotions_right = PawnMovesHandler<c, mt>::promotionCaptures(legal_right, promo_possible);
                Bitboard left_attacks     = PawnMovesHandler<c, mt>::captures(legal_left);
                Bitboard right_attacks    = PawnMovesHandler<c, mt>::captures(legal_right);

                enumerateMoves<PawnMoveType::LeftPromotion>(promotions_left, count);
                enumerateMoves<PawnMoveType::RightPromotion>(promotions_right, count);
                enumerateMoves<PawnMoveType::LeftCapture>(left_attacks, count);
                enumerateMoves<PawnMoveType::RightCapture>(right_attacks, count);

                Square ep = board.enPassant();

                if (!ep.isValid()) {
                    return;
                }

                if (((Bitboard(ep) | Bitboard(Square(ep + down))) & checkmask).empty()) {
                    return;
                }

                Bitboard ep_bb = Attacks::pawnAttacks<~c>(ep) & pawns_d;

                if (ep_bb.single()) {
                    generateEnpassantMoves<c, false>(board, ep, ep_bb, all, pinD, count);
                } else if (ep_bb) {
                    generateEnpassantMoves<c, true>(board, ep, ep_bb, all, pinD, count);
                }
            }
        }

        template <Color c, MoveGenType mt>
        static inline void generateKingMoves(const Board& board, Bitboard moveable_squares, Bitboard seen, Bitboard all,
                                             Bitboard pinHV, int& count) {
            Square   kingSq = board.kingSq<c>();
            Bitboard king   = Bitboard(kingSq);

            enumerateMoves(king, count, [&](Square sq) { return Attacks::kingAttacks(sq) & moveable_squares & ~seen; });

            if constexpr (mt != MoveGenType::CAPTURE) {
                if (seen & king) {
                    return;
                }
                count += !generateCastlingMove<c, CastlingSide::KING_SIDE>(board, kingSq, seen, all, pinHV).empty();
                count += !generateCastlingMove<c, CastlingSide::QUEEN_SIDE>(board, kingSq, seen, all, pinHV).empty();
            }
        }

        template <Color c, MoveGenType mt>
        static inline void generateLegalMoves(const Board& board, int& count) {
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
            Bitboard seen  = generateSeenSquares<~c>(board, all, enemy_or_empty);

            generateKingMoves<c, mt>(board, moveable_squares, seen, all, pinHV, count);

            moveable_squares &= checkmask;

            moveable_squares = moveable_squares * !(check_count == 2);

            if (moveable_squares.empty()) {
                return;
            }

            generatePawnMoves<c, mt>(board, them, all, pinD, pinHV, checkmask, count);

            Bitboard knights = board.bitboard<c, PieceType::KNIGHT>() & ~(pinD | pinHV);

            enumerateMoves(knights, count, [&](Square sq) { return Attacks::knightAttacks(sq) & moveable_squares; });

            Bitboard bishops = board.bitboard<c, PieceType::BISHOP>() & ~pinHV;

            // clang-format off
            enumerateMoves(bishops,count,
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
            enumerateMoves(rooks,count,
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
            enumerateMoves(queens, count,
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
        static inline int legalmoves(const Board& board) {
            int count = 0;
            if (board.sideToMove() == Color::WHITE) {
                generateLegalMoves<Color::WHITE, mt>(board, count);
            } else {
                generateLegalMoves<Color::BLACK, mt>(board, count);
            }
            return count;
        }
    };
} // namespace chess