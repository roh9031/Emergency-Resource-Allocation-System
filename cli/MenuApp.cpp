#include "MenuApp.h"

#include <chrono>
#include <exception>
#include <utility>

#include "InputReader.h"
#include "TablePrinter.h"

namespace {
const char* const kDefaultFolder = "data/sample/basic";
}  // namespace

MenuApp::MenuApp(AllocateFn allocate, std::istream& in, std::ostream& out)
    : allocate_(std::move(allocate)), in_(in), out_(out) {}

bool MenuApp::loadFolder(const std::string& folder) {
    try {
        LoadedData loaded = SampleLoader().load(folder);
        data_ = std::move(loaded);
        folder_ = folder;
        TablePrinter::printLoadSummary(folder_, *data_, out_);
        return true;
    } catch (const std::exception& e) {
        out_ << "Could not load " << folder << ": " << e.what() << "\n";
        if (data_) {
            out_ << "(still using " << folder_ << ")\n";
        }
        return false;
    }
}

void MenuApp::showProblems() const {
    if (!data_) {
        out_ << "Nothing loaded yet. Choose 1 first.\n";
        return;
    }
    out_ << "Problems in " << folder_ << ":\n";
    TablePrinter::printProblems(data_->problems, out_);
}

bool MenuApp::runAllocation() {
    if (!data_) {
        out_ << "Nothing loaded yet. Choose 1 first.\n";
        return false;
    }
    if (!allocate_) {
        out_ << "Allocation is not available in this build yet "
                "(LinkMaxFlowAllocator not merged).\n";
        return false;
    }
    try {
        const auto start = std::chrono::steady_clock::now();
        AllocationResult result = allocate_(*data_);
        const auto stop = std::chrono::steady_clock::now();
        result.setRuntimeMs(std::chrono::duration<double, std::milli>(stop - start).count());
        TablePrinter::printAllocation(result, *data_, out_);
        return true;
    } catch (const std::exception& e) {
        out_ << "Allocation failed: " << e.what() << "\n";
        return false;
    }
}

/* Pseudocode
   loop:
       print menu; choice = readInt(1..4); if input ended: exit
       1: folder = readLine (empty -> default); loadFolder(folder)
       2: showProblems
       3: runAllocation
       4: exit */
int MenuApp::run() {
    InputReader input(in_, out_);
    out_ << "ReliefRoute prototype\n";
    while (true) {
        out_ << "\n1. Load a sample folder\n"
                "2. Show load problems\n"
                "3. Run allocation (A2 max flow)\n"
                "4. Exit\n";
        if (data_) {
            out_ << "[loaded: " << folder_ << "]\n";
        }
        int choice = 0;
        if (!input.readInt("Choose 1-4: ", 1, 4, choice)) {
            out_ << "\n";
            return 0;  // input ended
        }
        switch (choice) {
            case 1: {
                std::string folder;
                if (!input.readLine(std::string("Folder [") + kDefaultFolder + "]: ", folder)) {
                    return 0;
                }
                loadFolder(folder.empty() ? kDefaultFolder : folder);
                break;
            }
            case 2:
                showProblems();
                break;
            case 3:
                runAllocation();
                break;
            case 4:
                out_ << "Goodbye.\n";
                return 0;
        }
    }
}
