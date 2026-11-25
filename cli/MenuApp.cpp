#include "MenuApp.h"

#include <exception>
#include <utility>

#include "InputReader.h"
#include "TablePrinter.h"
#include "evaluation/Timer.h"

namespace {
const char* const kDefaultFolder = "data/sample/basic";
}  // namespace

MenuApp::MenuApp(std::vector<Method> methods, std::istream& in, std::ostream& out)
    : methods_(std::move(methods)), in_(in), out_(out) {}

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

// Time: the allocator's cost; space: the result it returns
AllocationResult MenuApp::timedRun(const Method& method) const {
    Timer timer;
    timer.start();
    AllocationResult result = method.allocate(*data_);
    timer.stop();
    result.setRuntimeMs(timer.elapsedMs());
    return result;
}

bool MenuApp::runMethod(std::size_t index) {
    if (!data_) {
        out_ << "Nothing loaded yet. Choose 1 first.\n";
        return false;
    }
    if (index >= methods_.size() || !methods_[index].allocate) {
        out_ << "That method is not available in this build.\n";
        return false;
    }
    try {
        const AllocationResult result = timedRun(methods_[index]);
        TablePrinter::printAllocation(result, *data_, out_);
        return true;
    } catch (const std::exception& e) {
        out_ << "Allocation failed: " << e.what() << "\n";
        return false;
    }
}

/* Pseudocode
   for each method m: try result[m] = timedRun(m) (print table if asked)
                      on exception: print "m failed: ...", remember the failure
   printComparison(all results that succeeded)
   Time: sum of the allocators' costs, plus printing */
bool MenuApp::compareAll(bool showTables) {
    if (!data_) {
        out_ << "Nothing loaded yet. Choose 1 first.\n";
        return false;
    }
    if (methods_.empty()) {
        out_ << "No allocation methods are built in.\n";
        return false;
    }
    std::vector<AllocationResult> results;
    std::vector<std::string> names;
    bool allOk = true;
    for (const Method& method : methods_) {
        if (!method.allocate) {
            out_ << method.id << " is not available in this build.\n";
            allOk = false;
            continue;
        }
        try {
            AllocationResult result = timedRun(method);
            if (showTables) {
                TablePrinter::printAllocation(result, *data_, out_);
            }
            results.push_back(std::move(result));
            names.push_back(method.name);
        } catch (const std::exception& e) {
            out_ << method.id << " failed: " << e.what() << "\n";
            allOk = false;
        }
    }
    if (!results.empty()) {
        TablePrinter::printComparison(results, names, *data_, out_);
    }
    return allOk;
}

/* Pseudocode
   if nothing loaded or no methods: explain and return
   list methods 1..M, plus M+1 = compare (only when M >= 2)
   choice = readInt(1..M or M+1); if input ended: return
   choice <= M: runMethod(choice - 1)   else: compareAll() */
bool MenuApp::runAllocation() {
    if (!data_) {
        out_ << "Nothing loaded yet. Choose 1 first.\n";
        return false;
    }
    if (methods_.empty()) {
        out_ << "No allocation methods are built in.\n";
        return false;
    }
    const int count = static_cast<int>(methods_.size());
    const bool canCompare = count >= 2;
    out_ << "Which method?\n";
    for (int i = 0; i < count; ++i) {
        out_ << "  " << (i + 1) << ". " << methods_[i].id << " " << methods_[i].name << "\n";
    }
    if (canCompare) {
        out_ << "  " << (count + 1) << ". Compare ";
        if (count == 2) {
            out_ << methods_[0].id << " vs " << methods_[1].id;
        } else {
            out_ << "all";
        }
        out_ << "\n";
    }
    const int last = canCompare ? count + 1 : count;
    InputReader input(in_, out_);
    int choice = 0;
    if (!input.readInt("Method 1-" + std::to_string(last) + ": ", 1, last, choice)) {
        out_ << "\n";
        return false;  // input ended
    }
    if (choice <= count) {
        return runMethod(static_cast<std::size_t>(choice - 1));
    }
    return compareAll();
}

/* Pseudocode
   loop:
       print menu; choice = readInt(1..4); if input ended: exit
       1: folder = readLine (empty -> default); loadFolder(folder)
       2: showProblems
       3: runAllocation (asks which method)
       4: exit */
int MenuApp::run() {
    InputReader input(in_, out_);
    out_ << "ReliefRoute prototype\n";
    while (true) {
        out_ << "\n1. Load a sample folder\n"
                "2. Show load problems\n"
                "3. Run allocation\n"
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
