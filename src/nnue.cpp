#include "nnue/nnue.hpp"

#include <fstream>

#define INCBIN_STYLE INCBIN_STYLE_CAMEL
#include "incbin/incbin.h"
INCBIN(EVAL, NNFILE);

namespace jet {
    namespace nnue {

        std::array<int16_t, constants::INPUT_LAYER_SIZE> inputWeights;
        std::array<int16_t, constants::HIDDEN_SIZE>      inputBias;
        std::array<int16_t, constants::HIDDEN_SIZE * 2 * constants::OUTPUT_SIZE>  hiddenWeights;
        std::array<int32_t, constants::OUTPUT_SIZE>      hiddenBias;

        template<typename T, size_t N>
        void copyFromBin(std::array<T, N>& array, uint64_t& memoryIndex){
            std::memcpy(array.data(), &gEVALData[memoryIndex], array.size() * sizeof(T));
            memoryIndex += array.size() * sizeof(T);
        }

        void init() {
            uint64_t memoryIndex = 0;

            copyFromBin(inputWeights, memoryIndex);
            copyFromBin(inputBias, memoryIndex);
            copyFromBin(hiddenWeights, memoryIndex);
            copyFromBin(hiddenBias, memoryIndex);

// #ifdef DEBUG
            std::cout << "Memory index: " << memoryIndex << std::endl;
            std::cout << "Size: " << gEVALSize << std::endl;

            std::cout << std::endl;
// #endif
        }

    } // namespace nnue
} // namespace jet