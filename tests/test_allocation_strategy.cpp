// Unit tests for AllocationStrategy (core/algorithms/AllocationStrategy.h)
// allocate() cannot be called yet because Scenario does not exist; the
// allocator test files test it once Scenario is merged
#include <memory>
#include <string>
#include <type_traits>

#include "algorithms/AllocationStrategy.h"
#include "test_harness.h"

namespace {

// 9a. Compile-time contract: nobody can create a bare AllocationStrategy,
//     and deleting through a base pointer is safe
static_assert(std::is_abstract<AllocationStrategy>::value,
              "AllocationStrategy must be abstract");
static_assert(std::has_virtual_destructor<AllocationStrategy>::value,
              "AllocationStrategy needs a virtual destructor");

// Minimal subclass; allocate ignores the scenario, so the incomplete
// Scenario type is enough here
class FakeStrategy : public AllocationStrategy {
public:
    explicit FakeStrategy(bool* destroyed) : destroyed_(destroyed) {}
    ~FakeStrategy() override { *destroyed_ = true; }
    AllocationResult allocate(const Scenario&) const override {
        return AllocationResult(id());
    }
    std::string id() const override { return "AX"; }
    std::string name() const override { return "Fake strategy"; }

private:
    bool* destroyed_;
};

// 9b. Calls through the base pointer reach the subclass (virtual dispatch),
//     and destroying through it runs the subclass destructor
void testSubclassThroughBasePointer() {
    bool destroyed = false;
    {
        std::unique_ptr<AllocationStrategy> s =
            std::make_unique<FakeStrategy>(&destroyed);
        CHECK_EQ(s->id(), std::string("AX"));
        CHECK_EQ(s->name(), std::string("Fake strategy"));
        CHECK(!destroyed);
    }
    CHECK(destroyed);
}

}  // namespace

void runAllocationStrategyTests() {
    RUN_TEST(testSubclassThroughBasePointer);
}
