#include "utils/managers/glmanager.h"
#include "utils/debug.h"

void initVertexObject(ShapeBuffer &shapeBuffer, const std::vector<GLfloat> &vertexData,
                      GLsizei stride, const std::initializer_list<std::pair<GLuint, GLint>> &attributes)
{
    if (shapeBuffer.vbo == 0)
    {
        glGenBuffers(1, &shapeBuffer.vbo);
    }
    glBindBuffer(GL_ARRAY_BUFFER, shapeBuffer.vbo);
    glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(GLfloat), vertexData.data(), GL_STATIC_DRAW);

    if (shapeBuffer.vao == 0)
    {
        glGenVertexArrays(1, &shapeBuffer.vao);
    }
    glBindVertexArray(shapeBuffer.vao);

    GLuint offset = 0;
    for (const auto &[index, size] : attributes)
    {
        glEnableVertexAttribArray(index);
        glVertexAttribPointer(index, size, GL_FLOAT, GL_FALSE, stride * sizeof(GLfloat),
                              reinterpret_cast<void *>(offset * sizeof(GLfloat)));
        offset += size;
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

// {{0,3},{1,3}} means that the first attribute begins at index 0 and has 3 components
// and the second attribute begins at index 1 and has 3 components
void initShapeVertexObject(ShapeBuffer &shapeBuffer, const std::vector<GLfloat> &vertexData)
{
    initVertexObject(shapeBuffer, vertexData, 8, {{0, 3}, {1, 3}, {2, 2}});
}

void initScreenVertexObject(ShapeBuffer &shapeBuffer, const std::vector<GLfloat> &vertexData)
{
    initVertexObject(shapeBuffer, vertexData, 5, {{0, 3}, {1, 2}});
}

void cleanVertexObjects(ShapeBuffer &shapeBuffer)
{
    if (shapeBuffer.vbo != 0)
    {
        glDeleteBuffers(1, &shapeBuffer.vbo);
        shapeBuffer.vbo = 0;
    }
    if (shapeBuffer.vao != 0)
    {
        glDeleteVertexArrays(1, &shapeBuffer.vao);
        shapeBuffer.vao = 0;
    }
}

/**
 * @brief verifyVAO - prints in the terminal how OpenGL would interpret `triangleData` using the inputted VAO arguments
 * @param triangleData - the vector containing the triangle data
 * @param index - same as glVertexAttribPointer()
 * @param size - same as glVertexAttribPointer()
 * @param stride - same as glVertexAttribPointer()
 * @param offset - same as glVertexAttribPointer()
 */
void verifyVAO(const std::vector<GLfloat> &triangleData, GLuint index, GLsizei size, GLsizei stride, const void *offset)
{

    int newStride = int(stride / 4);
    int groupNum = 0;
    int newOffset = static_cast<int>(reinterpret_cast<intptr_t>(offset)) / 4;

    for (int i = newOffset; i < triangleData.size(); i = i + newStride)
    {
        std::cout << "Group " << groupNum << " of Values for VAO index " << index << std::endl;
        std::cout << "[";
        for (auto j = i; j < i + size; ++j)
        {
            if (j != i + size - 1)
            {
                std::cout << triangleData[j] << ", ";
            }
            else
            {
                std::cout << triangleData[j] << "]" << std::endl;
            }
        }
        groupNum = groupNum + 1;
    }
    std::cout << "" << std::endl;
}

void makeFBO(FBOConfig &fboConfig)
{

    glGenTextures(1, &fboConfig.texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, fboConfig.texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, fboConfig.width, fboConfig.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    // Generate and bind a renderbuffer of the right size, set its format, then unbind
    glGenRenderbuffers(1, &fboConfig.renderBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, fboConfig.renderBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, fboConfig.width, fboConfig.height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    // Generate and bind the FBO, then attach the texture and renderbuffer to it
    glGenFramebuffers(1, &fboConfig.fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fboConfig.fbo);

    // Add our texture as a color attachment, and our renderbuffer as a depth+stencil attachment, to FBO config
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fboConfig.texture, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, fboConfig.renderBuffer);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void switchFBO(FBOConfig &fboConfig)
{
    glBindFramebuffer(GL_FRAMEBUFFER, fboConfig.fbo);
    glViewport(0, 0, fboConfig.width, fboConfig.height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void deleteFBO(FBOConfig &fboConfig)
{
    glDeleteTextures(1, &fboConfig.texture);
    glDeleteRenderbuffers(1, &fboConfig.renderBuffer);
    glDeleteFramebuffers(1, &fboConfig.fbo);
}

void initTexture(GLuint &texture, const QImage &image)
{
    glGenTextures(1, &texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width(), image.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, image.bits());   
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void cleanTexture(GLuint &texture)
{
    glDeleteTextures(1, &texture);
    texture = 0;
}
