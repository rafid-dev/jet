#include "nnue/nnue.hpp"

#include <fstream>

#define INCBIN_STYLE INCBIN_STYLE_CAMEL
#include "incbin/incbin.h"
INCBIN(EVAL, "src/net.nn");

namespace jet {
    namespace nnue {

        std::array<int16_t, constants::INPUT_LAYER_SIZE> inputWeights;
        std::array<int16_t, constants::HIDDEN_SIZE>      inputBias;
        std::array<int16_t, constants::HIDDEN_SIZE * 2>  hiddenWeights;
        std::array<int32_t, constants::OUTPUT_SIZE>      hiddenBias;

        void readFromIncludedBinary() {
            uint64_t memoryIndex = 0;

            std::memcpy(inputWeights.data(), &gEVALData[memoryIndex], constants::INPUT_LAYER_SIZE * sizeof(int16_t));
            memoryIndex += constants::INPUT_LAYER_SIZE * sizeof(int16_t);

            std::memcpy(inputBias.data(), &gEVALData[memoryIndex], constants::HIDDEN_SIZE * sizeof(int16_t));
            memoryIndex += constants::HIDDEN_SIZE * sizeof(int16_t);

            std::memcpy(hiddenWeights.data(), &gEVALData[memoryIndex], constants::HIDDEN_LAYER_SIZE * sizeof(int16_t));
            memoryIndex += constants::HIDDEN_LAYER_SIZE * sizeof(int16_t);

            std::memcpy(hiddenBias.data(), &gEVALData[memoryIndex], constants::OUTPUT_LAYER_SIZE * sizeof(int32_t));
            memoryIndex += constants::OUTPUT_LAYER_SIZE * sizeof(int32_t);

#ifdef DEBUG
            std::cout << "Memory index: " << memoryIndex << std::endl;
            std::cout << "Size: " << gEVALSize << std::endl;
            std::cout << "Bias: " << hiddenBias[0] / INPUT_QUANTIZATION / HIDDEN_QUANTIZATON << std::endl;

            std::cout << std::endl;
#endif
        }

        void init(const std::string_view& path) {
            uint64_t bytesRead = 0;

            std::ifstream file(path.data(), std::ios::binary | std::ios::in);

            if (!file) {
                readFromIncludedBinary();
                return;
            }

            file.read(reinterpret_cast<char*>(inputWeights.data()), sizeof(inputWeights));
            bytesRead += sizeof(inputWeights);

            file.read(reinterpret_cast<char*>(inputBias.data()), sizeof(inputBias));
            bytesRead += sizeof(inputBias);

            file.read(reinterpret_cast<char*>(hiddenWeights.data()), sizeof(hiddenWeights));
            bytesRead += sizeof(hiddenWeights);

            file.read(reinterpret_cast<char*>(hiddenBias.data()), sizeof(hiddenBias));
            bytesRead += sizeof(hiddenBias);
        }

    } // namespace nnue
} // namespace jet