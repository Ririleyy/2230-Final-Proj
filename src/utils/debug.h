#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <iostream>
#include <vector>

namespace Debug
{
    inline void glErrorCheck(const char* fileName, int lineNumber) {
        GLenum errorNumber = glGetError();
        while (errorNumber != GL_NO_ERROR) {
            std::cout << "OpenGL error in file " << fileName << " at line " << lineNumber << ": " << errorNumber << std::endl;
            
            errorNumber = glGetError();
        }
    }
    
    inline void printMat4(const glm::mat4 &mat) {
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                std::cout << mat[j][i] << " ";
            }
            std::cout << std::endl;
        }
    }

    inline void printVec3(const glm::vec3 &vec) {
        std::cout << vec.x << " " << vec.y << " " << vec.z << std::endl;
    }

    inline void printFloatVec(const std::vector<float>& vec, int numPerLine = 3){
        std::cout << "Vector size: " << vec.size() << std::endl;
        for (size_t i = 0; i < vec.size(); i++) {
            std::cout << vec[i] << " ";
            if ((i + 1) % numPerLine == 0) {
                std::cout << std::endl;
            }
        }
        std::cout << std::endl;
    }
}
#define glErrorCheck() glErrorCheck(__FILE__, __LINE__)
