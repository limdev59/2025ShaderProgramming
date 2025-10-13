#version 330

in vec3 a_Position;

out vec4 v_Color;

uniform float u_Time;

void main()
{
	vec4 newPosition = vec4(a_Position, 1.0);
	gl_Position = newPosition;

	v_Color = vec4(1,1,1,1);
}
