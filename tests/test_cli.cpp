// Unit tests for the CLI layer: InputReader, TablePrinter, MenuApp (cli/)
// Input is scripted with istringstream and output captured with ostringstream
#include <sstream>
#include <stdexcept>
#include <string>

#include "InputReader.h"
#include "MenuApp.h"
#include "TablePrinter.h"
#include "test_harness.h"

#ifndef RELIEFROUTE_SOURCE_DIR
#define RELIEFROUTE_SOURCE_DIR "."
#endif

namespace {

bool contains(const std::string& text, const std::string& part) {
    return text.find(part) != std::string::npos;
}

std::string sample(const std::string& name) {
    return std::string(RELIEFROUTE_SOURCE_DIR) + "/data/sample/" + name;
}

// Stand-in allocator for tests: the motivating example's optimal answer
AllocationResult fakeMotivating(const LoadedData&) {
    AllocationResult r("A2");
    r.add(1, 2, 1);  // A -> Y
    r.add(2, 1, 1);  // B -> X
    return r;
}

// 1. Invalid input: letters, decimals, empty and out-of-range are re-asked
void testReadIntRejectsBadInput() {
    std::istringstream in("abc\n3.5\n\n9\n2\n");
    std::ostringstream out;
    InputReader reader(in, out);
    int v = 0;
    CHECK(reader.readInt("> ", 1, 4, v));
    CHECK_EQ(v, 2);
    CHECK(contains(out.str(), "whole number"));
    CHECK(contains(out.str(), "from 1 to 4"));
}

// 2. Empty input: end of input returns false instead of looping forever
void testReadIntEndOfInput() {
    std::istringstream in("abc\n");
    std::ostringstream out;
    InputReader reader(in, out);
    int v = 0;
    CHECK(!reader.readInt("> ", 1, 4, v));
}

// 3. Normal output: table rows use names and ids; summary and fill rates are right
void testPrintAllocationMotivating() {
    const LoadedData data = SampleLoader().load(sample("motivating"));
    std::ostringstream out;
    TablePrinter::printAllocation(fakeMotivating(data), data, out);
    const std::string s = out.str();
    CHECK(contains(s, "Depot A (1)"));
    CHECK(contains(s, "Site Y (2)"));
    CHECK(contains(s, "Total supply : 2"));
    CHECK(contains(s, "Total demand : 2"));
    CHECK(contains(s, "Delivered    : 2  (100.0% of demand)"));
}

// 4. Boundary: no shipments -> table says so, every site 0.0%, no crash
void testPrintAllocationEmptyResult() {
    const LoadedData data = SampleLoader().load(sample("basic"));
    std::ostringstream out;
    TablePrinter::printAllocation(AllocationResult("A2"), data, out);
    CHECK(contains(out.str(), "(no shipments)"));
    CHECK(contains(out.str(), "Delivered    : 0  (0.0% of demand)"));
}

// 5. Full menu session with mistakes: bad choice, allocate before loading,
//    missing folder, then a real load, problems, allocate, exit
void testMenuSessionSurvivesMistakes() {
    std::istringstream in("x\n7\n3\n1\n/no/such/folder\n1\n" + sample("invalid") +
                          "\n2\n3\n4\n");
    std::ostringstream out;
    MenuApp app(fakeMotivating, in, out);
    CHECK_EQ(app.run(), 0);
    const std::string s = out.str();
    CHECK(contains(s, "Please enter a whole number."));
    CHECK(contains(s, "Please enter a number from 1 to 4."));
    CHECK(contains(s, "Nothing loaded yet."));
    CHECK(contains(s, "Could not load /no/such/folder"));
    CHECK(contains(s, "3 sites, 2 depots, 4 links, 7 bad line(s) skipped"));
    CHECK(contains(s, "sites.csv:5: demand is not a number"));
    CHECK(contains(s, "Summary"));
    CHECK(contains(s, "Goodbye."));
}

// 6. No allocator built in: option 3 explains instead of crashing
void testMenuWithoutAllocator() {
    std::istringstream in("1\n" + sample("basic") + "\n3\n4\n");
    std::ostringstream out;
    MenuApp app(MenuApp::AllocateFn{}, in, out);
    CHECK_EQ(app.run(), 0);
    CHECK(contains(out.str(), "not available in this build yet"));
}

// 7. An allocator that throws is reported and the menu keeps going
void testMenuAllocatorThrows() {
    std::istringstream in("1\n" + sample("basic") + "\n3\n4\n");
    std::ostringstream out;
    MenuApp app([](const LoadedData&) -> AllocationResult {
        throw std::invalid_argument("boom");
    }, in, out);
    CHECK_EQ(app.run(), 0);
    CHECK(contains(out.str(), "Allocation failed: boom"));
    CHECK(contains(out.str(), "Goodbye."));
}

// 8. Input ends mid-session (Ctrl+D): exits cleanly
void testMenuEndOfInput() {
    std::istringstream in("1\n");
    std::ostringstream out;
    MenuApp app(fakeMotivating, in, out);
    CHECK_EQ(app.run(), 0);
}

}  // namespace

void runCliTests() {
    RUN_TEST(testReadIntRejectsBadInput);
    RUN_TEST(testReadIntEndOfInput);
    RUN_TEST(testPrintAllocationMotivating);
    RUN_TEST(testPrintAllocationEmptyResult);
    RUN_TEST(testMenuSessionSurvivesMistakes);
    RUN_TEST(testMenuWithoutAllocator);
    RUN_TEST(testMenuAllocatorThrows);
    RUN_TEST(testMenuEndOfInput);
}
