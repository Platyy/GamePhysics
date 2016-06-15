#version 330
in vec2 textureCoords;
uniform sampler2D diffuseMap;
void main()
{
	vec2 tex = textureCoords;
	tex.y = -tex.y;
	gl_FragColor = texture( diffuseMap, tex );
}