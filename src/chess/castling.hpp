#pragma once

#include "square.hpp"
#include "types.hpp"

#include <array>
#include <cstring>

namespace chess {
    enum class CastlingSide : uint8_t { KING_SIDE, QUEEN_SIDE };

    template <typename T>
    struct CastlingRightsBase {
        T whiteKingSide : 1;
        T whiteQueenSide : 1;
        T blackKingSide : 1;
        T blackQueenSide : 1;
    };

    class CastlingRights {
    public:
        constexpr CastlingRights() : m_rights({1, 1, 1, 1}) {
        }

        constexpr void clear() {
            std::memset(&m_rights, 0, sizeof(m_rights));
        }

        constexpr void loadFromString(std::string_view str) {
            clear();

            for (auto c : str) {
                switch (c) {
                    case 'K':
                        _setRights(Color::WHITE, CastlingSide::KING_SIDE, 1);
                        break;
                    case 'Q':
                        _setRights(Color::WHITE, CastlingSide::QUEEN_SIDE, 1);
                        break;
                    case 'k':
                        _setRights(Color::BLACK, CastlingSide::KING_SIDE, 1);
                        break;
                    case 'q':
                        _setRights(Color::BLACK, CastlingSide::QUEEN_SIDE, 1);
                        break;
                    case '-':
                        return;
                }
            }
        }

        bool hasCastlingRights(Color c, CastlingSide side) const {
            return _getRights(c, side);
        }

        bool hasCastlingRights(Color c) const {
            return _getRights(c, CastlingSide::KING_SIDE) || _getRights(c, CastlingSide::QUEEN_SIDE);
        }

        template <Color c, CastlingSide side>
        bool hasCastlingRights() const {
            return _getRights<c, side>();
        }

        constexpr void setCastlingRights(Color c, CastlingSide side, bool value) {
            _setRights(c, side, value);
        }

        int index() const {
            return m_rights.whiteKingSide | (m_rights.whiteQueenSide << 1) | (m_rights.blackKingSide << 2) |
                   (m_rights.blackQueenSide << 3);
        }

        static constexpr inline Square kingTo(Color c, CastlingSide side) {
            const bool kingSide = side == CastlingSide::KING_SIDE;
            return Square::relativeSquare(Square::C1 + 4 * kingSide, c);
        }

        static constexpr inline Square rookTo(Color c, CastlingSide side) {
            const bool kingSide = side == CastlingSide::KING_SIDE;
            return Square::relativeSquare(Square::D1 + 2 * kingSide, c);
        }

        static constexpr inline Square rookFrom(Color c, CastlingSide side) {
            const bool kingSide = side == CastlingSide::KING_SIDE;
            return Square::relativeSquare(Square::H1 * kingSide, c);
        }

        template <Color c, CastlingSide side>
        static constexpr inline Square kingTo() {
            if constexpr (side == CastlingSide::KING_SIDE) {
                return Square::relativeSquare<c, Square::G1>();
            } else {
                return Square::relativeSquare<c, Square::C1>();
            }
        }

        template <Color c, CastlingSide side>
        static constexpr inline Square rookTo() {
            if constexpr (side == CastlingSide::KING_SIDE) {
                return Square::relativeSquare<c, Square::F1>();
            } else {
                return Square::relativeSquare<c, Square::D1>();
            }
        }

        template <Color c, CastlingSide side>
        static constexpr inline Square rookFrom() {
            if constexpr (side == CastlingSide::KING_SIDE) {
                return Square::relativeSquare<c, Square::H1>();
            } else {
                return Square::relativeSquare<c, Square::A1>();
            }
        }

        inline std::string toString() const {
            std::string str;

            if (_getRights(Color::WHITE, CastlingSide::KING_SIDE)) {
                str += 'K';
            }

            if (_getRights(Color::WHITE, CastlingSide::QUEEN_SIDE)) {
                str += 'Q';
            }

            if (_getRights(Color::BLACK, CastlingSide::KING_SIDE)) {
                str += 'k';
            }

            if (_getRights(Color::BLACK, CastlingSide::QUEEN_SIDE)) {
                str += 'q';
            }

            return str;
        }

        static constexpr inline CastlingSide getCastlingSide(Square sq, Square kingSq) {
            return static_cast<CastlingSide>((sq > kingSq) ^ 1);
        }

    private:
        using RightsType = CastlingRightsBase<bool>;

        RightsType m_rights;

        template <Color c, CastlingSide side>
        constexpr bool _getRights() const {
            if constexpr (c == Color::WHITE) {
                if constexpr (side == CastlingSide::KING_SIDE) {
                    return m_rights.whiteKingSide;
                } else {
                    return m_rights.whiteQueenSide;
                }
            } else {
                if constexpr (side == CastlingSide::KING_SIDE) {
                    return m_rights.blackKingSide;
                } else {
                    return m_rights.blackQueenSide;
                }
            }
        }

        constexpr bool _getRights(Color c, CastlingSide side) const {
            if (c == Color::WHITE) {
                if (side == CastlingSide::KING_SIDE) {
                    return m_rights.whiteKingSide;
                } else {
                    return m_rights.whiteQueenSide;
                }
            } else {
                if (side == CastlingSide::KING_SIDE) {
                    return m_rights.blackKingSide;
                } else {
                    return m_rights.blackQueenSide;
                }
            }
        }

        constexpr void _setRights(Color c, CastlingSide side, bool value) {
            if (c == Color::WHITE) {
                if (side == CastlingSide::KING_SIDE) {
                    m_rights.whiteKingSide = value;
                } else {
                    m_rights.whiteQueenSide = value;
                }
            } else {
                if (side == CastlingSide::KING_SIDE) {
                    m_rights.blackKingSide = value;
                } else {
                    m_rights.blackQueenSide = value;
                }
            }
        }
    };

} // namespace chess