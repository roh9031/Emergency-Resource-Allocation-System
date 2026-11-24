/* Wall clock stopwatch in milliseconds
 Used to fill AllocationResult::setRuntimeMs and by ExperimentRunner
 Uses std::chrono::steady_clock, which never goes backwards (unlike
 system_clock which can jump when the PC syncs its time) so an elapsed
 time is never negative */
#pragma once

#include <chrono>

class Timer {
public:
    // A new timer is not running and reads 0 ms
    // Time O(1) space O(1)
    Timer();

    // Starts (or restarts) the timer from now; any earlier reading is discarded
    // Time O(1) space O(1)
    void start();

    // Stops the timer and freezes elapsedMs() at the time since start()
    // Throws std::logic_error if the timer is not running (never started or
    // already stopped); the timer is unchanged when it throws
    // Time O(1) space O(1)
    void stop();

    // Running: milliseconds since start() so far
    // Stopped: the value frozen by the last stop(), or 0 if never started
    // Always finite and >= 0
    // Time O(1) space O(1)
    double elapsedMs() const;

    // Time O(1) space O(1)
    bool isRunning() const;

private:
    using Clock = std::chrono::steady_clock;

    Clock::time_point start_;
    double elapsedMs_ = 0.0;
    bool running_ = false;
};
