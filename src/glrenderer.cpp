#include "glrenderer.h"
#include <QCoreApplication>
#include "src/shaderloader.h"
#include <cmath>
#include "glm/gtc/constants.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"
#include "settings.h"
#include "utils/particle.h"


GLRenderer::GLRenderer(QWidget *parent)
    : QOpenGLWidget(parent),
    m_angleX(6),
    m_angleY(0),
    m_zoom(2),
    m_particle_vao(0),
    m_particle_vbo(0),
    m_particle_shader(0)  // Initialize OpenGL handles to 0
{
    setFocusPolicy(Qt::StrongFocus);
    m_eye = glm::vec3(1, 0, 0);
    m_look = glm::vec3(-3, 0, 0);
    m_up = glm::vec3(0, 1, 0);
    m_keyMap[Qt::Key_W] = false;
    m_keyMap[Qt::Key_A] = false;
    m_keyMap[Qt::Key_S] = false;
    m_keyMap[Qt::Key_D] = false;
    m_keyMap[Qt::Key_Control] = false;
    m_keyMap[Qt::Key_Space] = false;
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
    if (m_shader) glDeleteProgram(m_shader);

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

void pushVec3(glm::vec4 vec, std::vector<float> *data)
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

// ================== Students, You'll Be Working In These Files

void GLRenderer::initializeGL()
{
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
        m_shader = ShaderLoader::createShaderProgram(":/resources/shaders/default.vert", ":/resources/shaders/default.frag");
        m_terrain_shader = ShaderLoader::createShaderProgram(":/resources/shaders/terrain.vert", ":/resources/shaders/terrain.frag");
        m_particle_shader = ShaderLoader::createShaderProgram(":/resources/shaders/particle.vert", ":/resources/shaders/particle.frag");

        // Initialize dome
        glGenBuffers(1, &m_sphere_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, m_sphere_vbo);
        m_sphereData = generateDomeData(50, 50);
        m_model = glm::scale(m_model, glm::vec3(50, 50, 50));
        glBufferData(GL_ARRAY_BUFFER, m_sphereData.size() * sizeof(GLfloat), m_sphereData.data(), GL_STATIC_DRAW);
        glGenVertexArrays(1, &m_sphere_vao);
        glBindVertexArray(m_sphere_vao);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), reinterpret_cast<void *>(0));

        // Initialize terrain
        bindTerrainVaoVbo();
        bindTerrainTexture();

        // Initialize particle system
        initializeParticleSystem();

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    } catch (const std::exception& e) {
        fprintf(stderr, "Error during initialization: %s\n", e.what());
    }
}


