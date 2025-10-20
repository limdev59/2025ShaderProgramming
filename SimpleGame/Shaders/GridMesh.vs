#version 330

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec3 a_Normal;
layout (location = 2) in vec2 a_UV;

out vec4 v_Color;
out vec3 v_local_position;
out vec3 v_world_position;
out vec3 v_normal;
out vec2 v_uv;


uniform float u_Time;


const float c_RENDER_DISTANCE = 10.0;
void WaveMinecraft()
{
    gl_Position = vec4(a_Position, 1.0);

    float distance = length(a_Position.xy);
    float FREQ = 30.0;
    float SPEED = 3.0;

    float colorFactor = (sin(distance * FREQ - u_Time * SPEED) + 1.0) / 2.0;
    vec3 lowColor = vec3(0.0, 0.1, 0.2);
    vec3 highColor = vec3(0.5, 0.8, 1.0);
    vec3 baseColor = mix(lowColor, highColor, colorFactor);
    baseColor += sin(a_Position.x + a_Position.y + a_Position.z) * 0.02;
    float dist_from_origin = length(a_Position.xyz * 3.0);
    float finalAlpha = clamp(dist_from_origin / c_RENDER_DISTANCE, 0.3, 0.7);

    v_Color = vec4(baseColor, finalAlpha);
}
void main()
{
    WaveMinecraft();

    v_local_position = a_Position;
    v_world_position = a_Position; 

    v_normal = a_Normal;
    v_uv = a_UV;
}