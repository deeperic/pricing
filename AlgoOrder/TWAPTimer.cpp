
#include "TWAPTimer.h"

void TWAPTimer(TWAPAlgo *algo) {
    std::cout << "timer thread start" << std::endl;
    while (algo->IsRunning()) {
        this_thread::sleep_for(1s);
        algo->OnTimer();
    }
    algo->TimerThreadEnded();
    std::cout << "timer thread exit" << std::endl;
}