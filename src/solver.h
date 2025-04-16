#ifndef SOLVER_H
#define SOLVER_H

#include <vector>
#include "tetris.h"

/***
 * Searcher definitions
 ***/

struct GraphNode {
    Tetrimino tetrimino;
    bool visited = false;
    GraphNode *prev;
    std::vector<GraphNode*> neighbours;
};

typedef std::vector<std::vector<GraphNode>> Graph; // first dimension is row second dimension is column

void setNodeNeighbours(GraphNode& node, Graph& graph, GameGrid& grid);
Graph makeGraph(int width, int height, Tetrimino& tetrimino, GameGrid& grid);
std::vector<GraphNode*> search(Graph &graph, Tetrimino& tetrimino, GameGrid& grid);

/***
 * Solver definitions
 ***/

struct EvaluationFactors {
    int totalLinesCleared = 0;
    int totalLockHeight = 0;
    int totalWellCells = 0;
    int totalColumnHoles = 0;
    int totalColumnTransistions = 0;
    int totalRowTransitions = 0;
};

EvaluationFactors computeEvaluationFactors(GameGrid grid, Tetrimino firstTetrimino, Tetrimino secondTetrimino);

#endif