#version 330

in Vertex {
  vec2 texCoord;
  vec2 one;
  float mod_factor;
};
out vec4 fragColor;

uniform sampler2D source[];
uniform vec4 sourceSize[];
uniform vec4 targetSize;
uniform vec2 outputSize;

uniform vec4 LPolygonColor[];

void main()
{
	//Set fragment
	fragColor = texture(source[0], texCoord);
}
