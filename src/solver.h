#ifndef SOLVER_H
#define SOLVER_H

#include <array>
#include <memory>
#include <utility>
#include <variant>
#include <vector>
#include "constants.h"
#include "tetris.h"

struct GraphNode; 

/* 
 * This class was made for optimization purposes.
 * It's goal is to act like a vector but under the hood it's an array of 
 * size 4.
 * Originally, node neighbours were stored in a vector but this was very costly
 * because the vectors were very small and many were allocated.
 * We know a node's neighbours will be at most 4 by looking at setNodeNeighbours.
 * We can take advantage of this by having a stack allocated array with a vector
 * like interface.
 */
 class NodeNeighbours {
    public:
    std::array<GraphNode*, 4> arr{};
    std::size_t size = 0;

    void push_back(GraphNode* neighbour) {this->arr[this->size] = neighbour; this->size++;};
    std::array<GraphNode*, 4>::iterator end() {return this->arr.begin() + this->size;};
    std::array<GraphNode*, 4>::iterator begin() { return this->arr.begin(); };
    std::array<GraphNode*, 4>::const_iterator begin() const {return this->arr.begin();};
    std::array<GraphNode*, 4>::const_iterator end() const {return this->arr.begin() + this->size;};
};

 //class Graph {
 //public:
 //    std::vector<GraphNode> graph{ GRID_FRAME_HEIGHT * GRID_WIDTH * 4 };
 //    GraphNode at(size_t row, size_t col, size_t rotation) { return graph[(row * GRID_WIDTH * 4) + (col * 4) + rotation]; }
 //    void set(size_t row, size_t col, size_t rotation, GraphNode node) { graph[(row * GRID_WIDTH * 4) + (col * 4) + rotation] = node; }
 //};

struct GraphNode {
    Tetrimino tetrimino;
    bool visited = false;
    GraphNode* prev = nullptr;
    NodeNeighbours neighbours;
};

struct EvaluationFactors {
    int totalLinesCleared = 0;
    int totalLockHeight = 0;
    int totalWellCells = 0;
    int totalColumnHoles = 0;
    int totalColumnTransistions = 0;
    int totalRowTransitions = 0;
};

struct EvaluationWeights {
    double totalLinesCleared;
    double totalLockHeight;
    double totalWellCells;
    double totalColumnHoles;
    double totalColumnTransitions;
    double totalRowTransitions;
};

typedef std::array<std::array<std::array<GraphNode, 4>, GRID_WIDTH>, GRID_HEIGHT> Graph; // first dimension is row second dimension is column third dimension is rotation
typedef std::variant<Direction, Rotation> Move;
typedef std::vector<Move> Moves;

void setNodeNeighbours(GraphNode& node, Graph* graph, GameGrid& grid);
std::unique_ptr<Graph> makeGraph(Tetrimino& tetrimino, GameGrid& grid);
std::vector<GraphNode*> search(Graph* graph, Tetrimino& tetrimino, GameGrid& grid);
Moves movesToReachSearchResult(GraphNode* searchResult);
void computeEvaluationFactors(GameGrid grid, EvaluationFactors& factors);
double computeFitness(EvaluationFactors factors, EvaluationWeights weights);

GraphNode* solve(
    Graph* firstTetriminoGraph,
    GameGrid& grid, 
    Tetrimino firstTetrimino, 
    Tetrimino secondTetrimino, 
    EvaluationWeights weights);

Moves solveForMovesToOptimalTetrimino(GameGrid grid, Tetrimino firstTetrimino, Tetrimino secondTetrimino, EvaluationWeights weights);
Tetrimino solveForOptimalTetrimino(GameGrid grid, Tetrimino firstTetrimino, Tetrimino secondTetrimino, EvaluationWeights weights);

template <typename Func>
GraphNode* analyzeAllCombinations(Func analyze, Graph* firstTetriminoGraph, GameGrid& grid, Tetrimino firstTetrimino, Tetrimino secondTetrimino) {
    std::vector<GraphNode*> firstResults = search(firstTetriminoGraph, firstTetrimino, grid);

    for (GraphNode* firstResult : firstResults) {
        if (grid.checkCollision(firstResult->tetrimino)) {
            continue;
        }

        GameGrid gridCopy = grid;
        gridCopy.setCells(firstResult->tetrimino);
        int linesCleared = static_cast<int>(gridCopy.getFullRows().size());
        gridCopy.clearFullRows();

        auto secondGraph = makeGraph(secondTetrimino, gridCopy);
        std::vector<GraphNode*> secondResults = search(secondGraph.get(), secondTetrimino, gridCopy);

        for (GraphNode* secondResult : secondResults) {
            if (gridCopy.checkCollision(secondResult->tetrimino)) {
                continue;
            }

            GameGrid secondGridCopy = gridCopy;
            secondGridCopy.setCells(secondResult->tetrimino);
            linesCleared += static_cast<int>(gridCopy.getFullRows().size());
            secondGridCopy.clearFullRows();

            analyze(secondGridCopy, firstTetrimino.getHeight() + secondTetrimino.getHeight(), linesCleared, firstResult);
        }
    }

    return firstResults.at(0); // need a default result in case everything causes collisions with the grid
}

#endif