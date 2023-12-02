#pragma once

#include "bitboards.hpp"
#include "types.hpp"
#include <array>
#include <functional>
#include <random>

namespace chess {
    class attacks {
        struct Magic {
            Bitboard mask;
            U64      magic;
            U64*     attacks;
            U64      shift;

            U64 index(U64 b) const {
                return ((b & mask) * magic) >> shift;
            }
        };

        static void initSliders(Square sq, Magic table[], U64 magic, const std::function<Bitboard(Square, Bitboard)>& attacks);

        // clang-format off
        static constexpr Bitboard PAWN_ATTACKS[NUM_COLORS][NUM_SQUARES] = {
            {0x200, 0x500, 0xa00, 0x1400, 0x2800, 0x5000, 0xa000, 0x4000,
            0x20000, 0x50000, 0xa0000, 0x140000, 0x280000, 0x500000, 0xa00000, 0x400000,
            0x2000000, 0x5000000, 0xa000000, 0x14000000, 0x28000000, 0x50000000, 0xa0000000, 0x40000000,
            0x200000000, 0x500000000, 0xa00000000, 0x1400000000, 0x2800000000, 0x5000000000, 0xa000000000, 0x4000000000,
            0x20000000000, 0x50000000000, 0xa0000000000, 0x140000000000, 0x280000000000, 0x500000000000, 0xa00000000000, 0x400000000000,
            0x2000000000000, 0x5000000000000, 0xa000000000000, 0x14000000000000, 0x28000000000000, 0x50000000000000, 0xa0000000000000, 0x40000000000000,
            0x200000000000000, 0x500000000000000, 0xa00000000000000, 0x1400000000000000, 0x2800000000000000, 0x5000000000000000, 0xa000000000000000, 0x4000000000000000,
            0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0
            },
            {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
            0x2, 0x5, 0xa, 0x14, 0x28, 0x50, 0xa0, 0x40,
            0x200, 0x500, 0xa00, 0x1400, 0x2800, 0x5000, 0xa000, 0x4000,
            0x20000, 0x50000, 0xa0000, 0x140000, 0x280000, 0x500000, 0xa00000, 0x400000,
            0x2000000, 0x5000000, 0xa000000, 0x14000000, 0x28000000, 0x50000000, 0xa0000000, 0x40000000,
            0x200000000, 0x500000000, 0xa00000000, 0x1400000000, 0x2800000000, 0x5000000000, 0xa000000000, 0x4000000000,
            0x20000000000, 0x50000000000, 0xa0000000000, 0x140000000000, 0x280000000000, 0x500000000000, 0xa00000000000, 0x400000000000,
            0x2000000000000, 0x5000000000000, 0xa000000000000, 0x14000000000000, 0x28000000000000, 0x50000000000000, 0xa0000000000000, 0x40000000000000
            }
        };

        static constexpr Bitboard KNIGHT_ATTACKS[NUM_SQUARES] = {
            0x20400, 0x50800, 0xa1100, 0x142200, 0x284400, 0x508800, 0xa01000, 0x402000,
            0x2040004, 0x5080008, 0xa110011, 0x14220022, 0x28440044, 0x50880088, 0xa0100010, 0x40200020,
            0x204000402, 0x508000805, 0xa1100110a, 0x1422002214, 0x2844004428, 0x5088008850, 0xa0100010a0, 0x4020002040,
            0x20400040200, 0x50800080500, 0xa1100110a00, 0x142200221400, 0x284400442800, 0x508800885000, 0xa0100010a000, 0x402000204000,
            0x2040004020000, 0x5080008050000, 0xa1100110a0000, 0x14220022140000, 0x28440044280000, 0x50880088500000, 0xa0100010a00000, 0x40200020400000,
            0x204000402000000, 0x508000805000000, 0xa1100110a000000, 0x1422002214000000, 0x2844004428000000, 0x5088008850000000, 0xa0100010a0000000, 0x4020002040000000,
            0x400040200000000, 0x800080500000000, 0x1100110a00000000, 0x2200221400000000, 0x4400442800000000, 0x8800885000000000, 0x100010a000000000, 0x2000204000000000,
            0x4020000000000, 0x8050000000000, 0x110a0000000000, 0x22140000000000, 0x44280000000000, 0x88500000000000, 0x10a00000000000, 0x20400000000000
        };

