#ifndef UTIL_H
#define UTIL_H

class Position {
    public:
        int x;
        int y;
        Position(int _x, int _y): x(_x), y(_y) {}
};

class FrameCounter {
    public:
    int framesPerGridCellCounter = 0;
    int framesPerSoftDropCounter = 0;
    int framesPerTetronimoResetCounter = 0;
    int framesPerLineClearCounter = 0;

    void nextFrame();
    void resetCounters();
};

#endif