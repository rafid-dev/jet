#pragma once

#include "bitboards.hpp"

#include <cassert>

namespace chess {

    class Attacks {
    private:
        // Magic numbers taken from Disservin/chess-library
        static constexpr U64 ROOK_MAGICS[NUM_SQUARES] = {
            0x8a80104000800020ULL, 0x140002000100040ULL,  0x2801880a0017001ULL,  0x100081001000420ULL,
            0x200020010080420ULL,  0x3001c0002010008ULL,  0x8480008002000100ULL, 0x2080088004402900ULL,
            0x800098204000ULL,     0x2024401000200040ULL, 0x100802000801000ULL,  0x120800800801000ULL,
            0x208808088000400ULL,  0x2802200800400ULL,    0x2200800100020080ULL, 0x801000060821100ULL,
            0x80044006422000ULL,   0x100808020004000ULL,  0x12108a0010204200ULL, 0x140848010000802ULL,
            0x481828014002800ULL,  0x8094004002004100ULL, 0x4010040010010802ULL, 0x20008806104ULL,
            0x100400080208000ULL,  0x2040002120081000ULL, 0x21200680100081ULL,   0x20100080080080ULL,
            0x2000a00200410ULL,    0x20080800400ULL,      0x80088400100102ULL,   0x80004600042881ULL,
            0x4040008040800020ULL, 0x440003000200801ULL,  0x4200011004500ULL,    0x188020010100100ULL,
            0x14800401802800ULL,   0x2080040080800200ULL, 0x124080204001001ULL,  0x200046502000484ULL,
            0x480400080088020ULL,  0x1000422010034000ULL, 0x30200100110040ULL,   0x100021010009ULL,
            0x2002080100110004ULL, 0x202008004008002ULL,  0x20020004010100ULL,   0x2048440040820001ULL,
            0x101002200408200ULL,  0x40802000401080ULL,   0x4008142004410100ULL, 0x2060820c0120200ULL,
            0x1001004080100ULL,    0x20c020080040080ULL,  0x2935610830022400ULL, 0x44440041009200ULL,
            0x280001040802101ULL,  0x2100190040002085ULL, 0x80c0084100102001ULL, 0x4024081001000421ULL,
            0x20030a0244872ULL,    0x12001008414402ULL,   0x2006104900a0804ULL,  0x1004081002402ULL};

        static constexpr U64 BISHOP_MAGICS[NUM_SQUARES] = {
            0x40040844404084ULL,   0x2004208a004208ULL,   0x10190041080202ULL,   0x108060845042010ULL,
            0x581104180800210ULL,  0x2112080446200010ULL, 0x1080820820060210ULL, 0x3c0808410220200ULL,
            0x4050404440404ULL,    0x21001420088ULL,      0x24d0080801082102ULL, 0x1020a0a020400ULL,
            0x40308200402ULL,      0x4011002100800ULL,    0x401484104104005ULL,  0x801010402020200ULL,
            0x400210c3880100ULL,   0x404022024108200ULL,  0x810018200204102ULL,  0x4002801a02003ULL,
            0x85040820080400ULL,   0x810102c808880400ULL, 0xe900410884800ULL,    0x8002020480840102ULL,
            0x220200865090201ULL,  0x2010100a02021202ULL, 0x152048408022401ULL,  0x20080002081110ULL,
            0x4001001021004000ULL, 0x800040400a011002ULL, 0xe4004081011002ULL,   0x1c004001012080ULL,
            0x8004200962a00220ULL, 0x8422100208500202ULL, 0x2000402200300c08ULL, 0x8646020080080080ULL,
            0x80020a0200100808ULL, 0x2010004880111000ULL, 0x623000a080011400ULL, 0x42008c0340209202ULL,
            0x209188240001000ULL,  0x400408a884001800ULL, 0x110400a6080400ULL,   0x1840060a44020800ULL,
            0x90080104000041ULL,   0x201011000808101ULL,  0x1a2208080504f080ULL, 0x8012020600211212ULL,
            0x500861011240000ULL,  0x180806108200800ULL,  0x4000020e01040044ULL, 0x300000261044000aULL,
            0x802241102020002ULL,  0x20906061210001ULL,   0x5a84841004010310ULL, 0x4010801011c04ULL,
            0xa010109502200ULL,    0x4a02012000ULL,       0x500201010098b028ULL, 0x8040002811040900ULL,
            0x28000010020204ULL,   0x6000020202d0240ULL,  0x8918844842082200ULL, 0x4010011029020020ULL};

