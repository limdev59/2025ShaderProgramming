#version 330

in vec3 a_Position;

out vec4 v_Color;

uniform float u_Time;

const float c_PI = 3.14159265359;

void main()
{
	vec4 pos = vec4(a_Position,1);

    float value = a_Position.x + 0.5;  
    
    float amp = 0.3;

    float dX = 0.0;
    float dY = value*sin(2*value*c_PI - u_Time)*amp;
    float newColor = (sin(2*value*c_PI - u_Time*10)+1)*0.5 ;

    pos.y = pos.y*(1-value);
    pos += vec4(dX, dY, 0, 0);
	gl_Position = pos;

    vec2 colorFactor = (pos.xy/amp) * 0.5 + 0.5; 
    vec3 lowColor = vec3(0.0, 0.2, 0.5); 
    vec3 highColor = vec3(0.5, 0.8, 1.0); 
	v_Color = vec4(mix(lowColor, highColor, colorFactor.y), 1.0);
}