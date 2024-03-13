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
        std::array<int32_t, constants::N_L2>                   L1_Bias;

        std::array<float, constants::N_L2 * constants::OUTPUT_SIZE> L2_Weights;
        std::array<float, constants::OUTPUT_SIZE>                   L2_Bias;

        template <typename T, std::size_t N>
        void copyEVALData(std::array<T, N>& weights, uint64_t& memoryIndex) {
            std::memcpy(weights.data(), &gEVALData[memoryIndex], weights.size() * sizeof(T));
            memoryIndex += weights.size() * sizeof(T);
        }

        void init() {
            uint64_t memoryIndex = 0;

            copyEVALData(inputWeights, memoryIndex);
            copyEVALData(inputBias, memoryIndex);
            copyEVALData(L1_Weights, memoryIndex);
            copyEVALData(L1_Bias, memoryIndex);
            copyEVALData(L2_Weights, memoryIndex);
            copyEVALData(L2_Bias, memoryIndex);

            // #ifdef DEBUG
            std::cout << "Memory index: " << memoryIndex << std::endl;
            std::cout << "Size: " << gEVALSize << std::endl;

            std::cout << std::endl;
            // #endif
        }

    } // namespace nnue
} // namespace jet