        static constexpr Bitboard KING_ATTACKS[NUM_SQUARES] = {
            0x302, 0x705, 0xe0a, 0x1c14, 0x3828, 0x7050, 0xe0a0, 0xc040,
            0x30203, 0x70507, 0xe0a0e, 0x1c141c, 0x382838, 0x705070, 0xe0a0e0, 0xc040c0,
            0x3020300, 0x7050700, 0xe0a0e00, 0x1c141c00, 0x38283800, 0x70507000, 0xe0a0e000, 0xc040c000,
            0x302030000, 0x705070000, 0xe0a0e0000, 0x1c141c0000, 0x3828380000, 0x7050700000, 0xe0a0e00000, 0xc040c00000,
            0x30203000000, 0x70507000000, 0xe0a0e000000, 0x1c141c000000, 0x382838000000, 0x705070000000, 0xe0a0e0000000, 0xc040c0000000,
            0x3020300000000, 0x7050700000000, 0xe0a0e00000000, 0x1c141c00000000, 0x38283800000000, 0x70507000000000, 0xe0a0e000000000, 0xc040c000000000,
            0x302030000000000, 0x705070000000000, 0xe0a0e0000000000, 0x1c141c0000000000, 0x3828380000000000, 0x7050700000000000, 0xe0a0e00000000000, 0xc040c00000000000,
            0x203000000000000, 0x507000000000000, 0xa0e000000000000, 0x141c000000000000, 0x2838000000000000, 0x5070000000000000, 0xa0e0000000000000, 0x40c0000000000000
        };

        // clang-format on

        static inline Bitboard RookAttacks[0x19000]  = {};
        static inline Bitboard BishopAttacks[0x1480] = {};

        static inline Magic RookTable[NUM_SQUARES]   = {};
        static inline Magic BishopTable[NUM_SQUARES] = {};

        // Magic numbers taken from Disservin/chess-library
        static constexpr Bitboard RookMagics[NUM_SQUARES] = {
            0x8a80104000800020ULL, 0x140002000100040ULL,  0x2801880a0017001ULL,  0x100081001000420ULL,  0x200020010080420ULL,  0x3001c0002010008ULL,  0x8480008002000100ULL, 0x2080088004402900ULL,
            0x800098204000ULL,     0x2024401000200040ULL, 0x100802000801000ULL,  0x120800800801000ULL,  0x208808088000400ULL,  0x2802200800400ULL,    0x2200800100020080ULL, 0x801000060821100ULL,
            0x80044006422000ULL,   0x100808020004000ULL,  0x12108a0010204200ULL, 0x140848010000802ULL,  0x481828014002800ULL,  0x8094004002004100ULL, 0x4010040010010802ULL, 0x20008806104ULL,
            0x100400080208000ULL,  0x2040002120081000ULL, 0x21200680100081ULL,   0x20100080080080ULL,   0x2000a00200410ULL,    0x20080800400ULL,      0x80088400100102ULL,   0x80004600042881ULL,
            0x4040008040800020ULL, 0x440003000200801ULL,  0x4200011004500ULL,    0x188020010100100ULL,  0x14800401802800ULL,   0x2080040080800200ULL, 0x124080204001001ULL,  0x200046502000484ULL,
            0x480400080088020ULL,  0x1000422010034000ULL, 0x30200100110040ULL,   0x100021010009ULL,     0x2002080100110004ULL, 0x202008004008002ULL,  0x20020004010100ULL,   0x2048440040820001ULL,
            0x101002200408200ULL,  0x40802000401080ULL,   0x4008142004410100ULL, 0x2060820c0120200ULL,  0x1001004080100ULL,    0x20c020080040080ULL,  0x2935610830022400ULL, 0x44440041009200ULL,
            0x280001040802101ULL,  0x2100190040002085ULL, 0x80c0084100102001ULL, 0x4024081001000421ULL, 0x20030a0244872ULL,    0x12001008414402ULL,   0x2006104900a0804ULL,  0x1004081002402ULL};

