#ifndef TETRIS_H
#define TETRIS_H

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
enum TetronimoShape { l, J, L, O, S, T, Z };
const int numTetronimoShapes = 6;

/// There are three sprite variants for each level
/// A GridCell instance stores a spriteType instead of the sprite itself.
/// A Tetromino instance can return its associated SpriteType.
/// The SpriteType value is used as an index when selecting a sprite for 
/// a given level. 
/// The none value is used for empty GridCells and is never passed to the 
/// Sprites class for retrieving a sprite.
enum SpriteType { first = 0, second = 1, third = 2, none = 4 };

/// Each tetronimo shape is associated with a SpriteType
/// the Tetromino class uses this when its getSpriteType() is called
const std::map<TetronimoShape,SpriteType> spriteTypeMap = { 
    {T, first}, 
    {J, second}, 
    {Z, third}, 
    {O, first}, 
    {S, second}, 
    {L, third}, 
    {l, first}
}; 

/// The first item of each rotation list is the initial orientation for a newly spawned tetronimo of
/// that shape. The next list of positions for a shape are the positions of the tetronimo if it were
/// rotated clockwise.
///
/// For example:
/// The first item in the rotation list for tetronimo shape T looks like
///  000
///   0
/// The second item in the rotation list is the previous representation rotated 90 degrees clockwise
///   0
///  00
///   0
const std::map<TetronimoShape, std::vector<std::vector<Position>>> rotationListMap = {
    { T, { 
        {{-1,0}, {0,0}, {1,0}, {0,1}},
        {{0,-1}, {-1,0}, {0,0}, {0,1}},
        {{0,-1}, {-1,0}, {0,0}, {1,0}},
        {{0,-1}, {0,0}, {1,0}, {0,1}}
    }},
    { l, {
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
/// When a Tetronimo is moved or rotated a new Tetronimo object is created
/// instead of mutating the original. This makes it easier to test a change 
/// for a collision before commiting to it
class Tetronimo {
    public:
    std::vector<std::vector<Position>> rotationList;
    int xDelta;
    int yDelta;
    int rotationStep;
    TetronimoShape shape;

    public:
    Tetronimo(TetronimoShape shape);
    Tetronimo(TetronimoShape shape, int xDelta, int yDelta, int rotationStep);
    std::vector<Position> getPositions();
    Tetronimo move(Direction direction);
    Tetronimo rotate(Rotation rotation);
    SpriteType getSpriteType();
};

class GameGrid {
    private:
    std::vector<std::vector<GridCell>> grid; // first dimension is row, second dimension is column

    public:
    GameGrid();
    bool isEmpty(Position p);
    SpriteType getSpriteType(Position p); 
    void setCell(Position p, SpriteType spriteType);
    void clearCell(Position p);
    bool checkCollision(std::vector<Position> positions);
    std::vector<int> getFullRows();
    void clearRows(std::vector<int> row_indexes);
};

class GameState {
    /*
    * - grid: keeps track of the fallen tetrominos that can no longer be moved. This usually does not 
    * include the current tetromino.
    * - currentTetronimo: the falling tetronimo being controlled by the player
    * - isCurrentTetronimoPlaced: Flag keeps track of if the current tetromino has been placed in the grid. 
    * The flag is set by moveTetronimo member function when the current tetronimo can no longer move
    * down and is placed in the grid.the flag is unset when initNewTetronimo is called.
    * - linesToClear: used for animating line clears. Grid indices of rows being cleared
    * - lineClearStep: current step in the row clear animation
    */
    private:
    GameGrid grid;
    Tetronimo currentTetronimo = Tetronimo(T); // replaced with random tetronimo in constructor
    Tetronimo nextTetronimo = Tetronimo(T); // replacedw tih random tetronimo in constructor
    bool isCurrentTetronimoPlaced = false;
    std::vector<int> linesToClear;
    int lineClearStep = 0; 

    public:
    int linesCleared = 0;
    int level = 1;
    bool gameOver = false;

    public:
    GameState();
    GameGrid getGrid();
    Tetronimo getCurrentTetronimo();
    Tetronimo getNextTetronimo();
    bool isCurrentTetrominoPlaced();
    void initNewTetronimo();

    /* 
    * Moves the current tetromino one cell in the given direction so long as it does not cause a 
    * collision with the grid bounds or placed tetrominoes.
    * Left/right movements are ignored if they would cause a collision
    * A down movement that results in a collision causes the current tetromino to be placed where it 
    * is in the grid and a new tetromino to be initialized
    */
    void moveTetronimo(Direction direction);

    /*
    * Rotates the current tetronimo as long as it doesn't result in a collision. Rotations
    * that would cause a collision are ignored
    */
    void rotateTetronimo(Rotation rotation);

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
    int getHorizontalOffset(Tetronimo tetronimo);
    void drawCurrentTetronimo(GameState& state);
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
    int framesPerTetronimoResetCounter = 0;
    int framesPerLineClearCounter = 0;
    int framesPerGameOverStepCounter = 0;

    void nextFrame();
    void resetCounters();
};

#endif