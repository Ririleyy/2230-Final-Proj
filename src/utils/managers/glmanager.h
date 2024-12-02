#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <unordered_map>
#include <string>
#include <QImage>
#include <QCoreApplication>
#include "utils/sceneparser.h"
#include "utils/scenedata.h"

struct ShapeBuffer
{
    GLuint vao;
    GLuint vbo;
};

struct FBOConfig
{
    GLuint fbo;
    GLuint texture;
    GLuint renderBuffer;
    int width;
    int height;
};

struct TextureBuffer
{
    GLuint texture;
    QImage image;
};

void initVertexObject(ShapeBuffer &shapeBuffer, const std::vector<GLfloat> &vertexData,
                      GLsizei stride, const std::initializer_list<std::pair<GLuint, GLint>> &attributes);
void initScreenVertexObject(ShapeBuffer &shapeBuffer, const std::vector<GLfloat> &vertexData);
void initShapeVertexObject(ShapeBuffer &shapeBuffer, const std::vector<GLfloat> &vertexData);
void cleanVertexObjects(ShapeBuffer &shapeBuffer);
void verifyVAO(const std::vector<GLfloat> &triangleData, GLuint index, GLsizei size, GLsizei stride, const void *offset);
void makeFBO(FBOConfig &fboConfig);
void switchFBO(FBOConfig &fboConfig);
void deleteFBO(FBOConfig &fboConfig);
void initTexture(GLuint &texture, const QImage &image);
void cleanTexture(GLuint &texture);
