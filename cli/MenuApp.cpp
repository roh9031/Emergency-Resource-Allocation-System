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

