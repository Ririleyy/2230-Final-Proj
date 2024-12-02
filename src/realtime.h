#pragma once

// Defined before including GLEW to suppress deprecation messages on macOS
#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#endif
#include <GL/glew.h>
#include <glm/glm.hpp>

#include <unordered_map>
#include <QElapsedTimer>
#include <QOpenGLWidget>
#include <QTime>
#include <QTimer>
#include <QImage>
#include <unordered_map>

#include "utils/shaderloader.h"
#include "utils/managers/geometrymanager.h"
#include "utils/managers/uniformmanager.h"
#include "utils/managers/glmanager.h"

std::vector<float> generateSphereData(int phiTesselations, int thetaTesselations);
glm::vec4 sphericalToCartesian(float phi, float theta);
void pushVec3(glm::vec4 vec, std::vector<float> *data);

class Realtime : public QOpenGLWidget
{
public:
    Realtime(QWidget *parent = nullptr);
    void finish(); // Called on program exit
    void sceneChanged();

    void settingsChanged();
    void saveViewportImage(std::string filePath);
    void setShader(const char *vertex_file_path, const char *fragment_file_path);

public slots:
    void tick(QTimerEvent *event); // Called once per tick of m_timer

protected:
    void initializeGL() override;                  // Called once at the start of the program
    void paintGL() override;                       // Called whenever the OpenGL context changes or by an update() request
    void resizeGL(int width, int height) override; // Called when window size changes

private:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void timerEvent(QTimerEvent *event) override;

    // VAOs and VBOs for shapes
    ShapeBuffer m_screenBuffer;

    std::unordered_map<PrimitiveType, ShapeBuffer> m_primitiveBuffers;
    std::unordered_map<std::string, ShapeBuffer> m_meshBuffers;
    std::unordered_map<LODKey, ShapeBuffer> m_LODshapeBuffers;

    ShapeBuffer &getShapeBuffer(const RenderShapeData &shape);
    template <typename T>
    ShapeBuffer &getBuffer(std::unordered_map<T, ShapeBuffer> &bufferMap, const T &key);

    template <typename T>
    void cleanBuffers(std::unordered_map<T, ShapeBuffer> &bufferMap);
    // Camera
    float m_aspectRatio, m_heightAngle;
    glm::mat4 m_viewMatrix = glm::mat4(1.0f);
    glm::mat4 m_invViewMatrix = glm::mat4(1.0f);
    glm::mat4 m_projMatrix = glm::mat4(1.0f);
    // temp
    void populateSceneData();

    // Shaders
    GLuint m_phong_shader;
    GLuint m_texture_shader;
    void paintScene();
    void paintTexture(GLuint texture);

    GeometryManager *m_geometryMgr;
    UniformManager *m_uniformMgr;

    // Tick Related Variables
    int m_timer;                  // Stores timer which attempts to run ~60 times per second
    QElapsedTimer m_elapsedTimer; // Stores timer which keeps track of actual time between frames
    const int m_translSpeed = 5;
    const float m_rotSpeed = 0.005;

    // Input Related Variables
    bool m_mouseDown = false;                   // Stores state of left mouse button
    glm::vec2 m_prev_mouse_pos;                 // Stores mouse position
    std::unordered_map<Qt::Key, bool> m_keyMap; // Stores whether keys are pressed or not

    // Device Correction Variables
    double m_devicePixelRatio;

    // Scene/Camera Variables
    RenderData m_renderData;

    // FBO Variables
    int m_screen_width, m_screen_height;
    FBOConfig m_fboConfig, m_defaultFbo;

    // Textures
    GLuint m_texture;
    QImage m_image;
    std::unordered_map<QString, TextureBuffer> m_textures;
};
