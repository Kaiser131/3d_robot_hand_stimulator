#pragma once

#include "MathUtil.h"

struct SceneObject {
    // Pose in world space (rigid transform).
    math::Mat4 worldFromObject = math::Mat4::identity();
    bool held = false;

    // If held, object pose is: gripperWorld * heldOffset
    math::Mat4 heldOffset = math::Mat4::identity();

    // Simple size for collision/distance checks
    float radius = 0.12f;

    math::Vec3 position() const { return {worldFromObject.m[12], worldFromObject.m[13], worldFromObject.m[14]}; }
};

void drawSceneObject(const SceneObject& obj);
