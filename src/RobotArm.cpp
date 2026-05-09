#include "RobotArm.h"
#include "InputState.h"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

#include <GL/glut.h>
#include <GL/glu.h>

static void drawCylinder(float radius, float height, int slices = 24) {
    // Draw along +Y
    GLUquadric* q = gluNewQuadric();
    gluQuadricNormals(q, GLU_SMOOTH);
    gluCylinder(q, radius, radius, height, slices, 1);

    // Caps
    glPushMatrix();
    glRotatef(180.0f, 1, 0, 0);
    gluDisk(q, 0.0, radius, slices, 1);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0, 0, height);
    gluDisk(q, 0.0, radius, slices, 1);
    glPopMatrix();

    gluDeleteQuadric(q);
}

static void setRobotMaterial() {
    const GLfloat kd[] = {0.75f, 0.75f, 0.78f, 1.0f};
    const GLfloat ks[] = {0.35f, 0.35f, 0.35f, 1.0f};
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, kd);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, ks);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 60.0f);
}

void RobotArm::resetPose() {
    baseYaw = 0.0f;
    shoulderPitch = 25.0f;
    elbowPitch = 65.0f;
    wristRoll = 0.0f;
    gripperOpen = 20.0f;
}

void RobotArm::update(float dt, const InputState& input) {
    const float baseSpeed = 70.0f;
    const float jointSpeed = 70.0f;
    const float wristSpeed = 120.0f;
    const float gripperSpeed = 60.0f;

    float baseDelta = 0.0f;
    if (input.isDown('a') || input.isDown('A')) baseDelta -= baseSpeed;
    if (input.isDown('d') || input.isDown('D')) baseDelta += baseSpeed;

    float shoulderDelta = 0.0f;
    if (input.isDown('w') || input.isDown('W')) shoulderDelta -= jointSpeed;
    if (input.isDown('s') || input.isDown('S')) shoulderDelta += jointSpeed;

    float elbowDelta = 0.0f;
    if (input.isDown('q') || input.isDown('Q')) elbowDelta += jointSpeed;
    if (input.isDown('e') || input.isDown('E')) elbowDelta -= jointSpeed;

    float wristDelta = 0.0f;
    if (input.isDown('r') || input.isDown('R')) wristDelta -= wristSpeed;
    if (input.isDown('f') || input.isDown('F')) wristDelta += wristSpeed;

    float gripDelta = 0.0f;
    if (input.isDown('t') || input.isDown('T')) gripDelta += gripperSpeed;
    if (input.isDown('g') || input.isDown('G')) gripDelta -= gripperSpeed;

    baseYaw += baseDelta * dt;
    shoulderPitch += shoulderDelta * dt;
    elbowPitch += elbowDelta * dt;
    wristRoll += wristDelta * dt;
    gripperOpen += gripDelta * dt;

    // Joint limits
    // base unlimited-ish
    if (baseYaw > 180.0f) baseYaw -= 360.0f;
    if (baseYaw < -180.0f) baseYaw += 360.0f;

    shoulderPitch = math::clamp(shoulderPitch, -60.0f, 95.0f);
    elbowPitch = math::clamp(elbowPitch, 0.0f, 140.0f);
    wristRoll = math::clamp(wristRoll, -180.0f, 180.0f);
    gripperOpen = math::clamp(gripperOpen, 0.0f, 35.0f);
}

math::Mat4 RobotArm::computePalmWorld() const {
    using namespace math;

    Mat4 m = Mat4::identity();

    // base at origin
    m = m * Mat4::rotationY(baseYaw);
    m = m * Mat4::translation({0.0f, baseHeight, 0.0f});

    // Shoulder pitch around Z (arm swings in X-Y)
    m = m * Mat4::rotationZ(shoulderPitch);

    // Upper arm along +X
    m = m * Mat4::translation({upperLen, 0.0f, 0.0f});

    // Elbow pitch
    m = m * Mat4::rotationZ(elbowPitch);

    // Forearm along +X
    m = m * Mat4::translation({foreLen, 0.0f, 0.0f});

    // Wrist roll around X
    m = m * Mat4::rotationX(wristRoll);

    // Wrist segment to palm base
    m = m * Mat4::translation({wristLen, 0.0f, 0.0f});

    return m;
}

