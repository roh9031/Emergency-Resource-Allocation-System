// Unit tests for the CLI layer: InputReader, TablePrinter, MenuApp (cli/)
// Input is scripted with istringstream and output captured with ostringstream
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "InputReader.h"
#include "MenuApp.h"
#include "TablePrinter.h"
#include "algorithms/LinkGreedyAllocator.h"
#include "algorithms/LinkMaxFlowAllocator.h"
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

// The two real methods, as main.cpp builds them
std::vector<MenuApp::Method> realMethods() {
    return {{"A1", "greedy (urgency first)",
             [](const LoadedData& d) { return LinkGreedyAllocator().allocate(d); }},
            {"A2", "max flow",
             [](const LoadedData& d) { return LinkMaxFlowAllocator().allocate(d); }}};
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

/* 5. Weighted value = sum of units x urgency of the receiving site.
   motivating: X urgency 10, Y urgency 9.
   A1 ships A->X only: 1 x 10 = 10.  A2 ships to both: 1 x 10 + 1 x 9 = 19 */
void testWeightedValueMotivating() {
    const LoadedData data = SampleLoader().load(sample("motivating"));
    const AllocationResult greedy = LinkGreedyAllocator().allocate(data);
    const AllocationResult maxFlow = LinkMaxFlowAllocator().allocate(data);
    CHECK_EQ(TablePrinter::weightedValue(greedy, data), 10.0);
    CHECK_EQ(TablePrinter::weightedValue(maxFlow, data), 19.0);
    CHECK_EQ(TablePrinter::weightedValue(AllocationResult("A1"), data), 0.0);  // boundary
}

// 6. Normal: the menu runs a chosen method (3 -> 1 = A1, 3 -> 2 = A2)
void testMenuRunsChosenMethod() {
    std::istringstream in("1\n" + sample("motivating") + "\n3\n1\n3\n2\n4\n");
    std::ostringstream out;
    MenuApp app(realMethods(), in, out);
    CHECK_EQ(app.run(), 0);
    const std::string s = out.str();
    CHECK(contains(s, "1. A1 greedy (urgency first)"));
    CHECK(contains(s, "2. A2 max flow"));
    CHECK(contains(s, "3. Compare A1 vs A2"));
    CHECK(contains(s, "Allocation (A1)"));
    CHECK(contains(s, "Allocation (A2)"));
    CHECK(contains(s, "Goodbye."));
}

// 7. Compare: one line per method with kits and weighted value (motivating)
void testMenuCompareMotivating() {
    std::istringstream in("1\n" + sample("motivating") + "\n3\n3\n4\n");
    std::ostringstream out;
    MenuApp app(realMethods(), in, out);
    CHECK_EQ(app.run(), 0);
    const std::string s = out.str();
    CHECK(contains(s, "Comparison"));
    CHECK(contains(s, "Weighted value"));
    CHECK(contains(s, "10.00"));  // A1
    CHECK(contains(s, "19.00"));  // A2
    CHECK(!contains(s, "Allocation (A1)"));  // compare prints summary lines only
    // In the comparison table, A1's line comes before A2's line
    const std::size_t table = s.find("Comparison");
    CHECK(table != std::string::npos);
    CHECK(s.find("A1 greedy (urgency first)", table) < s.find("A2 max flow", table));
}

// 8. Invalid: a bad method number is re-asked inside the method menu
void testMenuMethodChoiceRejectsBadInput() {
    std::istringstream in("1\n" + sample("basic") + "\n3\n9\nabc\n2\n4\n");
    std::ostringstream out;
    MenuApp app(realMethods(), in, out);
    CHECK_EQ(app.run(), 0);
    CHECK(contains(out.str(), "Please enter a number from 1 to 3."));
    CHECK(contains(out.str(), "Please enter a whole number."));
    CHECK(contains(out.str(), "Allocation (A2)"));
}

// 9. Full menu session with mistakes: bad choice, allocate before loading,
//    missing folder, then a real load, problems, allocate (A2), exit
void testMenuSessionSurvivesMistakes() {
    std::istringstream in("x\n7\n3\n1\n/no/such/folder\n1\n" + sample("invalid") +
                          "\n2\n3\n2\n4\n");
    std::ostringstream out;
    MenuApp app(realMethods(), in, out);
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

// 10. Empty: no methods built in -> option 3 explains instead of crashing
void testMenuWithoutMethods() {
    std::istringstream in("1\n" + sample("basic") + "\n3\n4\n");
    std::ostringstream out;
    MenuApp app(std::vector<MenuApp::Method>{}, in, out);
    CHECK_EQ(app.run(), 0);
    CHECK(contains(out.str(), "No allocation methods are built in."));
}

// 11. An allocator that throws is reported; in compare the other one still runs
void testMenuAllocatorThrows() {
    std::vector<MenuApp::Method> methods{
        {"A1", "broken", [](const LoadedData&) -> AllocationResult {
             throw std::invalid_argument("boom");
         }},
        {"A2", "fake", fakeMotivating}};
    std::istringstream in("1\n" + sample("motivating") + "\n3\n1\n3\n3\n4\n");
    std::ostringstream out;
    MenuApp app(methods, in, out);
    CHECK_EQ(app.run(), 0);
    const std::string s = out.str();
    CHECK(contains(s, "Allocation failed: boom"));  // run A1 alone
    CHECK(contains(s, "A1 failed: boom"));          // compare
    CHECK(contains(s, "A2 fake"));                  // A2 still compared
    CHECK(contains(s, "Goodbye."));
}

// 12. Input ends mid-session (Ctrl+D), including inside the method menu
void testMenuEndOfInput() {
    std::istringstream in1("1\n");
    std::ostringstream out1;
    MenuApp app1(realMethods(), in1, out1);
    CHECK_EQ(app1.run(), 0);

    std::istringstream in2("1\n" + sample("basic") + "\n3\n");
    std::ostringstream out2;
    MenuApp app2(realMethods(), in2, out2);
    CHECK_EQ(app2.run(), 0);
}

// 13. Larger input: compare on large/ (200 sites): A1 never beats A2 on kits
void testCompareLarge() {
    std::ostringstream sink;
    MenuApp quiet(realMethods(), std::cin, sink);
    CHECK(quiet.loadFolder(sample("large")));
    CHECK(quiet.compareAll());
    const LoadedData data = SampleLoader().load(sample("large"));
    CHECK(LinkGreedyAllocator().allocate(data).totalUnits() <=
          LinkMaxFlowAllocator().allocate(data).totalUnits());
    CHECK(contains(sink.str(), "Comparison"));
}

}  // namespace

void runCliTests() {
    RUN_TEST(testReadIntRejectsBadInput);
    RUN_TEST(testReadIntEndOfInput);
    RUN_TEST(testPrintAllocationMotivating);
    RUN_TEST(testPrintAllocationEmptyResult);
    RUN_TEST(testWeightedValueMotivating);
    RUN_TEST(testMenuRunsChosenMethod);
    RUN_TEST(testMenuCompareMotivating);
    RUN_TEST(testMenuMethodChoiceRejectsBadInput);
    RUN_TEST(testMenuSessionSurvivesMistakes);
    RUN_TEST(testMenuWithoutMethods);
    RUN_TEST(testMenuAllocatorThrows);
    RUN_TEST(testMenuEndOfInput);
    RUN_TEST(testCompareLarge);
}
