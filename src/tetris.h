#ifndef TETRIS_H
#define TETRIS_H

#include <map>
#include <vector>
#include "raylib.h"
#include "constants.h"

class Position {
    public:
    int x;
    int y;
    Position(int _x, int _y): x(_x), y(_y) {}
};

enum Rotation { clockwise, counterClockwise };
enum Direction { down, right, left };

/// used as a key to map a tetromino shape to data that relates to it
enum TetriminoShape { I, J, L, O, S, T, Z };
const int numTetriminoShapes = 6;

/// There are three sprite variants for each level
/// A GridCell instance stores a spriteType instead of the sprite itself.
/// A Tetromino instance can return its associated SpriteType.
/// The SpriteType value is used as an index when selecting a sprite for 
/// a given level. 
/// The none value is used for empty GridCells and is never passed to the 
/// Sprites class for retrieving a sprite.
enum SpriteType { first = 0, second = 1, third = 2, none = 4 };

/// Each tetrimino shape is associated with a SpriteType
/// the Tetromino class uses this when its getSpriteType() is called
const std::map<TetriminoShape,SpriteType> spriteTypeMap = { 
    {T, first}, 
    {J, second}, 
    {Z, third}, 
    {O, first}, 
    {S, second}, 
    {L, third}, 
    {I, first}
}; 

/// The first item of each rotation list is the initial orientation for a newly spawned tetrimino of
/// that shape. The next list of positions for a shape are the positions of the tetrimino if it were
/// rotated clockwise.
///
/// For example:
/// The first item in the rotation list for tetrimino shape T looks like
///  000
///   0
/// The second item in the rotation list is the previous representation rotated 90 degrees clockwise
///   0
///  00
///   0
const std::map<TetriminoShape, std::vector<std::vector<Position>>> rotationListMap = {
    { T, { 
        {{-1,0}, {0,0}, {1,0}, {0,1}},
        {{0,-1}, {-1,0}, {0,0}, {0,1}},
        {{0,-1}, {-1,0}, {0,0}, {1,0}},
        {{0,-1}, {0,0}, {1,0}, {0,1}}
    }},
    { I, {
        {{-2,0}, {-1,0}, {0,0}, {1,0}},
        {{0,-2}, {0,-1}, {0,0}, {0,1}}
    }},
    { O, {
        {{-1,0}, {0,0}, {-1,1}, {0,1}}
    }},
    {J, {
        {{-1,0}, {0,0}, {1,0}, {1,1}},
        {{0,-1}, {0,0}, {-1,1}, {0,1}},
        {{-1,-1}, {-1,0}, {0,0}, {1,0}},
        {{0,-1}, {1,-1}, {0,0}, {0,1}}
    }},
    {L, {
        {{-1,0}, {0,0}, {1,0}, {-1,1}},
        {{-1,-1}, {0,-1}, {0,0}, {0,1}},
        {{1,-1}, {-1,0}, {0,0}, {1,0}},
        {{0,-1}, {0,0}, {0,1}, {1,1}}
    }},
    {S, {
        {{0,0}, {1,0}, {-1,1}, {0,1}},
        {{0,-1}, {0,0}, {1,0}, {1,1}}
    }},
    {Z, {
        {{-1,0}, {0,0}, {0,1}, {1,1}},
        {{1,-1}, {0,0}, {1,0}, {0,1}}
    }}
};

/// used by the Sprites class when generating sprites.
/// each level has two colors.
const std::vector<std::vector<Color>> levelColors = {
    { BLUE, SKYBLUE },
    { LIME, GREEN },
    { {216,0,204,255},  {248,120,248,255} },
    { {0,88,248,255},   {0,184,0,255} },
    { {228,0,88,255},   {88,248,152,255} },
    { {88,248,152,255}, {104,136,252,255} },
    { {248,56,0,255},   {124,124,124,255} },
    { {104,68,252,255}, {168,0,32,255} },
    { {0,88,248,255},   {248,56,0,255} },
    { {248,56,0,255},   {252,160,68,255} },
};

/// The flat lists of integers are used by the Sprites class when generating sprites.
/// 1 means to color the sprite using levelColors.
/// 0 means to color the pixel white.
const int sprite_width = 5;
const int sprite_height = 5;
const std::vector<int> spritePixelLayout1 = {
    0, 1, 1, 1, 1,
    1, 0, 0, 0, 1,
    1, 0, 0, 0, 1,
    1, 0, 0, 0, 1,
    1, 1, 1, 1, 1
};
const std::vector<int> spritePixelLayout2 = {
    0, 1, 1, 1, 1,
    1, 0, 0, 1, 1, 
    1, 0, 1, 1, 1,
    1, 1, 1, 1, 1,
    1, 1, 1, 1, 1
};
const std::vector<std::vector<int>> spritePixelLayouts = { spritePixelLayout1, spritePixelLayout2 };

