#version 330

in vec3 a_Position;
in vec4 a_Color;

out vec4 v_Color;

uniform float u_Time;

const float c_PI = 3.14159;

void main()
{
	float value = fract(u_Time)*2-1;
	float rad = (value+1)*c_PI;
	float y = sin(rad);
	float x = cos(rad);

	vec4 newPosition = vec4(a_Position, 1.0f);
	newPosition.xy = newPosition.xy 
	+ fract(u_Time/4)*vec2( x,y );

	gl_Position = newPosition;
	
	v_Color = a_Color;
}
