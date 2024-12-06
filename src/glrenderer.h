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
#include "utils/terrain.h"
#include "utils/particle.h"
#include <memory>

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
    void setWeatherType(bool isSnow);
    void setWeatherEnabled(bool enabled) { m_weatherEnabled = enabled; }
    ~GLRenderer();

protected:
    // OpenGL initialization & rendering functions
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int width, int height) override;

    // camera handling functions
    void mousePressEvent(QMouseEvent *e) override;
    void mouseMoveEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *e) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void timerEvent(QTimerEvent *event) override;
    void rebuildMatrices();
    void timeToSunPos(const float time);

public slots:
    void tick(QTimerEvent *event);

private:
    void initializeParticleSystem();
    void updateParticles(float deltaTime);
    void renderParticles();
    void bindTerrainVaoVbo();
    void paintTerrain();
    void paintDome();

    // Particle system
    std::unique_ptr<ParticleSystem> m_particleSystem;
    GLuint m_particle_shader;
    GLuint m_particle_vbo;
    GLuint m_particle_vao;
    bool m_isSnow = true;
    bool m_weatherEnabled = true;

    // OpenGL objects
    GLuint m_shader;
    GLuint m_sphere_vbo;
    GLuint m_sphere_vao;
    std::vector<float> m_sphereData;
    glm::vec2 m_sunPos;

    // transformation matrices
    glm::mat4 m_model = glm::mat4(1);
    glm::mat4 m_view = glm::mat4(1);
    glm::mat4 m_proj = glm::mat4(1);
    float m_fov = 45.0f;

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

    // terrain
    GLuint m_terrain_shader;
    GLuint m_terrainVao;
    GLuint m_terrainVbo;
    std::vector<GLfloat> m_terrainData;
    TerrainGenerator m_terrain;
    void bindTerrainTexture();
    QImage m_image;
    GLuint m_textureID;
    void bindTexture();
    int textureLocation;
};
