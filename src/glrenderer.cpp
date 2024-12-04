#include "glrenderer.h"

#include <QCoreApplication>
#include "src/shaderloader.h"

#include <cmath>
#include "glm/gtc/constants.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"
#include "settings.h"

GLRenderer::GLRenderer(QWidget *parent)
    : QOpenGLWidget(parent),
      m_angleX(6),
      m_angleY(0),
      m_zoom(2)
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
    glDeleteBuffers(1, &m_sphere_vbo);
    glDeleteVertexArrays(1, &m_sphere_vao);
    glDeleteProgram(m_shader);
    doneCurrent();
}

// ================== Helper Functions

glm::vec4 sphericalToCartesian(float phi, float theta)
{
    return glm::vec4(glm::sin(phi) * glm::cos(theta),
                     glm::cos(phi),                    // Y component should use cos(phi) directly
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

    // Initialize GL extension wrangler
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK)
        fprintf(stderr, "Error while initializing GLEW: %s\n", glewGetErrorString(err));
    fprintf(stdout, "Successfully initialized GLEW %s\n", glewGetString(GLEW_VERSION));

    // Set clear color to black
    glClearColor(0, 0, 0, 1);

    // Enable depth testing
    glEnable(GL_DEPTH_TEST);

    m_shader = ShaderLoader::createShaderProgram(":/resources/shaders/default.vert", ":/resources/shaders/default.frag");
    m_sunPos = glm::vec2(glm::radians(settings.elevation), glm::radians(settings.azimuth));
    // Generate and bind VBO
    glGenBuffers(1, &m_sphere_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_sphere_vbo);
    // Generate sphere data
    m_sphereData = generateDomeData(50, 50);
    m_model = glm::scale(m_model, glm::vec3(50, 50, 50));
    // Send data to VBO
    glBufferData(GL_ARRAY_BUFFER, m_sphereData.size() * sizeof(GLfloat), m_sphereData.data(), GL_STATIC_DRAW);
    // Generate, and bind vao
    glGenVertexArrays(1, &m_sphere_vao);
    glBindVertexArray(m_sphere_vao);

    // Enable and define attribute 0 to store vertex positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), reinterpret_cast<void *>(0));

    // Clean-up bindings
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void GLRenderer::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glBindVertexArray(m_sphere_vao);
    glUseProgram(m_shader);

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

    glDrawArrays(GL_TRIANGLES, 0, m_sphereData.size() / 3);

    glBindVertexArray(0);
    glUseProgram(0);

}

void GLRenderer::settingsChanged()
{
    makeCurrent();
    m_sunPos = glm::vec2(glm::radians(settings.elevation), glm::radians(settings.azimuth));
    update(); // asks for a PaintGL() call to occur
}

// ================== Other stencil code

void GLRenderer::resizeGL(int w, int h)
{
    m_proj = glm::perspective(glm::radians(45.0), 1.0 * w / h, 0.01, 100.0);
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

void GLRenderer::timerEvent(QTimerEvent *event)
{
    int elapsedms = m_elapsedTimer.elapsed();
    float deltaTime = elapsedms * 0.001f;
    m_elapsedTimer.restart();

    // Calculate normalized look direction and right vector
    glm::vec3 lookDir = glm::normalize(m_look - m_eye);
    glm::vec3 rightDir = glm::normalize(glm::cross(lookDir, m_up));

    // Calculate movement vectors
    glm::vec3 moveFront = m_translSpeed * deltaTime * lookDir;
    glm::vec3 moveRight = m_translSpeed * deltaTime * rightDir;
    glm::vec3 moveUp = m_translSpeed * deltaTime * m_up;

    // Process key movements
    for (auto &key : m_keyMap)
    {
        if (key.second)
        {
            switch (key.first)
            {
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

void GLRenderer::rebuildMatrices()
{
    // Update view matrix by rotating eye vector based on x and y angles
    m_view = glm::lookAt(m_eye, m_look, m_up);
    m_proj = glm::perspective(glm::radians(90.0), 1.0 * width() / height(), 0.01, 100.0);
    update();
}
