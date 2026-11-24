#include "InputReader.h"

#include <cerrno>
#include <cstdlib>

namespace {

// Time O(c) space O(c)
std::string trim(const std::string& s) {
    const auto first = s.find_first_not_of(" \t\r");
    if (first == std::string::npos) {
        return "";
    }
    const auto last = s.find_last_not_of(" \t\r");
    return s.substr(first, last - first + 1);
}

}  // namespace

InputReader::InputReader(std::istream& in, std::ostream& out) : in_(in), out_(out) {}

// Time O(c) space O(c)
bool InputReader::readLine(const std::string& prompt, std::string& line) {
    out_ << prompt;
    out_.flush();
    std::string raw;
    if (!std::getline(in_, raw)) {
        return false;
    }
    line = trim(raw);
    return true;
}

/* Pseudocode
   loop:
       line = readLine(prompt); if input ended: return false
       parse line as a whole integer; if it fails: print "not a whole number", repeat
       if value < min or value > max: print the allowed range, repeat
       return value
   Time O(k * c) space O(c) */
bool InputReader::readInt(const std::string& prompt, int min, int max, int& value) {
    std::string line;
    while (readLine(prompt, line)) {
        errno = 0;
        char* end = nullptr;
        const long long v = std::strtoll(line.c_str(), &end, 10);
        if (line.empty() || *end != '\0' || errno == ERANGE) {
            out_ << "  Please enter a whole number.\n";
            continue;
        }
        if (v < min || v > max) {
            out_ << "  Please enter a number from " << min << " to " << max << ".\n";
            continue;
        }
        value = static_cast<int>(v);
        return true;
    }
    return false;
}
