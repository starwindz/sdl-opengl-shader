// Test program of CRT Shader for SWOS 2020
#include <stdio.h>
#include <string>
#include <fstream>
#include <streambuf>

#include <SDL.h>
#include "LTexture.h"
#include "LShaderProgram.h"

using namespace std;

// Define SDL variables
SDL_Window *m_window;
SDL_Renderer *m_renderer;

SDL_Surface *m_surfaceBackground;
SDL_Texture *m_textureBackground;
SDL_Texture *m_textureMenu;
SDL_Texture *m_textureTarget;

// Define OpenGL variables
LTexture m_glTextureBackground;
LTexture m_glTextureMenu;
LTexture m_glTextureTarget;

// Basic shader
LShaderProgram m_ShaderProgram;

// OpenGL context
SDL_GLContext m_context;

// Define window size
// -- logical
int kVgaWidth = 480;
int kVgaHeight = 270;
// -- physical
int m_windowWidth = kVgaWidth * 2;
int m_windowHeight = kVgaHeight * 2;

int useShader = 0;

#define RM_SDL    0
#define RM_OPENGL 1

#define GP_DISABLED 0
#define GP_ENABLED  1

#if (0)
int gRenderMode = RM_SDL;
#else
int gRenderMode = RM_OPENGL;
#endif

#if (1)
int gGPMode = GP_ENABLED;
#else
int gGPMode = GP_DISABLED;
#endif

// Create window
bool swosCreateWindow()
{
    bool success = true;

    if (gRenderMode == RM_SDL)
        printf("SDL rendering mode started.\n");
    else
        printf("OpenGL rendering mode started.\n");

    SDL_Init(SDL_INIT_EVERYTHING);

    if (gRenderMode == RM_SDL) {
        m_window = SDL_CreateWindow(
            "SWOS Rendering Engine Test - SDL Rendering Mode", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
            m_windowWidth, m_windowHeight, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
        );
        SDL_SetWindowSize(m_window, m_windowWidth, m_windowHeight);
        printf("SDL window created.\n");

        return success;
    }
    else {
		//Use OpenGL 3.3 core
		SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 3 );
		SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 3 );
		SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY );

        m_window = SDL_CreateWindow(
            "SWOS Rendering Engine Test - SDL Rendering Mode", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
            m_windowWidth, m_windowHeight, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
        );
        SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengl");

		if (m_window == NULL) {
			printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
			success = false;
		}
		else {
			//Create context
			m_context = SDL_GL_CreateContext( m_window );
			if( m_context == NULL ) {
				printf( "OpenGL context could not be created! SDL Error: %s\n", SDL_GetError() );
				success = false;
			}
			else {
				//Initialize GLEW
				glewExperimental = GL_TRUE;
				GLenum glewError = glewInit();
				if( glewError != GLEW_OK ) {
					printf( "Error initializing GLEW! %s\n", glewGetErrorString( glewError ) );
				}
				else {
                    printf( "GLEW successfully initialized.\n");
				}
				success = true;
			}
		}
    }

	return success;
}

bool loadGP()
{
    if (gGPMode == GP_ENABLED) {
        // Bind basic shader program
        std::string vsFn, fsFn;

        vsFn = "default.vs";
        fsFn = "default.fs";

        m_ShaderProgram.init();

        // Load shader programs
        if( !m_ShaderProgram.loadProgram(vsFn, fsFn) )
        {
            printf( "Unable to load basic shader: %s, %s\n", vsFn.c_str(), fsFn.c_str() );
            return false;
        }

        m_ShaderProgram.bind();
        printf("OpenGL shader programs loaded: %s, %s\n", vsFn.c_str(), fsFn.c_str());
    }
    return true;
}

// Create renderer
void swosCreateRenderer()
{
    if (gRenderMode == RM_SDL) {
        m_renderer = SDL_CreateRenderer(
            m_window, -1, SDL_RENDERER_ACCELERATED
        );
        SDL_RenderSetLogicalSize(m_renderer, kVgaWidth, kVgaHeight);
        printf("SDL renderer created.\n");
    }
    else {
        if (gGPMode == GP_ENABLED) {
            loadGP();
        }
    }
}

