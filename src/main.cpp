#include "bench.hpp"
#include "chess/movegen.hpp"
#include "chess/zobrist.hpp"
#include "evaluation/evaluate.hpp"
#include "nnue/nnue.hpp"
#include "perfsuite.hpp"
#include "search/search.hpp"
#include "search/searchinfo.hpp"
#include "search/searchthread.hpp"

#include <iostream>
#include <istream>
#include <memory>
#include <sstream>
#include <type_traits>

using namespace chess;
using namespace jet;
using namespace jet::search;

static constexpr std::string_view NAME    = "Jet";
static constexpr std::string_view VERSION = "1.2";
static constexpr std::string_view AUTHOR  = "Rafid Ahsan";

template<typename T>
void set_option(std::istream &is, std::string &token, const std::string& name, T &value) {
    if (token == name) {
        is >> std::skipws >> token;
        is >> std::skipws >> token;

        if constexpr (std::is_floating_point_v<T>) {
            value = std::stof(token);
        } else {
            value = std::stoi(token);
        }
    }
}

template<typename T>
struct TypeName {
    static std::string get() {
        return typeid(T).name();
    }
};

template<bool uci = false, typename T>
void print_parameter_inputs(const std::string& name,
                    T current_val, float min_val, float max_val,
                    float start_lr, float end_lr) {
    if constexpr(uci){
        std::cout << "option name " << name << " type string default " << current_val << std::endl;
    }else{
        std::cout << name << ", " << TypeName<T>::get() << ", " << current_val << ", " << min_val << ", " << max_val << ", "
         << start_lr << ", " << end_lr << std::endl;
    }
}

#define TUNING_OPTION(param) set_option(iss, token, #param, param)
#define PARAM_INPUT(param, min, max, start, end) if (!uci) { print_parameter_inputs(#param, param, min, max, start, end); } else { print_parameter_inputs<true>(#param, param, min, max, start, end); }  

void print_parameter_inputs(bool uci) {
    using namespace search::search_params;

    PARAM_INPUT(lmr_base, 0.5, 3.0, 0.1, 0.002);
    PARAM_INPUT(lmr_division, 0.5, 3.0, 0.1, 0.002);
    PARAM_INPUT(lmr_see_margin, -1000, 0, 2.25, 0.002);
    PARAM_INPUT(qs_see_ordering_threshold, -1000, 0, 2.25, 0.002);

    PARAM_INPUT(nmp_base, 3, 5, 1, 0.002);
    PARAM_INPUT(nmp_depth_divisor, 1, 5, 1, 0.002);
    PARAM_INPUT(nmp_max_scaled_depth, 2, 5, 1, 0.002);
    PARAM_INPUT(nmp_divisor, 100, 300, 2, 0.002);

    PARAM_INPUT(rfp_margin, 10, 100, 1, 0.002);
    PARAM_INPUT(rfp_depth, 6, 9, 1, 0.002);

    PARAM_INPUT(lmp_depth, 5, 9, 1, 0.002);
    PARAM_INPUT(lmp_base, 2, 5, 1, 0.002);
    PARAM_INPUT(lmp_scalar, 1, 5, 1, 0.002);

    PARAM_INPUT(se_depth, 5, 10, 1, 0.002);
    PARAM_INPUT(se_depth_offset, 1, 4, 1, 0.002);
    PARAM_INPUT(singular_scalar, 1, 10, 1, 0.002);
    PARAM_INPUT(singular_divisor, 1, 10, 1, 0.002);
    PARAM_INPUT(singular_depth_divisor, 1, 3, 1, 0.002);

    PARAM_INPUT(asp_delta, 5, 15, 1, 0.002);
}

