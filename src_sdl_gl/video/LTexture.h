// Modified version of source code from Lazy Foo' Productions (2004-2013)
#ifndef LTEXTURE_H
#define LTEXTURE_H

#include "LOpenGL.h"
#include <stdio.h>
#include <string.h>
#include <fstream>
#include <SDL.h>

class LTexture
{
    public:
        LTexture();
        ~LTexture();
        bool loadTextureFromPixels32( GLuint* pixels, GLuint width, GLuint height );
        bool loadTextureFromBitmapFile( std::string path, GLuint width, GLuint height );
        void freeTexture();
        void render();
        bool lock();
        bool unlock();
        GLuint* getPixelData32();
        GLuint getPixel32( GLuint x, GLuint y );
        void setPixel32( GLuint x, GLuint y, GLuint pixel );
        GLuint getTextureID();
        GLuint textureWidth();
        GLuint textureHeight();
    private:
        //Texture name
        GLuint mTextureID;

        //Texture dimensions
        GLuint mTextureWidth;
        GLuint mTextureHeight;

        //Current pixels
        GLuint* mPixels;
};

#endif
