/* Reads validated user input for the menu
 Re-prompts on anything that is not a whole number in range ("abc", "3.5",
 "", out of range) instead of crashing or looping on a failed stream.
 Takes the streams as references so tests can script the input */
#pragma once

#include <iostream>
#include <string>

class InputReader {
public:
    // Time O(1) space O(1)
    InputReader(std::istream& in, std::ostream& out);

    /* Shows `prompt` until the user types an integer in [min, max]
     Returns false if the input ends (Ctrl+D / end of a test script) before
     a valid number is read; the caller should then exit cleanly
     Time O(k * c) for k attempts of c characters, space O(c) */
    bool readInt(const std::string& prompt, int min, int max, int& value);

    /* Shows `prompt` and reads one line, spaces trimmed. Returns false on end
     of input. An empty line is valid (the caller decides what it means)
     Time O(c) space O(c) */
    bool readLine(const std::string& prompt, std::string& line);

private:
    std::istream& in_;
    std::ostream& out_;
};
