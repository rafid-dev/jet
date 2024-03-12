#pragma once

#include "../chess/board.hpp"
#include "../nnue/nnue.hpp"
#include "constants.hpp"
#include "history.hpp"
#include "searchinfo.hpp"
#include "timeman.hpp"

namespace jet {
    namespace search {

        class SearchThread {
        public:
            uint64_t nodes = 0;
            History  history;

            SearchThread() = default;
            SearchThread(const chess::Board& b) : m_board{b} {
            }

            auto& board() {
                return m_board;
            }

            const auto& board() const {
                return m_board;
            }

            void setFen(std::string_view fen) {
                m_board.setFen(fen);
                network.reset();
                refresh();
            }

             void start() {
                refresh();
                stop_flag = false;
                nodes     = 0;
                timeman.start(m_board.sideToMove());

                history.clear();
            }

            TimeManager& timeManager() {
                return timeman;
            }

            bool stop() const {
                return stop_flag && (timeman.timeset || timeman.nodeset());
            }

            void checkup() {
                if (timeman.nodeset()) {
                    stop_flag = timeman.checkNodes(nodes);
                    return;
                }
                if ((nodes & 2047) == 0) {
                    stop_flag = timeman.shouldStop();
                }
            }

            template <bool updateNNUE = false>
            void makeMove(const chess::Move& move) {
                if constexpr (!updateNNUE) {
                    m_board.makeMove(move);
                    return;
                }

                const auto from         = move.from();
                const auto to           = move.to();
                const auto pieceType    = m_board.pieceTypeAt(from);
                const bool is_capture   = m_board.isCapture(move) && move.type() != chess::MoveType::CASTLING;
                const auto capturedType = m_board.pieceTypeAt(to);

                const auto side = m_board.sideToMove();

                const auto kingSqWhite = m_board.kingSq<chess::Color::WHITE>();
                const auto kingSqBlack = m_board.kingSq<chess::Color::BLACK>();

                network.push();

                if (pieceType == chess::PieceType::KING &&
                    (nnue::constants::KING_BUCKET[from ^ (static_cast<bool>(side) * 56)] != nnue::constants::KING_BUCKET[to ^ (static_cast<bool>(side) * 56)] ||
                     static_cast<int>(from.file()) + static_cast<int>(to.file()) == 7)) {
                    m_board.makeMove(move);
                    refresh();

                    return;
                }

                if (is_capture) {
                    network.updateAccumulator<nnue::AccumulatorOP::SUB>(capturedType, ~side, to, kingSqWhite, kingSqBlack);
                }

                if (move.type() == chess::MoveType::CASTLING) {
                    const auto castleSide = chess::CastlingRights::getCastlingSide(move.to(), move.from());

                    const auto rookTo = chess::CastlingRights::rookTo(side, castleSide);
                    const auto kingTo = chess::CastlingRights::kingTo(side, castleSide);

                    if (nnue::constants::KING_BUCKET[from ^ (static_cast<bool>(side) * 56)] != nnue::constants::KING_BUCKET[kingTo ^ (static_cast<bool>(side) * 56)] ||
                        static_cast<int>(from.file()) + static_cast<int>(kingTo.file()) == 7) {
                        m_board.makeMove(move);
                        refresh();
                        return;
                    }

                    network.updateAccumulator<nnue::AccumulatorOP::SUB>(pieceType, side, from, kingSqWhite, kingSqBlack);
                    network.updateAccumulator<nnue::AccumulatorOP::SUB>(capturedType, side, to, kingSqWhite, kingSqBlack);
                    network.updateAccumulator<nnue::AccumulatorOP::ADD>(pieceType, side, kingTo, kingSqWhite, kingSqBlack);
                    network.updateAccumulator<nnue::AccumulatorOP::ADD>(capturedType, side, rookTo, kingSqWhite, kingSqBlack);

                    m_board.makeMove(move);
                    return;
                }

                if (move.type() == chess::MoveType::ENPASSANT) {
                    network.updateAccumulator<nnue::AccumulatorOP::SUB>(
                        chess::PieceType::PAWN, ~side, chess::Square(int(move.to()) ^ 8), kingSqWhite, kingSqBlack);
                }

                if (move.type() == chess::MoveType::PROMOTION) {
                    network.updateAccumulator<nnue::AccumulatorOP::SUB>(chess::PieceType::PAWN, side, from, kingSqWhite,
                                                                        kingSqBlack);
                    network.updateAccumulator<nnue::AccumulatorOP::ADD>(move.promoted(), side, to, kingSqWhite, kingSqBlack);

                    m_board.makeMove(move);
                    return;
                }

                network.updateAccumulator<nnue::AccumulatorOP::ADD_SUB>(pieceType, side, from, to, kingSqWhite, kingSqBlack);

                m_board.makeMove(move);
            }

            template <bool updateNNUE = false>
            void unmakeMove(const chess::Move& move) {
                m_board.unmakeMove(move);

                if constexpr (updateNNUE) {
                    network.pull();
                }
            }

            int32_t eval() {
                if (m_board.sideToMove() == chess::Color::WHITE) {
                    return network.eval<chess::Color::WHITE>();
                } else {
                    return network.eval<chess::Color::BLACK>();
                }
            }

            void refresh() {
                network.resetCurrentAccumulator();

                chess::Bitboard pieces = m_board.all();

                const chess::Square kingSqWhite = m_board.kingSq<chess::Color::WHITE>();
                const chess::Square kingSqBlack = m_board.kingSq<chess::Color::BLACK>();

                while (pieces.nonEmpty()) {
                    const chess::Square sq = pieces.poplsb();

                    const chess::PieceType pieceType = m_board.pieceTypeAt(sq);
                    const chess::Color     side      = m_board.colorOf(sq);

                    network.updateAccumulator<nnue::AccumulatorOP::ADD>(pieceType, side, sq, kingSqWhite, kingSqBlack);
                }
            }

        private:
            chess::Board m_board;
            TimeManager  timeman;
            bool         stop_flag = false;

            nnue::Network network;
        };
    } // namespace search
} // namespace jet