        static constexpr Bitboard BishopMagics[NUM_SQUARES] = {
            0x40040844404084ULL,   0x2004208a004208ULL,   0x10190041080202ULL,   0x108060845042010ULL,  0x581104180800210ULL,  0x2112080446200010ULL, 0x1080820820060210ULL, 0x3c0808410220200ULL,
            0x4050404440404ULL,    0x21001420088ULL,      0x24d0080801082102ULL, 0x1020a0a020400ULL,    0x40308200402ULL,      0x4011002100800ULL,    0x401484104104005ULL,  0x801010402020200ULL,
            0x400210c3880100ULL,   0x404022024108200ULL,  0x810018200204102ULL,  0x4002801a02003ULL,    0x85040820080400ULL,   0x810102c808880400ULL, 0xe900410884800ULL,    0x8002020480840102ULL,
            0x220200865090201ULL,  0x2010100a02021202ULL, 0x152048408022401ULL,  0x20080002081110ULL,   0x4001001021004000ULL, 0x800040400a011002ULL, 0xe4004081011002ULL,   0x1c004001012080ULL,
            0x8004200962a00220ULL, 0x8422100208500202ULL, 0x2000402200300c08ULL, 0x8646020080080080ULL, 0x80020a0200100808ULL, 0x2010004880111000ULL, 0x623000a080011400ULL, 0x42008c0340209202ULL,
            0x209188240001000ULL,  0x400408a884001800ULL, 0x110400a6080400ULL,   0x1840060a44020800ULL, 0x90080104000041ULL,   0x201011000808101ULL,  0x1a2208080504f080ULL, 0x8012020600211212ULL,
            0x500861011240000ULL,  0x180806108200800ULL,  0x4000020e01040044ULL, 0x300000261044000aULL, 0x802241102020002ULL,  0x20906061210001ULL,   0x5a84841004010310ULL, 0x4010801011c04ULL,
            0xa010109502200ULL,    0x4a02012000ULL,       0x500201010098b028ULL, 0x8040002811040900ULL, 0x28000010020204ULL,   0x6000020202d0240ULL,  0x8918844842082200ULL, 0x4010011029020020ULL};

    public:
        static inline std::array<std::array<Bitboard, 64>, 64> SQUARES_BETWEEN_BB{};
        static constexpr Bitboard                              MASK_RANK[8] = {0xff, 0xff00, 0xff0000, 0xff000000, 0xff00000000, 0xff0000000000, 0xff000000000000, 0xff00000000000000};

        static constexpr Bitboard MASK_FILE[8] = {
            0x101010101010101, 0x202020202020202, 0x404040404040404, 0x808080808080808, 0x1010101010101010, 0x2020202020202020, 0x4040404040404040, 0x8080808080808080,
        };

        template <Direction dir>
        static constexpr Bitboard shift(const Bitboard& b);

        template <Color c>
        static Bitboard pawnLeftAttacks(const Bitboard b);

        template <Color c>
        static Bitboard pawnRightAttacks(const Bitboard b);

        static Bitboard pawnAttacks(Square sq, const Color c);
        static Bitboard knightAttacks(Square sq);
        static Bitboard kingAttacks(Square sq);
        static Bitboard queenAttacks(Square sq, Bitboard occupied);

        static Bitboard bishopAttacks(Square sq, Bitboard occupied);
        static Bitboard rookAttacks(Square sq, Bitboard occupied);

        static Bitboard bishop(Square sq, Bitboard occupied) {
            return BishopTable[sq].attacks[BishopTable[sq].index(occupied)];
        }

