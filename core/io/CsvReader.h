/* Reads a comma separated file into rows of string fields
 Each row keeps its 1-based line number so callers can report problems as
 "<file>:<line>: <message>". Fields are trimmed of spaces/tabs and a trailing
 '\r' (Windows line endings) is removed. Blank lines are skipped silently.
 Double-quoted fields may contain commas ("Site, North"); "" inside quotes is
 a literal quote. A line with an unterminated quote is NOT returned; it is
 recorded in problems() instead.
 CsvReader knows nothing about what the columns mean: the header line is
 returned like any other row and the caller decides what to skip */
#pragma once

#include <string>
#include <vector>

class CsvReader {
public:
    struct Row {
        int lineNumber;
        std::vector<std::string> fields;
    };
    struct Problem {
        int lineNumber;
        std::string message;
    };

    // Time O(L) space O(L), L = path length
    explicit CsvReader(std::string path);

    // Reads the whole file. Can be called again (re-reads, clears problems)
    // Throws std::runtime_error naming the path if the file cannot be opened
    // Time O(C) space O(C), C = characters in the file
    std::vector<Row> read();

    // Lines that read() could not split into fields, in file order
    // Time O(1) space O(1)
    const std::vector<Problem>& problems() const;

private:
    // Splits one line; returns false on an unterminated quote
    // Time O(c) space O(c), c = characters in the line
    static bool splitLine(const std::string& line, std::vector<std::string>& out);

    std::string path_;
    std::vector<Problem> problems_;
};
