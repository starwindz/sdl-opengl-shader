// Modified version of source code from Lazy Foo' Productions (2004-2013)
#include "LShaderProgram.h"
#include <fstream>

LShaderProgram::LShaderProgram()
{
    mProgramID = 0; //NULL;
}

LShaderProgram::~LShaderProgram()
{
    //Free program if it exists
    freeProgram();
}

void LShaderProgram::freeProgram()
{
    if(vbo[0]) {
        glDeleteBuffers(3, &vbo[0]);
    }
    if(vao) {
        glDeleteVertexArrays(1, &vao);
        vao = 0;
    }

    //Delete program
    if (mProgramID != 0)
        glDeleteProgram( mProgramID );
}

void LShaderProgram::init()
{
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glGenBuffers(3, &vbo[0]);
}

bool LShaderProgram::bind()
{
    //Use shader
    glUseProgram( mProgramID );

    //Check for error
    GLenum error = glGetError();
    if( error != GL_NO_ERROR )
    {
        printf( "Error binding shader! %s\n", gluErrorString( error ) );
        printProgramLog( mProgramID );
        return false;
    }

    return true;
}

void LShaderProgram::unbind()
{
    //Use default program
    glUseProgram( 0 );
}

GLuint LShaderProgram::getProgramID()
{
    return mProgramID;
}

void LShaderProgram::printProgramLog( GLuint program )
{
    //Make sure name is shader
    if( glIsProgram( program ) )
    {
        //Program log length
        int infoLogLength = 0;
        int maxLength = infoLogLength;

        //Get info string length
        glGetProgramiv( program, GL_INFO_LOG_LENGTH, &maxLength );

        //Allocate string
        char* infoLog = new char[ maxLength ];

        //Get info log
        glGetProgramInfoLog( program, maxLength, &infoLogLength, infoLog );
        if( infoLogLength > 0 )
        {
            //Print Log
            printf( "%s\n", infoLog );
        }

        //Deallocate string
        delete[] infoLog;
    }
    else
    {
        printf( "Name %d is not a program\n", program );
    }
}

void LShaderProgram::printShaderLog( GLuint shader )
{
    //Make sure name is shader
    if( glIsShader( shader ) )
    {
        //Shader log length
        int infoLogLength = 0;
        int maxLength = infoLogLength;

        //Get info string length
        glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &maxLength );

        //Allocate string
        char* infoLog = new char[ maxLength ];

        //Get info log
        glGetShaderInfoLog( shader, maxLength, &infoLogLength, infoLog );
        if( infoLogLength > 0 )
        {
            //Print Log
            printf( "%s\n", infoLog );
        }

        //Deallocate string
        delete[] infoLog;
    }
    else
    {
        printf( "Name %d is not a shader\n", shader );
    }
}

GLuint LShaderProgram::loadShaderFromFile( std::string path, GLenum shaderType )
{
    //Open file
    GLuint shaderID = 0;
    std::string shaderString;
    std::ifstream sourceFile( path.c_str() );

    //Source file loaded
    if( sourceFile )
    {
        //Get shader source
        shaderString.assign( ( std::istreambuf_iterator< char >( sourceFile ) ), std::istreambuf_iterator< char >() );

        //Create shader ID
        shaderID = glCreateShader( shaderType );

        //Set shader source
        const GLchar* shaderSource = shaderString.c_str();
        glShaderSource( shaderID, 1, (const GLchar**)&shaderSource, NULL );

        //Compile shader source
        glCompileShader( shaderID );

        //Check shader for errors
        GLint shaderCompiled = GL_FALSE;
        glGetShaderiv( shaderID, GL_COMPILE_STATUS, &shaderCompiled );
        if( shaderCompiled != GL_TRUE )
        {
            printf( "Unable to compile shader %d!\n\nSource:\n%s\n", shaderID, shaderSource );
            printShaderLog( shaderID );
            glDeleteShader( shaderID );
            shaderID = 0;
        }
    }
    else
    {
        printf( "Unable to open file %s\n", path.c_str() );
    }

    return shaderID;
}

bool LShaderProgram::loadProgram(std::string vsPath, std::string fsPath)
{
    //Generate program
    mProgramID = glCreateProgram();

    //Load vertex shader
    GLuint vertexShader = loadShaderFromFile( vsPath.c_str(), GL_VERTEX_SHADER );

    //Check for errors
    if( vertexShader == 0 )
    {
        glDeleteProgram( mProgramID );
        mProgramID = 0;
        return false;
    }

    //Attach vertex shader to program
    glAttachShader( mProgramID, vertexShader );

    //Create fragment shader
    GLuint fragmentShader = loadShaderFromFile( fsPath.c_str(), GL_FRAGMENT_SHADER );

    //Check for errors
    if( fragmentShader == 0 )
    {
        glDeleteShader( vertexShader );
        glDeleteProgram( mProgramID );
        mProgramID = 0;
        return false;
    }

    //Attach fragment shader to program
    glAttachShader( mProgramID, fragmentShader );

    //Link program
    glLinkProgram( mProgramID );

    //Check for errors
    GLint programSuccess = GL_TRUE;
    glGetProgramiv( mProgramID, GL_LINK_STATUS, &programSuccess );
    if( programSuccess != GL_TRUE )
    {
        printf( "Error linking program %d!\n", mProgramID );
        printProgramLog( mProgramID );
        glDeleteShader( vertexShader );
        glDeleteShader( fragmentShader );
        glDeleteProgram( mProgramID );
        mProgramID = 0;
        return false;
    }

    //Clean up excess shader references
    glDeleteShader( vertexShader );
    glDeleteShader( fragmentShader );

    return true;
}