        static Bitboard rook(Square sq, Bitboard occupied) {
            return RookTable[sq].attacks[RookTable[sq].index(occupied)];
        }

        static Bitboard queen(Square sq, Bitboard occupied) {
            return bishop(sq, occupied) | rook(sq, occupied);
        }

        static constexpr Bitboard pawn(Square sq, const Color c) {
            return PAWN_ATTACKS[static_cast<int>(c)][sq];
        }

        static constexpr Bitboard knight(Square sq) {
            return KNIGHT_ATTACKS[sq];
        }

        static constexpr Bitboard king(Square sq) {
            return KING_ATTACKS[sq];
        }

        static void generatePawnAttacks();
        static void generateKnightAttacks();
        static void generateKingAttacks();

        static inline void init();

        static inline void initSquaresBetween() {
            U64 sqs = 0;

            for (Square sq = Square(Square::SQ_A1); sq < Square(NUM_SQUARES); ++sq) {
                for (Square sq2 = Square(Square::SQ_A1); sq2 < Square(NUM_SQUARES); ++sq2) {
                    sqs = (1ULL << sq) | (1ULL << sq2);

                    if (sq == sq2) {
                        SQUARES_BETWEEN_BB[int(sq)][int(sq2)] = 0ULL;
                    } else if (sq.file() == sq2.file() || sq.rank() == sq2.rank()) {
                        SQUARES_BETWEEN_BB[int(sq)][int(sq2)] = attacks::rook(sq, sqs) & attacks::rook(sq2, sqs);
                    } else if (sq.diagonal() == sq2.diagonal() || sq.antiDiagonal() == sq2.antiDiagonal()) {
                        SQUARES_BETWEEN_BB[int(sq)][int(sq2)] = attacks::bishop(sq, sqs) & attacks::bishop(sq2, sqs);
                    }
                }
            }
        }
    };

    template <Direction dir>
    inline constexpr Bitboard attacks::shift(const Bitboard& b) {
        if constexpr (dir == Direction::NORTH) {
            return b << 8;
        } else if constexpr (dir == Direction::SOUTH) {
            return b >> 8;
        } else if constexpr (dir == Direction::EAST) {
            return (b & ~MASK_FILE[7]) << 1;
        } else if constexpr (dir == Direction::WEST) {
            return (b & ~MASK_FILE[0]) >> 1;
        } else if constexpr (dir == Direction::NORTH_EAST) {
            return (b & ~MASK_FILE[7]) << 9;
        } else if constexpr (dir == Direction::NORTH_WEST) {
            return (b & ~MASK_FILE[0]) << 7;
        } else if constexpr (dir == Direction::SOUTH_EAST) {
            return (b & ~MASK_FILE[7]) >> 7;
        } else if constexpr (dir == Direction::SOUTH_WEST) {
            return (b & ~MASK_FILE[0]) >> 9;
        } else {
            return 0;
        }
    }

    template <Color c>
    inline Bitboard attacks::pawnLeftAttacks(const Bitboard b) {
        if constexpr (c == Color::WHITE) {
            return (b & ~MASK_FILE[0]) << 7;
        } else {
            return (b & ~MASK_FILE[0]) >> 9;
        }
    }

    template <Color c>
    inline Bitboard attacks::pawnRightAttacks(const Bitboard b) {
        if constexpr (c == Color::WHITE) {
            return (b & ~MASK_FILE[7]) << 9;
        } else {
            return (b & ~MASK_FILE[7]) >> 7;
        }
    }

    inline Bitboard attacks::pawnAttacks(Square sq, const Color c) {
        Bitboard b = 1ULL << sq;

        if (c == Color::WHITE) {
            return pawnLeftAttacks<Color::WHITE>(b) | pawnRightAttacks<Color::WHITE>(b);
        } else {
            return pawnLeftAttacks<Color::BLACK>(b) | pawnRightAttacks<Color::BLACK>(b);
        }
    }

