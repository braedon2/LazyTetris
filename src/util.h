#ifndef UTIL_H
#define UTIL_H

#include <raylib.h>

enum Direction {
    down,
    right,
    left
};

class Position {
    public:
        int x;
        int y;
        Position(int _x, int _y): x(_x), y(_y) {}
};

class Timer {
    private:
        double startTime;
    public:
        Timer() { this->startTime = GetTime(); }

        void start() { this->startTime = GetTime(); }

        double getElapsed() { return GetTime() - this->startTime; }
};

#endif