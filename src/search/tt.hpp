#pragma once

#include "../chess/moves.hpp"
#include "constants.hpp"
#include <cstdint>
#include <vector>

namespace jet {
    namespace search {
        class TT {
        public:
            using Key = uint16_t;
            enum class Flag : uint8_t { NONE, EXACT, LOWER, UPPER };

            class Entry {
            private:
                types::RawMove   m_move;
                types::Value_i16 m_score;
                types::Depth_u8  m_depth;
                Flag             m_flag;
                Key              m_key;

            public:
                Entry() = default;

                void set(types::RawMove move, types::Value_i16 score, types::Depth_u8 depth, Flag flag, Key key) {
                    m_move  = move;
                    m_score = score;
                    m_depth = depth;
                    m_flag  = flag;
                    m_key   = key;
                }

                auto score() const {
                    return static_cast<types::Value>(m_score);
                }

                auto move() const {
                    return chess::Move(m_move);
                }

                auto depth() const {
                    return static_cast<types::Depth>(m_depth);
                }

                auto flag() const {
                    return m_flag;
                }

                auto key() const {
                    return m_key;
                }

                auto keyMatches(uint64_t hash) const {
                    return static_cast<Key>(hash) == key();
                }
            };

            class Table {
            private:
                std::vector<Entry> m_table;

                static inline uint32_t _index(uint32_t x, uint32_t N) {
                    return (static_cast<uint64_t>(x) * static_cast<uint64_t>(N)) >> 32;
                }

            public:
                Table() = default;

                void initialize(int MB) {
                    m_table.resize(MB * 1024 * 1024 / sizeof(Entry));
                    std::fill(m_table.begin(), m_table.end(), Entry());

                    std::cout << "Transposition table initialized with " << MB << " MB ( " << m_table.size() << ") entries"
                              << std::endl;
                }

                void store(uint64_t hash, const chess::Move& move, types::Depth depth, types::Value score, Flag flag) {
                    auto& entry = m_table[_index(hash, m_table.size())];
                    entry.set(move.data(), score, depth, flag, static_cast<Key>(hash));
                }

                const Entry& probe(uint64_t hash, bool& hit) const {
                    const auto& entry = m_table[_index(hash, m_table.size())];
                    hit               = entry.keyMatches(hash);
                    return entry;
                }

                void resize(size_t size) {
                    clear();
                    m_table.resize(size);
                    std::fill(m_table.begin(), m_table.end(), Entry());
                }

                auto size() const {
                    return m_table.size();
                }

                void clear() {
                    m_table.clear();
                }
            };
        };
    } // namespace search
} // namespace jet