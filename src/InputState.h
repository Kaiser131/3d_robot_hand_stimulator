#pragma once

#include <array>

struct InputState {
    // ASCII keys 0..255
    std::array<bool, 256> keys{};

    // GLUT special keys: we only use a small set; map by known codes range.
    // We'll store by index [0..255] too (GLUT special codes are small ints).
    std::array<bool, 256> special{};

    void setKeyDown(unsigned char k, bool down);
    void setSpecialDown(int k, bool down);

    bool isDown(unsigned char k) const { return keys[k]; }
    bool isSpecialDown(int k) const {
        if (k < 0 || k >= static_cast<int>(special.size())) return false;
        return special[static_cast<size_t>(k)];
    }
};