Uint32 setRGBA(Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
    return (a << 24) + (b << 16) + (g << 8) + r;
}

void getRGBA(Uint32 color, Uint8 *r, Uint8 *g, Uint8 *b, Uint8 *a)
{
    *a = (color >> 24) & 255;
    *b = (color >> 16) & 255;
    *g = (color >>  8) & 255;
    *r = (color      ) & 255;
}

void initMenuPixels(Uint32 *pixels)
{
    for (register int y = 0; y < kVgaHeight; y++) {
        for (register int x = 0; x < kVgaWidth; x++) {
            pixels[y * kVgaWidth + x] = setRGBA(0, 0, 0, 0);
        }
    }
}

void updateTestMenuPixels(Uint32 *pixels)
{
    int randNo[3];
    for (register int y = 0; y < kVgaHeight; y++) {
        for (register int x = 0; x < kVgaWidth; x++) {
            if (x >= 150/2 && x <= kVgaWidth - 150/2 && y >= 200/2 && y <= kVgaHeight - 200/2) {
                for (int i = 0; i <= 2; i++) {
                    randNo[i] = 1 + (rand() % 255);
                }
                pixels[y * kVgaWidth + x] = setRGBA(randNo[0], randNo[1], randNo[2], 255);
            }
            else {
                pixels[y * kVgaWidth + x] = setRGBA(0, 0, 0, 0);
            }
        }
    }
}

void alphablendPixels(Uint32 *src1, Uint32 *src2, Uint32 *tar, int opacity)
{
    Uint8 r1, g1, b1, a1;
    Uint8 r2, g2, b2, a2;
    Uint8 r3, g3, b3;
    Uint32 p1, p2, p3;
    int actualOpacity;

    for (register int y = 0; y < kVgaHeight; y++) {
        for (register int x = 0; x < kVgaWidth; x++) {
            p1 = src1[y * kVgaWidth + x];
            p2 = src2[y * kVgaWidth + x];

            getRGBA(p1, &r1, &g1, &b1, &a1);
            getRGBA(p2, &r2, &g2, &b2, &a2);

            if (a2 == 0) {
                actualOpacity = 0;
            }
            else {
                actualOpacity = opacity;
            }

            r3 = r1 * (100 - actualOpacity) / 100. + r2 * actualOpacity / 100.;
            g3 = g1 * (100 - actualOpacity) / 100. + g2 * actualOpacity / 100.;
            b3 = b1 * (100 - actualOpacity) / 100. + b2 * actualOpacity / 100.;

            p3 = setRGBA(r3, g3, b3, 255);
            tar[y * kVgaWidth + x] = p3;
        }
    }
}

void clearPixels(Uint32 *pixels)
{
    for (register int y = 0; y < kVgaHeight; y++) {
        for (register int x = 0; x < kVgaWidth; x++) {
#if (1)
            pixels[y * kVgaWidth + x] = setRGBA(0, 0, 0, 255);
#else
            pixels[y * kVgaWidth + x] = setRGBA(112, 144, 0, 255);
#endif
        }
    }
}

// Create textures
void swosCreateTextures()
{
    Uint32 pixelformat;
    pixelformat = SDL_PIXELFORMAT_RGBA8888;
    if (gRenderMode == RM_SDL) {
        m_surfaceBackground = SDL_LoadBMP("cnv_background.bmp");
        m_textureBackground = SDL_CreateTextureFromSurface(m_renderer, m_surfaceBackground);
        printf("SDL BackgroundTexture created.\n");

        m_textureMenu = SDL_CreateTexture(
            m_renderer, pixelformat, SDL_TEXTUREACCESS_STREAMING,
            kVgaWidth, kVgaHeight
        );
        SDL_SetTextureBlendMode(m_textureMenu, SDL_BLENDMODE_BLEND);
        printf("SDL MenuTexture created.\n");

        m_textureTarget = SDL_CreateTexture(
            m_renderer, pixelformat, SDL_TEXTUREACCESS_STREAMING,
            kVgaWidth, kVgaHeight
        );
        SDL_SetTextureBlendMode(m_textureTarget, SDL_BLENDMODE_BLEND);
        printf("SDL TargetTexture created.\n");
    }
    else {
        m_glTextureBackground.loadTextureFromBitmapFile("cnv_background.bmp", kVgaWidth, kVgaHeight);
        printf("OpenGL BackgroundTexture created.\n");

        Uint32 *pixels;
        pixels = new Uint32[ kVgaHeight * kVgaWidth ];

        clearPixels(pixels);
        m_glTextureMenu.loadTextureFromPixels32(pixels, kVgaWidth, kVgaHeight);
        printf("OpenGL MenuTexture created.\n");
        m_glTextureTarget.loadTextureFromPixels32(pixels, kVgaWidth, kVgaHeight);
        printf("OpenGL TargetTexture created.\n");
    }
}

