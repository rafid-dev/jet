#include <array>

namespace jet {
    namespace nnue {

        namespace constants {

            constexpr int BUCKETS     = 32;
            constexpr int INPUT_SIZE  = 64 * 6 * 2 * BUCKETS;
            constexpr int HIDDEN_SIZE = 384;
            constexpr int OUTPUT_SIZE = 1;

            constexpr int INPUT_LAYER_SIZE  = INPUT_SIZE * HIDDEN_SIZE;
            constexpr int HIDDEN_LAYER_SIZE = HIDDEN_SIZE * 2;
            constexpr int OUTPUT_LAYER_SIZE = OUTPUT_SIZE;

            constexpr int INPUT_QUANTIZATION = 32;
            constexpr int HIDDEN_QUANTIZATON = 128;

            // clang-format off
            constexpr std::array<int, 64> KING_BUCKET {
        0,  1,  2,  3,  3,  2,  1,  0,
        4,  5,  6,  7,  7,  6,  5,  4,
        8,  9,  10, 11, 11, 10, 9,  8,
        12, 13, 14, 15, 15, 14, 13, 12,
        16, 17, 18, 19, 19, 18, 17, 16,
        20, 21, 22, 23, 23, 22, 21, 20,
        24, 25, 26, 27, 27, 26, 25, 24,
        28, 29, 30, 31, 31, 30, 29, 28,
            };
            // clang-format on

        } // namespace constants

    } // namespace nnue

} // namespace jet
