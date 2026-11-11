#include "../BTLEAdvFixup/BTLEAdvPatches.hpp"
#include <cassert>
#include <cstring>
#include <cstdio>

int main() {
    using namespace BTLEAdv;
#ifdef BTLEADV_SEQUOIA
    static_assert(sizeof(kPatches) / sizeof(Patch) == 4, "one Sequoia set");
#else
    static_assert(sizeof(kPatches) / sizeof(Patch) == 12, "three Tahoe sets");
    assert(kPatches[8].find == advertisingFind2672);
    assert(kPatches[9].find == randomAddressFind2672);
    assert(kPatches[10].find == extendedConnectionFind2672);
    assert(kPatches[11].find == scanningFind2672);
    // 2672 shares three signatures with 2673, but the advertising query differs.
    assert(memcmp(advertisingFind2672, advertisingFind2673, sizeof(advertisingFind2672)) != 0);
    assert(memcmp(scanningFind2672, scanningFind2673, sizeof(scanningFind2672)) == 0);
    assert(memcmp(randomAddressFind2672, randomAddressFind2673, sizeof(randomAddressFind2672)) == 0);
    assert(memcmp(extendedConnectionFind2672, extendedConnectionFind2673, sizeof(extendedConnectionFind2672)) == 0);
#endif
    for (size_t i = 0; i < sizeof(kPatches) / sizeof(Patch); ++i) {
        assert(kPatches[i].size > 0);
        assert(kPatches[i].scanningOnly == (i % 4 == 3));
    }
    puts("BTLEAdv four-operation target tables: passed");
}
