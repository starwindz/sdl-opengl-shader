// Modified version of source code from Lazy Foo' Productions (2004-2013)
#ifndef LSHADER_PROGRAM_H
#define LSHADER_PROGRAM_H

#include "LOpenGL.h"
#include <stdio.h>
#include <string>

class LShaderProgram
{
    public:
        LShaderProgram();
        virtual ~LShaderProgram();
        bool loadProgram(std::string vsPath, std::string fsPath);
        virtual void freeProgram();
        void init();
        bool bind();
        void unbind();
        GLuint getProgramID();
        void render(GLint sourceWidth, GLint sourceHeight, GLint targetX, GLint targetY, GLint targetWidth, GLint targetHeight, GLint texID);

    protected:
        void printProgramLog( GLuint program );
        void printShaderLog( GLuint shader );
        GLuint loadShaderFromFile( std::string path, GLenum shaderType );
        GLuint mProgramID;

        unsigned int vao;
        unsigned int vbo[3];
        GLint mTextureLocationID;
};

#endif
