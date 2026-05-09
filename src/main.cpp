#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

#include <chrono>
#include <cstdlib>
#include <string>
#include <vector>
#include <algorithm>

#include <GL/glut.h>
#include <GL/glu.h>

#include "Camera.h"
#include "InputState.h"
#include "RobotArm.h"
#include "SceneObject.h"
#include "MathUtil.h"

static int gWindowWidth = 1280;
static int gWindowHeight = 720;

static Camera gCamera;
static InputState gInput;
static RobotArm gArm;
static SceneObject gObject;

static bool gShowInstructions = false;

static std::chrono::steady_clock::time_point gPrevTime;

static void setupGL();
static void display();
static void reshape(int w, int h);
static void idle();
static void keyboardDown(unsigned char key, int x, int y);
static void keyboardUp(unsigned char key, int x, int y);
static void specialDown(int key, int x, int y);
static void specialUp(int key, int x, int y);
static void mouseButton(int button, int state, int x, int y);
static void mouseMotion(int x, int y);

static void drawGround();
static void updatePickup();
static void drawInstructionsOverlay();

static float distancePointToAabb(const math::Vec3& p, const math::Vec3& center, const math::Vec3& halfExtents) {
    // Computes distance from point p to an axis-aligned box centered at center with halfExtents.
    // If p is inside the box, distance is 0.
    float dx = 0.0f;
    float dy = 0.0f;
    float dz = 0.0f;

    float minX = center.x - halfExtents.x;
    float maxX = center.x + halfExtents.x;
    float minY = center.y - halfExtents.y;
    float maxY = center.y + halfExtents.y;
    float minZ = center.z - halfExtents.z;
    float maxZ = center.z + halfExtents.z;

    if (p.x < minX) dx = minX - p.x;
    else if (p.x > maxX) dx = p.x - maxX;

    if (p.y < minY) dy = minY - p.y;
    else if (p.y > maxY) dy = p.y - maxY;

    if (p.z < minZ) dz = minZ - p.z;
    else if (p.z > maxZ) dz = p.z - maxZ;

    return std::sqrt(dx * dx + dy * dy + dz * dz);
}

static bool gRightMouseDown = false;
static int gLastMouseX = 0;
static int gLastMouseY = 0;

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(gWindowWidth, gWindowHeight);
    glutCreateWindow("3D Robot Arm Simulation (OpenGL + GLUT)");

    setupGL();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutIdleFunc(idle);

    glutKeyboardFunc(keyboardDown);
    glutKeyboardUpFunc(keyboardUp);
    glutSpecialFunc(specialDown);
    glutSpecialUpFunc(specialUp);

    glutMouseFunc(mouseButton);
    glutMotionFunc(mouseMotion);

    gPrevTime = std::chrono::steady_clock::now();

    // Place initial object on the ground
    gObject.radius = 0.12f;
    gObject.worldFromObject = math::Mat4::translation({0.95f, gObject.radius, 0.35f});

    glutMainLoop();
    return 0;
}

static void setupGL() {
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glEnable(GL_NORMALIZE);
    glShadeModel(GL_SMOOTH);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    GLfloat lightPos[] = {2.0f, 4.5f, 2.0f, 1.0f};
    GLfloat lightAmbient[] = {0.12f, 0.12f, 0.12f, 1.0f};
    GLfloat lightDiffuse[] = {0.95f, 0.95f, 0.95f, 1.0f};
    GLfloat lightSpecular[] = {0.65f, 0.65f, 0.65f, 1.0f};

    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);

    glClearColor(0.07f, 0.08f, 0.10f, 1.0f);
}

static void reshape(int w, int h) {
    gWindowWidth = (w <= 0) ? 1 : w;
    gWindowHeight = (h <= 0) ? 1 : h;

    glViewport(0, 0, gWindowWidth, gWindowHeight);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (double)gWindowWidth / (double)gWindowHeight, 0.05, 50.0);
    glMatrixMode(GL_MODELVIEW);
}

static void drawGround() {
    glDisable(GL_LIGHTING);
    glColor3f(0.18f, 0.18f, 0.20f);

    // Ground quad
    glBegin(GL_QUADS);
    glVertex3f(-6.0f, 0.0f, -6.0f);
    glVertex3f(6.0f, 0.0f, -6.0f);
    glVertex3f(6.0f, 0.0f, 6.0f);
    glVertex3f(-6.0f, 0.0f, 6.0f);
    glEnd();

    // Simple grid
    glColor3f(0.25f, 0.25f, 0.28f);
    glBegin(GL_LINES);
    for (int i = -6; i <= 6; ++i) {
        glVertex3f((float)i, 0.001f, -6.0f);
        glVertex3f((float)i, 0.001f, 6.0f);
        glVertex3f(-6.0f, 0.001f, (float)i);
        glVertex3f(6.0f, 0.001f, (float)i);
    }
    glEnd();

    glEnable(GL_LIGHTING);
}

static void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    gCamera.applyView();

    // Positional lights are transformed by the current model-view matrix at the time you set them.
    // Setting this every frame keeps the light stable relative to the world as the camera moves.
    {
        const GLfloat lightPos[] = {2.0f, 4.5f, 2.0f, 1.0f};
        glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    }

    drawGround();

    // Robot arm
    gArm.draw();

    // Object
    drawSceneObject(gObject);

    if (gShowInstructions) {
        drawInstructionsOverlay();
    }

    glutSwapBuffers();
}