        // clang-format off
        static constexpr Bitboard PAWN_ATTACKS[NUM_COLORS][NUM_SQUARES] = {
            {0x200,
            0x500,0xa00,0x1400,0x2800,
            0x5000,0xa000,0x4000,0x20000,
            0x50000,0xa0000,0x140000,0x280000,
            0x500000,0xa00000,0x400000,0x2000000,
            0x5000000,0xa000000,0x14000000,0x28000000,
            0x50000000,0xa0000000,0x40000000,0x200000000,
            0x500000000,0xa00000000,0x1400000000,0x2800000000,
            0x5000000000,0xa000000000,0x4000000000,0x20000000000,
            0x50000000000,0xa0000000000,0x140000000000,0x280000000000,
            0x500000000000,0xa00000000000,0x400000000000,0x2000000000000,
            0x5000000000000,0xa000000000000,0x14000000000000,0x28000000000000,
            0x50000000000000,0xa0000000000000,0x40000000000000,0x200000000000000,
            0x500000000000000,0xa00000000000000,0x1400000000000000,0x2800000000000000,
            0x5000000000000000,0xa000000000000000,0x4000000000000000,0x0,
            0x0,0x0,0x0,0x0,
            0x0,0x0,0x0,},
            {0x0,
            0x0,0x0,0x0,0x0,
            0x0,0x0,0x0,0x2,
            0x5,0xa,0x14,0x28,
            0x50,0xa0,0x40,0x200,
            0x500,0xa00,0x1400,0x2800,
            0x5000,0xa000,0x4000,0x20000,
            0x50000,0xa0000,0x140000,0x280000,
            0x500000,0xa00000,0x400000,0x2000000,
            0x5000000,0xa000000,0x14000000,0x28000000,
            0x50000000,0xa0000000,0x40000000,0x200000000,
            0x500000000,0xa00000000,0x1400000000,0x2800000000,
            0x5000000000,0xa000000000,0x4000000000,0x20000000000,
            0x50000000000,0xa0000000000,0x140000000000,0x280000000000,
            0x500000000000,0xa00000000000,0x400000000000,0x2000000000000,
            0x5000000000000,0xa000000000000,0x14000000000000,0x28000000000000,
            0x50000000000000,0xa0000000000000,0x40000000000000,},
        };

        static constexpr Bitboard KNIGHT_ATTACKS[NUM_SQUARES] = {
            0x20400,
            0x50800,    0xa1100,    0x142200,    0x284400,
            0x508800,    0xa01000,    0x402000,    0x2040004,
            0x5080008,    0xa110011,    0x14220022,    0x28440044,
            0x50880088,    0xa0100010,    0x40200020,    0x204000402,
            0x508000805,    0xa1100110a,    0x1422002214,    0x2844004428,
            0x5088008850,    0xa0100010a0,    0x4020002040,    0x20400040200,
            0x50800080500,    0xa1100110a00,    0x142200221400,    0x284400442800,
            0x508800885000,    0xa0100010a000,    0x402000204000,    0x2040004020000,
            0x5080008050000,    0xa1100110a0000,    0x14220022140000,    0x28440044280000,
            0x50880088500000,    0xa0100010a00000,    0x40200020400000,    0x204000402000000,
            0x508000805000000,    0xa1100110a000000,    0x1422002214000000,    0x2844004428000000,
            0x5088008850000000,    0xa0100010a0000000,    0x4020002040000000,    0x400040200000000,
            0x800080500000000,    0x1100110a00000000,    0x2200221400000000,    0x4400442800000000,
            0x8800885000000000,    0x100010a000000000,    0x2000204000000000,    0x4020000000000,
            0x8050000000000,    0x110a0000000000,    0x22140000000000,    0x44280000000000,
            0x88500000000000,    0x10a00000000000,    0x20400000000000,
        };

