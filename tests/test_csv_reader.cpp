// Unit tests for CsvReader (core/io/CsvReader.h)
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>

#include "io/CsvReader.h"
#include "test_harness.h"

namespace {

// Writes `content` to a fresh temp file and returns its path
std::string writeTemp(const std::string& name, const std::string& content) {
    const auto path = std::filesystem::temp_directory_path() / ("rr_csv_" + name);
    std::ofstream out(path, std::ios::binary);
    out << content;
    return path.string();
}

// 1. Normal input: fields split on commas, spaces trimmed, line numbers kept
void testSplitsFieldsAndKeepsLineNumbers() {
    CsvReader r(writeTemp("basic.csv", "id,name\n1, Site X \n2,Site Y\n"));
    const auto rows = r.read();
    CHECK_EQ(rows.size(), static_cast<std::size_t>(3));
    if (rows.size() == 3) {
        CHECK_EQ(rows[0].lineNumber, 1);
        CHECK_EQ(rows[1].lineNumber, 2);
        CHECK_EQ(rows[1].fields.size(), static_cast<std::size_t>(2));
        CHECK_EQ(rows[1].fields[1], std::string("Site X"));
        CHECK_EQ(rows[2].fields[0], std::string("2"));
    }
    CHECK(r.problems().empty());
}

// 2. Blank lines are skipped but still counted, so later line numbers match the file
void testBlankLinesSkipped() {
    CsvReader r(writeTemp("blank.csv", "a,b\n\n   \n1,2\n"));
    const auto rows = r.read();
    CHECK_EQ(rows.size(), static_cast<std::size_t>(2));
    if (rows.size() == 2) {
        CHECK_EQ(rows[1].lineNumber, 4);
    }
}

// 3. Windows line endings: the '\r' must not end up inside the last field
void testWindowsLineEndings() {
    CsvReader r(writeTemp("crlf.csv", "a,b\r\n1,2\r\n"));
    const auto rows = r.read();
    CHECK_EQ(rows.size(), static_cast<std::size_t>(2));
    if (rows.size() == 2) {
        CHECK_EQ(rows[1].fields[1], std::string("2"));
    }
}

// 4. Quoted field keeps its comma; "" is a literal quote
void testQuotedFields() {
    CsvReader r(writeTemp("quoted.csv", "1,\"Site, North\",3\n2,\"say \"\"hi\"\"\",4\n"));
    const auto rows = r.read();
    CHECK_EQ(rows.size(), static_cast<std::size_t>(2));
    if (rows.size() == 2) {
        CHECK_EQ(rows[0].fields.size(), static_cast<std::size_t>(3));
        CHECK_EQ(rows[0].fields[1], std::string("Site, North"));
        CHECK_EQ(rows[1].fields[1], std::string("say \"hi\""));
    }
}

// 5. Invalid: unterminated quote is reported, not returned, and does not stop the file
void testUnterminatedQuoteReported() {
    CsvReader r(writeTemp("badquote.csv", "a,b\n1,\"oops\n2,ok\n"));
    const auto rows = r.read();
    CHECK_EQ(rows.size(), static_cast<std::size_t>(2));
    CHECK_EQ(r.problems().size(), static_cast<std::size_t>(1));
    if (r.problems().size() == 1) {
        CHECK_EQ(r.problems()[0].lineNumber, 2);
    }
}

// 6. Empty file: no rows, no problems, no crash
void testEmptyFile() {
    CsvReader r(writeTemp("empty.csv", ""));
    CHECK(r.read().empty());
    CHECK(r.problems().empty());
}

// 7. Missing file throws runtime_error
void testMissingFileThrows() {
    CsvReader r("/definitely/not/here/nope.csv");
    CHECK_THROWS(r.read(), std::runtime_error);
}

}  // namespace

void runCsvReaderTests() {
    RUN_TEST(testSplitsFieldsAndKeepsLineNumbers);
    RUN_TEST(testBlankLinesSkipped);
    RUN_TEST(testWindowsLineEndings);
    RUN_TEST(testQuotedFields);
    RUN_TEST(testUnterminatedQuoteReported);
    RUN_TEST(testEmptyFile);
    RUN_TEST(testMissingFileThrows);
}
