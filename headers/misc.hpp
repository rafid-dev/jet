#pragma once

#include <string_view>
#include <vector>

namespace misc {

    std::vector<std::string_view> splitString(std::string_view str, char delimiter) {
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
