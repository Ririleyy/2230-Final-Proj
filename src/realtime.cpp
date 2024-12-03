#include "realtime.h"

#include <QCoreApplication>
#include <QMouseEvent>
#include <QKeyEvent>
#include <iostream>
#include "settings.h"

#include "utils/debug.h"
#include "utils/camera.h"
#include "glm/gtc/constants.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"


// ================== Project 5: Lights, Camera

Realtime::Realtime(QWidget *parent)
    : QOpenGLWidget(parent)
{
    m_prev_mouse_pos = glm::vec2(size().width() / 2, size().height() / 2);
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);

    m_keyMap[Qt::Key_W] = false;
    m_keyMap[Qt::Key_A] = false;
    m_keyMap[Qt::Key_S] = false;
    m_keyMap[Qt::Key_D] = false;
    m_keyMap[Qt::Key_Control] = false;
    m_keyMap[Qt::Key_Space] = false;

    // If you must use this function, do not edit anything above this
}

void Realtime::finish()
{
    killTimer(m_timer);
    this->makeCurrent();

    // Students: anything requiring OpenGL calls when the program exits should be done here

    delete m_uniformMgr;
    glDeleteProgram(m_phong_shader);
    this->doneCurrent();
}

void Realtime::initializeGL()
{
    m_devicePixelRatio = this->devicePixelRatio();

    m_timer = startTimer(1000 / 60);
    m_elapsedTimer.start();

    // Initializing GL.
    // GLEW (GL Extension Wrangler) provides access to OpenGL functions.
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK)
    {
        std::cerr << "Error while initializing GL: " << glewGetErrorString(err) << std::endl;
    }
    std::cout << "Initialized GL: Version " << glewGetString(GLEW_VERSION) << std::endl;

    // Allows OpenGL to draw objects appropriately on top of one another
    glEnable(GL_DEPTH_TEST);
    // Tells OpenGL to only draw the front face
    // glEnable(GL_CULL_FACE);
    // Tells OpenGL how big the screen is
    glViewport(0, 0, size().width() * m_devicePixelRatio, size().height() * m_devicePixelRatio);

    // Students: anything requiring OpenGL calls when the program starts should be done here
    glClearColor(0, 0, 0, 1);

    m_skydome = new Dome();
    m_skydome->updateParams(settings.shapeParameter1, settings.shapeParameter2);
    initVertexObject(m_skydomeBuffer, m_skydome->generateShape());
    m_uniformMgr = new UniformManager(m_phong_shader);
    m_phong_shader = ShaderLoader::createShaderProgram(":/resources/shaders/phong.vert", ":/resources/shaders/phong.frag");
    m_skydomeModelMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(50));
    glUseProgram(0);
    Debug::glErrorCheck();
}

void Realtime::paintGL()
{
    // // Students: anything requiring OpenGL calls every frame should be done here
    paintScene();
}

void Realtime::paintScene()
{
    m_uniformMgr->setShader(m_phong_shader);

    glm::vec4 cameraPos = glm::vec4(m_invViewMatrix[3]);
    m_uniformMgr->setCameraPosUniform(cameraPos);
    m_uniformMgr->setMVPUniform(m_projMatrix, m_viewMatrix, m_skydomeModelMatrix);
    glBindVertexArray(m_skydomeBuffer.vao);
    int numVertices = m_skydome->getNumVerticies();
    glDrawArrays(GL_TRIANGLES, 0, numVertices);

    glBindVertexArray(0);
    glUseProgram(0);
}

void Realtime::resizeGL(int w, int h)
{
    // Tells OpenGL how big the screen is

    glViewport(0, 0, size().width() * m_devicePixelRatio, size().height() * m_devicePixelRatio);
    // Students: anything requiring OpenGL calls when the program starts should be done here
    m_aspectRatio = static_cast<float>(w) / h;
    m_projMatrix = Camera::getProjectionMatrix(m_heightAngle, m_aspectRatio, settings.nearPlane, settings.farPlane);
}

void Realtime::sceneChanged()
{
    makeCurrent();
    update(); // asks for a PaintGL() call to occur
}

void Realtime::settingsChanged()
{
    makeCurrent();
    m_projMatrix = Camera::getProjectionMatrix(m_heightAngle, m_aspectRatio, settings.nearPlane, settings.farPlane);
    const std::vector<float> &shapeData = m_skydome->generateShape(settings.shapeParameter1, settings.shapeParameter2);
    initShapeVertexObject(m_skydomeBuffer, shapeData);
    update(); // asks for a PaintGL() call to occur
}

// ================== Project 6: Action!

void Realtime::keyPressEvent(QKeyEvent *event)
{
    m_keyMap[Qt::Key(event->key())] = true;
}

void Realtime::keyReleaseEvent(QKeyEvent *event)
{
    m_keyMap[Qt::Key(event->key())] = false;
}

