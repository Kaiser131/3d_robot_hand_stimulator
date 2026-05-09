#pragma once

#include "MathUtil.h"

struct InputState;

struct RobotArm {
    // Angles in degrees.
    float baseYaw = 0.0f;
    float shoulderPitch = 25.0f;
    float elbowPitch = 65.0f;
    float wristRoll = 0.0f;
    float gripperOpen = 20.0f; // 0 = closed, higher = open

    // Dimensions
    float baseHeight = 0.35f;
    float baseRadius = 0.25f;

    float upperLen = 0.85f;
    float upperThickness = 0.16f;

    float foreLen = 0.75f;
    float foreThickness = 0.14f;

    float wristLen = 0.18f;
    float wristRadius = 0.10f;

    float palmLen = 0.20f;
    float palmThickness = 0.10f;

    float fingerLen = 0.28f;
    float fingerThickness = 0.06f;

    void update(float dt, const InputState& input);
    void draw() const;

    void resetPose();

    // For pickup logic.
    math::Mat4 computePalmWorld() const;
    math::Vec3 computeGripperTipWorld() const;
};
