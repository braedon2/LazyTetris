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
    std::vector<std::vector<GridCell>> grid;

    public:
    GameGrid();
    bool isEmpty(Position p);
    Color getColor(Position p);
    void setCell(Position p, Color color);
    void clearCell(Position p);
    bool checkCollision(Position p);
};

class GameState {
    private:
    GameGrid grid;
    Position currentTetronimo = Position(0, 0);
    bool isCurrentTetronimoPlaced = false;

    public:
    GameGrid getGrid();
    Position getCurrentTetronimo();
    bool isCurrentTetrominoPlaced();
    void initNewTetronimo();

    // Moves the current tetromino one cell in the given direction so long as it does not cause a 
    // collision with the grid bounds or placed tetrominoes.
    // Left/right movements are ignored if they would cause a collision
    // A down movement that results in a collision causes the current tetromino to be placed where it 
    // is in the grid and a new tetromino to be initialized
    void moveTetronimo(Direction direction);
};

#endif