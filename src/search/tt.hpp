#pragma once

#include "../chess/moves.hpp"
#include "constants.hpp"
#include <cstdint>
#include <vector>

namespace jet {
    namespace search {
        class TT {
        public:
            using U64 = uint64_t;
            enum class Flag : uint8_t { NONE, EXACT, LOWER, UPPER };

            class Entry {
            private:
                types::RawMove   m_move;
                types::Value_i16 m_score;
                types::Depth_u8  m_depth;
                Flag             m_flag;
                U64              m_key;

            public:
                Entry() = default;

                void set(types::RawMove move, types::Value_i16 score, types::Depth_u8 depth, Flag flag, U64 key) {
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
            };

            class Table {
            private:
                std::vector<Entry> m_table;

                static inline uint64_t _index(uint64_t x, uint64_t N) {
                    return static_cast<__uint128_t>((static_cast<__uint128_t>(x) * static_cast<__uint128_t>(N)) >> 64);
                }

            public:
                Table() = default;

                template <bool print = true>
                void initialize(int MB) {
                    resize(MB * 0x100000 / sizeof(Entry));

                    if constexpr (print) {
                        std::cout << "Transposition table initialized with " << MB << " MB ( " << m_table.size() << ") entries"
                                  << std::endl;
                    }
                }

                void store(U64 hash, const chess::Move& move, types::Depth depth, types::Value score, Flag flag) {
                    auto& entry = m_table[_index(hash, m_table.size())];
                    entry.set(move.data(), score, depth, flag, hash);
                }

                const Entry& probe(U64 hash, bool& hit) const {
                    const auto& entry = m_table[_index(hash, m_table.size())];
                    hit               = hash == entry.key();
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