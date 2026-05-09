#pragma once

#include "MathUtil.h"

struct Camera {
    // Orbit camera around a target.
    math::Vec3 target{0.0f, 0.6f, 0.0f};
    float yawDeg = 35.0f;
    float pitchDeg = 20.0f;
    float distance = 4.0f;

    void update(float dt, const struct InputState& input);
    void addOrbit(float deltaYawDeg, float deltaPitchDeg);
    void applyView() const;
    void reset();
};
