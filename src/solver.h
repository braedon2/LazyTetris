#ifndef SOLVER_H
#define SOLVER_H

#include <array>
#include <variant>
#include <vector>
#include "constants.h"
#include "tetris.h"

struct GraphNode {
    Tetrimino tetrimino;
    bool visited = false;
    GraphNode* prev;
    std::vector<GraphNode*> neighbours;
};

typedef std::array<std::array<std::array<GraphNode, 4>, GRID_WIDTH>, GRID_HEIGHT> Graph; // first dimension is row second dimension is column third dimension is rotation

struct EvaluationFactors {
    int totalLinesCleared = 0;
    int totalLockHeight = 0;
    int totalWellCells = 0;
    int totalColumnHoles = 0;
    int totalColumnTransistions = 0;
    int totalRowTransitions = 0;
};

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