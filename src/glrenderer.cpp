#include "glrenderer.h"
#include <QCoreApplication>
#include "src/shaderloader.h"
#include <cmath>
#include "glm/gtc/constants.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"
#include "settings.h"
#include "utils/particle.h"


GLRenderer::GLRenderer(QWidget* parent)
    :QOpenGLWidget(parent),
    m_angleX(6),
    m_angleY(0),
    m_zoom(2),
    m_particle_vao(0),
    m_particle_vbo(0),
    m_particle_shader(0),
    // Modify these values for higher initial position
    m_trajectoryRadius(150.0f),    // Increased from 30.0f
    m_trajectoryHeight(20.0f),    // Increased from 15.0f
    m_rotationAngle(0.0f),
    // Initialize camera parameters with higher position
    m_eye(glm::vec3(m_trajectoryRadius, m_trajectoryHeight, 0)),
    m_look(glm::vec3(0, 0, 0)),
    m_up(glm::vec3(0, 1, 0))
{
    setFocusPolicy(Qt::StrongFocus);

    // Initialize key mappings
    m_keyMap[Qt::Key_W] = false;
    m_keyMap[Qt::Key_A] = false;
    m_keyMap[Qt::Key_S] = false;
    m_keyMap[Qt::Key_D] = false;
    m_keyMap[Qt::Key_Control] = false;
    m_keyMap[Qt::Key_Space] = false;
    m_keyMap[Qt::Key_R] = false;  // Add R key for auto-rotation

    rebuildMatrices();
}

GLRenderer::~GLRenderer()
{
    makeCurrent();
    killTimer(m_timer);

    // Delete terrain resources
    glDeleteBuffers(1, &m_terrainVbo);
    glDeleteVertexArrays(1, &m_terrainVao);
    if (m_terrain_shader) glDeleteProgram(m_terrain_shader);

    // Delete dome resources
    glDeleteBuffers(1, &m_sphere_vbo);
    glDeleteVertexArrays(1, &m_sphere_vao);
    if (m_skydome_shader) glDeleteProgram(m_skydome_shader);

    // Clean up water resources
    if (m_water_disp_texture) {
        glDeleteTextures(1, &m_water_disp_texture);
    }

    // Clean up water planes
    for (auto& [key, plane] : m_waterPlanes) {
        glDeleteBuffers(1, &plane.vbo);
        glDeleteVertexArrays(1, &plane.vao);
    }
    m_waterPlanes.clear();

    // Delete particle resources
    if (m_particle_vbo) glDeleteBuffers(1, &m_particle_vbo);
    if (m_particle_vao) glDeleteVertexArrays(1, &m_particle_vao);
    if (m_particle_shader) glDeleteProgram(m_particle_shader);



    doneCurrent();
}

// ================== Helper Functions

glm::vec4 sphericalToCartesian(float phi, float theta)
{
    return glm::vec4(glm::sin(phi) * glm::cos(theta),
                     glm::cos(phi), // Y component should use cos(phi) directly
                     glm::sin(phi) * glm::sin(theta),
                     1);
}

void pushVec3(glm::vec4 vec, std::vector<float>* data)
{
    data->push_back(vec.x);
    data->push_back(vec.y);
    data->push_back(vec.z);
}

std::vector<float> generateSphereData(int phiTesselations, int thetaTesselations)
{
    std::vector<float> data;

    data.clear();
    data.reserve(phiTesselations * thetaTesselations * 6 * 3);

    for (int iTheta = 0; iTheta < thetaTesselations; iTheta++)
    {
        for (int iPhi = 0; iPhi < phiTesselations; iPhi++)
        {
            float phi1 = 1.0 * iPhi / phiTesselations * glm::pi<float>();
            float phi2 = 1.0 * (iPhi + 1) / phiTesselations * glm::pi<float>();

            float the1 = 1.0 * iTheta / thetaTesselations * 2 * glm::pi<float>();
            float the2 = 1.0 * (iTheta + 1) / thetaTesselations * 2 * glm::pi<float>();

            glm::vec4 p1 = sphericalToCartesian(phi1, the1);
            glm::vec4 p2 = sphericalToCartesian(phi2, the1);
            glm::vec4 p3 = sphericalToCartesian(phi2, the2);
            glm::vec4 p4 = sphericalToCartesian(phi1, the2);

            pushVec3(p1, &data);
            pushVec3(p2, &data);
            pushVec3(p3, &data);

            pushVec3(p1, &data);
            pushVec3(p3, &data);
            pushVec3(p4, &data);
        }
    }

    return data;
}

