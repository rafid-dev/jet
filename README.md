# Jet Chess Engine

Jet is a cutting-edge chess engine designed for high-performance computing tasks. With its ultra-fast move generator, advanced search algorithms, and neural network-based evaluation system, Jet pushes the boundaries of chess engine development.

## Key Features

- **Ultra Fast Move Generator**: Jet's move generator is optimized for speed, capable of processing up to 1 billion nodes per second on high-end hardware like the i7 14700KF.

- **GPL 3 Licensed**: Jet is released under the GNU General Public License Version 3, affirming its commitment to openness and collaboration within the community.

- **NNUE Evaluation with Autovectorized Inference**: Jet leverages Neural Network with Efficiently Updatable Neural Network (NNUE) evaluation, utilizing autovectorized inference for position evaluations.

- **Search features** Jet employs Alpha-beta pruning alongside advanced search techniques like Late Move Reduction and Singular Extensions to optimize its search, enhancing computational efficiency and playing strength.

## Building Instructions

### Required Tools

- Make
- Clang compiler (GCC works but not preferred)

### Building on Windows

```bash
git clone https://github.com/rafid-dev/jet.git
cd jet
make -j CXX=clang++
```

### Building on Linux

Replace `XX` with your Linux's clang version or leave as `clang++` if symlinked.

```bash
git clone https://github.com/rafid-dev/jet
cd jet
make -j CXX=clang++-XX
```

Ensure everything works fine by running `./Jet bench` and verifying that the nodes match the current commit bench nodes.

## Testing and Support

Testing of Jet is supported by the OpenBench Instance at [https://rafiddev.pythonanywhere.com/](https://rafiddev.pythonanywhere.com/).

Special thanks to contributors:

- **Vast** ([GitHub](https://github.com/Vast342)), author of Clarity ([GitHub](https://github.com/Vast342/Clarity/))
- **Sazgr** ([GitHub](https://github.com/Sazgr/peacekeeper))

And my great friends:
- **Raiyad**
- **Ariyan737**
- **Rafsan60**
- **Uganiga**
- **Daredevil1618**
- **Rafsan60**

## Acknowledgements

Jet acknowledges the following projects for their contributions and inspiration:

- **Disservin's Chess Library** ([GitHub](https://github.com/Disservin/chess-library)) for ideas on organizing move generation functions.
- **Grapheus NNUE Trainer** by Luecx ([GitHub](https://github.com/luecx/Grapheus/))
- **OpenBench** for distributed SPRT testing by Andrew ([GitHub](https://github.com/AndyGrant/OpenBench))

## Supporting Development

Support the development of Jet by contributing threads on [https://rafiddev.pythonanywhere.com/](https://rafiddev.pythonanywhere.com/). Register an account and contact the developer for more information.

## Contact

For inquiries and collaboration opportunities, reach out to the developer on Discord at `j.en`.

Let's push the boundaries of chess engine development with Jet! 🚀♟️
