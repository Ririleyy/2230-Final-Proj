#include "realtime.h"

#include <QCoreApplication>
#include <QMouseEvent>
#include <QKeyEvent>
#include <iostream>
#include "settings.h"

#include "utils/sceneparser.h"
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
    cleanBuffers(m_primitiveBuffers);
    cleanBuffers(m_meshBuffers);
    cleanBuffers(m_LODshapeBuffers);
    cleanTexture(m_texture);

    delete m_geometryMgr;
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
    glEnable(GL_CULL_FACE);
    // Tells OpenGL how big the screen is
    glViewport(0, 0, size().width() * m_devicePixelRatio, size().height() * m_devicePixelRatio);

    // Students: anything requiring OpenGL calls when the program starts should be done here
    glClearColor(0, 0, 0, 1);

    m_geometryMgr = new GeometryManager();
    m_uniformMgr = new UniformManager(m_phong_shader);
    m_phong_shader = ShaderLoader::createShaderProgram(":/resources/shaders/phong.vert", ":/resources/shaders/phong.frag");
    m_texture_shader = ShaderLoader::createShaderProgram(":/resources/shaders/texture.vert", ":/resources/shaders/texture.frag");

    m_screen_width = size().width() * m_devicePixelRatio;
    m_screen_height = size().height() * m_devicePixelRatio;

    m_fboConfig.width = m_screen_width;
    m_fboConfig.height = m_screen_height;

    m_defaultFbo.fbo = 2;
    m_defaultFbo.width = m_screen_width;
    m_defaultFbo.height = m_screen_height;

    m_uniformMgr->setShader(m_texture_shader);
    m_uniformMgr->setTextureUniform(0);
    glUseProgram(0);

    std::vector<GLfloat> fullscreen_quad_data =
        {//   POSITIONS   ||    UVs   //
         -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
         -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
         1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
         1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
         -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
         1.0f, -1.0f, 0.0f, 1.0f, 0.0f};
    initScreenVertexObject(m_screenBuffer, fullscreen_quad_data);

    makeFBO(m_fboConfig);
    Debug::glErrorCheck();
}

void Realtime::paintGL()
{
    // // Students: anything requiring OpenGL calls every frame should be done here
    switchFBO(m_fboConfig);
    paintScene();
    switchFBO(m_defaultFbo);
    paintTexture(m_fboConfig.texture);
}

void Realtime::paintTexture(GLuint texture)
{
    // Task 32: Set your bool uniform on whether or not to filter the texture drawn
    m_uniformMgr->setShader(m_texture_shader);
    m_uniformMgr->setPostProcessUniforms(settings.perPixelFilter, settings.kernelBasedFilter, settings.extraCredit2, settings.extraCredit3);
    glBindVertexArray(m_screenBuffer.vao);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
    glUseProgram(0);
}

void Realtime::paintScene()
{
    m_uniformMgr->setShader(m_phong_shader);
    // scene-global / object-independent uniforms
    Debug::glErrorCheck();
    glm::vec4 cameraPos = glm::vec4(m_invViewMatrix[3]);
    m_uniformMgr->setCameraPosUniform(cameraPos);
    m_uniformMgr->setSceneGlobalUniform(m_renderData.globalData);

    // lights
    for (size_t i = 0; i < m_renderData.lights.size(); i++)
    {
        m_uniformMgr->setLightUniform(m_renderData.lights[i], i);
    }

    // shapes
    for (const RenderShapeData &renderShape : m_renderData.shapes)
    {
        ShapeBuffer &shapeBuffer = getShapeBuffer(renderShape);
        glBindVertexArray(shapeBuffer.vao);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_texture);
        m_uniformMgr->setMVPUniform(m_projMatrix, m_viewMatrix, renderShape.ctm);
        m_uniformMgr->setMaterialUniform(renderShape.primitive.material);
        int numVertices = m_geometryMgr->getNumVertices(renderShape, m_invViewMatrix);
        glDrawArrays(GL_TRIANGLES, 0, numVertices);
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
    glUseProgram(0);
}

