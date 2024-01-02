#pragma once

#include <array>
#include <cstdint>

#include "accumulator.hpp"
#include "types.hpp"

namespace jet {

    namespace nnue {

        void init();

        extern std::array<int16_t, constants::INPUT_LAYER_SIZE> inputWeights;
        extern std::array<int16_t, constants::HIDDEN_SIZE>      inputBias;
        extern std::array<int16_t, constants::HIDDEN_SIZE * 2>  hiddenWeights;
        extern std::array<int32_t, constants::OUTPUT_SIZE>      hiddenBias;

        class Network {
        private:
            std::array<Accumulator, 512> accumulatorStack;

            int currentAccumulator = 0;

            static int16_t ReLU(int16_t x) {
                return std::max(x, static_cast<int16_t>(0));
            }

        public:
            Network() = default;

            void resetCurrentAccumulator() {
                accumulatorStack[currentAccumulator].load(inputBias);
            }

            void push() {
                accumulatorStack[currentAccumulator + 1].copy(accumulatorStack[currentAccumulator]);
                currentAccumulator++;
            }

            void pull() {
                currentAccumulator--;
            }

            void reset() {
                currentAccumulator = 0;
            }

            template <chess::Color side>
            int32_t eval() const {
                const auto& accumulator = accumulatorStack[currentAccumulator];

                int32_t output = hiddenBias[0];

                for (int i = 0; i < constants::HIDDEN_SIZE; ++i) {
                    output += ReLU(accumulator.data<side>()[i]) * hiddenWeights[i];
                }

                for (int i = 0; i < constants::HIDDEN_SIZE; ++i) {
                    output += ReLU(accumulator.data<~side>()[i]) * hiddenWeights[constants::HIDDEN_SIZE + i];
                }

                return output / constants::INPUT_QUANTIZATION / constants::HIDDEN_QUANTIZATON;
            }

            template <chess::Color side, AccumulatorOP operation>
            void updateAccumulator(chess::PieceType pieceType, chess::Color pieceColor, chess::Square square,
                                   chess::Square kingSq) {
                static_assert(operation != AccumulatorOP::ADD_SUB,
                              "This overloaded function isn't for add sub. Only add or sub.");

                auto&     accumulator = accumulatorStack[currentAccumulator];
                const int inputs      = index<side>(pieceType, pieceColor, square, kingSq);

                if constexpr (operation == AccumulatorOP::ADD) {
                    accumulator.add<side>(inputWeights.data() + inputs * constants::HIDDEN_SIZE);
                } else if constexpr (operation == AccumulatorOP::SUB) {
                    accumulator.sub<side>(inputWeights.data() + inputs * constants::HIDDEN_SIZE);
                }
            }

            template <chess::Color side, AccumulatorOP operation>
            void updateAccumulator(chess::PieceType pieceType, chess::Color pieceColor, chess::Square from, chess::Square to,
                                   chess::Square kingSq) {
                static_assert(operation == AccumulatorOP::ADD_SUB,
                              "This overloaded function isn't for add sub. Only add or sub.");

                auto& accumulator = accumulatorStack[currentAccumulator];

                const int inputsAdd = index<side>(pieceType, pieceColor, to, kingSq);
                const int inputsSub = index<side>(pieceType, pieceColor, from, kingSq);

                accumulator.addSub<side>(inputWeights.data() + inputsAdd * constants::HIDDEN_SIZE,
                                         inputWeights.data() + inputsSub * constants::HIDDEN_SIZE);
            }

            template <AccumulatorOP operation>
            void updateAccumulator(chess::PieceType pieceType, chess::Color pieceColor, chess::Square square,
                                   chess::Square kingSqWhite, chess::Square kingSqBlack) {
                static_assert(operation != AccumulatorOP::ADD_SUB,
                              "This overloaded function isn't for subbing or adding. Only add sub.");

                if constexpr (operation == AccumulatorOP::ADD) {
                    updateAccumulator<chess::Color::WHITE, AccumulatorOP::ADD>(pieceType, pieceColor, square, kingSqWhite);
                    updateAccumulator<chess::Color::BLACK, AccumulatorOP::ADD>(pieceType, pieceColor, square, kingSqBlack);
                } else if constexpr (operation == AccumulatorOP::SUB) {
                    updateAccumulator<chess::Color::WHITE, AccumulatorOP::SUB>(pieceType, pieceColor, square, kingSqWhite);
                    updateAccumulator<chess::Color::BLACK, AccumulatorOP::SUB>(pieceType, pieceColor, square, kingSqBlack);
                }
            }

            template <AccumulatorOP operation>
            void updateAccumulator(chess::PieceType pieceType, chess::Color pieceColor, chess::Square from, chess::Square to,
                                   chess::Square kingSqWhite, chess::Square kingSqBlack) {
                static_assert(operation == AccumulatorOP::ADD_SUB,
                              "This overloaded function isn't for subbing or adding. Only add sub.");

                updateAccumulator<chess::Color::WHITE, AccumulatorOP::ADD_SUB>(pieceType, pieceColor, from, to, kingSqWhite);
                updateAccumulator<chess::Color::BLACK, AccumulatorOP::ADD_SUB>(pieceType, pieceColor, from, to, kingSqBlack);
            }
        };

    } // namespace nnue

} // namespace jet
