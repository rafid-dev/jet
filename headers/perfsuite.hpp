#pragma once

#include "../headers/misc.hpp"
#include <string>
#include <vector>

namespace perft {
    class DepthNodes {
    private:
        uint64_t m_nodes;
        uint32_t m_depth;

    public:
        constexpr DepthNodes(const uint64_t nodes, const uint32_t depth) : m_nodes(nodes), m_depth(depth) {
        }

        constexpr uint64_t nodes() const {
            return m_nodes;
        }

        constexpr uint32_t depth() const {
            return m_depth;
        }
    };
    class EpdInfo {
    private:
        std::string             m_fen;
        std::vector<DepthNodes> m_info_vec;

    public:
        EpdInfo(const std::string_view& epd) {
            auto split = misc::split(epd, ';');

            m_fen = split[0];

            for (size_t i = 1; i < split.size(); i++) {
                auto info_split = misc::split(split[i], ' ');

                auto depth = std::stoi(std::string{info_split[0][1]});
                auto nodes = std::stoull(std::string{info_split[1]});

                m_info_vec.emplace_back(nodes, depth);
            }
        }

        constexpr const std::string& fen() const {
            return m_fen;
        }

        constexpr const std::vector<DepthNodes>& fetch() const {
            return m_info_vec;
        }
    };


    void bulkSuite(const std::string& name, const uint64_t max);
    void startBulk(const std::string& fen, const int depth = 1);
    void bulkSpeedTest(const std::string_view& fen = chess::START_FEN, const int depth = 7);
} // namespace perft