#include "util.h"

void FrameCounter::nextFrame() {
    this->framesPerGridCellCounter++;
    this->framesPerSoftDropCounter++;
    this->framesPerTetronimoResetCounter++;
    this->framesPerLineClearCounter++;
}

void FrameCounter::resetCounters() {
    this->framesPerGridCellCounter = 0;
    this->framesPerSoftDropCounter = 0;
    this->framesPerTetronimoResetCounter = 0;
    this->framesPerLineClearCounter = 0;
}