void Realtime::resizeGL(int w, int h)
{
    // Tells OpenGL how big the screen is

    glViewport(0, 0, size().width() * m_devicePixelRatio, size().height() * m_devicePixelRatio);
    m_defaultFbo.height = size().height() * m_devicePixelRatio;
    m_defaultFbo.width = size().width() * m_devicePixelRatio;
    // Students: anything requiring OpenGL calls when the program starts should be done here
    m_screen_width = size().width() * m_devicePixelRatio;
    m_screen_height = size().height() * m_devicePixelRatio;
    m_fboConfig.width = m_screen_width;
    m_fboConfig.height = m_screen_height;
    m_aspectRatio = static_cast<float>(w) / h;
    m_projMatrix = Camera::getProjectionMatrix(m_heightAngle, m_aspectRatio, settings.nearPlane, settings.farPlane);
    deleteFBO(m_fboConfig);
    makeFBO(m_fboConfig);
}

void Realtime::sceneChanged()
{
    makeCurrent();
    SceneParser::parse(settings.sceneFilePath, m_renderData);
    // populateSceneData();
    // update camera-related parameters
    m_heightAngle = m_renderData.cameraData.heightAngle;
    m_viewMatrix = Camera::getViewMatrix(m_renderData.cameraData);
    m_invViewMatrix = glm::inverse(m_viewMatrix);
    m_projMatrix = Camera::getProjectionMatrix(m_heightAngle, m_aspectRatio, settings.nearPlane, settings.farPlane);

    Debug::glErrorCheck();
    cleanBuffers(m_primitiveBuffers);
    cleanBuffers(m_meshBuffers);
    cleanBuffers(m_LODshapeBuffers);
    std::unordered_set<PrimitiveType> newActivatedShapes;
    for (const RenderShapeData &shape : m_renderData.shapes)
    {
        ShapeBuffer &shapeBuffer = getShapeBuffer(shape);
        if (shape.primitive.type == PrimitiveType::PRIMITIVE_MESH)
        {
            const std::vector<float> &shapeData = m_geometryMgr->getMeshData(shape.primitive.meshfile);
            initShapeVertexObject(shapeBuffer, shapeData);
        }
        else
        {
            if (shape.primitive.material.textureMap.isUsed)
            {
                QString textfile = QString::fromStdString(shape.primitive.material.textureMap.filename);
                // QString textfile = QString(":/resources/images/kitten.png");
                m_image = QImage(textfile);
                if (m_image.isNull())
                {
                    qDebug() << "Failed to load texture:" << textfile;
                    return;
                }
                m_image = m_image.convertToFormat(QImage::Format_RGBA8888).mirrored();
                // initTexture(m_texture, m_image);
                glGenTextures(1, &m_texture);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, m_texture);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_image.width(), m_image.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, m_image.bits());   
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                glBindTexture(GL_TEXTURE_2D, 0);
                m_uniformMgr->setShader(m_phong_shader);
                m_uniformMgr->setTextImageUniform(0);
                glUseProgram(0);
                Debug::glErrorCheck();
            }
            if (!settings.extraCredit1)
            {
                const std::vector<float> &shapeData = m_geometryMgr->getUpdatedShapeParms(shape.primitive.type);
                initShapeVertexObject(shapeBuffer, shapeData);
            }
            else
            {
                LODKey key = m_geometryMgr->generateLODKey(shape, m_invViewMatrix);
                const std::vector<float> &shapeData = m_geometryMgr->getLODShapeData(key);
                initShapeVertexObject(shapeBuffer, shapeData);
            }
        }
        newActivatedShapes.insert(shape.primitive.type);
    }
    m_geometryMgr->activatedShapes = newActivatedShapes;
    // make all lights invalid
    glUseProgram(m_phong_shader);
    m_uniformMgr->clearUniforms();
    glUseProgram(0);

    Debug::glErrorCheck();
    update(); // asks for a PaintGL() call to occur
}

