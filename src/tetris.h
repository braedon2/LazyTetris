#ifndef TETRIS_H
#define TETRIS_H

#include <ranlib.h>
#include <vector>
#include "util.h"

class GridCell {
    public:
    Color color;
    bool isEmpty;
    GridCell(Color _color, bool _isEmpty): color(_color), isEmpty(_isEmpty) {}
};

class GameGrid {
    private:
    // first dimension is row, second dimension is column
    std::vector<std::vector<GridCell>> grid;

    public:
    GameGrid();
    bool isEmpty(Position p);
    Color getColor(Position p);
    void setCell(Position p, Color color);
    void clearCell(Position p);
    bool checkCollision(Position p);
    std::vector<int> getFullRows();
    void clearRows(std::vector<int> row_indexes);
};

class GameState {
    private:
    // Keeps track of the fallen tetrominos that can no longer be moved. This usually does not
    // include the current tetromino
    GameGrid grid;
    Position currentTetronimo = Position(0, 0);

    // Flag keeps track of if the current tetromino has been placed in the grid. 
    // Flag is set by moveTetronimo member function when the current tetronimo can no longer move
    // down and is placed in the grid.
    // Flag is unset when initNewTetronimo is called.
    bool isCurrentTetronimoPlaced = false;

    public:
    GameGrid getGrid();
    Position getCurrentTetronimo();
    bool isCurrentTetrominoPlaced();
    std::vector<int> clearFullRows(); // returns indices of full rows that were cleared
    void initNewTetronimo();

    // Moves the current tetromino one cell in the given direction so long as it does not cause a 
    // collision with the grid bounds or placed tetrominoes.
    // Left/right movements are ignored if they would cause a collision
    // A down movement that results in a collision causes the current tetromino to be placed where it 
    // is in the grid and a new tetromino to be initialized
    void moveTetronimo(Direction direction);
};

#endif