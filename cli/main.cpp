/* ReliefRoute command-line entry point
   reliefroute-cli                        interactive menu
   reliefroute-cli data/sample/motivating load that folder, run every method
                                          (full tables), print the comparison, exit
 Run from the repository root so the default folder data/sample/basic resolves */
#include <string>
#include <utility>
#include <vector>

#include "MenuApp.h"
#include "algorithms/LinkGreedyAllocator.h"
#include "algorithms/LinkMaxFlowAllocator.h"

int main(int argc, char* argv[]) {
    std::vector<MenuApp::Method> methods;
    methods.push_back({"A1", "greedy (urgency first)",
                       [](const LoadedData& d) { return LinkGreedyAllocator().allocate(d); }});
    methods.push_back({"A2", "max flow",
                       [](const LoadedData& d) { return LinkMaxFlowAllocator().allocate(d); }});

    MenuApp app(std::move(methods));
    if (argc >= 2) {
        // One-shot mode: load, report problems, run every method, compare
        if (!app.loadFolder(argv[1])) {
            return 1;
        }
        app.showProblems();
        return app.compareAll(true) ? 0 : 1;
    }
    return app.run();
}
