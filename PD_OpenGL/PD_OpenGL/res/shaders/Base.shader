#shader vertex
#version 330 core

layout(location = 0) in vec4 position; 
layout(location = 1) in vec2 texCoord;

out vec2 v_TexCoord;
uniform mat4 _MVP;

void main()
{
	gl_Position = _MVP * position; 
	v_TexCoord = texCoord;
};

#shader fragment
#version 330 core

layout(location = 0) out vec4 color; 

in vec2 v_TexCoord;

uniform vec4 _Color;
uniform sampler2D _Texture;

void main()
{
	vec4 texColor = texture(_Texture, v_TexCoord);
	color = texColor * _Color;
	//color = _Color;
};