void GLRenderer::initializeParticleSystem() {
    m_particleSystem = std::make_unique<ParticleSystem>(10000);
    m_particleSystem->setParticleType(m_isSnow);
    m_particleSystem->setEmissionArea(100.0f, 100.0f);

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

void GLRenderer::bindTerrainVaoVbo(){

    glUseProgram(m_terrain_shader);

    // Terrain VAO and VBO setup
    glGenVertexArrays(1, &m_terrainVao);
    glBindVertexArray(m_terrainVao);

    // Generate terrain data
    m_terrainData = m_terrain.generateTerrain();
    // std::cout << "m_terrainData size: " << m_terrainData.size() << std::endl;
    // for (size_t i = 0; i < m_terrainData.size(); ++i) {
    //     std::cout << std::fixed << std::setprecision(6) << m_terrainData[i] << " ";

    //     // Add formatting for readability (e.g., print new line after every 11 floats)
    //     if ((i + 1) % 11 == 0) {
    //         std::cout << std::endl; // New line after one vertex's data (if 11 floats per vertex)
    //     }
    // }
    // std::cout << std::endl;

    // Generate and bind VBO
    glGenBuffers(1, &m_terrainVbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_terrainVbo);
    glBufferData(GL_ARRAY_BUFFER, m_terrainData.size() * sizeof(GLfloat), m_terrainData.data(), GL_STATIC_DRAW);

    // Configure vertex attributes
    glEnableVertexAttribArray(0); // Vertex position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), reinterpret_cast<void *>(0));

    glEnableVertexAttribArray(1); // Vertex normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), reinterpret_cast<void *>(3 * sizeof(GLfloat)));

    glEnableVertexAttribArray(2); // Vertex color
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), reinterpret_cast<void *>(6 * sizeof(GLfloat)));

    glEnableVertexAttribArray(3); // uv

    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat),
                          reinterpret_cast<void *>(9 * sizeof(GLfloat)));

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glUseProgram(0);

}
void GLRenderer::bindTerrainTexture(){

    glUseProgram(m_terrain_shader);

    QString kitten_filepath = QString(":/resources/images/2.jpg");
    if (!m_image.load(kitten_filepath)) {
        std::cerr << "Failed to load texture: " << kitten_filepath.toStdString() << std::endl;
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

    glUseProgram(0);

}

void GLRenderer::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Paint terrain first
    paintTerrain();

    // Paint dome
    //paintDome();

    // Paint particles last for proper transparency
    // if (m_weatherEnabled && m_particleSystem) {
    //     renderParticles();
    // }
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

void GLRenderer::timerEvent(QTimerEvent *event)
{
    int elapsedms = m_elapsedTimer.elapsed();
    float deltaTime = elapsedms * 0.001f;
    m_elapsedTimer.restart();

    // Update particles
    if (m_weatherEnabled && m_particleSystem) {
        m_particleSystem->update(deltaTime);

        glBindBuffer(GL_ARRAY_BUFFER, m_particle_vbo);
        const auto& particles = m_particleSystem->getParticles();
        glBufferSubData(GL_ARRAY_BUFFER, 0, particles.size() * sizeof(Particle), particles.data());
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    // Your existing camera movement code...
    glm::vec3 lookDir = glm::normalize(m_look - m_eye);
    glm::vec3 rightDir = glm::normalize(glm::cross(lookDir, m_up));
    glm::vec3 moveFront = m_translSpeed * deltaTime * lookDir;
    glm::vec3 moveRight = m_translSpeed * deltaTime * rightDir;
    glm::vec3 moveUp = m_translSpeed * deltaTime * m_up;

    for (auto &key : m_keyMap) {
        if (key.second) {
            switch (key.first) {
            case Qt::Key_W:
                m_eye += moveFront;
                m_look += moveFront;
                break;
            case Qt::Key_S:
                m_eye -= moveFront;
                m_look -= moveFront;
                break;
            case Qt::Key_A:
                m_eye -= moveRight;
                m_look -= moveRight;
                break;
            case Qt::Key_D:
                m_eye += moveRight;
                m_look += moveRight;
                break;
            case Qt::Key_Control:
                m_eye -= moveUp;
                m_look -= moveUp;
                break;
            case Qt::Key_Space:
                m_eye += moveUp;
                m_look += moveUp;
                break;
            default:
                break;
            }
        }
    }

    rebuildMatrices();
}

void GLRenderer::paintDome(){

    glUseProgram(m_shader);
    glBindVertexArray(m_sphere_vao);

    GLint modelLoc = glGetUniformLocation(m_shader, "model");
    GLint viewLoc = glGetUniformLocation(m_shader, "view");
    GLint projLoc = glGetUniformLocation(m_shader, "projection");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &m_model[0][0]);
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &m_view[0][0]);
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, &m_proj[0][0]);

    glm::vec4 cameraPos = glm::vec4(glm::inverse(m_view)[3]);
    GLint camPosLoc = glGetUniformLocation(m_shader, "camPos");
    glUniform4fv(camPosLoc, 1, &cameraPos[0]);

    GLint sunPosLoc = glGetUniformLocation(m_shader, "sunPosition");
    glUniform2fv(sunPosLoc, 1, &m_sunPos[0]);

    GLint tPosLoc = glGetUniformLocation(m_shader, "T");
    float turbidity = (settings.weather == WeatherType::CLEAR) ? 2.0f : 10.0f;
    glUniform1f(tPosLoc, turbidity);

    glDrawArrays(GL_TRIANGLES, 0, m_sphereData.size() / 3);

    glBindVertexArray(0);
    glUseProgram(0);

}

