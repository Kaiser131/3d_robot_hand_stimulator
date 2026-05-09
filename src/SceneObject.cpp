#include "SceneObject.h"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

#include <GL/glut.h>

static void drawUnitCube() {
    glutSolidCube(1.0);
}

void drawSceneObject(const SceneObject& obj) {
    glPushMatrix();
    glMultMatrixf(obj.worldFromObject.m);

    const GLfloat kd[] = {0.15f, 0.5f, 0.85f, 1.0f};
    const GLfloat ks[] = {0.25f, 0.25f, 0.25f, 1.0f};
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, kd);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, ks);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 30.0f);

    glPushMatrix();
    glScalef(obj.radius * 2.0f, obj.radius * 2.0f, obj.radius * 2.0f);
    drawUnitCube();
    glPopMatrix();

    glPopMatrix();
}
