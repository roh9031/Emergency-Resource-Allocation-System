#include "io/CsvReader.h"

#include <fstream>
#include <stdexcept>
#include <utility>

namespace {

// Removes leading/trailing spaces and tabs
// Time O(c) space O(c), c = characters in s
std::string trim(const std::string& s) {
    std::size_t begin = 0;
    std::size_t end = s.size();
    while (begin < end && (s[begin] == ' ' || s[begin] == '\t')) {
        ++begin;
    }
    while (end > begin && (s[end - 1] == ' ' || s[end - 1] == '\t')) {
        --end;
    }
    return s.substr(begin, end - begin);
}

}  // namespace

// Time O(L) space O(L)
CsvReader::CsvReader(std::string path) : path_(std::move(path)) {}

// Time O(C) space O(C), C = characters in the file
std::vector<CsvReader::Row> CsvReader::read() {
    std::ifstream in(path_);
    if (!in) {
        throw std::runtime_error("cannot open file: " + path_);
    }
    problems_.clear();

    std::vector<Row> rows;
    std::string line;
    int lineNumber = 0;
    while (std::getline(in, line)) {
        ++lineNumber;
        // Windows line endings leave a '\r' at the end of each line
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        if (trim(line).empty()) {
            continue;  // blank line: nothing to report
        }
        Row row{lineNumber, {}};
        if (!splitLine(line, row.fields)) {
            problems_.push_back({lineNumber, "unterminated quote"});
            continue;
        }
        rows.push_back(std::move(row));
    }
    return rows;
}

// Time O(1) space O(1)
const std::vector<CsvReader::Problem>& CsvReader::problems() const {
    return problems_;
}

/* Pseudocode
   field = "", inQuotes = false
   for each character ch in line:
       if inQuotes:
           if ch == '"' and next char is '"': append '"', skip next
           else if ch == '"': inQuotes = false
           else append ch
       else if ch == '"': inQuotes = true
       else if ch == ',': push trim(field); field = ""
       else append ch
   if inQuotes: return false
   push trim(field); return true
   Time O(c) space O(c) */
bool CsvReader::splitLine(const std::string& line, std::vector<std::string>& out) {
    out.clear();
    std::string field;
    bool inQuotes = false;
    for (std::size_t i = 0; i < line.size(); ++i) {
        const char ch = line[i];
        if (inQuotes) {
            if (ch == '"' && i + 1 < line.size() && line[i + 1] == '"') {
                field += '"';
                ++i;
            } else if (ch == '"') {
                inQuotes = false;
            } else {
                field += ch;
            }
        } else if (ch == '"') {
            inQuotes = true;
        } else if (ch == ',') {
            out.push_back(trim(field));
            field.clear();
        } else {
            field += ch;
        }
    }
    if (inQuotes) {
        return false;
    }
    out.push_back(trim(field));
    return true;
}