void Realtime::mousePressEvent(QMouseEvent *event)
{
    if (event->buttons().testFlag(Qt::LeftButton))
    {
        m_mouseDown = true;
        m_prev_mouse_pos = glm::vec2(event->position().x(), event->position().y());
    }
}

void Realtime::mouseReleaseEvent(QMouseEvent *event)
{
    if (!event->buttons().testFlag(Qt::LeftButton))
    {
        m_mouseDown = false;
    }
}

void Realtime::mouseMoveEvent(QMouseEvent *event)
{
    if (m_mouseDown)
    {
        int posX = event->position().x();
        int posY = event->position().y();
        int deltaX = posX - m_prev_mouse_pos.x;
        int deltaY = posY - m_prev_mouse_pos.y;
        m_prev_mouse_pos = glm::vec2(posX, posY);

        // Use deltaX and deltaY here to rotate
        glm::vec3 look = glm::vec3(m_cameraData.look);
        glm::vec3 up = glm::vec3(m_cameraData.up);
        glm::vec3 rotAxisH = glm::vec3(0, 1, 0);
        glm::vec3 rotAxisV = glm::normalize(glm::cross(look, up));
        glm::mat3 rotMatH = Camera::getRotationMatrix(rotAxisH, -deltaX * m_rotSpeed);
        glm::mat3 rotMatV = Camera::getRotationMatrix(rotAxisV, -deltaY * m_rotSpeed);
        m_cameraData.look = glm::vec4(rotMatV * rotMatH * look, 0);
        m_viewMatrix = Camera::getViewMatrix(m_cameraData);
        m_invViewMatrix = glm::inverse(m_viewMatrix);

        update(); // asks for a PaintGL() call to occur
    }
}

void Realtime::timerEvent(QTimerEvent *event)
{
    int elapsedms = m_elapsedTimer.elapsed();
    float deltaTime = elapsedms * 0.001f;
    m_elapsedTimer.restart();

    // Use deltaTime and m_keyMap here to move around
    glm::vec3 look = glm::vec3(m_cameraData.look);
    glm::vec3 up = glm::vec3(m_cameraData.up);

    glm::vec3 moveFront = m_translSpeed * deltaTime * glm::normalize(look);
    glm::vec3 moveRight = m_translSpeed * deltaTime * glm::normalize(glm::cross(look, up)); // normalize just in case
    glm::vec3 moveUp = m_translSpeed * deltaTime * glm::vec3(0, 1, 0);

    for (auto &key : m_keyMap)
    {
        if (key.second)
        {
            switch (key.first)
            {
            case Qt::Key_W:
                m_cameraData.pos += glm::vec4(moveFront, 0);
                break;
            case Qt::Key_A:
                m_cameraData.pos -= glm::vec4(moveRight, 0);
                break;
            case Qt::Key_S:
                m_cameraData.pos -= glm::vec4(moveFront, 0);
                break;
            case Qt::Key_D:
                m_cameraData.pos += glm::vec4(moveRight, 0);
                break;
            case Qt::Key_Control:
                m_cameraData.pos -= glm::vec4(moveUp, 0);
                break;
            case Qt::Key_Space:
                m_cameraData.pos += glm::vec4(moveUp, 0);
                break;
            default:
                break;
            }
        }
    }
    m_viewMatrix = Camera::getViewMatrix(cameraData);
    m_invViewMatrix = glm::inverse(m_viewMatrix);
    update(); // asks for a PaintGL() call to occur
}

// DO NOT EDIT
void Realtime::saveViewportImage(std::string filePath)
{
    // Make sure we have the right context and everything has been drawn
    makeCurrent();

    int fixedWidth = 1024;
    int fixedHeight = 768;

    // Create Frame Buffer
    GLuint fbo;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    // Create a color attachment texture
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, fixedWidth, fixedHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

    // Optional: Create a depth buffer if your rendering uses depth testing
    GLuint rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, fixedWidth, fixedHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cerr << "Error: Framebuffer is not complete!" << std::endl;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        return;
    }

    // Render to the FBO
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glViewport(0, 0, fixedWidth, fixedHeight);

    // Clear and render your scene here
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    paintGL();

    // Read pixels from framebuffer
    std::vector<unsigned char> pixels(fixedWidth * fixedHeight * 3);
    glReadPixels(0, 0, fixedWidth, fixedHeight, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());

    // Unbind the framebuffer to return to default rendering to the screen
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Convert to QImage
    QImage image(pixels.data(), fixedWidth, fixedHeight, QImage::Format_RGB888);
    QImage flippedImage = image.mirrored(); // Flip the image vertically

    // Save to file using Qt
    QString qFilePath = QString::fromStdString(filePath);
    if (!flippedImage.save(qFilePath))
    {
        std::cerr << "Failed to save image to " << filePath << std::endl;
    }

    // Clean up
    glDeleteTextures(1, &texture);
    glDeleteRenderbuffers(1, &rbo);
    glDeleteFramebuffers(1, &fbo);
}
