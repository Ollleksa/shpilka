#include "SDL/SDL.h"

class Timer
{
    private:
        int startTicks;         //clock time when started
        int pausedTicks;        //clock time when paused
        bool started;           //is started/paused?
        bool paused;
    public:
        Timer();
        //clock actions
        void start();
        void stop();
        void pause();
        void unpause();
        //Get time
        int get_Ticks();
        //Status?
        bool is_started();
        bool is_paused();
};
