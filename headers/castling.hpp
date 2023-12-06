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

        bool hasCastlingRights(Color c, CastlingSide side) const {
            return getRights(c, side);
        }

        bool hasCastlingRights(Color c) const {
            return getRights(c, CastlingSide::KING_SIDE) || getRights(c, CastlingSide::QUEEN_SIDE);
        }

        int removeCastlingRights(Color c, CastlingSide side) {
            setRights(c, side, 0);
            return static_cast<uint8_t>(c) * 2 + static_cast<uint8_t>(side);
        }

        void removeCastlingRights(Color c) {
            setRights(c, CastlingSide::KING_SIDE, 0);
            setRights(c, CastlingSide::QUEEN_SIDE, 0);
        }

        void setCastlingRights(Color c, CastlingSide side) {
            setRights(c, side, 1);
        }

        int index() const {
            return m_rights.whiteKingSide | (m_rights.whiteQueenSide << 1) | (m_rights.blackKingSide << 2) | (m_rights.blackQueenSide << 3);
        }

        static constexpr inline Square kingTo(Color c, CastlingSide side) {
            if (side == CastlingSide::KING_SIDE) {
                return relativeSquare(Square::G1, c);
            } else {
                return relativeSquare(Square::C1, c);
            }
        }

        static constexpr inline Square rookTo(Color c, CastlingSide side) {
            if (side == CastlingSide::KING_SIDE) {
                return relativeSquare(Square::F1, c);
            } else {
                return relativeSquare(Square::D1, c);
            }
        }

        static constexpr inline Square rookFrom(Color c, CastlingSide side) {
            if (side == CastlingSide::KING_SIDE) {
                return relativeSquare(Square::H1, c);
            } else {
                return relativeSquare(Square::A1, c);
            }
        }

        template <Color c>
        static constexpr inline Square kingTo(CastlingSide side) {
            if (side == CastlingSide::KING_SIDE) {
                return relativeSquare<c>(Square::G1);
            } else {
                return relativeSquare<c>(Square::C1);
            }
        }

        template <Color c>
        static constexpr inline Square rookTo(CastlingSide side) {
            if (side == CastlingSide::KING_SIDE) {
                return relativeSquare<c>(Square::F1);
            } else {
                return relativeSquare<c>(Square::D1);
            }
        }

        template <Color c>
        static constexpr inline Square rookFrom(CastlingSide side) {
            if (side == CastlingSide::KING_SIDE) {
                return relativeSquare<c>(Square::H1);
            } else {
                return relativeSquare<c>(Square::A1);
            }
        }

    private:
        using RightsType = CastlingRightsBase<bool>;

        RightsType m_rights;

        constexpr bool getRights(Color c, CastlingSide side) const {
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

        constexpr void setRights(Color c, CastlingSide side, bool value) {
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