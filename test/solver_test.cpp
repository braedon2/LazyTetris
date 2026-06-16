#include <iostream>
#include <gtest/gtest.h>
#include "constants.h"
#include "tetris.h"
#include "solver.h"

TEST(SearchTest, EmptyGridWithOTetrimino) {
    GameGrid grid;
    Tetrimino tetrimino(O);
    tetrimino.xDelta = SPAWN_X_DELTA;
    Graph graph = makeGraph(tetrimino, grid);

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

TEST(SearchTest, EmptyGridWithITetrimino) {
    GameGrid grid;
    Tetrimino tetrimino(I);
    tetrimino.xDelta = SPAWN_X_DELTA;
    Graph graph = makeGraph(tetrimino, grid);

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
    
    // make sure it's actually the root
    EXPECT_EQ(node->tetrimino.yDelta, 0);
    EXPECT_EQ(node->tetrimino.xDelta, 5);

    // make sure enough results were generated
    EXPECT_EQ(results.size(), 17);
}

TEST(SearchTest, NonEmptyGridSlideIntoPlace) {
    GameGrid grid;
    grid.setCell(Position(0, 19), first); // set S shape in botton left
    grid.setCell(Position(1, 19), first);
    grid.setCell(Position(1, 18), first);
    grid.setCell(Position(2, 18), first);
    Tetrimino tetrimino(I);
    tetrimino.xDelta = SPAWN_X_DELTA;
    Graph graph = makeGraph(tetrimino, grid);

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

TEST(SearchTest, FollowMovesOfVerticalITetrimino) {
    GameGrid grid;
    Tetrimino tetrimino(I);
    tetrimino.xDelta = SPAWN_X_DELTA;
    Graph graph = makeGraph(tetrimino, grid);

    std::vector<GraphNode*> results = search(graph, tetrimino, grid);
    EXPECT_EQ(results.at(1)->tetrimino.rotationStep, 0);

    Moves moves = movesToReachSearchResult(results.at(1));

    for (Move move : moves) {
        if (std::holds_alternative<Direction>(move) and not grid.checkCollision(tetrimino.move(std::get<Direction>(move)))) {
            tetrimino = tetrimino.move(std::get<Direction>(move));
        } 
        else if (std::holds_alternative<Rotation>(move)) {
            tetrimino = tetrimino.rotate(std::get<Rotation>(move));
        }

        GameGrid gridCopy = grid;
        gridCopy.setCells(tetrimino);
        gridCopy.print();
    }

    EXPECT_EQ(results.at(1)->tetrimino.xDelta, tetrimino.xDelta);
    EXPECT_EQ(results.at(1)->tetrimino.yDelta, tetrimino.yDelta);
    EXPECT_EQ(results.at(1)->tetrimino, tetrimino);
}

TEST(SearchTest, VisualizeAllTwoTetriminoCombinations) {
    GameGrid grid;
    Tetrimino firstTetrimino = Tetrimino(T);
    firstTetrimino.xDelta = SPAWN_X_DELTA;
    Tetrimino secondTetrimino = Tetrimino(L);
    secondTetrimino.xDelta = SPAWN_X_DELTA;

    Graph firstGraph = makeGraph(firstTetrimino, grid);
    std::vector<GraphNode*> firstResults = search(firstGraph, firstTetrimino, grid);

    for (GraphNode* firstResult : firstResults) {
        int firstTetriminoLockHeight = firstResult->tetrimino.getHeight();

        GameGrid gridCopy = grid;
        gridCopy.setCells(firstResult->tetrimino);
        int firstTetriminoLineClears = gridCopy.getFullRows().size();
        gridCopy.clearRows(gridCopy.getFullRows());

        Graph secondGraph = makeGraph(secondTetrimino, gridCopy);
        std::vector<GraphNode*> secondResults = search(secondGraph, secondTetrimino, gridCopy);

        for (GraphNode* secondResult : secondResults) {
            EvaluationFactors factors;

            GameGrid secondGridCopy = gridCopy;
            factors.totalLockHeight = firstTetriminoLockHeight + secondResult->tetrimino.getHeight();
            secondGridCopy.setCells(secondResult->tetrimino);
            factors.totalLinesCleared = firstTetriminoLineClears + secondGridCopy.getFullRows().size();
            secondGridCopy.clearRows(secondGridCopy.getFullRows());
            secondGridCopy.print();

            computeEvaluationFactors(secondGridCopy, factors);
            double fitness = computeFitness(factors);

            std::cout << "total lines cleared: " << factors.totalLinesCleared << std::endl;
            std::cout << "total lock height: " << factors.totalLockHeight << std::endl;
            std::cout << "total well cells: " << factors.totalWellCells << std::endl;
            std::cout << "total column holes: " << factors.totalColumnHoles << std::endl;
            std::cout << "total column transisionts: " << factors.totalColumnTransistions << std::endl;
            std::cout << "total row transistions: " << factors.totalRowTransitions << std::endl << std::endl;
        }
    }
}

TEST(EvaluationTest, AllFactors) {
    GameGrid grid;
    std::vector<std::vector<int>> gridFillData = {
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // starting at row 15 (0 indexed)
        { 1, 0, 1, 0, 0, 0, 1, 0, 1, 0},
        { 1, 0, 0, 1, 1, 1, 1, 1, 1, 1},
        { 1, 0, 1, 1, 1, 1, 1, 0, 1, 1},
        { 0, 1, 0, 1, 1, 0, 1, 0, 1, 0}
    };
    for (int i = 0; i < gridFillData.size(); i++) {
        for (int j = 0; j < gridFillData[0].size(); j++) {
            if (gridFillData[i][j]) {
                grid.setCell(Position(j, i + 15), first);
            }
        }
    }

    std::cout << "\nGrid:" << std::endl;
    grid.print();

    EvaluationFactors factors;
    computeEvaluationFactors(grid, factors);

    EXPECT_EQ(factors.totalWellCells, 4);
    EXPECT_EQ(factors.totalColumnHoles, 6);
    EXPECT_EQ(factors.totalColumnTransistions, 7);
    EXPECT_EQ(factors.totalRowTransitions, 24);
}

TEST(EvaluationTest, EmptyColumnHasNoTransitions) {
    GameGrid grid;
    std::vector<std::vector<int>> gridFillData = {
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // starting at row 15 (0 indexed)
        { 1, 0, 1, 0, 0, 0, 1, 0, 1, 0},
        { 1, 0, 1, 1, 1, 1, 1, 1, 1, 1},
        { 1, 0, 1, 1, 1, 1, 1, 1, 1, 1},
        { 0, 0, 0, 1, 1, 1, 1, 1, 1, 1}
    };
    for (int i = 0; i < gridFillData.size(); i++) {
        for (int j = 0; j < gridFillData[0].size(); j++) {
            if (gridFillData[i][j]) {
                grid.setCell(Position(j, i + 15), first);
            }
        }
    }

    std::cout << "\nGrid:" << std::endl;
    grid.print();

    EvaluationFactors factors;
    computeEvaluationFactors(grid, factors);

    EXPECT_EQ(factors.totalColumnTransistions, 2);
}