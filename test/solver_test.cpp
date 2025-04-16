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

    // print all the search results
    for (GraphNode *node : results) {
        std::cout << "\n" << std::endl;
        GameGrid newGrid = grid;
        newGrid.setCells(node->tetrimino);
        newGrid.print();
    }

    // follow a node's prev pointers all the way to the root node
    GraphNode *node = results.at(0);
    while (node->prev != nullptr) {
        node = node->prev;
    }
    
    //make sure it's actually the root
    EXPECT_EQ(node->tetrimino.yDelta, 0);
    EXPECT_EQ(node->tetrimino.xDelta, 5);
}

TEST(SearchTest, NonEmptyGridSlideIntoPlace) {
    GameGrid grid;
    grid.setCell(Position(0, 19), first); // set S shape in botton left
    grid.setCell(Position(1, 19), first);
    grid.setCell(Position(1, 18), first);
    grid.setCell(Position(2, 18), first);
    Tetrimino tetrimino(I);
    tetrimino.xDelta = SPAWN_X_DELTA;
    Graph graph = makeGraph(GRID_WIDTH, GRID_HEIGHT, tetrimino, grid);

    std::vector<GraphNode*> results = search(graph, tetrimino, grid);

    std::cout << "Initial grid:" << std::endl;
    grid.print();

    // print all the search results, look for one that slid into place
    bool found = false;
    for (GraphNode *node : results) {
        std::cout << "\n" << std::endl;
        GameGrid newGrid = grid;
        newGrid.setCells(node->tetrimino);
        newGrid.print();
        if (node->tetrimino.xDelta == 4 and node->tetrimino.yDelta == 19){
            found = true;
            std::cout << "Found it!" << std::endl;
        }
    }

    EXPECT_EQ(found, true);
}

TEST(EvaluationTest, AllFactors) {
    GameGrid grid;
    Tetrimino firstTetrimino = Tetrimino(I, 1, 17, 1);
    Tetrimino secondTetrimino = Tetrimino(S, 4, 15, 0);
    std::vector<std::vector<int>> gridFillData = {
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // starting at row 15 (0 indexed)
        { 0, 0, 0, 0, 0, 0, 1, 0, 1, 0},
        { 1, 0, 1, 1, 1, 1, 1, 1, 1, 1},
        { 1, 0, 1, 1, 1, 1, 1, 1, 1, 1},
        { 0, 1, 0, 1, 1, 0, 1, 0, 1, 0}
    };
    for (int i = 0; i < gridFillData.size(); i++) {
        for (int j = 0; j < gridFillData[0].size(); j++) {
            if (gridFillData[i][j]) {
                grid.setCell(Position(j, i + 15), first);
            }
        }
    }

    std::cout << "\ngrid without tetriminos:" << std::endl;
    grid.print();
    GameGrid tmpGrid = grid;
    tmpGrid.setCells(firstTetrimino);
    tmpGrid.setCells(secondTetrimino);
    std::cout << "\n grid with tetriminos:" << std::endl;
    tmpGrid.print();

    EvaluationFactors factors = computeEvaluationFactors(grid, firstTetrimino, secondTetrimino);

    EXPECT_EQ(factors.totalLinesCleared, 2);
    EXPECT_EQ(factors.totalLockHeight, 4);
    EXPECT_EQ(factors.totalWellCells, 5);
}

TEST(EvaluationTest, WellCellsInColumnWithIntermittentGapsInWellWalls) {
    GameGrid grid;
}

TEST(EvaluationTest, EmptyColumnHasNoTransitions) {
    GameGrid grid;
}