        static constexpr Bitboard KING_ATTACKS[NUM_SQUARES] = {
            0x302,
            0x705,    0xe0a,    0x1c14,    0x3828,
            0x7050,    0xe0a0,    0xc040,    0x30203,
            0x70507,    0xe0a0e,    0x1c141c,    0x382838,
            0x705070,    0xe0a0e0,    0xc040c0,    0x3020300,
            0x7050700,    0xe0a0e00,    0x1c141c00,    0x38283800,
            0x70507000,    0xe0a0e000,    0xc040c000,    0x302030000,
            0x705070000,    0xe0a0e0000,    0x1c141c0000,    0x3828380000,
            0x7050700000,    0xe0a0e00000,    0xc040c00000,    0x30203000000,
            0x70507000000,    0xe0a0e000000,    0x1c141c000000,    0x382838000000,
            0x705070000000,    0xe0a0e0000000,    0xc040c0000000,    0x3020300000000,
            0x7050700000000,    0xe0a0e00000000,    0x1c141c00000000,    0x38283800000000,
            0x70507000000000,    0xe0a0e000000000,    0xc040c000000000,    0x302030000000000,
            0x705070000000000,    0xe0a0e0000000000,    0x1c141c0000000000,    0x3828380000000000,
            0x7050700000000000,    0xe0a0e00000000000,    0xc040c00000000000,    0x203000000000000,
            0x507000000000000,    0xa0e000000000000,    0x141c000000000000,    0x2838000000000000,
            0x5070000000000000,    0xa0e0000000000000,    0x40c0000000000000,
        };
        // clang-format on

        // Calculate pawn right attacks
        template <Color c>
        static constexpr inline Bitboard _pawnRightAttacks(const Bitboard b) {
            if constexpr (c == Color::WHITE) {
                return (b << 9) & ~Bitboard(File::FILE_A);
            } else {
                return (b >> 9) & ~Bitboard(File::FILE_H);
            }
        }

        // Calculate pawn left attacks
        template <Color c>
        static constexpr inline Bitboard _pawnLeftAttacks(const Bitboard b) {
            if constexpr (c == Color::WHITE) {
                return (b << 7) & ~Bitboard(File::FILE_H);
            } else {
                return (b >> 7) & ~Bitboard(File::FILE_A);
            }
        }

        // Calculate pawn attacks
        template <Color c>
        static constexpr inline Bitboard _pawnAttacks(Square s) {
            Bitboard b{s};
            return _pawnLeftAttacks<c>(b) | _pawnRightAttacks<c>(b);
        }

        // Calculate knight attacks
        static constexpr inline Bitboard _knightAttacks(Square s) {
            Bitboard b{s};

            const Bitboard knight_not_a_file  = b & ~Bitboard(File::FILE_A);
            const Bitboard knight_not_h_file  = b & ~Bitboard(File::FILE_H);
            const Bitboard knight_not_ab_file = knight_not_a_file & ~Bitboard(File::FILE_B);
            const Bitboard knight_not_gh_file = knight_not_h_file & ~Bitboard(File::FILE_G);

            Bitboard attacks;

            attacks |= (knight_not_a_file << 15) | (knight_not_a_file >> 17);
            attacks |= (knight_not_h_file << 17) | (knight_not_h_file >> 15);
            attacks |= (knight_not_ab_file << 6) | (knight_not_ab_file >> 10);
            attacks |= (knight_not_gh_file << 10) | (knight_not_gh_file >> 6);

            return attacks;
        }

        // Calculate king attacks
        static constexpr inline Bitboard _kingAttacks(Square s) {
            Bitboard b{s};

            return b.shift<Direction::NORTH>() | b.shift<Direction::SOUTH>() | b.shift<Direction::EAST>() |
                   b.shift<Direction::WEST>() | b.shift<Direction::NORTH_EAST>() |
                   b.shift<Direction::NORTH_WEST>() | b.shift<Direction::SOUTH_EAST>() |
                   b.shift<Direction::SOUTH_WEST>();
        }

        // Calculate bishop attacks
        static constexpr inline Bitboard _bishopAttacks(Square s, Bitboard occupied) {
            Bitboard attacks;

            Rank r;
            File f;

            Rank squareRank = s.rank();
            File squareFile = s.file();

            // Attacks in north east direction
            for (r = squareRank + 1, f = squareFile + 1; r <= Rank::RANK_8 && f <= File::FILE_H; ++r, ++f) {
                Bitboard b{Square(f, r)};

                attacks |= b;

                if (occupied & b) {
                    break;
                }
            }

            // Attacks in south east direction
            for (r = squareRank - 1, f = squareFile + 1; r >= Rank::RANK_1 && f <= File::FILE_H; --r, ++f) {
                Bitboard b{Square(f, r)};

                attacks |= b;

                if (occupied & b) {
                    break;
                }
            }

            // Attacks in north west direction
            for (r = squareRank + 1, f = squareFile - 1; r <= Rank::RANK_8 && f >= File::FILE_A; ++r, --f) {
                Bitboard b{Square(f, r)};

                attacks |= b;

                if (occupied & b) {
                    break;
                }
            }

            // Attacks in south west direction
            for (r = squareRank - 1, f = squareFile - 1; r >= Rank::RANK_1 && f >= File::FILE_A; --r, --f) {
                Bitboard b{Square(f, r)};

                attacks |= b;

                if (occupied & b) {
                    break;
                }
            }

            return attacks;
        }