/// When a tetromino is locked into place it is stored as GridCells in the State class.
/// The game starts with a board of empty GridCells.
class GridCell {
    public:
    SpriteType spriteType;
    bool isEmpty;
    GridCell(SpriteType _spriteType, bool _isEmpty): spriteType(_spriteType), isEmpty(_isEmpty) {}
};

/// A collection of Positions that can be translated or rotated.
/// When a Tetrimino is moved or rotated a new Tetrimino object is created
/// instead of mutating the original. This makes it easier to test a change 
/// for a collision before commiting to it
class Tetrimino {
    public:
    std::vector<std::vector<Position>> rotationList;
    int xDelta;
    int yDelta;
    int rotationStep;
    TetriminoShape shape;

    public:
    Tetrimino(TetriminoShape shape);
    Tetrimino(TetriminoShape shape, int xDelta, int yDelta, int rotationStep);
    std::vector<Position> getPositions();
    Tetrimino move(Direction direction);
    Tetrimino rotate(Rotation rotation);
    SpriteType getSpriteType();
};

class GameGrid {
    private:
    std::vector<std::vector<GridCell>> grid; // first dimension is row, second dimension is column

    public:
    GameGrid();
    bool isEmpty(Position p);
    SpriteType getSpriteType(Position p); 
    void setCells(Tetrimino tetrimino);
    void setCell(Position position, SpriteType spriteType);
    void clearCell(Position p);
    bool checkCollision(std::vector<Position> positions);
    std::vector<int> getFullRows();
    void clearRows(std::vector<int> row_indexes);
    void print();
};

class GameState {
    /*
    * - grid: keeps track of the fallen tetrominos that can no longer be moved. This usually does not 
    * include the current tetromino.
    * - currentTetrimino: the falling tetrimino being controlled by the player
    * - isCurrentTetriminoPlaced: Flag keeps track of if the current tetromino has been placed in the grid. 
    * The flag is set by moveTetrimino member function when the current tetrimino can no longer move
    * down and is placed in the grid.the flag is unset when initNewTetrimino is called.
    * - linesToClear: used for animating line clears. Grid indices of rows being cleared
    * - lineClearStep: current step in the row clear animation
    */
    private:
    GameGrid grid;
    Tetrimino currentTetrimino = Tetrimino(T); // replaced with random tetrimino in constructor
    Tetrimino nextTetrimino = Tetrimino(T); // replacedw tih random tetrimino in constructor
    bool isCurrentTetriminoPlaced = false;
    std::vector<int> linesToClear;
    int lineClearStep = 0; 

    public:
    int linesCleared = 0;
    int level = 1;
    bool gameOver = false;

    public:
    GameState();
    GameGrid getGrid();
    Tetrimino getCurrentTetrimino();
    Tetrimino getNextTetrimino();
    bool isCurrentTetrominoPlaced();
    void initNewTetrimino();

    /* 
    * Moves the current tetromino one cell in the given direction so long as it does not cause a 
    * collision with the grid bounds or placed tetrominoes.
    * Left/right movements are ignored if they would cause a collision
    * A down movement that results in a collision causes the current tetromino to be placed where it 
    * is in the grid and a new tetromino to be initialized
    */
    void moveTetrimino(Direction direction);

    /*
    * Rotates the current tetrimino as long as it doesn't result in a collision. Rotations
    * that would cause a collision are ignored
    */
    void rotateTetrimino(Rotation rotation);

    /* 
    * returns true if a row clear animation is in progress. This means that rowsToClear and 
    * rowClearStep attributes are set
    */
    bool isLineClearInProgress();

    // Advances the row clear animation.
    // Unsets the rowsToClear and rowClearStep attributes when the animation is finished
    void nextLineClearStep();
};


class Sprites {
    private:
    std::vector<std::vector<Texture2D>> sprites;

    private:
    Texture2D generateSprite(int pixelLayoutIndex, Color colour);

    public:
    Sprites();
    Texture2D getSprite(SpriteType spriteType, int level);
};

class FrameDrawer {
    private:
    Font font;
    Sprites sprites;
    int gameOverStep = 0;

    private:
    int getHorizontalOffset(Tetrimino tetrimino);
    void drawCurrentTetrimino(GameState& state);
    void drawGridCells(GameState& state);
    void drawSideBar(GameState& state);
    void drawGameOver(int level);

    public:
    FrameDrawer(); 
    void drawFrame(GameState& state);
    void nextGameOverStep();
};

class FrameCounter {
    public:
    int framesPerGridCellCounter = 0;
    int framesPerSoftDropCounter = 0;
    int framesPerTetriminoResetCounter = 0;
    int framesPerLineClearCounter = 0;
    int framesPerGameOverStepCounter = 0;

    void nextFrame();
    void resetCounters();
};

#endif