static void drawBitmapText(float x, float y, const std::string& text) {
    glRasterPos2f(x, y);
    for (unsigned char c : text) {
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, c);
    }
}

static void drawInstructionsOverlay() {
    // 2D overlay in screen space.
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0.0, (double)gWindowWidth, 0.0, (double)gWindowHeight);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);

    // Text color
    glColor3f(0.92f, 0.92f, 0.92f);

    const float startX = 18.0f;
    float y = (float)gWindowHeight - 26.0f;
    const float lineH = 18.0f;

    const std::vector<std::string> lines = {
        "Controls (press I to hide)",
        "", 
        "Robot joints:",
        "  Base yaw:      A / D",
        "  Shoulder:      W / S",
        "  Elbow:         Q / E",
        "  Wrist roll:    R / F",
        "  Gripper:       T (open) / G (close)",
        "", 
        "Camera:",
        "  Orbit:         Arrow keys",
        "  Orbit (mouse): Right mouse drag",
        "  Zoom:          + / -",
        "  Reset view:    C",
        "", 
        "Misc:",
        "  Reset pose:    X",
        "  Off / Quit:    Esc"
    };

    for (const auto& line : lines) {
        drawBitmapText(startX, y, line);
        y -= lineH;
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

static void updatePickup() {
    // Touch-based pickup: if the gripper tip touches/gets close to the cube and you close the gripper,
    // it latches immediately. Tuned for easy demo use.
    const float touchRadius = 0.22f;          // how "forgiving" touching is
    const float releaseThresholdDeg = 22.0f;  // open enough to drop

    math::Mat4 palmWorld = gArm.computePalmWorld();
    // A frame at the gripper tip (end of the palm) so a held object moves with the tip.
    math::Mat4 tipWorld = palmWorld * math::Mat4::translation({gArm.palmLen, 0.0f, 0.0f});
    math::Vec3 tipPos = tipWorld.transformPoint({0.0f, 0.0f, 0.0f});

    const bool isClosing = (gInput.isDown('g') || gInput.isDown('G'));

    if (!gObject.held) {
        math::Vec3 objCenter = gObject.position();
        math::Vec3 halfExtents{gObject.radius, gObject.radius, gObject.radius};
        float distToCube = distancePointToAabb(tipPos, objCenter, halfExtents);

        if (isClosing && distToCube <= touchRadius) {
            // Grab: compute held offset so object stays fixed relative to the gripper tip.
            math::Mat4 invTip = math::Mat4::inverseRigidBody(tipWorld);
            gObject.heldOffset = invTip * gObject.worldFromObject;
            gObject.held = true;

            // Visual: when we successfully grab something, show the parallel "| |" pose.
            // Users can keep squeezing (key 'G') to go past the limit into the crossed "X" pose.
            gArm.gripperOpen = 0.0f;
        }
    } else {
        // Release on opening.
        const bool isOpening = (gInput.isDown('t') || gInput.isDown('T'));
        if (isOpening || gArm.gripperOpen >= releaseThresholdDeg) {
            gObject.held = false;

            // Default state is open "V".
            // Keep it at least slightly open after release, but don't force-close
            // if the user already opened wider.
            gArm.gripperOpen = std::max(gArm.gripperOpen, 35.0f);
        }
    }

    if (gObject.held) {
        gObject.worldFromObject = tipWorld * gObject.heldOffset;

        // Keep object above ground (simple floor constraint)
        math::Vec3 pos = gObject.position();
        float minY = gObject.radius;
        if (pos.y < minY) {
            gObject.worldFromObject.m[13] = minY;
        }
    }
}

static void idle() {
    auto now = std::chrono::steady_clock::now();
    float dt = std::chrono::duration<float>(now - gPrevTime).count();
    gPrevTime = now;

    // Clamp dt to avoid huge jumps when debugging / resizing
    dt = math::clamp(dt, 0.0f, 1.0f / 20.0f);

    gCamera.update(dt, gInput);
    gArm.update(dt, gInput);

    updatePickup();

    glutPostRedisplay();
}

static void keyboardDown(unsigned char key, int, int) {
    gInput.setKeyDown(key, true);

    if (key == 'i' || key == 'I') {
        gShowInstructions = !gShowInstructions;
    }

    if (key == 27) {
        std::exit(0);
    }

    if (key == 'c' || key == 'C') {
        gCamera.reset();
    }

    if (key == 'x' || key == 'X') {
        gArm.resetPose();
        if (gObject.held) gObject.held = false;
    }
}

static void keyboardUp(unsigned char key, int, int) {
    gInput.setKeyDown(key, false);
}

static void specialDown(int key, int, int) {
    gInput.setSpecialDown(key, true);
}

static void specialUp(int key, int, int) {
    gInput.setSpecialDown(key, false);
}

static void mouseButton(int button, int state, int x, int y) {
    if (button == GLUT_RIGHT_BUTTON) {
        gRightMouseDown = (state == GLUT_DOWN);
        gLastMouseX = x;
        gLastMouseY = y;
    }
}

static void mouseMotion(int x, int y) {
    if (!gRightMouseDown) return;

    int dx = x - gLastMouseX;
    int dy = y - gLastMouseY;
    gLastMouseX = x;
    gLastMouseY = y;

    // Sensitivity tuned for typical 720p/1080p windows.
    const float sens = 0.25f; // degrees per pixel
    gCamera.addOrbit(dx * sens, -dy * sens);
}
