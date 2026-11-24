// Unit tests for SampleLoader (core/io/SampleLoader.h) against data/sample/
// Expected values come from data/sample/README.md
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>

#include "io/SampleLoader.h"
#include "test_harness.h"

#ifndef RELIEFROUTE_SOURCE_DIR
#define RELIEFROUTE_SOURCE_DIR "."
#endif

namespace {

std::string sample(const std::string& name) {
    return std::string(RELIEFROUTE_SOURCE_DIR) + "/data/sample/" + name;
}

long long totalSupply(const LoadedData& d) {
    long long t = 0;
    for (const auto& dep : d.depots) t += dep.supply();
    return t;
}

// Writes a three-file scenario into a fresh temp folder
std::string writeFolder(const std::string& name, const std::string& sites,
                        const std::string& depots, const std::string& links) {
    const auto dir = std::filesystem::temp_directory_path() / ("rr_loader_" + name);
    std::filesystem::create_directories(dir);
    std::ofstream(dir / "sites.csv", std::ios::binary) << sites;
    std::ofstream(dir / "depots.csv", std::ios::binary) << depots;
    std::ofstream(dir / "links.csv", std::ios::binary) << links;
    return dir.string();
}

// 1. Normal input: basic/ loads completely with no problems
void testBasicLoadsCleanly() {
    const LoadedData d = SampleLoader().load(sample("basic"));
    CHECK_EQ(d.sites.size(), static_cast<std::size_t>(3));
    CHECK_EQ(d.depots.size(), static_cast<std::size_t>(2));
    CHECK_EQ(d.links.size(), static_cast<std::size_t>(6));
    CHECK(d.problems.empty());
    CHECK_EQ(totalSupply(d), 18LL);
    if (d.sites.size() == 3) {
        CHECK_EQ(d.sites[0].name(), std::string("Site X"));  // file order kept
        CHECK_EQ(d.sites[0].demand(), 10);
        CHECK_EQ(d.sites[0].urgency(), 10.0);
    }
}

// 2. Small exact case: motivating/ (A->X, A->Y, B->X)
void testMotivatingLinks() {
    const LoadedData d = SampleLoader().load(sample("motivating"));
    CHECK_EQ(d.sites.size(), static_cast<std::size_t>(2));
    CHECK_EQ(d.depots.size(), static_cast<std::size_t>(2));
    CHECK_EQ(d.links.size(), static_cast<std::size_t>(3));
    CHECK(d.problems.empty());
    if (d.links.size() == 3) {
        CHECK_EQ(d.links[2].depotId, 2);
        CHECK_EQ(d.links[2].siteId, 1);
        CHECK_EQ(d.links[2].capacity, 1);
    }
}

// 3. Invalid input: exactly the 7 documented bad lines are reported and skipped
void testInvalidReportsEveryBadLine() {
    const LoadedData d = SampleLoader().load(sample("invalid"));
    CHECK_EQ(d.problems.size(), static_cast<std::size_t>(7));
    if (d.problems.size() == 7) {
        CHECK_EQ(d.problems[0], std::string("sites.csv:4: wrong number of fields (expected 6, got 5)"));
        CHECK_EQ(d.problems[1], std::string("sites.csv:5: demand is not a number"));
        CHECK_EQ(d.problems[2], std::string("sites.csv:6: demand is negative"));
        CHECK_EQ(d.problems[3], std::string("sites.csv:7: duplicate site id 2 (first seen on line 3)"));
        CHECK_EQ(d.problems[4], std::string("depots.csv:3: supply is negative"));
        CHECK_EQ(d.problems[5], std::string("links.csv:5: unknown site id 99"));
        CHECK_EQ(d.problems[6], std::string("links.csv:6: capacity is negative"));
    }
    // What remains: sites 1, 2, 6 / depots 1, 3 / 4 links, supply 17
    CHECK_EQ(d.sites.size(), static_cast<std::size_t>(3));
    if (d.sites.size() == 3) {
        CHECK_EQ(d.sites[0].id(), 1);
        CHECK_EQ(d.sites[1].id(), 2);
        CHECK_EQ(d.sites[1].name(), std::string("Site Q"));  // first occurrence kept
        CHECK_EQ(d.sites[2].id(), 6);
    }
    CHECK_EQ(d.depots.size(), static_cast<std::size_t>(2));
    if (d.depots.size() == 2) {
        CHECK_EQ(d.depots[0].id(), 1);
        CHECK_EQ(d.depots[1].id(), 3);
    }
    CHECK_EQ(d.links.size(), static_cast<std::size_t>(4));
    CHECK_EQ(totalSupply(d), 17LL);
}

// 4. Larger input: 200 sites, 20 depots, 400 links, supply 3990
void testLargeLoads() {
    const LoadedData d = SampleLoader().load(sample("large"));
    CHECK_EQ(d.sites.size(), static_cast<std::size_t>(200));
    CHECK_EQ(d.depots.size(), static_cast<std::size_t>(20));
    CHECK_EQ(d.links.size(), static_cast<std::size_t>(400));
    CHECK(d.problems.empty());
    CHECK_EQ(totalSupply(d), 3990LL);
}

// 5. Empty input: header-only files load as an empty scenario, no crash
void testHeaderOnlyFiles() {
    const auto dir = writeFolder("empty", "id,name,demand,urgency,lat,lon\n",
                                 "id,name,supply,lat,lon\n", "depotId,siteId,capacity\n");
    const LoadedData d = SampleLoader().load(dir);
    CHECK(d.sites.empty());
    CHECK(d.depots.empty());
    CHECK(d.links.empty());
    CHECK(d.problems.empty());
}

// 6. Boundary/edge: urgency 0, a link to a skipped depot, duplicate link, CRLF
void testOtherBadLines() {
    const auto dir = writeFolder(
        "edge",
        "id,name,demand,urgency,lat,lon\r\n1,A,5,0,0,0\r\n2,B,0,1.5,0,0\r\n",
        "id,name,supply,lat,lon\r\n1,D1,3,0,0\r\n2,D2,-1,0,0\r\n",
        "depotId,siteId,capacity\r\n1,2,3\r\n2,2,3\r\n1,2,1\r\n1,2.5,1\r\n");
    const LoadedData d = SampleLoader().load(dir);
    CHECK_EQ(d.sites.size(), static_cast<std::size_t>(1));  // demand 0 is allowed
    CHECK_EQ(d.depots.size(), static_cast<std::size_t>(1));
    CHECK_EQ(d.links.size(), static_cast<std::size_t>(1));
    CHECK_EQ(d.problems.size(), static_cast<std::size_t>(5));
    if (d.problems.size() == 5) {
        CHECK_EQ(d.problems[0], std::string("sites.csv:2: urgency must be > 0"));
        CHECK_EQ(d.problems[1], std::string("depots.csv:3: supply is negative"));
        CHECK_EQ(d.problems[2], std::string("links.csv:3: unknown depot id 2"));
        CHECK_EQ(d.problems[3], std::string("links.csv:4: duplicate link 1 -> 2"));
        CHECK_EQ(d.problems[4], std::string("links.csv:5: siteId is not an integer"));
    }
}

// 7. Missing folder throws runtime_error (the CLI prints it and returns to the menu)
void testMissingFolderThrows() {
    CHECK_THROWS(SampleLoader().load("/definitely/not/a/folder"), std::runtime_error);
}

}  // namespace

void runSampleLoaderTests() {
    RUN_TEST(testBasicLoadsCleanly);
    RUN_TEST(testMotivatingLinks);
    RUN_TEST(testInvalidReportsEveryBadLine);
    RUN_TEST(testLargeLoads);
    RUN_TEST(testHeaderOnlyFiles);
    RUN_TEST(testOtherBadLines);
    RUN_TEST(testMissingFolderThrows);
}
