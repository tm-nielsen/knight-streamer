# pragma once


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

    public:
    EllipsisDisplay(int period = 250, int length = 3);

    void start();
    void pause(bool clearLine = true);
    void stop();
};