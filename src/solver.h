#ifndef SOLVER_H
#define SOLVER_H

#include <array>
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
    GraphNode** begin() {return this->arr.begin();}; 
    GraphNode** end() {return this->arr.begin() + this->size;}; 
    std::array<GraphNode*, 4>::const_iterator begin() const {return this->arr.begin();};
    std::array<GraphNode*, 4>::const_iterator end() const {return this->arr.begin() + this->size;};
};

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

typedef std::array<std::array<std::array<GraphNode, 4>, GRID_WIDTH>, GRID_HEIGHT> Graph; // first dimension is row second dimension is column third dimension is rotation
typedef std::variant<Direction, Rotation> Move;
typedef std::vector<Move> Moves;

void setNodeNeighbours(GraphNode& node, Graph& graph, GameGrid& grid);
Graph makeGraph(Tetrimino& tetrimino, GameGrid& grid);
std::vector<GraphNode*> search(Graph &graph, Tetrimino& tetrimino, GameGrid& grid);
Moves movesToReachSearchResult(GraphNode* searchResult);

void computeEvaluationFactors(GameGrid grid, EvaluationFactors& factors);
double computeFitness(EvaluationFactors factors);

GraphNode* solve(Graph& firstTetriminoGraph, GameGrid& grid, Tetrimino firstTetrimino, Tetrimino secondTetrimino);
Moves solveForMovesToOptimalTetrimino(GameGrid grid, Tetrimino firstTetrimino, Tetrimino secondTetrimino);
Tetrimino solveForOptimalTetrimino(GameGrid grid, Tetrimino firstTetrimino, Tetrimino secondTetrimino);

#endif