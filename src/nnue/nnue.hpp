#pragma once

#include <array>
#include <cstdint>

#include "accumulator.hpp"
#include "types.hpp"

#include <algorithm>

namespace jet {

    namespace nnue {

        void init();


        extern std::array<int16_t, constants::INPUT_LAYER_SIZE> inputWeights;
        extern std::array<int16_t, constants::N_HIDDEN>      inputBias;

        extern std::array<int16_t, constants::N_L1 * constants::N_L2>  L1_Weights;
        extern std::array<int32_t, constants::N_L2>      L1_Bias;

        extern std::array<float, constants::N_L2 * constants::OUTPUT_SIZE>  L2_Weights;
        extern std::array<float, constants::OUTPUT_SIZE>      L2_Bias;

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
                int16_t activatedInputs[constants::N_L1];

                for (int i = 0; i < constants::N_HIDDEN; ++i){
                    activatedInputs[i] = ReLU(accumulator.data<side>()[i]);
                    activatedInputs[i + constants::N_HIDDEN] = ReLU(accumulator.data<~side>()[i]);
                }

                int32_t l1_outputs[constants::N_L2];

                for (int i = 0; i < constants::N_L2; ++i){
                    l1_outputs[i] = L1_Bias[i];
                }
                
                for (int i = 0; i < constants::N_L2; ++i){
                    for (int j = 0; j < constants::N_L1; j++){
                        l1_outputs[i] += activatedInputs[j] * L1_Weights[i * constants::N_L1 + j];  
                    }
                }

                float output = L2_Bias[0];

                for (int i = 0; i < constants::N_L2; ++i){
                    output += std::max(l1_outputs[i], 0) * L2_Weights[i];
                }

                return output / 32 / 32;
            }

            template <chess::Color side, AccumulatorOP operation>
            void updateAccumulator(chess::PieceType pieceType, chess::Color pieceColor, chess::Square square,
                                   chess::Square kingSq) {
                static_assert(operation != AccumulatorOP::ADD_SUB,
                              "This overloaded function isn't for add sub. Only add or sub.");

                auto&     accumulator = accumulatorStack[currentAccumulator];
                const int inputs      = index<side>(pieceType, pieceColor, square, kingSq);

                if constexpr (operation == AccumulatorOP::ADD) {
                    accumulator.add<side>(inputWeights.data() + inputs * constants::N_HIDDEN);
                } else if constexpr (operation == AccumulatorOP::SUB) {
                    accumulator.sub<side>(inputWeights.data() + inputs * constants::N_HIDDEN);
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

                accumulator.addSub<side>(inputWeights.data() + inputsAdd * constants::N_HIDDEN,
                                         inputWeights.data() + inputsSub * constants::N_HIDDEN);
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