std::vector<float> generateDomeData(int phiTesselations, int thetaTesselations)
{
    std::vector<float> data;

    data.clear();
    data.reserve(phiTesselations * thetaTesselations * 6 * 3);

    for (int iTheta = 0; iTheta < thetaTesselations; iTheta++)
    {
        for (int iPhi = 0; iPhi < phiTesselations; iPhi++)
        {
            // Changed the phi range to go from 0 to π/2 instead of 0 to π
            float phi1 = 1.0 * iPhi / phiTesselations * (glm::pi<float>() / 2.0f);
            float phi2 = 1.0 * (iPhi + 1) / phiTesselations * (glm::pi<float>() / 2.0f);

            float the1 = 1.0 * iTheta / thetaTesselations * 2 * glm::pi<float>();
            float the2 = 1.0 * (iTheta + 1) / thetaTesselations * 2 * glm::pi<float>();

            glm::vec4 p1 = sphericalToCartesian(phi1, the1);
            glm::vec4 p2 = sphericalToCartesian(phi2, the1);
            glm::vec4 p3 = sphericalToCartesian(phi2, the2);
            glm::vec4 p4 = sphericalToCartesian(phi1, the2);

            pushVec3(p1, &data);
            pushVec3(p2, &data);
            pushVec3(p3, &data);

            pushVec3(p1, &data);
            pushVec3(p3, &data);
            pushVec3(p4, &data);
        }
    }

    return data;
}

void GLRenderer::initializeGL() {
    m_timer = startTimer(1000 / 60);
    m_elapsedTimer.start();

    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK)
        fprintf(stderr, "Error while initializing GLEW: %s\n", glewGetErrorString(err));
    fprintf(stdout, "Successfully initialized GLEW %s\n", glewGetString(GLEW_VERSION));

    glClearColor(0, 0, 0, 1);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    try {
        // Initialize shaders
        m_skydome_shader = ShaderLoader::createShaderProgram(":/resources/shaders/skydome.vert", ":/resources/shaders/skydome.frag");
        m_terrain_shader = ShaderLoader::createShaderProgram(":/resources/shaders/terrain.vert", ":/resources/shaders/terrain.frag");
        m_particle_shader = ShaderLoader::createShaderProgram(":/resources/shaders/particle.vert", ":/resources/shaders/particle.frag");
        m_water_shader = ShaderLoader::createShaderProgram(":/resources/shaders/water.vert", ":/resources/shaders/water.frag");

        // Initialize dome
        glGenBuffers(1, &m_sphere_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, m_sphere_vbo);
        m_sphereData = generateDomeData(25, 25);
        glBufferData(GL_ARRAY_BUFFER, m_sphereData.size() * sizeof(GLfloat), m_sphereData.data(), GL_STATIC_DRAW);
        glGenVertexArrays(1, &m_sphere_vao);
        glBindVertexArray(m_sphere_vao);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), reinterpret_cast<void*>(0));

        // Initialize terrain
        bindTerrainVaoVbo();
        bindTerrainTexture();

        // Initialize water displacement texture
        QString disp_filepath = QString(":/resources/images/water_displacement_2.jpg");
        if (!m_disp_image.load(disp_filepath)) {
            std::cerr << "Failed to load water displacement texture" << std::endl;
            return;
        }

        m_disp_image = m_disp_image.convertToFormat(QImage::Format_RGBA8888);

        glGenTextures(1, &m_water_disp_texture);
        glBindTexture(GL_TEXTURE_2D, m_water_disp_texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
                     m_disp_image.width(), m_disp_image.height(), 0,
                     GL_RGBA, GL_UNSIGNED_BYTE, m_disp_image.bits());

        // Set texture parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glBindTexture(GL_TEXTURE_2D, 5);

        // Initialize water animation time
        m_waterAnimTime = 0.0f;

        // Initialize particle system
        initializeParticleSystem();

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    catch (const std::exception& e) {
        fprintf(stderr, "Error during initialization: %s\n", e.what());
    }
}

void GLRenderer::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Paint terrain first
    paintTerrain();

    // Paint dome
    paintDome();

    // Paint water
    // paintWater();
    paintWaterPlanes();

    // Paint particles last for proper transparency
    if (m_weatherEnabled && m_particleSystem) {
        renderParticles();
    }
}

void GLRenderer::initializeParticleSystem() {
    m_particleSystem = std::make_unique<ParticleSystem>(10000);
    m_particleSystem->setParticleType(m_isSnow);
    m_particleSystem->setEmissionArea(400.0f, 400.0f);

    glGenVertexArrays(1, &m_particle_vao);
    glGenBuffers(1, &m_particle_vbo);

    glBindVertexArray(m_particle_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_particle_vbo);

    glBufferData(GL_ARRAY_BUFFER, sizeof(Particle) * 10000, nullptr, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)offsetof(Particle, size));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)offsetof(Particle, life));
}

void GLRenderer::setWeatherType(bool isSnow) {
    if (!m_particleSystem) return;  // Guard against null pointer

    makeCurrent();  // Ensure OpenGL context is current
    m_isSnow = isSnow;
    m_particleSystem->setParticleType(isSnow);
    update();
    doneCurrent();
}

