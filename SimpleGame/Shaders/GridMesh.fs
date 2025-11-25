#version 330 core

layout(location=0) out vec4 FragColor;

in vec2 v_UV;
in vec4 v_Color; 
uniform float u_Time;
uniform sampler2D u_Texture;

void main()
{
    vec4 sampledColor = texture(u_Texture, v_UV);
    
    FragColor = sampledColor;
}