void glrMatrixMultiply(
    GLfloat* output,
    const GLfloat* xdata, GLint xrows, GLint xcols,
    const GLfloat* ydata, GLint yrows, GLint ycols
    )
{
  if(xcols != yrows) return;

  for(GLint y = 0; y < xrows; y++) {
    for(GLint x = 0; x < ycols; x++) {
      GLfloat sum = 0;
      for(GLint z = 0; z < xcols; z++) {
        sum += xdata[y * xcols + z] * ydata[z * ycols + x];
      }
      *output++ = sum;
    }
  }
}

void glrUniformMatrix4fv(GLint shaderProgramID, const std::string name, GLfloat* values)
{
  GLint location = glGetUniformLocation(shaderProgramID, name.c_str());
  glUniformMatrix4fv(location, 1, GL_FALSE, values);
}

void glrUniform1i(GLint shaderProgramID, const std::string name, GLint value)
{
  GLint location = glGetUniformLocation(shaderProgramID, name.c_str());
  glUniform1i(location, value);
}

void glrUniform4f(GLint shaderProgramID, const std::string name, GLfloat value0, GLfloat value1, GLfloat value2, GLfloat value3)
{
  GLint location = glGetUniformLocation(shaderProgramID, name.c_str());
  glUniform4f(location, value0, value1, value2, value3);
}

void LShaderProgram::render(GLint sourceWidth, GLint sourceHeight, GLint targetX, GLint targetY, GLint targetWidth, GLint targetHeight, GLint texID)
{
  // Set parameters
  // -- source[0]
  mTextureLocationID = glGetUniformLocation(mProgramID, "source[0]");
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texID);

  // -- targetSize, outputSize, sourceSize[0]
  glrUniform4f(mProgramID, "targetSize", targetWidth, targetHeight, 1.0 / targetWidth, 1.0 / targetHeight);
  glrUniform4f(mProgramID, "outputSize", targetWidth, targetHeight, 1.0 / targetWidth, 1.0 / targetHeight);
  glrUniform4f(mProgramID, "sourceSize[0]", sourceWidth, sourceHeight, 1.0 / sourceWidth, 1.0 / sourceHeight);

  // Actual main
  glViewport(targetX, targetY, targetWidth, targetHeight);

  float w = (float)sourceWidth / (float)sourceWidth;
  float h = (float)sourceHeight / (float)sourceHeight;

  float u = (float)targetWidth;
  float v = (float)targetHeight;

  GLfloat modelView[] = {
    1, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, 1, 0,
    0, 0, 0, 1,
  };

  GLfloat projection[] = {
     2.0f/u,  0.0f,    0.0f, 0.0f,
     0.0f,    2.0f/v,  0.0f, 0.0f,
     0.0f,    0.0f,   -1.0f, 0.0f,
    -1.0f,   -1.0f,    0.0f, 1.0f,
  };

  GLfloat modelViewProjection[4 * 4];
  glrMatrixMultiply(modelViewProjection, modelView, 4, 4, projection, 4, 4);

  GLfloat vertices[] = {
    0, 0, 0, 1,
    u, 0, 0, 1,
    0, v, 0, 1,
    u, v, 0, 1,
  };

  GLfloat positions[4 * 4];
  for(GLint n = 0; n < 16; n += 4) {
    glrMatrixMultiply(&positions[n], &vertices[n], 1, 4, modelViewProjection, 4, 4);
  }

  GLfloat texCoords[] = {
    0, 0,
    w, 0,
    0, h,
    w, h,
  };

  glrUniformMatrix4fv(mProgramID, "modelView", modelView);
  glrUniformMatrix4fv(mProgramID, "projection", projection);
  glrUniformMatrix4fv(mProgramID, "modelViewProjection", modelViewProjection);

  glBindVertexArray(vao);

  glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
  glBufferData(GL_ARRAY_BUFFER, 16 * sizeof(GLfloat), vertices, GL_STATIC_DRAW);
  GLint locationVertex = glGetAttribLocation(mProgramID, "vertex");
  glEnableVertexAttribArray(locationVertex);
  glVertexAttribPointer(locationVertex, 4, GL_FLOAT, GL_FALSE, 0, 0);

  glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
  glBufferData(GL_ARRAY_BUFFER, 16 * sizeof(GLfloat), positions, GL_STATIC_DRAW);
  GLint locationPosition = glGetAttribLocation(mProgramID, "position");
  glEnableVertexAttribArray(locationPosition);
  glVertexAttribPointer(locationPosition, 4, GL_FLOAT, GL_FALSE, 0, 0);

  glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
  glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(GLfloat), texCoords, GL_STATIC_DRAW);
  GLint locationTexCoord = glGetAttribLocation(mProgramID, "texCoord");
  glEnableVertexAttribArray(locationTexCoord);
  glVertexAttribPointer(locationTexCoord, 2, GL_FLOAT, GL_FALSE, 0, 0);

  glBindFragDataLocation(mProgramID, 0, "fragColor");
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

  glDisableVertexAttribArray(locationVertex);
  glDisableVertexAttribArray(locationPosition);
  glDisableVertexAttribArray(locationTexCoord);

  glBindVertexArray(0);
}