void GLRenderer::bindTerrainVaoVbo() {

    glUseProgram(m_terrain_shader);

    // Terrain VAO and VBO setup
    glGenVertexArrays(1, &m_terrainVao);
    glBindVertexArray(m_terrainVao);

    // Generate terrain data
    m_terrainData = m_terrain.generateTerrain();


    // Generate and bind VBO
    glGenBuffers(1, &m_terrainVbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_terrainVbo);
    glBufferData(GL_ARRAY_BUFFER, m_terrainData.size() * sizeof(GLfloat), m_terrainData.data(), GL_STATIC_DRAW);

    // Configure vertex attributes
    glEnableVertexAttribArray(0); // Vertex position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), reinterpret_cast<void*>(0));

    glEnableVertexAttribArray(1); // Vertex normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), reinterpret_cast<void*>(3 * sizeof(GLfloat)));

    glEnableVertexAttribArray(2); // Vertex color
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), reinterpret_cast<void*>(6 * sizeof(GLfloat)));

    glEnableVertexAttribArray(3); // uv

    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat),
                          reinterpret_cast<void*>(9 * sizeof(GLfloat)));

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glUseProgram(0);

}
void GLRenderer::bindTerrainTexture() {

    glUseProgram(m_terrain_shader);

    QString snow_filepath = QString(":/resources/images/indian-travel-destination-beautiful-attractive.jpg");
    //QString kitten_filepath = QString(":/resources/images/front-view-tree-bark.jpg");

    if (!m_image.load(snow_filepath)) {
        std::cerr << "Failed to load texture: " << snow_filepath.toStdString() << std::endl;
        return;
    }

    m_image = m_image.convertToFormat(QImage::Format_RGBA8888); // Ensure format compatibility

    // Task 2: Generate texture
    glGenTextures(1, &m_textureID);
    // Task 9: Set the active texture slot to texture slot 0
    glActiveTexture(GL_TEXTURE0);

    glBindTexture(GL_TEXTURE_2D, m_textureID);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_image.width(), m_image.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, m_image.bits());
    glGenerateMipmap(GL_TEXTURE_2D);

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);



    glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture

    // Load the second texture
    QString rock_filepath = QString(":/resources/images/front-view-tree-bark.jpg");
    if (!m_image.load(rock_filepath)) {
        std::cerr << "Failed to load texture: " << rock_filepath.toStdString() << std::endl;
        return;
    }
    m_image = m_image.convertToFormat(QImage::Format_RGBA8888);

    glGenTextures(1, &m_textureID2);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_textureID2);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_image.width(), m_image.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, m_image.bits());
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    QString grass_filepath = QString(":/resources/images/natural-landscape.jpg");
    if (!m_image.load(grass_filepath)) {
        std::cerr << "Failed to load texture: " << grass_filepath.toStdString() << std::endl;
        return;
    }
    m_image = m_image.convertToFormat(QImage::Format_RGBA8888);

    // Generate and bind texture ID for grass
    glGenTextures(1, &m_textureID3);
    glActiveTexture(GL_TEXTURE2); // Use texture unit 2 for grass texture
    glBindTexture(GL_TEXTURE_2D, m_textureID3);

    // Upload texture data to GPU
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_image.width(), m_image.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, m_image.bits());
    glGenerateMipmap(GL_TEXTURE_2D);

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Unbind texture
    glBindTexture(GL_TEXTURE_2D, 0);

    // Load sand texture
    QString sand_filepath = QString(":/resources/images/top-view-corn-flour-texture.jpg");
    if (!m_image.load(sand_filepath)) {
        std::cerr << "Failed to load texture: " << sand_filepath.toStdString() << std::endl;
        return;
    }
    m_image = m_image.convertToFormat(QImage::Format_RGBA8888);

    // Generate and bind texture ID for sand
    glGenTextures(1, &m_textureID4);
    glActiveTexture(GL_TEXTURE3); // Bind to texture unit 3
    glBindTexture(GL_TEXTURE_2D, m_textureID4);

    // Upload sand texture data to GPU
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_image.width(), m_image.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, m_image.bits());
    glGenerateMipmap(GL_TEXTURE_2D);

    // Set texture parameters for sand texture
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Unbind sand texture
    glBindTexture(GL_TEXTURE_2D, 0);

    // Load sea bottom texture
    QString water_filepath = QString(":/resources/images/water_displacement_1.jpg");
    if (!m_image.load(water_filepath)) {
        std::cerr << "Failed to load texture: " << water_filepath.toStdString() << std::endl;
        return;
    }
    m_image = m_image.convertToFormat(QImage::Format_RGBA8888);

    // Generate and bind texture ID for water
    glGenTextures(1, &m_textureID5);
    glActiveTexture(GL_TEXTURE4); // Bind to texture unit 4
    glBindTexture(GL_TEXTURE_2D, m_textureID5);

    // Upload water texture data to GPU
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_image.width(), m_image.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, m_image.bits());
    glGenerateMipmap(GL_TEXTURE_2D);

    // Set texture parameters for water texture
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Unbind water texture
    glBindTexture(GL_TEXTURE_2D, 0);



    glUseProgram(0);

}

void GLRenderer::renderParticles() {
    glUseProgram(m_particle_shader);
    glBindVertexArray(m_particle_vao);

    GLint projLoc = glGetUniformLocation(m_particle_shader, "projection");
    GLint viewLoc = glGetUniformLocation(m_particle_shader, "view");
    GLint isSnowLoc = glGetUniformLocation(m_particle_shader, "isSnow");

    glUniformMatrix4fv(projLoc, 1, GL_FALSE, &m_proj[0][0]);
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &m_view[0][0]);
    glUniform1i(isSnowLoc, m_isSnow);

    glEnable(GL_POINT_SPRITE);
    glEnable(GL_PROGRAM_POINT_SIZE);

    const auto& particles = m_particleSystem->getParticles();
    glDrawArrays(GL_POINTS, 0, particles.size());

    glDisable(GL_POINT_SPRITE);
    glDisable(GL_PROGRAM_POINT_SIZE);
}

