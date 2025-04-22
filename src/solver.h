#ifndef SOLVER_H
#define SOLVER_H

#include <variant>
#include <vector>
#include "tetris.h"

struct GraphNode {
    Tetrimino tetrimino;
    bool visited = false;
    GraphNode *prev;
    std::vector<GraphNode*> neighbours;
};

typedef std::vector<std::vector<std::vector<GraphNode>>> Graph; // first dimension is row second dimension is column

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
Moves movesToReachSearchResult(GraphNode *searchResult);

void computeEvaluationFactors(GameGrid grid, EvaluationFactors& factors);
double computeFitness(EvaluationFactors factors);

Moves solve(GameGrid grid, Tetrimino firstTetrimino, Tetrimino secondTetrimino);
Tetrimino solveForFinalPos(GameGrid grid, Tetrimino firstTetrimino, Tetrimino secondTetrimino);

#endif