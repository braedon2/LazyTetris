#include <iostream>
#include <gtest/gtest.h>
#include "constants.h"
#include "tetris.h"
#include "solver.h"

TEST(SearchTest, EmptyGridWithOTetrimino) {
    GameGrid grid;
    Tetrimino tetrimino(O);
    tetrimino.xDelta = SPAWN_X_DELTA;
    Graph graph = makeGraph(GRID_WIDTH, GRID_HEIGHT, tetrimino, grid);

    std::vector<GraphNode*> results = search(graph, tetrimino, grid);

    std::cout << "Initial grid:" << std::endl;
    grid.print();

    for (GraphNode *node : results) {
        std::cout << "\n" << std::endl;
        GameGrid newGrid = grid;
        newGrid.setCells(node->tetrimino);
        newGrid.print();
    }

    // follow a nodes prev pointers all the way to the root node and make sure it's the right one
    GraphNode *node = results.at(0);
    while (node->prev != nullptr) {
        node = node->prev;
    }
    
    EXPECT_EQ(node->tetrimino.yDelta, 0);
    EXPECT_EQ(node->tetrimino.xDelta, 5);
}