void GLRenderer::mouseMoveEvent(QMouseEvent* event) {
    if (m_mouseDown == MouseStaus::LEFT) {
        int posX = event->position().x();
        int posY = event->position().y();
        int deltaX = posX - m_prev_mouse_pos.x;
        int deltaY = posY - m_prev_mouse_pos.y;
        m_prev_mouse_pos = glm::vec2(posX, posY);

        // First rotate around Y axis (yaw)
        glm::vec3 dir = m_look - m_eye;
        glm::mat3 rotY = Camera::getRotationMatrix(glm::vec3(0, 1, 0), -deltaX * m_rotSpeed);
        dir = rotY * dir;

        // Then rotate around local right axis (pitch)
        glm::vec3 right = glm::normalize(glm::cross(dir, m_up));
        glm::mat3 rotX = Camera::getRotationMatrix(right, -deltaY * m_rotSpeed);
        dir = rotX * dir;

        // Update look position and up vector
        m_look = m_eye + dir;

        rebuildMatrices();
    }
    else if (m_mouseDown == MouseStaus::RIGHT) {
        int posX = event->position().x();
        int posY = event->position().y();
        int deltaX = posX - m_prev_mouse_pos.x;
        int deltaY = posY - m_prev_mouse_pos.y;
        m_prev_mouse_pos = glm::vec2(posX, posY);

        // Get the current view direction and distance
        glm::vec3 dir = m_look - m_eye;
        float distance = glm::length(dir);
        dir = glm::normalize(dir);

        // Rotate around Y axis (yaw)
        glm::mat3 rotY = Camera::getRotationMatrix(glm::vec3(0, 1, 0), -deltaX * m_rotSpeed);
        dir = rotY * dir;

        // Rotate around local right axis (pitch)
        glm::vec3 right = glm::normalize(glm::cross(dir, m_up));
        glm::mat3 rotX = Camera::getRotationMatrix(right, deltaY * m_rotSpeed);
        dir = rotX * dir;

        // Update camera position, maintaining the same distance from look point
        m_eye = m_look - dir * distance;

        // Update up vector
        m_up = glm::normalize(glm::cross(right, dir));

        // Apply constraints
        constrainCamera();

        rebuildMatrices();
    }
}

void GLRenderer::constrainCamera() {
    const float maxHeight = 100.0f;
    const float minHeight = 1.0f;

    if (m_eye.y > maxHeight) {
        float adjustment = m_eye.y - maxHeight;
        m_eye.y = maxHeight;
        m_look.y -= adjustment;
    }
    else if (m_eye.y < minHeight) {
        float adjustment = minHeight - m_eye.y;
        m_eye.y = minHeight;
        m_look.y += adjustment;
    }
}

// Fix the auto-rotation functionality in updateCameraPosition():
void GLRenderer::updateCameraPosition() {
    if (!m_autoRotate) return;

    float deltaTime = m_elapsedTimer.elapsed() * 0.001f; // Convert to seconds
    m_elapsedTimer.restart();

    // Update rotation angle
    m_rotationAngle += m_rotationSpeed * deltaTime;

    // Calculate new camera position on circular path
    m_eye.x = m_trajectoryRadius * cos(m_rotationAngle);
    m_eye.z = m_trajectoryRadius * sin(m_rotationAngle);
    m_eye.y = m_trajectoryHeight;

    // Keep camera looking at center
    m_look = glm::vec3(0.0f, 0.0f, 0.0f);

    // Maintain up vector
    m_up = glm::vec3(0.0f, 1.0f, 0.0f);

    // Update view matrix
    rebuildMatrices();
}