        // Calculate rook attacks
        static constexpr inline Bitboard _rookAttacks(Square s, Bitboard occupied) {
            Bitboard attacks;

            File f;
            Rank r;

            Rank squareRank = s.rank();
            File squareFile = s.file();

            // Attacks in north direction
            for (r = squareRank + 1; r <= Rank::RANK_8; ++r) {
                Bitboard b{Square(squareFile, r)};

                attacks |= b;

                if (occupied & b) {
                    break;
                }
            }

            // Attacks in south direction
            for (r = squareRank - 1; r >= Rank::RANK_1; --r) {
                Bitboard b{Square(squareFile, r)};

                attacks |= b;

                if (occupied & b) {
                    break;
                }
            }

            // Attacks in east direction
            for (f = squareFile + 1; f <= File::FILE_H; ++f) {
                Bitboard b{Square(f, squareRank)};

                attacks |= b;

                if (occupied & b) {
                    break;
                }
            }

            // Attacks in west direction
            for (f = squareFile - 1; f >= File::FILE_A; --f) {
                Bitboard b{Square(f, squareRank)};

                attacks |= b;

                if (occupied & b) {
                    break;
                }
            }

            return attacks;
        }

        static inline void printPawnAttacksTable() {
            std::cout << "static constexpr Bitboard PAWN_ATTACKS[NUM_COLORS][NUM_SQUARES] = {\n";

            std::cout << "    {";

            for (const auto s : SquareIterator()) {
                std::cout << "0x" << std::hex << static_cast<U64>(_pawnAttacks<Color::WHITE>(s)) << std::dec
                          << ",";

                if (s % 4 == 0) {
                    std::cout << "\n     ";
                }
            }

            std::cout << "},\n";

            std::cout << "    {";

            for (const auto s : SquareIterator()) {
                std::cout << "0x" << std::hex << static_cast<U64>(_pawnAttacks<Color::BLACK>(s)) << std::dec
                          << ",";

                if (s % 4 == 0) {
                    std::cout << "\n     ";
                }
            }

            std::cout << "},\n";

            std::cout << "};\n";

            std::cout << "\n";
        }

        static inline void printKnightAttacksTable() {
            std::cout << "static constexpr Bitboard KNIGHT_ATTACKS[NUM_SQUARES] = {\n";

            for (const auto s : SquareIterator()) {
                std::cout << "    0x" << std::hex << static_cast<U64>(_knightAttacks(s)) << std::dec << ",";

                if (s % 4 == 0) {
                    std::cout << "\n";
                }
            }

            std::cout << "};\n";

            std::cout << "\n";
        }

        static inline void printKingAttacksTable() {
            std::cout << "static constexpr Bitboard KING_ATTACKS[NUM_SQUARES] = {\n";

            for (const auto s : SquareIterator()) {
                std::cout << "    0x" << std::hex << static_cast<U64>(_kingAttacks(s)) << std::dec << ",";

                if (s % 4 == 0) {
                    std::cout << "\n";
                }
            }

            std::cout << "};\n";

            std::cout << "\n";
        }

        struct MagicEntry {
            U64     magic;
            U64     mask;
            U64*    table;
            uint8_t offset;

            constexpr std::size_t index(U64 blockers) const {
                return _pext_u64(blockers, mask);
            }
        };

        static constexpr std::size_t ROOK_ATTACKS_SIZE   = 102400;
        static constexpr std::size_t BISHOP_ATTACKS_SIZE = 5248;

        static inline std::array<U64, ROOK_ATTACKS_SIZE>   ROOK_ATTACKS;
        static inline std::array<U64, BISHOP_ATTACKS_SIZE> BISHOP_ATTACKS;

        static inline std::array<MagicEntry, NUM_SQUARES> ROOK_MAGICS_ENTRIES;
        static inline std::array<MagicEntry, NUM_SQUARES> BISHOP_MAGICS_ENTRIES;

    public:
        static inline std::array<std::array<Bitboard, 64>, 64> SQUARES_BETWEEN;