void Realtime::settingsChanged()
{
    makeCurrent();
    m_projMatrix = Camera::getProjectionMatrix(m_heightAngle, m_aspectRatio, settings.nearPlane, settings.farPlane);
    if (m_geometryMgr != nullptr)
    {
        Debug::glErrorCheck();
        cleanBuffers(m_LODshapeBuffers);
        cleanBuffers(m_primitiveBuffers);
        for (const RenderShapeData &shape : m_renderData.shapes)
        {
            ShapeBuffer &shapeBuffer = getShapeBuffer(shape);
            if (shape.primitive.type == PrimitiveType::PRIMITIVE_MESH)
            {
                const std::vector<float> &shapeData = m_geometryMgr->getMeshData(shape.primitive.meshfile);
                initShapeVertexObject(shapeBuffer, shapeData);
            }
            else
            {
                if (shape.primitive.material.textureMap.isUsed)
                {
                    QString meshfile = QString::fromStdString(shape.primitive.material.textureMap.filename);
                    m_image = QImage(meshfile);
                    if (m_image.isNull())
                    {
                        qDebug() << "Failed to load texture:" << meshfile;
                        return;
                    }
                    m_image = m_image.convertToFormat(QImage::Format_RGBA8888).mirrored();
                    initTexture(m_texture, m_image);
                    m_uniformMgr->setShader(m_phong_shader);
                    m_uniformMgr->setTextureUniform(0);
                    glUseProgram(0);
                }
                if (!settings.extraCredit1)
                {
                    const std::vector<float> &shapeData = m_geometryMgr->getUpdatedShapeParms(shape.primitive.type);
                    initShapeVertexObject(shapeBuffer, shapeData);
                }
                else
                {
                    LODKey key = m_geometryMgr->generateLODKey(shape, m_invViewMatrix);
                    const std::vector<float> &shapeData = m_geometryMgr->getLODShapeData(key);
                    initShapeVertexObject(shapeBuffer, shapeData);
                }
            }
        }
        Debug::glErrorCheck();
    }
    update(); // asks for a PaintGL() call to occur
}

template <typename T>
ShapeBuffer &Realtime::getBuffer(std::unordered_map<T, ShapeBuffer> &bufferMap, const T &key)
{
    if (bufferMap.find(key) == bufferMap.end())
    {
        // create a new entry in map
        bufferMap.insert({key, {0, 0}});
    }
    return bufferMap[key];
}

template <typename T>
void Realtime::cleanBuffers(std::unordered_map<T, ShapeBuffer> &bufferMap)
{
    for (auto &bufferItem : bufferMap)
    {
        cleanVertexObjects(bufferItem.second);
    }
    bufferMap.clear();
}

ShapeBuffer &Realtime::getShapeBuffer(const RenderShapeData &shape)
{
    if (shape.primitive.type == PrimitiveType::PRIMITIVE_MESH)
    {
        return getBuffer(m_meshBuffers, shape.primitive.meshfile);
    }
    else
    {
        if (!settings.extraCredit1)
        {
            return getBuffer(m_primitiveBuffers, shape.primitive.type);
        }
        LODKey key = m_geometryMgr->generateLODKey(shape, m_invViewMatrix);
        return getBuffer(m_LODshapeBuffers, key);
    }
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
        glm::vec3 look = glm::vec3(m_renderData.cameraData.look);
        glm::vec3 up = glm::vec3(m_renderData.cameraData.up);
        glm::vec3 rotAxisH = glm::vec3(0, 1, 0);
        glm::vec3 rotAxisV = glm::normalize(glm::cross(look, up));
        glm::mat3 rotMatH = Camera::getRotationMatrix(rotAxisH, -deltaX * m_rotSpeed);
        glm::mat3 rotMatV = Camera::getRotationMatrix(rotAxisV, -deltaY * m_rotSpeed);
        m_renderData.cameraData.look = glm::vec4(rotMatV * rotMatH * look, 0);
        m_viewMatrix = Camera::getViewMatrix(m_renderData.cameraData);
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
    glm::vec3 look = glm::vec3(m_renderData.cameraData.look);
    glm::vec3 up = glm::vec3(m_renderData.cameraData.up);

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
                m_renderData.cameraData.pos += glm::vec4(moveFront, 0);
                break;
            case Qt::Key_A:
                m_renderData.cameraData.pos -= glm::vec4(moveRight, 0);
                break;
            case Qt::Key_S:
                m_renderData.cameraData.pos -= glm::vec4(moveFront, 0);
                break;
            case Qt::Key_D:
                m_renderData.cameraData.pos += glm::vec4(moveRight, 0);
                break;
            case Qt::Key_Control:
                m_renderData.cameraData.pos -= glm::vec4(moveUp, 0);
                break;
            case Qt::Key_Space:
                m_renderData.cameraData.pos += glm::vec4(moveUp, 0);
                break;
            default:
                break;
            }
        }
    }
    m_viewMatrix = Camera::getViewMatrix(m_renderData.cameraData);
    m_invViewMatrix = glm::inverse(m_viewMatrix);
    if (m_geometryMgr != nullptr && settings.extraCredit1)
    {
        Debug::glErrorCheck();
        cleanBuffers(m_LODshapeBuffers);
        for (const RenderShapeData &shape : m_renderData.shapes)
        {
            ShapeBuffer &shapeBuffer = getShapeBuffer(shape);
            LODKey key = m_geometryMgr->generateLODKey(shape, m_invViewMatrix);
            const std::vector<float> &shapeData = m_geometryMgr->getLODShapeData(key);
            initShapeVertexObject(shapeBuffer, shapeData);
        }
        Debug::glErrorCheck();
    }
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