void GLRenderer::paintTerrain(){
    glUseProgram(m_terrain_shader);

    glBindVertexArray(m_terrainVao);

    // Set up the model, view, and projection matrices
    glm::mat4 terrainModel = glm::mat4(1.0f);
    terrainModel = glm::translate(terrainModel, glm::vec3(0.0f, -0.5f, 0.0f)); // Adjust terrain position

    terrainModel = glm::rotate(terrainModel, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

    glUniformMatrix4fv(glGetUniformLocation(m_terrain_shader, "model"), 1, GL_FALSE, &terrainModel[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(m_terrain_shader, "view"), 1, GL_FALSE, &m_view[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(m_terrain_shader, "projection"), 1, GL_FALSE, &m_proj[0][0]);


    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_textureID);
    textureLocation = glGetUniformLocation(m_terrain_shader, "texture1");
    glUniform1i(textureLocation, 0);



    int res = m_terrain.getResolution();
    glBindVertexArray(m_terrainVao);  // Bind VAO
    //glDrawArrays(GL_TRIANGLES, 0,  m_terrainData.size() / 11);  // Use correct vertex count
    glDrawArrays(GL_TRIANGLES, 0,  res*res*6);
    glBindVertexArray(0);

    glBindVertexArray(0);
    glUseProgram(0);

}

void GLRenderer::settingsChanged()
{
    makeCurrent();
    timeToSunPos(settings.time);
    if (m_particleSystem != nullptr) {
        m_weatherEnabled = (settings.weather == WeatherType::SNOW || settings.weather == WeatherType::RAIN);
        m_isSnow = settings.weather == WeatherType::SNOW;
        m_particleSystem->setParticleType(m_isSnow);
    }
    m_fov = settings.fov;
    rebuildMatrices();
    update(); // asks for a PaintGL() call to occur
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

// ================== Other stencil code

void GLRenderer::resizeGL(int w, int h)
{
    m_proj = glm::perspective(glm::radians(m_fov), 1.0f * w / h, 0.01f, 1000.0f);
}

void GLRenderer::mousePressEvent(QMouseEvent *event)
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

void GLRenderer::mouseMoveEvent(QMouseEvent *event)
{
    if (m_mouseDown == MouseStaus::LEFT)
    {
        int posX = event->position().x();
        int posY = event->position().y();
        int deltaX = posX - m_prev_mouse_pos.x;
        int deltaY = posY - m_prev_mouse_pos.y;
        m_prev_mouse_pos = glm::vec2(posX, posY);

        glm::vec3 look = m_look;
        glm::vec3 up = m_up;
        glm::vec3 rotAxisH = glm::vec3(0, 1, 0);
        glm::vec3 rotAxisV = glm::normalize(glm::cross(look, up));
        glm::mat3 rotMatH = Camera::getRotationMatrix(rotAxisH, -deltaX * m_rotSpeed);
        glm::mat3 rotMatV = Camera::getRotationMatrix(rotAxisV, -deltaY * m_rotSpeed);
        m_look = rotMatV * rotMatH * look;
        rebuildMatrices();
    }
    else if (m_mouseDown == MouseStaus::RIGHT)
    {
        int posX = event->position().x();
        int posY = event->position().y();
        int deltaX = posX - m_prev_mouse_pos.x;
        int deltaY = posY - m_prev_mouse_pos.y;
        m_prev_mouse_pos = glm::vec2(posX, posY);

        float distanceToLook = glm::length(m_eye - m_look);

        glm::vec3 rotAxisH = glm::vec3(0, 1, 0);
        glm::mat3 rotMatH = Camera::getRotationMatrix(rotAxisH, -deltaX * m_rotSpeed);

        glm::vec3 lookDir = glm::normalize(m_look - m_eye);
        glm::vec3 rightDir = glm::normalize(glm::cross(lookDir, m_up));
        glm::mat3 rotMatV = Camera::getRotationMatrix(rightDir, deltaY * m_rotSpeed);

        glm::vec3 eyeToLook = m_eye - m_look;
        eyeToLook = rotMatV * rotMatH * eyeToLook;

        m_eye = m_look + glm::normalize(eyeToLook) * distanceToLook;

        m_up = rotMatV * rotMatH * m_up;

        rebuildMatrices();
    }
}

void GLRenderer::mouseReleaseEvent(QMouseEvent *event)
{
    if (!event->buttons().testFlag(Qt::LeftButton))
    {
        m_mouseDown = MouseStaus::NONE;
    }
}

void GLRenderer::wheelEvent(QWheelEvent *event)
{
    // Update zoom based on event parameter
    m_zoom -= event->angleDelta().y() / 100.f;
    rebuildMatrices();
}

void GLRenderer::keyPressEvent(QKeyEvent *event)
{
    m_keyMap[Qt::Key(event->key())] = true;
}

void GLRenderer::keyReleaseEvent(QKeyEvent *event)
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