void GLRenderer::timerEvent(QTimerEvent* event) {
    float deltaTime = m_elapsedTimer.elapsed() * 0.001f;  // Convert to seconds

    // Handle particle system updates
    if (m_weatherEnabled && m_particleSystem) {
        m_particleSystem->update(deltaTime);
        glBindBuffer(GL_ARRAY_BUFFER, m_particle_vbo);
        const auto& particles = m_particleSystem->getParticles();
        glBufferSubData(GL_ARRAY_BUFFER, 0, particles.size() * sizeof(Particle), particles.data());
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    if (m_autoRotate) {
        updateCameraPosition();
    }
    else {
        // Manual WASD movement
        // Calculate movement vectors
        glm::vec3 lookDir = glm::normalize(m_look - m_eye);
        glm::vec3 rightDir = glm::normalize(glm::cross(lookDir, m_up));
        glm::vec3 moveFront = m_translSpeed * deltaTime * glm::vec3(lookDir.x, 0, lookDir.z); // Keep y movement separate
        glm::vec3 moveRight = m_translSpeed * deltaTime * rightDir;
        glm::vec3 moveUp = m_translSpeed * deltaTime * m_up;

        // Store original positions
        glm::vec3 originalEye = m_eye;
        glm::vec3 originalLook = m_look;

        // Apply movements
        bool moved = false;
        for (const auto& [key, isPressed] : m_keyMap) {
            if (isPressed) {
                moved = true;
                glm::vec3 movement(0.0f);
                switch (key) {
                case Qt::Key_W:
                    movement = moveFront;
                    break;
                case Qt::Key_S:
                    movement = -moveFront;
                    break;
                case Qt::Key_A:
                    movement = -moveRight;
                    break;
                case Qt::Key_D:
                    movement = moveRight;
                    break;
                case Qt::Key_Control:
                    m_eye.y -= moveUp.y;
                    m_look.y -= moveUp.y;
                    break;
                case Qt::Key_Space:
                    m_eye.y += moveUp.y;
                    m_look.y += moveUp.y;
                    break;
                }

                if (movement != glm::vec3(0.0f)) {
                    m_eye += movement;
                    m_look += movement;
                }
            }
        }

        if (moved) {
            // Apply constraints after movement
            constrainCamera();
            rebuildMatrices();
        }
    }

    m_elapsedTimer.restart();
    update();
}


void GLRenderer::paintDome() {

    glUseProgram(m_skydome_shader);
    glBindVertexArray(m_sphere_vao);

    glm::mat4 domeModel = glm::mat4(1.0f);
    domeModel = glm::translate(domeModel, glm::vec3(m_eye.x, 0, m_eye.z));
    domeModel = glm::scale(domeModel, glm::vec3(200, 200, 200));

    GLint modelLoc = glGetUniformLocation(m_skydome_shader, "model");
    GLint viewLoc = glGetUniformLocation(m_skydome_shader, "view");
    GLint projLoc = glGetUniformLocation(m_skydome_shader, "projection");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &domeModel[0][0]);
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &m_view[0][0]);
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, &m_proj[0][0]);

    glm::vec4 cameraPos = glm::vec4(glm::inverse(m_view)[3]);
    GLint camPosLoc = glGetUniformLocation(m_skydome_shader, "camPos");
    glUniform4fv(camPosLoc, 1, &cameraPos[0]);

    GLint sunPosLoc = glGetUniformLocation(m_skydome_shader, "sunPosition");
    glUniform2fv(sunPosLoc, 1, &m_sunPos[0]);

    GLint tPosLoc = glGetUniformLocation(m_skydome_shader, "T");
    float turbidity = (settings.weather == WeatherType::CLEAR) ? 2.0f : 10.0f;
    glUniform1f(tPosLoc, turbidity);

    glDrawArrays(GL_TRIANGLES, 0, m_sphereData.size() / 3);

    glBindVertexArray(0);
    glUseProgram(0);

}


void GLRenderer::paintTerrain() {

    glUseProgram(m_terrain_shader);
    updateTerrainChunks();

    // Render all visible chunks
    for (auto& [key, chunk] : m_terrainChunks) {
        if (chunk.state == ChunkState::FADING_IN) {
            float fadeDuration = 2000.0f; // 2 seconds fade-in duration
            chunk.alpha = std::min(chunk.fadeTimer.elapsed() / fadeDuration, 1.0f);

            if (chunk.alpha >= 1.0f) {
                chunk.state = ChunkState::ACTIVE; // Fully visible
            }
        }else if (chunk.state == ChunkState::FADING_OUT) {
            float fadeDuration = 2000.0f; // 2 seconds fade-out duration
            chunk.alpha = std::max(1.0f - (chunk.fadeTimer.elapsed() / fadeDuration), 0.0f);

            if (chunk.alpha <= 0.0f) {
                continue; // Skip rendering fully transparent chunks
            }
        }

        glBindVertexArray(chunk.vao);
        glm::mat4 model(1.0);
        glUniformMatrix4fv(glGetUniformLocation(m_terrain_shader, "model"), 1, GL_FALSE, &model[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(m_terrain_shader, "view"), 1, GL_FALSE, &m_view[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(m_terrain_shader, "projection"), 1, GL_FALSE, &m_proj[0][0]);

        glUniform1f(glGetUniformLocation(m_terrain_shader, "transitionWidth"), 0.1f);



        // Bind texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_textureID);
        glUniform1i(glGetUniformLocation(m_terrain_shader, "texture1"), 0);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, m_textureID2);
        glUniform1i(glGetUniformLocation(m_terrain_shader, "texture2"), 1);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, m_textureID3);
        glUniform1i(glGetUniformLocation(m_terrain_shader, "texture3"), 2);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, m_textureID4);
        glUniform1i(glGetUniformLocation(m_terrain_shader, "texture4"), 3);
        // glActiveTexture(GL_TEXTURE4);
        // glBindTexture(GL_TEXTURE_2D, m_textureID5);
        // glUniform1i(glGetUniformLocation(m_terrain_shader, "texture5"), 4);

        glUniform1f(glGetUniformLocation(m_terrain_shader, "brightness"), m_brightness);
        glUniform1f(glGetUniformLocation(m_terrain_shader, "minBrightness"), 0.3f); // Set minimum brightness



        // Pass alpha to shader
        glUniform1f(glGetUniformLocation(m_terrain_shader, "alpha"), chunk.alpha);

        if(settings.mountain == MountainType::SNOW_MOUNTAIN){
            activeTexture = 0;
        }else if(settings.mountain==MountainType::ROCK_MOUNTAIN){
            activeTexture = 1;
        }else if(settings.mountain==MountainType::GRASS_MOUNTAIN){
            activeTexture = 2;
        }

        glUniform1i(glGetUniformLocation(m_terrain_shader, "activeTexture"), activeTexture);
        glDrawArrays(GL_TRIANGLES, 0, chunk.vertexCount);
    }

    glBindVertexArray(0);
    glUseProgram(0);
}

