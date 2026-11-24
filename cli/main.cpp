/* ReliefRoute command-line entry point
   reliefroute-cli                        interactive menu
   reliefroute-cli data/sample/motivating load that folder, allocate, print, exit
 Run from the repository root so the default folder data/sample/basic resolves.
 The allocator is Rohini's LinkMaxFlowAllocator (A2). Until that file is merged
 the program still builds and runs; option 3 just says allocation is not
 available yet.
 Note: __has_include is checked when this file is compiled, and the build does
 not track a header that did not exist yet. After LinkMaxFlowAllocator.h is
 added, re-run cmake (or touch cli/main.cpp) so an existing build picks it up */
#include <iostream>
#include <string>

#include "MenuApp.h"
#include "TablePrinter.h"

#if __has_include("algorithms/LinkMaxFlowAllocator.h")
#include "algorithms/LinkMaxFlowAllocator.h"
#define RELIEFROUTE_HAS_A2 1
#endif

int main(int argc, char* argv[]) {
    MenuApp::AllocateFn allocate;
#ifdef RELIEFROUTE_HAS_A2
    allocate = [](const LoadedData& data) { return LinkMaxFlowAllocator().allocate(data); };
#endif

    MenuApp app(allocate);
    if (argc >= 2) {
        // One-shot mode: load, report problems, allocate, print
        if (!app.loadFolder(argv[1])) {
            return 1;
        }
        app.showProblems();
        return app.runAllocation() ? 0 : 1;
    }
    return app.run();
}