    private:
        template <typename F>
        static inline void generateSlidersAttacks(Square sq, std::array<MagicEntry, NUM_SQUARES>& magicTable,
                                                  U64 magic, F attacks) {
            Bitboard edgesMask = ((Bitboard(Rank::RANK_1) | Bitboard(Rank::RANK_8)) & ~Bitboard(sq.rank())) |
                                 ((Bitboard(File::FILE_A) | Bitboard(File::FILE_H)) & ~Bitboard(sq.file()));

            U64 occupancy = 0;

            magicTable[sq].magic  = magic;
            magicTable[sq].mask   = attacks(sq, Bitboard(occupancy)) & ~edgesMask;
            magicTable[sq].offset = NUM_SQUARES - bitops::popcount(magicTable[sq].mask);

            if (sq < (NUM_SQUARES - 1)) {
                magicTable[sq + 1].table =
                    magicTable[sq].table + (1ULL << bitops::popcount(magicTable[sq].mask));
            }

            do {
                const auto index            = magicTable[sq].index(occupancy);
                magicTable[sq].table[index] = attacks(sq, Bitboard(occupancy));

                occupancy = (occupancy - magicTable[sq].mask) & magicTable[sq].mask;
            } while (occupancy);
        }

        static inline void generateSquaresBetween() {
            Bitboard b;

            for (const auto sq : SquareIterator()) {
                for (const auto sq2 : SquareIterator()) {
                    b = Bitboard(sq) | Bitboard(sq2);

                    if (sq == sq2) {
                        SQUARES_BETWEEN[sq][sq2] = Bitboard();
                    } else if (sq.diagonal() == sq2.diagonal() || sq.antiDiagonal() == sq2.antiDiagonal()) {
                        SQUARES_BETWEEN[sq][sq2] = bishopAttacks(sq, b) & bishopAttacks(sq2, b);
                    } else if (sq.rank() == sq2.rank() || sq.file() == sq2.file()) {
                        SQUARES_BETWEEN[sq][sq2] = rookAttacks(sq, b) & rookAttacks(sq2, b);
                    }
                }
            }
        }

    public:
        static inline void init() {
            // set the pointer to the attacks table
            BISHOP_MAGICS_ENTRIES[0].table = BISHOP_ATTACKS.data();
            ROOK_MAGICS_ENTRIES[0].table   = ROOK_ATTACKS.data();

            for (const auto sq : SquareIterator()) {
                generateSlidersAttacks(sq, BISHOP_MAGICS_ENTRIES, BISHOP_MAGICS[sq], _bishopAttacks);
                generateSlidersAttacks(sq, ROOK_MAGICS_ENTRIES, ROOK_MAGICS[sq], _rookAttacks);
            }

            generateSquaresBetween();
        }

        template <Color c>
        static constexpr inline Bitboard pawnLeftAttacks(Bitboard b) {
            return _pawnLeftAttacks<c>(b);
        }

        template <Color c>
        static constexpr inline Bitboard pawnRightAttacks(Bitboard b) {
            return _pawnRightAttacks<c>(b);
        }

        template <Color c>
        static constexpr inline Bitboard pawnAttacks(Square s) {
            return PAWN_ATTACKS[static_cast<int>(c)][s];
        }

        static constexpr inline Bitboard pawnAttacks(Square s, Color c) {
            return PAWN_ATTACKS[static_cast<int>(c)][s];
        }

        static constexpr inline Bitboard knightAttacks(Square s) {
            return KNIGHT_ATTACKS[s];
        }

        static constexpr inline Bitboard kingAttacks(Square s) {
            return KING_ATTACKS[s];
        }

        static constexpr inline Bitboard bishopAttacks(Square s, Bitboard occupied) {
            return BISHOP_MAGICS_ENTRIES[s].table[BISHOP_MAGICS_ENTRIES[s].index(occupied)];
        }

        static constexpr inline Bitboard rookAttacks(Square s, Bitboard occupied) {
            return ROOK_MAGICS_ENTRIES[s].table[ROOK_MAGICS_ENTRIES[s].index(occupied)];
        }

        static constexpr inline Bitboard queenAttacks(Square s, Bitboard occupied) {
            return bishopAttacks(s, occupied) | rookAttacks(s, occupied);
        }

        static constexpr inline Bitboard calculateBishopAttacks(Square s, Bitboard occupied) {
            return _bishopAttacks(s, occupied);
        }

        static constexpr inline Bitboard calculateRookAttacks(Square s, Bitboard occupied) {
            return _rookAttacks(s, occupied);
        }

        static constexpr inline Bitboard squaresBetween(Square s1, Square s2) {
            return SQUARES_BETWEEN[s1][s2] | Bitboard(s2);
        }
    };

} // namespace chess