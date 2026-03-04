# include "interaction/ellipsis_display.hpp"


EllipsisDisplay::EllipsisDisplay(int period, int length)
: mPeriod(period), mLength(length) {}


void EllipsisDisplay::start()
{
    mDotCounter = 0;
    mKeepDisplayRunning = true;
    if (!mIsDisplayThreadRunning)
    {
        mKeepDisplayThreadAlive = true;
        mDisplayThread = std::thread(
            &EllipsisDisplay::runDisplay, this
        );
        mIsDisplayThreadRunning = true;
    }
}

void EllipsisDisplay::pause()
{
    mKeepDisplayRunning = false;
    clearDots();
}
void EllipsisDisplay::pauseAndClearLine()
{
    mKeepDisplayRunning = false;
    clear_line();
}

void EllipsisDisplay::stop()
{
    mKeepDisplayRunning = false;
    mKeepDisplayThreadAlive = false;

    clearDots();

    if (mDisplayThread.joinable())
    {
        mDisplayThread.join();
    }
    mIsDisplayThreadRunning = false;
}


void EllipsisDisplay::runDisplay()
{
    while (mKeepDisplayThreadAlive)
    {
        if (mKeepDisplayRunning)
        {
            if (mDotCounter < mLength)
            {
                std::cout << '.';
                mDotCounter++;
            }
            else
            {
                clearDots();
                mDotCounter = 0;
            }
        }
        sleep(mPeriod);
    }
}