# include "interaction/ellipsis_display.hpp"
# include "utils/io_helpers.hpp"


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

void EllipsisDisplay::pause(bool clearLine)
{
    mKeepDisplayRunning = false;
    if (clearLine) clear_line();
}

void EllipsisDisplay::stop()
{
    mKeepDisplayRunning = false;
    mKeepDisplayThreadAlive = false;

    OUTF("\x1b[{}D", mDotCounter);

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
            if (++mDotCounter <= mLength)
            {
                std::cout << '.';
            }
            else
            {
                OUTF("\x1b[{}D", mLength)
                COUT("\x1b[K")
                mDotCounter = 0;
            }
        }
        sleep(mPeriod);
    }
}