    inline Bitboard attacks::knightAttacks(Square sq) {
        Bitboard b = 1ULL << sq;

        constexpr Bitboard A_FILE             = MASK_FILE[0];
        const Bitboard     KNIGHT_NOT_A_FILE  = b & ~A_FILE;
        const Bitboard     KNIGHT_NOT_H_FILE  = b & ~MASK_FILE[7];
        const Bitboard     KNIGHT_NOT_AB_FILE = KNIGHT_NOT_A_FILE & ~MASK_FILE[1];
        const Bitboard     KNIGHT_NOT_GH_FILE = KNIGHT_NOT_H_FILE & ~MASK_FILE[6];

        Bitboard attacks = (KNIGHT_NOT_A_FILE << 15) | (KNIGHT_NOT_A_FILE >> 17);

        attacks |= (KNIGHT_NOT_H_FILE << 17) | (KNIGHT_NOT_H_FILE >> 15);
        attacks |= (KNIGHT_NOT_AB_FILE << 6) | (KNIGHT_NOT_AB_FILE >> 10);
        attacks |= (KNIGHT_NOT_GH_FILE << 10) | (KNIGHT_NOT_GH_FILE >> 6);

        return attacks;
    }

    inline Bitboard attacks::kingAttacks(Square sq) {
        Bitboard b = 1ULL << sq;

        return shift<Direction::NORTH_EAST>(b) | shift<Direction::NORTH_WEST>(b) | shift<Direction::SOUTH_EAST>(b) | shift<Direction::SOUTH_WEST>(b) | shift<Direction::EAST>(b) | shift<Direction::WEST>(b) |
               shift<Direction::NORTH>(b) | shift<Direction::SOUTH>(b);
    }

    inline Bitboard attacks::bishopAttacks(Square sq, Bitboard occupied) {
        Bitboard attacks = 0ULL;

        int r, f;

        int br = static_cast<int>(sq) / 8;
        int bf = static_cast<int>(sq) % 8;

        for (r = br + 1, f = bf + 1; r <= 7 && f <= 7; r++, f++) {
            Square s = Square(static_cast<File>(f), static_cast<Rank>(r));

            attacks |= 1ULL << s;

            if (occupied & (1ULL << s)) {
                break;
            }
        }

        for (r = br - 1, f = bf + 1; r >= 0 && f <= 7; r--, f++) {
            Square s = Square(static_cast<File>(f), static_cast<Rank>(r));

            attacks |= 1ULL << s;

            if (occupied & (1ULL << s)) {
                break;
            }
        }

        for (r = br + 1, f = bf - 1; r <= 7 && f >= 0; r++, f--) {
            Square s = Square(static_cast<File>(f), static_cast<Rank>(r));

            attacks |= 1ULL << s;

            if (occupied & (1ULL << s)) {
                break;
            }
        }

        for (r = br - 1, f = bf - 1; r >= 0 && f >= 0; r--, f--) {
            Square s = Square(static_cast<File>(f), static_cast<Rank>(r));

            attacks |= 1ULL << s;

            if (occupied & (1ULL << s)) {
                break;
            }
        }

        return attacks;
    }

    inline Bitboard attacks::rookAttacks(Square sq, Bitboard occupied) {
        Bitboard attacks = 0ULL;

        int r, f;

        int br = static_cast<int>(sq) / 8;
        int bf = static_cast<int>(sq) % 8;

        for (r = br + 1, f = bf; r <= 7; r++) {
            Square s = Square(static_cast<File>(f), static_cast<Rank>(r));

            attacks |= 1ULL << s;

            if (occupied & (1ULL << s)) {
                break;
            }
        }

        for (r = br - 1, f = bf; r >= 0; r--) {
            Square s = Square(static_cast<File>(f), static_cast<Rank>(r));

            attacks |= 1ULL << s;

            if (occupied & (1ULL << s)) {
                break;
            }
        }

        for (r = br, f = bf + 1; f <= 7; f++) {
            Square s = Square(static_cast<File>(f), static_cast<Rank>(r));

            attacks |= 1ULL << s;

            if (occupied & (1ULL << s)) {
                break;
            }
        }

        for (r = br, f = bf - 1; f >= 0; f--) {
            Square s = Square(static_cast<File>(f), static_cast<Rank>(r));

            attacks |= 1ULL << s;

            if (occupied & (1ULL << s)) {
                break;
            }
        }

        return attacks;
    }