int main(int argc, char** argv) {
    Attacks::init();
    nnue::init();
    search::init();

    std::cout << NAME << " " << VERSION << std::endl;
    std::cout << "Copyright (C) 2023  " << AUTHOR << std::endl;
    print_parameter_inputs(true);
    search::TranspositionTable.initialize<false>(16);

    auto   heapSt = std::make_unique<SearchThread>();
    auto&  st     = *heapSt;
    Board& board  = st.board();

    std::string line;
    std::string token;

    std::vector<Move> moves;

    jet::search::SearchInfo info;

    if (argc > 1 && std::string(argv[1]) == "bench") {
        StartBenchmark(st);
        return 0;
    }

    while (std::getline(std::cin, line)) {
        token.clear();

        std::istringstream iss(line);

        iss >> token;

        if (token == "export"){
            iss >> token;
            if (token == "searchparams"){
                print_parameter_inputs(false);
            } else {
                std::cout << "Unknown export option: " << token << std::endl;
                std::cout << "Did you mean: export searchparams" << std::endl;
            }
        }
        else if (token == "uci") {
            std::cout << "id name " << NAME << " " << VERSION << std::endl;
            std::cout << "id author " << AUTHOR << std::endl;
            std::cout << "option name Hash type spin default 8 min 8 max 32768" << std::endl;
            std::cout << "option name Threads type spin default 1 min 1 max 1" << std::endl;
            std::cout << "uciok" << std::endl;
        } else if (token == "isready") {
            std::cout << "readyok\n";
        } else if (token == "ucinewgame") {
            // Re initialize the transposition table upon a new game
            search::TranspositionTable.initialize<false>(16);
            st.setFen(FENS::STARTPOS);
        } else if (token == "movegen") {
            Movelist list;
            MoveGen::legalmoves<MoveGenType::ALL>(board, list);

            std::cout << list << std::endl;

            list.clear();
            MoveGen::legalmoves<MoveGenType::QUIET>(board, list);
            std::cout << "Quiet moves: " << list.size() << '\n';
            std::cout << list << std::endl;

            list.clear();
            MoveGen::legalmoves<MoveGenType::CAPTURE>(board, list);
            std::cout << "Capture moves: " << list.size() << '\n';
            std::cout << list << std::endl;

        } else if (token == "bench"){
            StartBenchmark(st);
            exit(0);
        } else if (token == "position") {
            iss >> token;

            if (token == "startpos") {
                st.setFen(FENS::STARTPOS);
                iss >> token;

            } else if (token == "kiwipete") {
                st.setFen(FENS::KIWIPETE);
                iss >> token;

            } else if (token == "fen") {
                std::string fen;

                while (iss >> token && token != "moves") {
                    fen += token + ' ';
                }

                st.setFen(fen);
            }

            if (token == "moves") {
                while (iss >> token) {
                    Move move = board.uciToMove(token);
                    // std::cout << "Parsed move: " << move << '\n';
                    board.makeMove(move);
                }
            }

            continue;
        } else if (token == "makemove" || token == "make") {
            while (iss >> token) {
                Move move = board.uciToMove(token);
                board.makeMove(move);
                moves.push_back(move);
            }

            std::cout << board << std::endl;
        } else if (token == "unmakemove" || token == "unmake") {
            board.unmakeMove(moves.back());
            moves.pop_back();

            std::cout << board << std::endl;

        } else if (token == "perftsuite") {
            iss >> token;

            perft::bulkSuite(token, 1000);
        } else if (token == "perft") {
            iss >> token;

            int depth = 6;
            if (token == "depth") {
                iss >> token;
                depth = std::stoi(token);
            }

            iss >> token;
            if (token == "speed") {
                perft::bulkSpeedTest(board, depth);
            } else {
                perft::startBulk(board, depth);
            }

            // Search things
        } else if (token == "go") {
            jet::search::TimeManager& tm = st.timeManager();

            tm.setNodes(0);
            tm.setMTG(0);

            while (iss >> token) {
                if (token == "depth") {
                    iss >> token;
                    info.setDepth(std::stoi(token));
                } else if (token == "wtime") {
                    iss >> token;
                    tm.setTime<TimeType::WTIME>(std::stod(token));
                } else if (token == "btime") {
                    iss >> token;
                    tm.setTime<TimeType::BTIME>(std::stod(token));
                } else if (token == "winc") {
                    iss >> token;
                    tm.setTime<TimeType::WINC>(std::stod(token));
                } else if (token == "binc") {
                    iss >> token;
                    tm.setTime<TimeType::BINC>(std::stod(token));
                } else if (token == "movetime") {
                    iss >> token;
                    tm.setTime<TimeType::MOVETIME>(std::stod(token));
                } else if (token == "movestogo") {
                    iss >> token;
                    tm.setMTG(std::stoi(token));
                } else if (token == "nodes") {
                    iss >> token;
                    tm.setNodes(std::stoull(token));
                }
            }

            jet::search::search(st, info);
        } else if (token == "setoption") {
            iss >> token;
            iss >> token;

            using namespace search::search_params;

            TUNING_OPTION(lmr_base);
            TUNING_OPTION(lmr_division);
            TUNING_OPTION(lmr_see_margin);

            TUNING_OPTION(qs_see_ordering_threshold);

            TUNING_OPTION(nmp_base);
            TUNING_OPTION(nmp_depth_divisor);
            TUNING_OPTION(nmp_max_scaled_depth);
            TUNING_OPTION(nmp_divisor);

            TUNING_OPTION(rfp_margin);
            TUNING_OPTION(rfp_depth);

            TUNING_OPTION(lmp_depth);
            TUNING_OPTION(lmp_base);
            TUNING_OPTION(lmp_scalar);

            TUNING_OPTION(se_depth);
            TUNING_OPTION(se_depth_offset);
            TUNING_OPTION(singular_scalar);
            TUNING_OPTION(singular_divisor);
            TUNING_OPTION(singular_depth_divisor);
            TUNING_OPTION(singular_depth_intercept);

            TUNING_OPTION(asp_delta);

            if (token == "Hash") {
                iss >> token;
                iss >> token;
                search::TranspositionTable.initialize<true>(std::clamp(std::stoi(token), 8, 32768));
            }

            search::init();
        } else if (token == "print") {
            std::cout << board << std::endl;
            st.refresh();
            std::cout << "Eval: " << jet::evaluation::evaluate(st) << std::endl;
        } else if (token == "quit" || token == "exit") {
            break;
        } else if (token == "\n") {
            continue;
        } else {
            std::cout << "Unknown command: " << token << '\n';
        }
    }
    return 0;
}