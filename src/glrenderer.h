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

enum class MouseStaus
{
    NONE,
    LEFT,
    RIGHT
};
class GLRenderer : public QOpenGLWidget
{
public:
    GLRenderer(QWidget *parent = nullptr);
    void settingsChanged();
    ~GLRenderer();

protected:
    // OpenGL initialization & rendering functions
    void initializeGL() override;                  // Called once at the start of the program
    void paintGL() override;                       // Called every frame in a loop
    void resizeGL(int width, int height) override; // Called when window size changes

    // camera handling functions
    void mousePressEvent(QMouseEvent *e) override;   
    void mouseMoveEvent(QMouseEvent *e) override;    
    void mouseReleaseEvent(QMouseEvent *event) override; 
    void wheelEvent(QWheelEvent *e) override;        
    void keyPressEvent(QKeyEvent *event) override;   
    void keyReleaseEvent(QKeyEvent *event) override; 
    void timerEvent(QTimerEvent *event) override;    
    void rebuildMatrices();                          

public slots:
    void tick(QTimerEvent *event); // Called once per tick of m_timer

private:
    // OpenGL objects
    GLuint m_shader;     // Stores id of shader program
    GLuint m_sphere_vbo; // Stores id of vbo
    GLuint m_sphere_vao; // Stores id of vao
    std::vector<float> m_sphereData;
    glm::vec2 m_sunPos;

    // transformation matrices
    glm::mat4 m_model = glm::mat4(1);
    glm::mat4 m_view = glm::mat4(1);
    glm::mat4 m_proj = glm::mat4(1);

    // QPoint m_prevMousePos;
    glm::vec2 m_prev_mouse_pos;
    QElapsedTimer m_elapsedTimer; 
    MouseStaus m_mouseDown = MouseStaus::NONE;
    std::unordered_map<Qt::Key, bool> m_keyMap;
    int m_timer; 

    // camera parameters
    float m_angleX;
    float m_angleY;
    float m_zoom;
    glm::vec3 m_eye;
    glm::vec3 m_look;
    glm::vec3 m_up;

    const float m_rotSpeed = 0.005;
    const float m_translSpeed = 5;

};