    inline Bitboard attacks::queenAttacks(Square sq, Bitboard occupied) {
        return bishopAttacks(sq, occupied) | rookAttacks(sq, occupied);
    }

    inline void attacks::generatePawnAttacks() {
        std::cout << "static constexpr Bitboard PAWN_ATTACKS[NUM_COLORS][NUM_SQUARES] = {\n";

        for (const auto c : {Color::WHITE, Color::BLACK}) {
            std::cout << "    {";

            for (int sq = 0; sq < NUM_SQUARES; sq++) {
                std::cout << std::hex << "0x" << pawnAttacks(Square(sq), c) << std::dec;

                if (sq != NUM_SQUARES - 1) {
                    std::cout << ", ";
                }

                if (sq % 8 == 7) {
                    std::cout << "\n     ";
                }
            }

            std::cout << "}";

            if (c != Color::BLACK) {
                std::cout << ",\n";
            }
        }

        std::cout << "};\n";
    }

    inline void attacks::generateKnightAttacks() {
        std::cout << "static constexpr Bitboard KNIGHT_ATTACKS[NUM_SQUARES] = {\n";

        for (int sq = 0; sq < NUM_SQUARES; sq++) {
            std::cout << std::hex << "0x" << knightAttacks(Square(sq)) << std::dec;

            if (sq != NUM_SQUARES - 1) {
                std::cout << ", ";
            }

            if (sq % 8 == 7) {
                std::cout << "\n     ";
            }
        }

        std::cout << "};\n";
    }

    inline void attacks::generateKingAttacks() {
        std::cout << "static constexpr Bitboard KING_ATTACKS[NUM_SQUARES] = {\n";

        for (int sq = 0; sq < NUM_SQUARES; sq++) {
            std::cout << std::hex << "0x" << kingAttacks(Square(sq)) << std::dec;

            if (sq != NUM_SQUARES - 1) {
                std::cout << ", ";
            }

            if (sq % 8 == 7) {
                std::cout << "\n     ";
            }
        }

        std::cout << "};\n";
    }

    inline void attacks::initSliders(Square sq, Magic table[], U64 magic, const std::function<Bitboard(Square, Bitboard)>& attacks) {
        Bitboard rank_mask = ((MASK_RANK[0] | MASK_RANK[7]) & ~MASK_RANK[static_cast<int>(sq.rank())]);

        Bitboard file_mask = ((MASK_FILE[0] | MASK_FILE[7]) & ~MASK_FILE[static_cast<int>(sq.file())]);

        Bitboard edges = rank_mask | file_mask;

        Bitboard occupancy = 0ULL;

        table[int(sq)].magic = magic;
        table[int(sq)].mask  = attacks(sq, occupancy) & ~edges;
        table[int(sq)].shift = NUM_SQUARES - popcount(table[sq].mask);

        if (sq < NUM_SQUARES - 1) {
            table[int(sq) + 1].attacks = table[sq].attacks + (1 << popcount(table[int(sq)].mask));
        }

        do {
            table[int(sq)].attacks[table[int(sq)].index(occupancy)] = attacks(int(sq), occupancy);

            occupancy = (occupancy - table[int(sq)].mask) & table[int(sq)].mask;
        } while (occupancy);
    }

    inline void attacks::init() {
        BishopTable[0].attacks = BishopAttacks;
        RookTable[0].attacks   = RookAttacks;

        for (int sq = 0; sq < NUM_SQUARES; sq++) {
            initSliders(Square(sq), BishopTable, BishopMagics[sq], bishopAttacks);
            initSliders(Square(sq), RookTable, RookMagics[sq], rookAttacks);
        }

        initSquaresBetween();
    }

} // namespace chess