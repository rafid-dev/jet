#pragma once

#include <array>
#include <cstdint>

#include "types.hpp"

namespace jet {

    namespace nnue {

        enum class AccumulatorOP : uint8_t {
            ADD,
            SUB,
            ADD_SUB,
        };

        template <typename T>
        class AccumulatorBase {
        private:
            using Weights = std::array<T, constants::HIDDEN_SIZE>;

            // 0 = white POV, 1 = black POV
            std::array<Weights, 2> weights;

        public:
            AccumulatorBase() = default;

            void load(const Weights& bias) {
                std::memcpy(weights[0].data(), bias.data(), sizeof(Weights));
                std::memcpy(weights[1].data(), bias.data(), sizeof(Weights));
            }

            template <chess::Color c>
            void add(const T* input) {
                for (int i = 0; i < constants::HIDDEN_SIZE; ++i) {
                    weights[static_cast<int>(c)][i] += input[i];
                }
            }

            template <chess::Color c>
            void sub(const T* input) {
                for (int i = 0; i < constants::HIDDEN_SIZE; ++i) {
                    weights[static_cast<int>(c)][i] -= input[i];
                }
            }

            template <chess::Color c>
            void addSub(const T* inputAdd, const T* inputSub) {
                for (int i = 0; i < constants::HIDDEN_SIZE; ++i) {
                    weights[static_cast<int>(c)][i] += inputAdd[i] - inputSub[i];
                }
            }

            template <chess::Color c>
            const auto& data() const {
                return weights[static_cast<int>(c)];
            }

            void copy(const AccumulatorBase& other) {
                std::memcpy(weights[0].data(), other.weights[0].data(), sizeof(Weights));
                std::memcpy(weights[1].data(), other.weights[1].data(), sizeof(Weights));
            }
        };

        using Accumulator = AccumulatorBase<int16_t>;

    } // namespace nnue

} // namespace jet
