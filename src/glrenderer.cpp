#include "glrenderer.h"

#include <QCoreApplication>
#include "src/shaderloader.h"

#include <QMouseEvent>
#include "glm/gtc/constants.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"

GLRenderer::GLRenderer(QWidget *parent)
    : QOpenGLWidget(parent),
      m_lightPos(10,0,0,1),
      m_ka(0.1),
      m_kd(0.8),
      m_ks(1),
      m_shininess(15),
      m_angleX(6),
      m_angleY(0),
      m_zoom(2)
{
    rebuildMatrices();
}

GLRenderer::~GLRenderer()
{
    makeCurrent();
    doneCurrent();
}

// ================== Helper Functions

glm::vec4 sphericalToCartesian(float phi, float theta)
{
    return glm::vec4(glm::cos(theta) * glm::sin(phi),
                     glm::sin(theta) * glm::sin(phi),
                     glm::cos(phi),1);
}

void pushVec3(glm::vec4 vec, std::vector<float>* data)
{
    data->push_back(vec.x);
    data->push_back(vec.y);
    data->push_back(vec.z);
}

// ================== Students, You'll Be Working In These Files

void GLRenderer::initializeGL()
{
    // Initialize GL extension wrangler
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK) fprintf(stderr, "Error while initializing GLEW: %s\n", glewGetErrorString(err));
    fprintf(stdout, "Successfully initialized GLEW %s\n", glewGetString(GLEW_VERSION));

    // Set clear color to black
    glClearColor(0,0,0,1);

    // Enable depth testing
    glEnable(GL_DEPTH_TEST);

    // Task 1: call ShaderLoader::createShaderProgram with the paths to the vertex
    //         and fragment shaders. Then, store its return value in `m_shader`
    m_shader = ShaderLoader::createShaderProgram(":/resources/shaders/default.vert", ":/resources/shaders/default.frag");

    // Generate and bind VBO
    glGenBuffers(1, &m_sphere_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_sphere_vbo);
    // Generate sphere data
    m_sphereData = generateSphereData(10,20);
    // Send data to VBO
    glBufferData(GL_ARRAY_BUFFER,m_sphereData.size() * sizeof(GLfloat),m_sphereData.data(), GL_STATIC_DRAW);
    // Generate, and bind vao
    glGenVertexArrays(1, &m_sphere_vao);
    glBindVertexArray(m_sphere_vao);

    // Enable and define attribute 0 to store vertex positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3 * sizeof(GLfloat),reinterpret_cast<void *>(0));

    // Clean-up bindings
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER,0);
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
    m_proj = glm::perspective(glm::radians(45.0),1.0 * w / h,0.01,100.0);
}

void GLRenderer::mousePressEvent(QMouseEvent *event) {
    // Set initial mouse position
    m_prevMousePos = event->pos();
}

void GLRenderer::mouseMoveEvent(QMouseEvent *event) {
    // Update angle member variables based on event parameters
    m_angleX += 10 * (event->position().x() - m_prevMousePos.x()) / (float) width();
    m_angleY += 10 * (event->position().y() - m_prevMousePos.y()) / (float) height();
    m_prevMousePos = event->pos();
    rebuildMatrices();
}

void GLRenderer::wheelEvent(QWheelEvent *event) {
    // Update zoom based on event parameter
    m_zoom -= event->angleDelta().y() / 100.f;
    rebuildMatrices();
}

void GLRenderer::rebuildMatrices() {
    // Update view matrix by rotating eye vector based on x and y angles
    m_view = glm::mat4(1);
    glm::mat4 rot = glm::rotate(glm::radians(-10 * m_angleX),glm::vec3(0,0,1));
    glm::vec3 eye = glm::vec3(2,0,0);
    eye = glm::vec3(rot * glm::vec4(eye,1));

    rot = glm::rotate(glm::radians(-10 * m_angleY),glm::cross(glm::vec3(0,0,1),eye));
    eye = glm::vec3(rot * glm::vec4(eye,1));

    eye = eye * m_zoom;

    m_view = glm::lookAt(eye,glm::vec3(0,0,0),glm::vec3(0,0,1));

    m_proj = glm::perspective(glm::radians(45.0),1.0 * width() / height(),0.01,100.0);

    update();
}
