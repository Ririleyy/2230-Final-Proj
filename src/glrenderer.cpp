#include "glrenderer.h"

#include <QCoreApplication>
#include "src/shaderloader.h"

#include "glm/gtc/constants.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"

GLRenderer::GLRenderer(QWidget *parent)
    : QOpenGLWidget(parent),
      m_lightPos(10, 0, 0, 1),
      m_ka(0.1),
      m_kd(0.8),
      m_ks(1),
      m_shininess(15),
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

    // Task 1: call ShaderLoader::createShaderProgram with the paths to the vertex
    //         and fragment shaders. Then, store its return value in `m_shader`
    m_shader = ShaderLoader::createShaderProgram(":/resources/shaders/default.vert", ":/resources/shaders/default.frag");

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
    // Clear screen color and depth before painting
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Bind Sphere Vertex Data
    glBindVertexArray(m_sphere_vao);

    // Task 2: activate the shader program by calling glUseProgram with `m_shader`
    glUseProgram(m_shader);

    // Task 6: pass in m_model as a uniform into the shader program
    GLint modelLoc = glGetUniformLocation(m_shader, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &m_model[0][0]);

    // Task 7: pass in m_view and m_proj
    GLint viewLoc = glGetUniformLocation(m_shader, "view");
    GLint projLoc = glGetUniformLocation(m_shader, "projection");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &m_view[0][0]);
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, &m_proj[0][0]);

    // Task 12: pass m_ka into the fragment shader as a uniform
    GLint kaLoc = glGetUniformLocation(m_shader, "k_a");
    glUniform1f(kaLoc, m_ka);
    // Task 13: pass light position and m_kd into the fragment shader as a uniform
    GLint kdLoc = glGetUniformLocation(m_shader, "k_d");
    GLint lightPosLoc = glGetUniformLocation(m_shader, "lightPos");
    glUniform1f(kdLoc, m_kd);
    glUniform4fv(lightPosLoc, 1, &m_lightPos[0]);
    // Task 14: pass shininess, m_ks, and world-space camera position
    glm::vec4 cameraPos = glm::vec4(glm::inverse(m_view)[3]);

    GLint ksLoc = glGetUniformLocation(m_shader, "k_s");
    GLint shininessLoc = glGetUniformLocation(m_shader, "shininess");
    GLint camPosLoc = glGetUniformLocation(m_shader, "camPos");
    glUniform1f(ksLoc, m_ks);
    glUniform1f(shininessLoc, m_shininess);
    glUniform4fv(camPosLoc, 1, &cameraPos[0]);

    // Draw Command
    glDrawArrays(GL_TRIANGLES, 0, m_sphereData.size() / 3);
    // Unbind Vertex Array
    glBindVertexArray(0);

    // Task 3: deactivate the shader program by passing 0 into glUseProgram
    glUseProgram(0);
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
        // New right-click orbiting logic
        int posX = event->position().x();
        int posY = event->position().y();
        int deltaX = posX - m_prev_mouse_pos.x;
        int deltaY = posY - m_prev_mouse_pos.y;
        m_prev_mouse_pos = glm::vec2(posX, posY);

        // Calculate distance from eye to look point
        float distanceToLook = glm::length(m_eye - m_look);

        // Horizontal rotation (around world up vector)
        glm::vec3 rotAxisH = glm::vec3(0, 1, 0);
        glm::mat3 rotMatH = Camera::getRotationMatrix(rotAxisH, -deltaX * m_rotSpeed);

        // Vertical rotation (around perpendicular axis)
        glm::vec3 lookDir = glm::normalize(m_look - m_eye);
        glm::vec3 rightDir = glm::normalize(glm::cross(lookDir, m_up));
        glm::mat3 rotMatV = Camera::getRotationMatrix(rightDir, deltaY * m_rotSpeed);

        // Apply rotations to eye position around the look point
        glm::vec3 eyeToLook = m_eye - m_look;
        eyeToLook = rotMatV * rotMatH * eyeToLook;

        // Restore original distance
        m_eye = m_look + glm::normalize(eyeToLook) * distanceToLook;

        // Update look direction
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
    m_proj = glm::perspective(glm::radians(45.0), 1.0 * width() / height(), 0.01, 100.0);
    update();
}
