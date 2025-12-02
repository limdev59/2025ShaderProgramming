#version 330 core

layout(location=0) out vec4 FragColor;

in vec4 v_Color; 
uniform float u_Time;

void main()
{    
    FragColor = v_Color;
}