math::Vec3 RobotArm::computeGripperTipWorld() const {
    using namespace math;
    Mat4 palmWorld = computePalmWorld();
    // Tip is at the end of the palm in local +X
    return palmWorld.transformPoint({palmLen, 0.0f, 0.0f});
}

void RobotArm::draw() const {
    setRobotMaterial();

    // Platform
    glPushMatrix();
    {
        const GLfloat platformKd[] = {0.25f, 0.25f, 0.28f, 1.0f};
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, platformKd);
    }
    glTranslatef(0.0f, 0.03f, 0.0f);
    glScalef(1.1f, 0.06f, 1.1f);
    glutSolidCube(1.0);
    glPopMatrix();

    setRobotMaterial();

    // Base
    glPushMatrix();
    glRotatef(baseYaw, 0, 1, 0);

    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.0f);
    glRotatef(-90.0f, 1, 0, 0);
    drawCylinder(baseRadius, baseHeight);
    glPopMatrix();

    // Shoulder joint position (top of base)
    glTranslatef(0.0f, baseHeight, 0.0f);

    // Shoulder pivot sphere
    glPushMatrix();
    glutSolidSphere(upperThickness * 0.55, 20, 16);
    glPopMatrix();

    glRotatef(shoulderPitch, 0, 0, 1);

    // Upper arm (box along X)
    glPushMatrix();
    glTranslatef(upperLen * 0.5f, 0.0f, 0.0f);
    glScalef(upperLen, upperThickness, upperThickness);
    glutSolidCube(1.0);
    glPopMatrix();

    // Elbow pivot
    glTranslatef(upperLen, 0.0f, 0.0f);
    glPushMatrix();
    glutSolidSphere(foreThickness * 0.55, 20, 16);
    glPopMatrix();

    glRotatef(elbowPitch, 0, 0, 1);

    // Forearm
    glPushMatrix();
    glTranslatef(foreLen * 0.5f, 0.0f, 0.0f);
    glScalef(foreLen, foreThickness, foreThickness);
    glutSolidCube(1.0);
    glPopMatrix();

    // Wrist pivot
    glTranslatef(foreLen, 0.0f, 0.0f);
    glPushMatrix();
    glutSolidSphere(wristRadius * 0.65, 18, 14);
    glPopMatrix();

    // Wrist roll
    glRotatef(wristRoll, 1, 0, 0);

    // Wrist segment
    glPushMatrix();
    glTranslatef(wristLen * 0.5f, 0.0f, 0.0f);
    glScalef(wristLen, wristRadius * 1.3f, wristRadius * 1.3f);
    glutSolidCube(1.0);
    glPopMatrix();

    // Palm
    glTranslatef(wristLen, 0.0f, 0.0f);

    glPushMatrix();
    glTranslatef(palmLen * 0.5f, 0.0f, 0.0f);
    glScalef(palmLen, palmThickness, palmThickness);
    glutSolidCube(1.0);
    glPopMatrix();

    // Gripper fingers (open/close around Y)
    float fingerAngle = gripperOpen;

    // Left finger
    glPushMatrix();
    glTranslatef(palmLen * 0.95f, 0.0f, palmThickness * 0.65f);
    glRotatef(fingerAngle, 0, 1, 0);
    glTranslatef(fingerLen * 0.5f, 0.0f, 0.0f);
    glScalef(fingerLen, fingerThickness, fingerThickness);
    glutSolidCube(1.0);
    glPopMatrix();

    // Right finger
    glPushMatrix();
    glTranslatef(palmLen * 0.95f, 0.0f, -palmThickness * 0.65f);
    glRotatef(-fingerAngle, 0, 1, 0);
    glTranslatef(fingerLen * 0.5f, 0.0f, 0.0f);
    glScalef(fingerLen, fingerThickness, fingerThickness);
    glutSolidCube(1.0);
    glPopMatrix();

    glPopMatrix(); // base
}
