#include "nnue/nnue.hpp"

#include <fstream>

#define INCBIN_STYLE INCBIN_STYLE_CAMEL
#include "incbin/incbin.h"
INCBIN(EVAL, NNFILE);

namespace jet {
    namespace nnue {

        std::array<int16_t, constants::INPUT_LAYER_SIZE> inputWeights;
        std::array<int16_t, constants::HIDDEN_SIZE>      inputBias;
        std::array<int16_t, constants::HIDDEN_SIZE * 2>  hiddenWeights;
        std::array<int32_t, constants::OUTPUT_SIZE>      hiddenBias;

        template<typename T, size_t SIZE>
        void copyDataFromMemory(std::array<T, SIZE>& dataArray, uint64_t& memoryIndex) {
            constexpr auto size = sizeof(T) * SIZE;
            std::memcpy(dataArray.data(), &gEVALData[memoryIndex], size);
            memoryIndex += size;
        }

        void init() {
            uint64_t memoryIndex = 0;

            copyDataFromMemory(inputWeights, memoryIndex);
            copyDataFromMemory(inputBias, memoryIndex);
            copyDataFromMemory(hiddenWeights, memoryIndex);
            copyDataFromMemory(hiddenBias, memoryIndex);

#ifdef DEBUG
            std::cout << "Memory index: " << memoryIndex << std::endl;
            std::cout << "Size: " << gEVALSize << std::endl;
#endif
        }

    } // namespace nnue
} // namespace jet