void GLRenderer::settingsChanged() {
    makeCurrent();

    // Check if this is a weather type change
    if (m_particleSystem != nullptr &&
        ((settings.weather == WeatherType::SNOW || settings.weather == WeatherType::RAIN) != m_weatherEnabled ||
         (settings.weather == WeatherType::SNOW) != m_isSnow)) {

        m_weatherEnabled = (settings.weather == WeatherType::SNOW || settings.weather == WeatherType::RAIN);
        m_isSnow = settings.weather == WeatherType::SNOW;
        m_particleSystem->setParticleType(m_isSnow);
    }




    // Update time and view-related settings
    timeToSunPos(settings.time);
    sunPosToBrightness();
    m_fov = settings.fov;

    rebuildMatrices();
    update();
    doneCurrent();
}

void GLRenderer::timeToSunPos(const float time)
{
    float azimuth, zenith;
    if (time >= 0 && time <= 12)
    {
        azimuth = glm::radians(0.0f);
        zenith = glm::radians(180.0f - 15.0f * time);
    }
    else
    {
        azimuth = glm::radians(180.0f);
        zenith = glm::radians(15.0f * (time - 12));
    }
    zenith = glm::min(zenith, glm::radians(130.0f));
    // std::cout << "Time: " << time << " Zenith: " << glm::degrees(zenith) << " Azimuth: " << glm::degrees(azimuth) << std::endl;
    m_sunPos = glm::vec2(azimuth, zenith);
}

void GLRenderer::sunPosToBrightness()
{
    float zenith = m_sunPos.y;
    float dynamicBrightness = glm::cos(zenith);
    dynamicBrightness = glm::clamp(dynamicBrightness, 0.0f, 1.0f); // Clamp between 0 and 1
    float minBrightness = 0.3f; // 30% brightness
    m_brightness = glm::mix(minBrightness, 1.0f, dynamicBrightness);

}

// ================== Other stencil code

void GLRenderer::resizeGL(int w, int h)
{
    m_proj = glm::perspective(glm::radians(m_fov), 1.0f * w / h, 0.01f, 1000.0f);
}

void GLRenderer::mousePressEvent(QMouseEvent* event)
{
    // Set initial mouse position
    if (event->buttons().testFlag(Qt::LeftButton))
    {
        m_mouseDown = MouseStaus::LEFT;
        m_prev_mouse_pos = glm::vec2(event->position().x(), event->position().y());
    }
    else if (event->buttons().testFlag(Qt::RightButton))
    {
        m_mouseDown = MouseStaus::RIGHT;
        m_prev_mouse_pos = glm::vec2(event->position().x(), event->position().y());
    }
}

// void GLRenderer::mouseMoveEvent(QMouseEvent *event) {
//     // Update angle member variables based on event parameters
//     m_angleX += 10 * (event->position().x() - m_prevMousePos.x()) / (float) width();
//     m_angleY += 10 * (event->position().y() - m_prevMousePos.y()) / (float) height();
//     m_prevMousePos = event->pos();
//     rebuildMatrices();
// }


void GLRenderer::mouseReleaseEvent(QMouseEvent* event)
{
    if (!event->buttons().testFlag(Qt::LeftButton))
    {
        m_mouseDown = MouseStaus::NONE;
    }
}

void GLRenderer::wheelEvent(QWheelEvent* event)
{
    // Update zoom based on event parameter
    m_zoom -= event->angleDelta().y() / 100.f;
    rebuildMatrices();
}

void GLRenderer::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_R && !event->isAutoRepeat()) {
        m_autoRotate = !m_autoRotate;
        if (m_autoRotate) {
            // Clear any active key states when entering auto-rotate
            for (auto& [key, isPressed] : m_keyMap) {
                isPressed = false;
            }
            // Reset position and angle
            m_rotationAngle = 0.0f;
            m_eye = glm::vec3(m_trajectoryRadius, m_trajectoryHeight, 0.0f);
            m_look = glm::vec3(0.0f, 0.0f, 0.0f);
            m_up = glm::vec3(0.0f, 1.0f, 0.0f);
            m_elapsedTimer.restart();
        }
    }
    m_keyMap[Qt::Key(event->key())] = true;
}

void GLRenderer::keyReleaseEvent(QKeyEvent* event)
{
    m_keyMap[Qt::Key(event->key())] = false;
}

void GLRenderer::rebuildMatrices()
{
    // Update view matrix by rotating eye vector based on x and y angles
    m_view = glm::lookAt(m_eye, m_look, m_up);
    m_proj = glm::perspective(glm::radians(m_fov), 1.0f * width() / height(), 0.01f, 1000.0f);
    update();
}

