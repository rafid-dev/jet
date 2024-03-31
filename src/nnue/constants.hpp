#include <array>

namespace jet {
    namespace nnue {

        namespace constants {

            constexpr int BUCKETS     = 4;
            constexpr int INPUT_SIZE  = 64 * 6 * 2 * BUCKETS;
            constexpr int HIDDEN_SIZE = 768;
            constexpr int OUTPUT_SIZE = 1;

            constexpr int INPUT_LAYER_SIZE  = INPUT_SIZE * HIDDEN_SIZE;
            constexpr int HIDDEN_LAYER_SIZE = HIDDEN_SIZE * 2;
            constexpr int OUTPUT_LAYER_SIZE = OUTPUT_SIZE;

            constexpr int INPUT_QUANTIZATION = 32;
            constexpr int HIDDEN_QUANTIZATON = 128;

            // clang-format off
            constexpr std::array<int, 64> KING_BUCKET {
                0, 0, 1, 1, 1, 1, 0, 0,
                0, 0, 1, 1, 1, 1, 0, 0,
                0, 0, 1, 1, 1, 1, 0, 0,
                0, 0, 1, 1, 1, 1, 0, 0,
                2, 2, 3, 3, 3, 3, 2, 2,
                2, 2, 3, 3, 3, 3, 2, 2,
                2, 2, 3, 3, 3, 3, 2, 2,
                2, 2, 3, 3, 3, 3, 2, 2,
            };
            // clang-format on

        } // namespace constants

    } // namespace nnue

} // namespace jet