void swosUpdateTexture()
{
    if (gRenderMode == RM_SDL) {
        Uint32 *pixels;
        int pitch;

        SDL_LockTexture(m_textureMenu, NULL, (void**)&pixels, &pitch);
        updateTestMenuPixels(pixels);
        SDL_UnlockTexture(m_textureMenu);

        delete pixels;
    }
    else {
        m_glTextureBackground.lock();
        m_glTextureMenu.lock();
        m_glTextureTarget.lock();

        Uint32 *pixelsBackground = (Uint32*) m_glTextureBackground.getPixelData32();
        Uint32 *pixelsMenu = (Uint32*) m_glTextureMenu.getPixelData32();
        Uint32 *pixelsTarget = (Uint32*) m_glTextureTarget.getPixelData32();

        updateTestMenuPixels(pixelsMenu);
        alphablendPixels(pixelsBackground, pixelsMenu, pixelsTarget, 50);

        m_glTextureTarget.unlock();
        m_glTextureMenu.unlock();
        m_glTextureBackground.unlock();
    }
}

// Update screen
void swosDoRendering()
{
    if (gRenderMode == RM_SDL) {
        SDL_SetRenderTarget(m_renderer, m_textureTarget);
        SDL_RenderCopy(m_renderer, m_textureBackground, NULL, NULL);
        SDL_RenderCopy(m_renderer, m_textureMenu, NULL, NULL);
        SDL_SetRenderTarget(m_renderer, NULL);
        SDL_RenderPresent(m_renderer);
    }
    else {
        m_ShaderProgram.createTextureLocationIDForRendering("source[0]");
        m_ShaderProgram.setTextureIDForRendering(m_glTextureTarget.getTextureID());
        m_ShaderProgram.render(kVgaWidth, kVgaHeight, 0, 0, m_windowWidth, m_windowHeight);
        SDL_GL_SwapWindow(m_window);
    }
}

void finishRendering()
{
    if (gRenderMode == RM_SDL) {
        if (m_textureBackground)
            SDL_DestroyTexture(m_textureBackground);

        if (m_textureMenu)
            SDL_DestroyTexture(m_textureMenu);

        if (m_renderer)
            SDL_DestroyRenderer(m_renderer);

        if (m_window)
            SDL_DestroyWindow(m_window);

        SDL_Quit();
        printf("SDL rendering mode terminated.\n");
    }
    else {
        m_ShaderProgram.freeProgram();
        glUseProgram(0);

        if (m_window)
            SDL_DestroyWindow(m_window);

        SDL_Quit();
        printf("SDL rendering mode terminated.\n");
    }
}

// Universal version of main
void runMain()
{
    atexit(finishRendering);

    swosCreateWindow();
    swosCreateRenderer();
    swosCreateTextures();

    // While application is running
    bool quit = false;
    SDL_Event e;
    while(!quit) {
        // Handle events on queue
        while(SDL_PollEvent(&e) != 0) {
            // User requests quit
            if (e.type == SDL_QUIT) {
                quit = true;
            }

            if (gRenderMode == RM_OPENGL) {
                if (e.type == SDL_WINDOWEVENT) {
                    if (e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                        m_windowWidth = e.window.data1;
                        m_windowHeight = e.window.data2;
                    }
                }
            }
        }

        swosUpdateTexture();
        swosDoRendering();
    }
}

int main(int argc, char* args[])
{
    runMain();
    return 0;
}
