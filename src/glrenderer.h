#pragma once

// Defined before including GLEW to suppress deprecation messages on macOS
#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#endif

#include "GL/glew.h" // Must always be first include
#include <QCoreApplication>
#include <QOpenGLWidget>
#include <QElapsedTimer>
#include <QKeyEvent>
#include <QMouseEvent>
#include "glm/glm.hpp"
#include "utils/camera.h"

QT_FORWARD_DECLARE_CLASS(QOpenGLShaderProgram)

class GLRenderer : public QOpenGLWidget
{
public:
    GLRenderer(QWidget *parent = nullptr);
    ~GLRenderer();

protected:
    void initializeGL() override;                  // Called once at the start of the program
    void paintGL() override;                       // Called every frame in a loop
    void resizeGL(int width, int height) override; // Called when window size changes

    void mousePressEvent(QMouseEvent *e) override;   // Used for camera movement
    void mouseMoveEvent(QMouseEvent *e) override;    // Used for camera movement
    void mouseReleaseEvent(QMouseEvent *event) override; // Used for camera movement
    void wheelEvent(QWheelEvent *e) override;        // Used for camera movement
    void keyPressEvent(QKeyEvent *event) override;   // Used for camera movement
    void keyReleaseEvent(QKeyEvent *event) override; // Used for camera movement
    void timerEvent(QTimerEvent *event) override;    // Used for camera movement
    void rebuildMatrices();                          // Used for camera movement

public slots:
    void tick(QTimerEvent *event); // Called once per tick of m_timer

private:
    GLuint m_shader;     // Stores id of shader program
    GLuint m_sphere_vbo; // Stores id of vbo
    GLuint m_sphere_vao; // Stores id of vao
    std::vector<float> m_sphereData;

    glm::mat4 m_model = glm::mat4(1);
    glm::mat4 m_view = glm::mat4(1);
    glm::mat4 m_proj = glm::mat4(1);

    glm::vec4 m_lightPos; // The world-space position of the point light

    float m_ka;
    float m_kd;
    float m_ks;
    float m_shininess;

    QPoint m_prevMousePos;
    float m_angleX;
    float m_angleY;
    float m_zoom;
    glm::vec3 m_eye;
    glm::vec3 m_look;
    glm::vec3 m_up;
    std::unordered_map<Qt::Key, bool> m_keyMap;
    int m_timer; 

    bool m_mouseDown = false;
    glm::vec2 m_prev_mouse_pos;
    const float m_rotSpeed = 0.005;
    const float m_translSpeed = 5;
    QElapsedTimer m_elapsedTimer; // Stores timer which keeps track of actual time between frames
};
