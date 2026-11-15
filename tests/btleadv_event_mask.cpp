#include "../BTLEAdvFixup/BTLEAdvPatches.hpp"
#include <cassert>
#include <initializer_list>
#include <cstdio>

int main() {
    using namespace BTLEAdv;
    auto apply = [](bool scanning, uint8_t *page, size_t size) {
        bool changed = false;
        for (const auto &patch : kPatches) {
            if (patch.scanningOnly && !scanning) continue;
            for (size_t i = 0; i + patch.size <= size; ++i)
                if (!memcmp(page + i, patch.find, patch.size)) {
                    memcpy(page + i, patch.replace, patch.size);
                    changed = true;
                    break;
                }
        }
        return changed;
    };
    for (const char *build : {"25G332", "25G342"}) {
        const auto *find = strcmp(build, "25G332") == 0 ? eventMaskFind2672 : eventMaskFind2673;
        const auto *replace = strcmp(build, "25G332") == 0 ? eventMaskReplace2672 : eventMaskReplace2673;
        uint8_t page[4096] = {};
        memcpy(page + 64, find, sizeof(eventMaskFind2672));
        assert(!apply(false, page, sizeof(page)));
#if defined(BTLEADV_SEQUOIA) || defined(BTLEADV_SONOMA)
        assert(!apply(true, page, sizeof(page)));
        assert(memcmp(page + 64, find, sizeof(eventMaskFind2672)) == 0);
#else
        assert(apply(true, page, sizeof(page)));
        assert(memcmp(page + 64, replace, sizeof(eventMaskFind2672)) == 0);
        assert(!apply(true, page, sizeof(page)));
        assert(page[63] == 0 && page[84] == 0);
#endif
    }
    puts("BTLEAdv event mask build/scanning gates: passed");
}
