#include "Camera.h"
#include "InputState.h"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

#include <GL/glut.h>
#include <GL/glu.h>

void Camera::reset() {
    target = {0.0f, 0.6f, 0.0f};
    yawDeg = 35.0f;
    pitchDeg = 20.0f;
    distance = 4.0f;
}

void Camera::addOrbit(float deltaYawDeg, float deltaPitchDeg) {
    yawDeg += deltaYawDeg;
    pitchDeg += deltaPitchDeg;

    // Keep yaw bounded to avoid precision growth.
    if (yawDeg > 180.0f) yawDeg -= 360.0f;
    if (yawDeg < -180.0f) yawDeg += 360.0f;

    pitchDeg = math::clamp(pitchDeg, -10.0f, 85.0f);
}

void Camera::update(float dt, const InputState& input) {
    const float orbitSpeed = 80.0f; // deg/s
    const float zoomSpeed = 2.5f;   // units/s

    float dYaw = 0.0f;
    float dPitch = 0.0f;
    if (input.isSpecialDown(GLUT_KEY_LEFT)) dYaw -= orbitSpeed * dt;
    if (input.isSpecialDown(GLUT_KEY_RIGHT)) dYaw += orbitSpeed * dt;
    if (input.isSpecialDown(GLUT_KEY_UP)) dPitch += orbitSpeed * dt;
    if (input.isSpecialDown(GLUT_KEY_DOWN)) dPitch -= orbitSpeed * dt;
    if (dYaw != 0.0f || dPitch != 0.0f) addOrbit(dYaw, dPitch);

    if (input.isDown('+') || input.isDown('=')) distance -= zoomSpeed * dt;
    if (input.isDown('-') || input.isDown('_')) distance += zoomSpeed * dt;

    distance = math::clamp(distance, 1.5f, 15.0f);
    pitchDeg = math::clamp(pitchDeg, -10.0f, 85.0f);
}

void Camera::applyView() const {
    float yaw = math::degToRad(yawDeg);
    float pitch = math::degToRad(pitchDeg);

    math::Vec3 eye;
    eye.x = target.x + distance * std::cos(pitch) * std::cos(yaw);
    eye.z = target.z + distance * std::cos(pitch) * std::sin(yaw);
    eye.y = target.y + distance * std::sin(pitch);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(eye.x, eye.y, eye.z, target.x, target.y, target.z, 0.0, 1.0, 0.0);
}
