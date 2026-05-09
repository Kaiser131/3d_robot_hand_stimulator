#include "InputState.h"

void InputState::setKeyDown(unsigned char k, bool down) {
    keys[k] = down;
}

void InputState::setSpecialDown(int k, bool down) {
    if (k < 0 || k >= static_cast<int>(special.size())) return;
    special[static_cast<size_t>(k)] = down;
}