glm::vec4 sphericalToCartesian(float phi, float theta)
{
    return glm::vec4(glm::cos(theta) * glm::sin(phi),
                     glm::sin(theta) * glm::sin(phi),
                     glm::cos(phi), 1);
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
            pushVec3(p1, &data);
            pushVec3(p2, &data);
            pushVec3(p2, &data);
            pushVec3(p3, &data);
            pushVec3(p3, &data);

            pushVec3(p1, &data);
            pushVec3(p1, &data);
            pushVec3(p3, &data);
            pushVec3(p3, &data);
            pushVec3(p4, &data);
            pushVec3(p4, &data);
        }
    }

    return data;
}

void Realtime::populateSceneData()
{
    m_renderData.lights.clear();
    m_renderData.shapes.clear();

    RenderShapeData sampleShape;
    sampleShape.primitive.type = PrimitiveType::PRIMITIVE_SPHERE;
    sampleShape.primitive.material.shininess = 15;
    sampleShape.ctm = glm::mat4(1.0f);
    m_renderData.shapes.push_back(sampleShape);

    float m_angleX = 6;
    float m_angleY = 0;
    float m_zoom = 2;
    m_viewMatrix = glm::mat4(1);
    glm::mat4 rot = glm::rotate(glm::radians(-10 * m_angleX), glm::vec3(0, 0, 1));
    glm::vec3 eye = glm::vec3(2, 0, 0);
    eye = glm::vec3(rot * glm::vec4(eye, 1));

    rot = glm::rotate(glm::radians(-10 * m_angleY), glm::cross(glm::vec3(0, 0, 1), eye));
    eye = glm::vec3(rot * glm::vec4(eye, 1));

    eye = eye * m_zoom;

    m_viewMatrix = glm::lookAt(eye, glm::vec3(0, 0, 0), glm::vec3(0, 0, 1));
    m_renderData.cameraData.heightAngle = 45.0;
    m_renderData.cameraData.pos = glm::vec4(eye, 1.0f);
    m_renderData.cameraData.look = glm::vec4(glm::vec3(0, 0, 0) - eye, 0.0f);
    m_renderData.cameraData.up = glm::vec4(0, 0, 1, 0.0f);
    m_aspectRatio = 1.0 * width() / height();
    m_projMatrix = Camera::getProjectionMatrix(m_heightAngle, m_aspectRatio, settings.nearPlane, settings.farPlane);

    m_renderData.globalData.ka = 0.1;
    m_renderData.globalData.kd = 0.8;
    m_renderData.globalData.ks = 1;
    // m_lightData.pos = glm::vec4(10, 0, 0, 1);
}
