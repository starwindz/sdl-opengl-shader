#version 330

in vec4 position;
in vec2 texCoord;
out Vertex {
  vec2 texCoord;
  // Define some calculations that will be used in fragment shader.
  vec2 one;
  float mod_factor;
} vertexOut;

uniform vec4 targetSize;
uniform vec2 outputSize;
uniform vec4 sourceSize[];

void main()
{
	//Process vertex
	gl_Position = position * vec4(1.0, -1.0, 1.0, 1.0);
	vertexOut.texCoord = texCoord;	
}