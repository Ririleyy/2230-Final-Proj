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
#include "utils/terrainQueue.h"
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
    GLRenderer(QWidget* parent = nullptr);
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
    void mousePressEvent(QMouseEvent* e) override;
    void mouseMoveEvent(QMouseEvent* e) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* e) override;
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;
    void timerEvent(QTimerEvent* event) override;
    void rebuildMatrices();
    void timeToSunPos(const float time);

public slots:
    void tick(QTimerEvent* event);

private slots:
    void handleChunkReady(const TerrainGenerationQueue::ChunkData& chunk);

private:
    void initializeParticleSystem();
    void updateParticles(float deltaTime);
    void renderParticles();
    void bindTerrainVaoVbo();
    void paintTerrain();
    void paintDome();
    void sunPosToBrightness();




    // Particle system
    std::unique_ptr<ParticleSystem> m_particleSystem;
    GLuint m_particle_shader;
    GLuint m_particle_vbo;
    GLuint m_particle_vao;
    bool m_isSnow = true;
    bool m_weatherEnabled = true;

    // OpenGL objects
    GLuint m_skydome_shader;
    GLuint m_sphere_vbo;
    GLuint m_sphere_vao;
    std::vector<float> m_sphereData;
    glm::vec2 m_sunPos;

    // transformation matrices
    glm::mat4 m_view = glm::mat4(1);
    glm::mat4 m_proj = glm::mat4(1);
    float m_fov = 45.0f;

    glm::vec2 m_prev_mouse_pos;
    QElapsedTimer m_elapsedTimer;
    MouseStaus m_mouseDown = MouseStaus::NONE;
    std::unordered_map<Qt::Key, bool> m_keyMap;
    int m_timer;
    // Camera trajectory parameters
    bool m_autoRotate = false;
    float m_trajectoryRadius = 30.0f;    // Radius of circular path
    float m_trajectoryHeight = 15.0f;    // Fixed height for camera
    float m_rotationAngle = 0.0f;        // Current angle of rotation
    const float m_rotationSpeed = 0.2f;   // Speed of rotation in radians/second
    const float m_maxHeight = 45.0f;      // Maximum camera height
    const float m_minHeight = 0.5f;       // Minimum camera height
    void updateCameraPosition();
    void constrainCamera();

    // camera parameters
    float m_angleX;
    float m_angleY;
    float m_zoom;
    glm::vec3 m_eye;
    glm::vec3 m_look;
    glm::vec3 m_up;

    const float m_rotSpeed = 0.005;
    const float m_translSpeed = 15;

    // terrain
    enum class ChunkState {
        ACTIVE,
        FADING_IN,
        FADING_OUT
    };
    struct TerrainChunk {
        GLuint vao;
        GLuint vbo;
        int vertexCount;
        glm::ivec2 position; // Chunk coordinates
    };
    std::unique_ptr<TerrainGenerationQueue> m_terrainQueue;

    struct WaterPlane {
        GLuint vao, vbo;
        glm::ivec2 position;        // Same coordinate system as terrain chunks
        float alpha;                // For fade effects
        ChunkState state;          // Reuse the same state enum as terrain
        QElapsedTimer fadeTimer;    // For transitions
        int vertexCount;
    };


    std::unordered_map<int64_t, TerrainChunk> m_terrainChunks;
    glm::ivec2 m_prevCamChunk; // chunk where the camera previously sits

    void updateTerrainChunks(bool force = false);
    void addChunkIfNeeded(int x, int z, int priority, std::vector<std::pair<int, std::pair<int, int>>>& chunks);
    void createChunk(int chunkX, int chunkZ);
    int64_t getChunkKey(int chunkX, int chunkZ) {
        return (static_cast<int64_t>(chunkX) << 32) | static_cast<uint32_t>(chunkZ);
    }
    GLuint m_terrain_shader;
    GLuint m_terrainVao;
    GLuint m_terrainVbo;
    std::vector<GLfloat> m_terrainData;
    TerrainGenerator m_terrain;
    void bindTerrainTexture();
    QImage m_image;
    GLuint m_textureID; //snow
    GLuint m_textureID2;//rock
    GLuint m_textureID3;//grass
    GLuint m_textureID4;//sand
    GLuint m_textureID5;//water
    GLuint m_textureID6;//snowTop
    GLuint m_textureID7;//snowSand
    GLuint m_textureID8;//grassTop
    GLuint m_textureID9;//rockSend
    GLuint m_textureID10;//seafloor
    GLuint m_textureID11;//rockMiddle
    void bindTexture();
    int textureLocation;
    float m_brightness;


    // Water rendering related
    GLuint m_water_shader;     // Shader program for water
    GLuint m_water_disp_texture;  // Displacement map texture
    float m_water_time;           // For animation
    QImage m_disp_image;          // Store displacement map image
    int activeTexture = 0;
    std::unordered_map<int64_t, WaterPlane> m_waterPlanes;
    float m_waterLevel = 0.02f;  // Match with TerrainGenerator
    float m_waterAnimTime = 0.0f;

    // Add new water-related function declarations
    void createWaterPlane(int chunkX, int chunkZ);
    void updateWaterPlanes();
    void paintWaterPlanes();
    std::vector<float> generateWaterPlaneData(const glm::dvec2& position);

    static const int RENDER_DISTANCE = 20;        // Distance for terrain generation
    static const int WATER_RENDER_DISTANCE = 10;  // Distance for water plane generation, smaller than terrain
};
