# pragma once
# include "utils/io_helpers.hpp"


class EllipsisDisplay
{
    private:
    std::thread mDisplayThread;

    int mPeriod;
    int mLength;
    int mDotCounter = 0;

    bool mIsDisplayThreadRunning = false;
    bool mKeepDisplayThreadAlive = false;
    bool mKeepDisplayRunning = false;

    void runDisplay();
    inline void clearDots() { OUTF("\x1b[{}D\x1b[K", mDotCounter); }

    public:
    EllipsisDisplay(int period = 250, int length = 3);

    void start();
    void pause();
    void pauseAndClearLine();
    void stop();
};