void GLRenderer::updateTerrainChunks() {
    // Calculate current chunk position based on camera position
    int currentChunkX = static_cast<int>(m_eye.x / TerrainGenerator::CHUNK_SIZE);
    int currentChunkZ = static_cast<int>(m_eye.z / TerrainGenerator::CHUNK_SIZE);

    // Mark chunks for fading out if they are out of range
    for (auto& [key, chunk] : m_terrainChunks) {
        int chunkX = chunk.position.x;
        int chunkZ = chunk.position.y;

        if (chunk.state != ChunkState::FADING_OUT &&
            (abs(chunkX - currentChunkX) > RENDER_DISTANCE ||
             abs(chunkZ - currentChunkZ) > RENDER_DISTANCE)) {
            chunk.state = ChunkState::FADING_OUT;
            chunk.fadeTimer.restart();
        }
    }

    // Create new terrain chunks that are in range
    for (int x = -RENDER_DISTANCE; x <= RENDER_DISTANCE; x++) {
        for (int z = -RENDER_DISTANCE; z <= RENDER_DISTANCE; z++) {
            int chunkX = currentChunkX + x;
            int chunkZ = currentChunkZ + z;
            int64_t key = getChunkKey(chunkX, chunkZ);

            if (m_terrainChunks.find(key) == m_terrainChunks.end()) {
                createChunk(chunkX, chunkZ);
            }
        }
    }

    // Remove far chunks
    for (auto it = m_terrainChunks.begin(); it != m_terrainChunks.end();) {
        if (it->second.state == ChunkState::FADING_OUT &&
            it->second.fadeTimer.elapsed() > 2000) {
            glDeleteBuffers(1, &it->second.vbo);
            glDeleteVertexArrays(1, &it->second.vao);
            it = m_terrainChunks.erase(it);
        } else {
            ++it;
        }
    }

    // Update terrain fade states
    for (auto& [key, chunk] : m_terrainChunks) {
        if (chunk.state == ChunkState::FADING_IN) {
            float fadeDuration = 2000.0f;
            chunk.alpha = std::min(chunk.fadeTimer.elapsed() / fadeDuration, 1.0f);

            if (chunk.alpha >= 1.0f) {
                chunk.state = ChunkState::ACTIVE;
            }
        }
    }

    // Update water planes
    // Mark water planes for removal if they're too far
    for (auto& [key, plane] : m_waterPlanes) {
        int chunkX = plane.position.x;
        int chunkZ = plane.position.y;

        if (plane.state != ChunkState::FADING_OUT &&
            (abs(chunkX - currentChunkX) > WATER_RENDER_DISTANCE ||
             abs(chunkZ - currentChunkZ) > WATER_RENDER_DISTANCE)) {
            plane.state = ChunkState::FADING_OUT;
            plane.fadeTimer.restart();
        }
    }

    // Create new water planes for all chunks within render distance
    for (int x = -WATER_RENDER_DISTANCE; x <= WATER_RENDER_DISTANCE; x++) {
        for (int z = -WATER_RENDER_DISTANCE; z <= WATER_RENDER_DISTANCE; z++) {
            int chunkX = currentChunkX + x;
            int chunkZ = currentChunkZ + z;
            int64_t key = getChunkKey(chunkX, chunkZ);

            if (m_waterPlanes.find(key) == m_waterPlanes.end()) {
                createWaterPlane(chunkX, chunkZ);
            }
        }
    }

    // Remove far water planes
    for (auto it = m_waterPlanes.begin(); it != m_waterPlanes.end();) {
        if (it->second.state == ChunkState::FADING_OUT &&
            it->second.fadeTimer.elapsed() > 2000) {
            glDeleteBuffers(1, &it->second.vbo);
            glDeleteVertexArrays(1, &it->second.vao);
            it = m_waterPlanes.erase(it);
        } else {
            ++it;
        }
    }
}

void GLRenderer::createChunk(int chunkX, int chunkZ) {
    TerrainChunk chunk;
    chunk.position = glm::ivec2(chunkX, chunkZ);
    chunk.alpha = m_brightness;
    chunk.fadeTimer.start();
    chunk.state = ChunkState::FADING_IN;

    // Generate terrain data for this chunk
    std::vector<float> terrainData = m_terrain.generateTerrainChunk(chunkX, chunkZ);
    chunk.vertexCount = terrainData.size() / 11;

    glGenVertexArrays(1, &chunk.vao);
    glGenBuffers(1, &chunk.vbo);

    glBindVertexArray(chunk.vao);
    glBindBuffer(GL_ARRAY_BUFFER, chunk.vbo);
    glBufferData(GL_ARRAY_BUFFER, terrainData.size() * sizeof(float), terrainData.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), 0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float)));

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(9 * sizeof(float)));

    m_terrainChunks[getChunkKey(chunkX, chunkZ)] = chunk;
}


