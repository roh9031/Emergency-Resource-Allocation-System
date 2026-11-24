// Unit tests for Timer (core/evaluation/Timer.h)
// Sleeps are short (20 ms) so the whole file runs in well under a second
#include <chrono>
#include <cmath>
#include <stdexcept>
#include <thread>

#include "domain/Allocation.h"
#include "evaluation/Timer.h"
#include "test_harness.h"

namespace {

void sleepMs(int ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

// 1. A new timer has a well-defined starting state
void testNewTimerIsStoppedAtZero() {
    Timer t;
    CHECK(!t.isRunning());
    CHECK_EQ(t.elapsedMs(), 0.0);
}

// 2. Wrong call order is a bug: stop() without start(), and stop() twice,
//    throw and leave the timer unchanged
void testStopWhenNotRunningThrows() {
    Timer t;
    CHECK_THROWS(t.stop(), std::logic_error);
    CHECK_EQ(t.elapsedMs(), 0.0);

    t.start();
    sleepMs(5);
    t.stop();
    const double frozen = t.elapsedMs();
    CHECK_THROWS(t.stop(), std::logic_error);
    CHECK_EQ(t.elapsedMs(), frozen);  // second stop did not overwrite it
    CHECK(!t.isRunning());
}

// 3. Measures real time in the right unit. sleep_for waits AT LEAST 20 ms, so
//    >= 20 is safe; < 2000 catches seconds-vs-ms or ns-vs-ms mistakes
void testMeasuresSleepInMilliseconds() {
    Timer t;
    t.start();
    sleepMs(20);
    t.stop();
    CHECK(t.elapsedMs() >= 20.0);
    CHECK(t.elapsedMs() < 2000.0);
}

// 4. After stop() the reading is frozen: time passing does not change it
void testStoppedValueIsFrozen() {
    Timer t;
    t.start();
    sleepMs(5);
    t.stop();
    const double first = t.elapsedMs();
    sleepMs(20);
    CHECK_EQ(t.elapsedMs(), first);
}

// 5. start() again resets: a short second run is not added to a long first one
void testRestartResets() {
    Timer t;
    t.start();
    sleepMs(50);
    t.stop();
    const double longRun = t.elapsedMs();

    t.start();
    t.stop();  // almost no time passes
    CHECK(t.elapsedMs() < longRun);
    CHECK(t.elapsedMs() < 50.0);
}

// 6. While running, elapsedMs() reports the time so far and keeps growing
void testElapsedWhileRunning() {
    Timer t;
    t.start();
    CHECK(t.isRunning());
    sleepMs(10);
    const double early = t.elapsedMs();
    CHECK(early >= 10.0);
    sleepMs(10);
    CHECK(t.elapsedMs() >= early + 10.0);
    CHECK(t.isRunning());  // reading does not stop it
}

// 7. The reading is always a valid runtime for AllocationResult
//    (finite, >= 0), running or stopped
void testWorksWithAllocationResult() {
    Timer t;
    t.start();
    CHECK(std::isfinite(t.elapsedMs()) && t.elapsedMs() >= 0.0);
    t.stop();
    AllocationResult r("A2");
    r.setRuntimeMs(t.elapsedMs());  // would throw if negative or NaN
    CHECK_EQ(r.runtimeMs(), t.elapsedMs());
}

}  // namespace

void runTimerTests() {
    RUN_TEST(testNewTimerIsStoppedAtZero);
    RUN_TEST(testStopWhenNotRunningThrows);
    RUN_TEST(testMeasuresSleepInMilliseconds);
    RUN_TEST(testStoppedValueIsFrozen);
    RUN_TEST(testRestartResets);
    RUN_TEST(testElapsedWhileRunning);
    RUN_TEST(testWorksWithAllocationResult);
}
