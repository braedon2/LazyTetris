#include <algorithm>
#include <queue>
#include <vector>
#include "constants.h"
#include "tetris.h"
#include "solver.h"

void setNodeNeighbours(GraphNode& node, Graph* graph, GameGrid& grid) {
    // hot path optimization: this function gets called a lot so instead of using tetrimino.move which makes a copy,
    // a copy of the node tetrimino is made and its state is modified directly
    Tetrimino tetriminoCopy(node.tetrimino.shape, node.tetrimino.xDelta, node.tetrimino.yDelta, node.tetrimino.rotationStep);

    tetriminoCopy.xDelta -= 1; // move left
    if (not grid.checkCollision(tetriminoCopy)) {
        node.neighbours.push_back(&(*graph)[tetriminoCopy.yDelta][tetriminoCopy.xDelta][tetriminoCopy.rotationStep]);
    }

    tetriminoCopy.xDelta += 2; // move right
    if (not grid.checkCollision(tetriminoCopy)) {
        node.neighbours.push_back(&(*graph)[tetriminoCopy.yDelta][tetriminoCopy.xDelta][tetriminoCopy.rotationStep]);
    }

    tetriminoCopy.xDelta -= 1; // undo move right
    int oldRotationStep = tetriminoCopy.rotationStep;
    tetriminoCopy.rotationStep = (tetriminoCopy.rotationStep + 1) % tetriminoCopy.rotationList->size();
    if (not grid.checkCollision(tetriminoCopy)) {
        node.neighbours.push_back(&(*graph)[tetriminoCopy.yDelta][tetriminoCopy.xDelta][tetriminoCopy.rotationStep]);
    }
    tetriminoCopy.rotationStep = oldRotationStep; // restore rotation step

    tetriminoCopy.yDelta += 1; // move down
    if (not grid.checkCollision(tetriminoCopy)) {
        node.neighbours.push_back(&(*graph)[tetriminoCopy.yDelta][tetriminoCopy.xDelta][tetriminoCopy.rotationStep]);
    }
}


std::unique_ptr<Graph> makeGraph(Tetrimino& tetrimino, GameGrid& grid) {
    auto graph = std::make_unique<Graph>();

    for (int y = 0; y < GRID_HEIGHT; y++) {
        for (int x = 0; x < GRID_WIDTH; x++) {
            for (int rotation = 0; rotation < tetrimino.rotationList->size(); rotation++) {
                (*graph)[y][x][rotation] = { // GraphNode
                    .tetrimino = Tetrimino(tetrimino.shape, x, y, rotation),
                };
            }
        }
    }

    for (std::array<std::array<GraphNode, 4>, GRID_WIDTH>& row : *graph) {
        for (std::array<GraphNode, 4>& col : row) {
            for(GraphNode& node : col) {
                if (node.tetrimino.shape != N) { // tetrimino is not null
                    setNodeNeighbours(node, graph.get(), grid);
                }
            }
        }
    }
    return graph;
}


std::vector<GraphNode*> search(Graph* graph, Tetrimino& tetrimino, GameGrid& grid) {
    std::queue<GraphNode*> queue;
    std::vector<GraphNode*> results;

    GraphNode& root = (*graph)[tetrimino.yDelta][tetrimino.xDelta][tetrimino.rotationStep];
    root.visited = true;  
    queue.push(&root);

    while (not queue.empty()) {
        GraphNode* node = queue.front();
        queue.pop();

        if (grid.checkCollision(node->tetrimino.move(down))) {
            results.push_back(node);
        }

        for (GraphNode* neighbour : node->neighbours) {
            if (not neighbour->visited) {
                neighbour->visited = true;
                neighbour->prev = node;
                queue.push(neighbour);
            }
        }
    }
    return results;
}


Moves movesToReachSearchResult(GraphNode* searchResult) {
    GraphNode* node = searchResult;
    Moves moves; // iterating from locked position to spawn point means this will need to be reversed before returning

    while (node->prev != nullptr) {
        Tetrimino current = node->tetrimino;
        Tetrimino parent = node->prev->tetrimino;

        if (current.xDelta != parent.xDelta) {
            parent.xDelta < current.xDelta ? moves.push_back(right) : moves.push_back(left);
        }
        else if (current.rotationStep != parent.rotationStep) {
            parent.rotationStep < current.rotationStep ? moves.push_back(clockwise) : moves.push_back(counterClockwise);
        }
        else {
            moves.push_back(down);
        }
        node = node->prev;
    }
    std::reverse(moves.begin(), moves.end());
    moves.push_back(down); // add a final down move to lock piece in place on the grid
    return moves;
}


