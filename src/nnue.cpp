#include "nnue/nnue.hpp"

#include <fstream>

#define INCBIN_STYLE INCBIN_STYLE_CAMEL
#include "incbin/incbin.h"
INCBIN(EVAL, NNFILE);

namespace jet {
    namespace nnue {

        std::array<int16_t, constants::INPUT_LAYER_SIZE> inputWeights;
        std::array<int16_t, constants::N_HIDDEN>         inputBias;

        std::array<int16_t, constants::N_L1 * constants::N_L2> L1_Weights;
        std::array<float, constants::N_L2>                   L1_Bias;

        std::array<float, constants::N_L2 * constants::OUTPUT_SIZE> L2_Weights;
        std::array<float, constants::OUTPUT_SIZE>                   L2_Bias;

        template <typename T, std::size_t N>
        void copyEVALData(std::array<T, N>& weights, uint64_t& memoryIndex) {
            std::memcpy(weights.data(), &gEVALData[memoryIndex], weights.size() * sizeof(T));
            memoryIndex += weights.size() * sizeof(T);
        }

        void init() {
            uint64_t memoryIndex = 0;

            // std::memcpy(inputWeights.data(), &gEVALData[memoryIndex], constants::INPUT_LAYER_SIZE * sizeof(int16_t));
            // memoryIndex += constants::INPUT_LAYER_SIZE * sizeof(int16_t);

            // std::memcpy(inputBias.data(), &gEVALData[memoryIndex], constants::N_L1 * sizeof(int16_t));
            // memoryIndex += constants::N_L1 * sizeof(int16_t);

            // std::memcpy(L1_Weights.data(), &gEVALData[memoryIndex], constants::HIDDEN_LAYER_SIZE * sizeof(int16_t));
            // memoryIndex += constants::HIDDEN_LAYER_SIZE * sizeof(int16_t);

            // std::memcpy(L1_Bias.data(), &gEVALData[memoryIndex], constants::OUTPUT_LAYER_SIZE * sizeof(int32_t));
            // memoryIndex += constants::OUTPUT_LAYER_SIZE * sizeof(int32_t);

            copyEVALData(inputWeights, memoryIndex);
            copyEVALData(inputBias, memoryIndex);
            copyEVALData(L1_Weights, memoryIndex);
            copyEVALData(L1_Bias, memoryIndex);
            copyEVALData(L2_Weights, memoryIndex);
            copyEVALData(L2_Bias, memoryIndex);

            // #ifdef DEBUG
            std::cout << "Memory index: " << memoryIndex << std::endl;
            std::cout << "Size: " << gEVALSize << std::endl;
            std::cout << "Bias: " << L2_Bias[0] / 32 << std::endl;

            std::cout << std::endl;
            // #endif
        }

    } // namespace nnue
} // namespace jet