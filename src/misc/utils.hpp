#pragma once

#include <chrono>
#include <string_view>
#include <vector>

namespace misc {

    static inline std::vector<std::string_view> splitString(std::string_view str, char delimiter) {
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

    // Returns the current time in Duration
    template <typename Duration = std::chrono::milliseconds>
    inline double tick() {
        return (double) std::chrono::duration_cast<Duration>(std::chrono::steady_clock::now().time_since_epoch()).count();
    }

} // namespace misc