void computeEvaluationFactors(GameGrid grid, EvaluationFactors& factors) {
    // A well cell is an empty cell located above all the solid cells within its column such that 
    // its left and right neighbors are both solid cells; the playfield walls are treated as solid 
    // cells in this determination. The idea is that a well is a structure open at the top, sealed 
    // at the bottom and surrounded by walls on both sides. The possibility of intermittent gaps in
    //  the well walls means that well cells do not necessarily appear in a contiguous stack within a column.
    for (int col = 0; col < GRID_WIDTH; col++) {
        for (int row = 1; row < GRID_HEIGHT; row++) {
            if (not grid.isEmpty(col, row)) {
                break;
            }

            if (col == 0) {
                if (not grid.isEmpty(col+1, row)) {
                    factors.totalWellCells++;
                }
            } else if (col == GRID_WIDTH-1) {
                if (not grid.isEmpty(col-1, row)) {
                    factors.totalWellCells++;
                }
            } else if (not grid.isEmpty(col-1, row) and not grid.isEmpty(col+1, row)) {
                factors.totalWellCells++;
            }
        }
    }

    // A column hole is an empty cell directly beneath a solid cell. The playfield floor is not 
    // compared to the cell directly above it. Empty columns contain no holes.

    // A column transition is an empty cell adjacent to a solid cell (or vice versa) within the 
    // same column. The changeover from the highest solid block in the column to the empty space 
    // above it is not considered a transition. Similarly, the playfield floor is not compared to 
    // the cell directly above it. As a result, a completely empty column has no transitions.
    for (int col = 0; col < GRID_WIDTH; col++) {
        bool firstBlockFound = false; // since loop scans from top of grid to bottom, this is set true when first solid block is found
        bool solid = true; // tracks if the last block was solid or empty

        for (int row = 1; row < GRID_HEIGHT; row++) {
            if (not grid.isEmpty(col, row)) {
                firstBlockFound = true;
            }
            if (grid.isEmpty(col, row) and firstBlockFound) {
                if ( not grid.isEmpty(col, row - 1)) {
                    factors.totalColumnHoles += 1;
                }
            }
            if (grid.isEmpty(col, row) and firstBlockFound and solid) {
                factors.totalColumnTransistions += 1;
                solid = false;
            }
            if (not grid.isEmpty(col, row) and firstBlockFound and not solid) {
                factors.totalColumnTransistions += 1;
                solid = true;
            }
        }
    }

    // A row transition is an empty cell adjacent to a solid cell (or vice versa) within the same 
    // row. Empty cells adjoining playfield walls are considered transitions. The total is computed
    //  across all rows in the playfield. However, rows that are completely empty do not contribute to the sum.
    for (int row = 0; row < GRID_HEIGHT; row++) {
        bool solid = true; // tracks if the last block was solid or empty
        int transitions = 0; // 2 transitions means row is empty and this is not added to the total
        bool lineEmpty = true;
        for (int col = 0; col < GRID_WIDTH; col++) {
            if (lineEmpty and not grid.isEmpty(col, row)) {
                lineEmpty = false;
            }

            if (col == GRID_WIDTH-1 and grid.isEmpty(col, row)) {
                transitions += 1;
            }
            if (grid.isEmpty(col, row) and solid) {
                transitions += 1;
                solid = false;
            }
            else if (not grid.isEmpty(col, row) and not solid) {
                transitions += 1;
                solid = true;
            }
        }
        if (not lineEmpty) {
            factors.totalRowTransitions += transitions;
        }
    }
}


double computeFitness(EvaluationFactors factors, EvaluationWeights weights) {
    return (
        factors.totalLinesCleared * weights.totalLinesCleared +
        factors.totalLockHeight * weights.totalLockHeight +
        factors.totalWellCells * weights.totalWellCells + 
        factors.totalColumnHoles * weights.totalColumnHoles + 
        factors.totalColumnTransistions * weights.totalColumnTransitions + 
        factors.totalRowTransitions * weights.totalRowTransitions
    );
}

GraphNode* solve(Graph* firstTetriminoGraph, GameGrid& grid, Tetrimino firstTetrimino, Tetrimino secondTetrimino, EvaluationWeights weights) {
    GraphNode* bestResult = nullptr;
    double bestFitness = -1.0;

    auto analyze = [&firstTetriminoGraph, &bestResult, &bestFitness, &weights](GameGrid& grid, int totalLockHeight, int linesCleared, GraphNode* tetriminoPlacement) {
        EvaluationFactors factors;
        computeEvaluationFactors(grid, factors);
        factors.totalLinesCleared = linesCleared;
        factors.totalLockHeight = totalLockHeight;

        double fitness = computeFitness(factors, weights);

        if (bestFitness < 0 or fitness < bestFitness) {
            bestFitness = fitness;
            bestResult = tetriminoPlacement;
        }
    };

    GraphNode* defaultResult = analyzeAllCombinations(analyze, firstTetriminoGraph, grid, firstTetrimino, secondTetrimino);

    return bestResult ? bestResult : defaultResult;
}


Moves solveForMovesToOptimalTetrimino(GameGrid grid, Tetrimino firstTetrimino, Tetrimino secondTetrimino, EvaluationWeights weights) {
    auto firstGraph = makeGraph(firstTetrimino, grid);
    GraphNode* bestResult = solve(firstGraph.get(), grid, firstTetrimino, secondTetrimino, weights);
    return movesToReachSearchResult(bestResult);
}


Tetrimino solveForOptimalTetrimino(GameGrid grid, Tetrimino firstTetrimino, Tetrimino secondTetrimino, EvaluationWeights weights) {
    auto firstGraph = makeGraph(firstTetrimino, grid);
    GraphNode* bestResult = solve(firstGraph.get(), grid, firstTetrimino, secondTetrimino, weights);
    return bestResult->tetrimino;
}