void GLRenderer::paintWaterPlanes() {
    glUseProgram(m_water_shader);

    // Enable blending for transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Update animation time
    m_waterAnimTime += 0.01f;

    // Calculate effective brightness for water (same as terrain)
    float effectiveBrightness = std::max(m_brightness, 0.3f);  // Using same minimum brightness as terrain

    // Set all uniform values
    glUniform1f(glGetUniformLocation(m_water_shader, "time"), m_waterAnimTime);
    glUniform1f(glGetUniformLocation(m_water_shader, "dispStrength"), 0.5f);
    glUniform1f(glGetUniformLocation(m_water_shader, "brightness"), effectiveBrightness);
    glUniform1f(glGetUniformLocation(m_water_shader, "minBrightness"), 0.3f);

    // Create and set model matrix with water level
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, m_waterLevel, 0.0f));
    glUniformMatrix4fv(glGetUniformLocation(m_water_shader, "model"), 1, GL_FALSE, &model[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(m_water_shader, "view"), 1, GL_FALSE, &m_view[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(m_water_shader, "projection"), 1, GL_FALSE, &m_proj[0][0]);

    // Set water displacement texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_water_disp_texture);
    glUniform1i(glGetUniformLocation(m_water_shader, "dispTexture"), 0);

    // Render all water planes
    for (const auto& [key, plane] : m_waterPlanes) {
        if (plane.state == ChunkState::FADING_OUT && plane.fadeTimer.elapsed() > 2000) {
            continue;
        }

        // Calculate alpha for fading effect
        float alpha = plane.state == ChunkState::FADING_IN ?
                          std::min(plane.fadeTimer.elapsed() / 2000.0f, 1.0f) :
                          std::max(1.0f - plane.fadeTimer.elapsed() / 2000.0f, 0.0f);

        glUniform1f(glGetUniformLocation(m_water_shader, "alpha"), alpha);

        glBindVertexArray(plane.vao);
        glDrawArrays(GL_TRIANGLES, 0, plane.vertexCount);
    }

    // Clean up state
    glDisable(GL_BLEND);
    glBindVertexArray(0);
    glUseProgram(0);
}

void GLRenderer::createWaterPlane(int chunkX, int chunkZ) {
    int64_t key = getChunkKey(chunkX, chunkZ);
    if (m_waterPlanes.find(key) != m_waterPlanes.end()) {
        return;
    }

    // Initialize water plane data
    WaterPlane plane;
    plane.position = glm::ivec2(chunkX, chunkZ);
    plane.alpha = 0.0f;
    plane.fadeTimer.start();
    plane.state = ChunkState::FADING_IN;

    // Calculate world position for this chunk
    glm::vec2 worldPos = glm::vec2(
        static_cast<float>(chunkX) * TerrainGenerator::CHUNK_SIZE,
        static_cast<float>(chunkZ) * TerrainGenerator::CHUNK_SIZE
        );

    // Generate geometry data
    std::vector<float> waterData = generateWaterPlaneData(worldPos);
    plane.vertexCount = waterData.size() / 5;

    // Create and setup OpenGL buffers
    glGenVertexArrays(1, &plane.vao);
    glGenBuffers(1, &plane.vbo);

    glBindVertexArray(plane.vao);
    glBindBuffer(GL_ARRAY_BUFFER, plane.vbo);
    glBufferData(GL_ARRAY_BUFFER, waterData.size() * sizeof(float), waterData.data(), GL_STATIC_DRAW);

    // Setup vertex attributes
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    m_waterPlanes[key] = plane;
}

std::vector<float> GLRenderer::generateWaterPlaneData(const glm::vec2& position) {
    std::vector<float> data;
    float chunkSize = TerrainGenerator::CHUNK_SIZE;
    int verticesPerSide = static_cast<int>(chunkSize / TerrainGenerator::VERTEX_SPACING);

    // Pre-allocate memory for better performance
    data.reserve(verticesPerSide * verticesPerSide * 30);

    for (int x = 0; x < verticesPerSide; x++) {
        for (int z = 0; z < verticesPerSide; z++) {
            // Calculate exact world positions
            float x1 = position.x + x * TerrainGenerator::VERTEX_SPACING;
            float x2 = position.x + (x + 1) * TerrainGenerator::VERTEX_SPACING;
            float z1 = position.y + z * TerrainGenerator::VERTEX_SPACING;
            float z2 = position.y + (z + 1) * TerrainGenerator::VERTEX_SPACING;

            // Calculate UV coordinates
            float u1 = static_cast<float>(x) / verticesPerSide;
            float u2 = static_cast<float>(x + 1) / verticesPerSide;
            float v1 = static_cast<float>(z) / verticesPerSide;
            float v2 = static_cast<float>(z + 1) / verticesPerSide;

            // First triangle
            data.push_back(x1);
            data.push_back(0.0f);  // Water height will be set by model matrix
            data.push_back(z1);
            data.push_back(u1);
            data.push_back(v1);

            data.push_back(x2);
            data.push_back(0.0f);
            data.push_back(z1);
            data.push_back(u2);
            data.push_back(v1);

            data.push_back(x2);
            data.push_back(0.0f);
            data.push_back(z2);
            data.push_back(u2);
            data.push_back(v2);

            // Second triangle
            data.push_back(x1);
            data.push_back(0.0f);
            data.push_back(z1);
            data.push_back(u1);
            data.push_back(v1);

            data.push_back(x2);
            data.push_back(0.0f);
            data.push_back(z2);
            data.push_back(u2);
            data.push_back(v2);

            data.push_back(x1);
            data.push_back(0.0f);
            data.push_back(z2);
            data.push_back(u1);
            data.push_back(v2);
        }
    }

    return data;
}
