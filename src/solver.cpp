#include <algorithm>
#include <queue>
#include <variant>
#include <vector>
#include "constants.h"
#include "tetris.h"
#include "solver.h"

void setNodeNeighbours(GraphNode& node, Graph& graph, GameGrid& grid) {
    if (not grid.checkCollision(node.tetrimino.move(left).getPositions())) {
        node.neighbours.push_back(&graph.at(node.tetrimino.yDelta).at(node.tetrimino.xDelta - 1).at(node.tetrimino.rotationStep));
    }
    if (not grid.checkCollision(node.tetrimino.move(down).getPositions())) {
        node.neighbours.push_back(&graph.at(node.tetrimino.yDelta + 1).at(node.tetrimino.xDelta).at(node.tetrimino.rotationStep));
    }
    if (not grid.checkCollision(node.tetrimino.move(right).getPositions())) {
        node.neighbours.push_back(&graph.at(node.tetrimino.yDelta).at(node.tetrimino.xDelta + 1).at(node.tetrimino.rotationStep));
    }
    if (not grid.checkCollision(node.tetrimino.rotate(clockwise).getPositions())) {
        node.neighbours.push_back(
            &graph.at(
                node.tetrimino.yDelta).at(
                    node.tetrimino.xDelta).at(
                        (node.tetrimino.rotationStep + 1) % node.tetrimino.rotationList.size()));
    }
}


Graph makeGraph(Tetrimino& tetrimino, GameGrid& grid) {
    Graph graph;
    for (int i = 0; i < GRID_HEIGHT; i++) {
        graph.push_back(std::vector<std::vector<GraphNode>>());
        for (int j = 0; j < GRID_WIDTH; j++) {
            graph.at(i).push_back(std::vector<GraphNode>());
            for (int rotation = 0; rotation < tetrimino.rotationList.size(); rotation++) {
                GraphNode node = {
                    Tetrimino(tetrimino.shape, j, i, rotation),
                    false, // visited
                    nullptr, // prev
                    std::vector<GraphNode*>() // neighbours
                };
                graph.at(i).at(j).push_back(node);
            }
        }
    }

    for (std::vector<std::vector<GraphNode>>& row : graph) {
        for (std::vector<GraphNode>& col : row) {
            for(GraphNode& node : col) {
                setNodeNeighbours(node, graph, grid);
            }
        }
    }
    return graph;
}


std::vector<GraphNode*> search(Graph& graph, Tetrimino& tetrimino, GameGrid& grid) {
    std::queue<GraphNode*> queue;
    std::vector<GraphNode*> results;

    GraphNode& root = graph.at(tetrimino.yDelta).at(tetrimino.xDelta).at(tetrimino.rotationStep);
    root.visited = true;
    //setNodeNeighbours(root, graph, grid);   
    queue.push(&root);

    while (not queue.empty()) {
        GraphNode *node = queue.front();
        queue.pop();

        if (grid.checkCollision(node->tetrimino.move(down).getPositions())) {
            results.push_back(node);
        }

        for (GraphNode *neighbour : node->neighbours) {
            if (not neighbour->visited) {
                neighbour->visited = true;
                neighbour->prev = node;
                queue.push(neighbour);
            }
        }
    }
    return results;
}


Moves movesToReachSearchResult(GraphNode *searchResult) {
    GraphNode *node = searchResult;
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


double computeFitness(EvaluationFactors factors) {
    return (
        factors.totalLinesCleared * 1.0 +
        factors.totalLockHeight * 12.885008263218383 +
        factors.totalWellCells * 15.842707182438396 + 
        factors.totalColumnHoles * 26.894496507795950 + 
        factors.totalColumnTransistions * 27.616914062397015 + 
        factors.totalRowTransitions + 30.185110719279040
    );
}


Moves solve(GameGrid grid, Tetrimino firstTetrimino, Tetrimino secondTetrimino) {
    GraphNode *bestResult;
    double bestFitness = -1.0;
    Graph firstGraph = makeGraph(firstTetrimino, grid);
    std::vector<GraphNode*> firstResults = search(firstGraph, firstTetrimino, grid);

    for (GraphNode *firstResult : firstResults) {
        EvaluationFactors factors;
        factors.totalLockHeight = firstResult->tetrimino.getHeight();

        GameGrid gridCopy = grid;
        gridCopy.setCells(firstResult->tetrimino);
        factors.totalLinesCleared = gridCopy.getFullRows().size();
        gridCopy.clearRows(gridCopy.getFullRows());

        Graph secondGraph = makeGraph(secondTetrimino, gridCopy);
        std::vector<GraphNode*> secondResults = search(secondGraph, secondTetrimino, gridCopy);

        for (GraphNode *result : secondResults) {
            factors.totalLockHeight += result->tetrimino.getHeight();
            gridCopy.setCells(result->tetrimino);
            factors.totalLinesCleared += gridCopy.getFullRows().size();
            gridCopy.clearRows(gridCopy.getFullRows());

            computeEvaluationFactors(gridCopy, factors);
            double fitness = computeFitness(factors);
            
            if (bestFitness < 0 or fitness < bestFitness) {
                bestFitness = fitness;
                bestResult = firstResult;
            }
        }
    }

    return movesToReachSearchResult(bestResult);
}