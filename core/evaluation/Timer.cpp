#include "evaluation/Timer.h"

#include <stdexcept>

// Time O(1) space O(1)
Timer::Timer() : start_(Clock::now()) {}

// Time O(1) space O(1)
void Timer::start() {
    start_ = Clock::now();
    running_ = true;
}

// Time O(1) space O(1)
void Timer::stop() {
    if (!running_) {
        throw std::logic_error("Timer::stop: timer is not running");
    }

    elapsedMs_ = std::chrono::duration<double, std::milli>(
        Clock::now() - start_
    ).count();

    running_ = false;
}

// Time O(1) space O(1)
double Timer::elapsedMs() const {
    if (running_) {
        return std::chrono::duration<double, std::milli>(
            Clock::now() - start_
        ).count();
    }

    return elapsedMs_;
}

// Time O(1) space O(1)
bool Timer::isRunning() const { return running_; }
