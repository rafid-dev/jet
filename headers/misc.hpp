#pragma once

#include "bitboards.hpp"
#include "types.hpp"

#include <algorithm>
#include <array>
#include <bitset>
#include <iostream>
#include <vector>

namespace chess {
    namespace misc {
        inline void printBitboard(Bitboard bb) {
            std::bitset<NUM_SQUARES> b(bb);
            std::string              str_bitset = b.to_string();

            for (int i = 0; i < NUM_SQUARES; i += 8) {
                std::string x = str_bitset.substr(i, 8);
                reverse(x.begin(), x.end());
                std::cout << x << '\n';
            }

            std::cout << '\n' << std::endl;
        }

        inline std::vector<std::string_view> split(std::string_view str, char delimiter) {
            std::vector<std::string_view> result;
            size_t                        start = 0;
            size_t                        end   = str.find(delimiter);

            while (end != std::string_view::npos) {
                result.push_back(str.substr(start, end - start));
                start = end + 1;
                end   = str.find(delimiter, start);
            }

            result.push_back(str.substr(start));

            return result;
        }
    } // namespace misc
} // namespace chess