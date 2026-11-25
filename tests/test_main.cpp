/* Test runner Each class has its own test file with a run<Class>Tests()
function; call them from main() Exit code is non zero if any check fails
so ctest and CI fail */
#include <iostream>

#include "test_harness.h"

int main() {
    runFlowNetworkTests();
    runBinarySearchTests();
    runAllocationResultTests();
    runAllocationStrategyTests();
    runSiteTests();
    runDepotTests();
    runParamsTests();
    runCsvReaderTests();
    runSampleLoaderTests();
    runCliTests();
    runTimerTests();
    runLinkMaxFlowTests();
    runMergeSortTests();
    runLinkGreedyTests();

    if (testing::failures == 0) {
        std::cout << "All tests passed.\n";
        return 0;
    }
    std::cerr << testing::failures << " check(s) failed.\n";
    return 1;
}
