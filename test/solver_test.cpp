#include <iostream>
#include <gtest/gtest.h>
#include "constants.h"
#include "tetris.h"
#include "solver.h"

TEST(SolverTest, IterateNodeNeighbours) {
    Tetrimino expectedShapes[] = {T, T, L, I, J};
    int i = 0;

    GraphNode g1 = {.tetrimino = Tetrimino(T)};
    GraphNode g2 = {.tetrimino = Tetrimino(L)};
    GraphNode g3 = {.tetrimino = Tetrimino(I)};
    GraphNode g4 = {.tetrimino = Tetrimino(J)};

    // node neighbours will be iterated with between 1 and 4 items so test both extremes
    NodeNeighbours neighbours;
    neighbours.push_back(&g1);

    for (GraphNode* n : neighbours) {
        EXPECT_EQ(expectedShapes[i], n->tetrimino.shape);
        i++;
    }

    neighbours.push_back(&g2);
    neighbours.push_back(&g3);
    neighbours.push_back(&g4);

    for (GraphNode* n : neighbours) {
        EXPECT_EQ(expectedShapes[i], n->tetrimino.shape);
        i++;
    }
}

TEST(SolverTest, SearchEmptyGridWithOTetrimino) {
    GameGrid grid;
    Tetrimino tetrimino(O);
    tetrimino.xDelta = SPAWN_X_DELTA;
    auto graph = makeGraph(tetrimino, grid);

    std::vector<GraphNode*> results = search(graph.get(), tetrimino, grid);

    std::cout << "Initial grid:" << std::endl;
    grid.print();

    // print all the search results
    for (GraphNode* node : results) {
        std::cout << "\n" << std::endl;
        GameGrid newGrid = grid;
        newGrid.setCells(node->tetrimino);
        newGrid.print();
    }

    // follow a node's prev pointers all the way to the root node
    GraphNode* node = results.at(0);
    while (node->prev != nullptr) {
        node = node->prev;
    }
    
    //make sure it's actually the root
    EXPECT_EQ(node->tetrimino.yDelta, 0);
    EXPECT_EQ(node->tetrimino.xDelta, 5);
}

TEST(SolverTest, SearchEmptyGridWithITetrimino) {
    GameGrid grid;
    Tetrimino tetrimino(I);
    tetrimino.xDelta = SPAWN_X_DELTA;
    auto graph = makeGraph(tetrimino, grid);

    std::vector<GraphNode*> results = search(graph.get(), tetrimino, grid);

    std::cout << "Initial grid:" << std::endl;
    grid.print();

    // print all the search results
    for (GraphNode* node : results) {
        std::cout << "\n" << std::endl;
        GameGrid newGrid = grid;
        newGrid.setCells(node->tetrimino);
        newGrid.print();
    }

    // follow a node's prev pointers all the way to the root node
    GraphNode* node = results.at(0);
    while (node->prev != nullptr) {
        node = node->prev;
    }
    
    // make sure it's actually the root
    EXPECT_EQ(node->tetrimino.yDelta, 0);
    EXPECT_EQ(node->tetrimino.xDelta, 5);

    // make sure enough results were generated
    EXPECT_EQ(results.size(), 17);
}

TEST(SolverTest, SearchNonEmptyGridSlideIntoPlace) {
    GameGrid grid;
    grid.setCell(Position(0, 19), first); // set S shape in botton left
    grid.setCell(Position(1, 19), first);
    grid.setCell(Position(1, 18), first);
    grid.setCell(Position(2, 18), first);
    Tetrimino tetrimino(I);
    tetrimino.xDelta = SPAWN_X_DELTA;
    auto graph = makeGraph(tetrimino, grid);

    std::vector<GraphNode*> results = search(graph.get(), tetrimino, grid);

    std::cout << "Initial grid:" << std::endl;
    grid.print();

    // print all the search results, look for one that slid into place
    bool found = false;
    for (GraphNode* node : results) {
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

TEST(SolverTest, FollowMovesOfVerticalITetrimino) {
    GameGrid grid;
    Tetrimino tetrimino(I);
    tetrimino.xDelta = SPAWN_X_DELTA;
    auto graph = makeGraph(tetrimino, grid);

    std::vector<GraphNode*> results = search(graph.get(), tetrimino, grid);
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

TEST(SolverTest, AnalyzeAllCombinations) {
    GameGrid grid;
    Tetrimino firstTetrimino = Tetrimino(T);
    firstTetrimino.xDelta = SPAWN_X_DELTA;
    Tetrimino secondTetrimino = Tetrimino(L);
    secondTetrimino.xDelta = SPAWN_X_DELTA;

    auto firstGraph = makeGraph(firstTetrimino, grid);

    auto analyze = [](GameGrid& grid, int totalLockHeight, int linesCleared, GraphNode* result) {
        EvaluationFactors factors;
        computeEvaluationFactors(grid, factors);
        factors.totalLinesCleared = linesCleared;
        factors.totalLockHeight = totalLockHeight;

        grid.print();
        std::cout << "total lines cleared: " << factors.totalLinesCleared << std::endl;
        std::cout << "total lock height: " << factors.totalLockHeight << std::endl;
        std::cout << "total well cells: " << factors.totalWellCells << std::endl;
        std::cout << "total column holes: " << factors.totalColumnHoles << std::endl;
        std::cout << "total column transisionts: " << factors.totalColumnTransistions << std::endl;
        std::cout << "total row transistions: " << factors.totalRowTransitions << std::endl << std::endl;
        std::cout << "x: " << result->tetrimino.xDelta << std::endl;
    };

    analyzeAllCombinations(analyze, firstGraph.get(), grid, firstTetrimino, secondTetrimino);
}

TEST(SolverTest, EvaluateAllFactors) {
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

TEST(SolverTest, EmptyColumnHasNoTransitions) {
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