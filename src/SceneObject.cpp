#include "SceneObject.h"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

#include <GL/glut.h>

static void drawUnitCube() {
    glutSolidCube(1.0);
}

static void drawUnitTriangle() {
    // A simple "triangle"-like 3D object.
    // GLUT provides a solid tetrahedron which reads well as a triangular object.
    glutSolidTetrahedron();
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
    switch (obj.shape) {
        case SceneObjectShape::Cube:
            glScalef(obj.radius * 2.0f, obj.radius * 2.0f, obj.radius * 2.0f);
            drawUnitCube();
            break;
        case SceneObjectShape::Triangle:
            glScalef(obj.radius * 2.2f, obj.radius * 2.2f, obj.radius * 2.2f);
            drawUnitTriangle();
            break;
        case SceneObjectShape::Round:
            // Use radius directly for the sphere.
            glutSolidSphere(obj.radius, 24, 18);
            break;
    }
    glPopMatrix();

    glPopMatrix();
}
