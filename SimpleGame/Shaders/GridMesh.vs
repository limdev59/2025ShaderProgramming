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
const vec2 c_points[3] = vec2[](vec2(0.5,0.5),vec2(-0.5,-0.5),vec2(0,0));


const float c_RENDER_DISTANCE = 10.0;
vec4 Wave(vec2 points[3]) 
{
    float FREQ = 30.0;
    float SPEED = 3.0;
    float AMP = 0.05;

    float total_wave_value = 0.0; 

    for(int i = 0; i < 3; i++) {
        float distance = length(a_Position.xy - points[i]);
        total_wave_value += sin(distance * FREQ - u_Time * SPEED);
    }

    vec4 newPos = vec4(a_Position, 1.0);
    float normalized_wave = total_wave_value / 3.0;
    newPos.z += normalized_wave * AMP;
    gl_Position = newPos;

    float colorFactor = (normalized_wave + 1.0) * 0.5;
    vec3 lowColor = vec3(0.0, 0.1, 0.2);
    vec3 highColor = vec3(0.5, 0.8, 1.0);
    vec3 baseColor = mix(lowColor, highColor, colorFactor);

    baseColor += sin(a_Position.x + a_Position.y + a_Position.z) * 0.02;
    float dist_from_origin = length(a_Position.xyz * 3.0);
    float finalAlpha = clamp(dist_from_origin / c_RENDER_DISTANCE, 0.3, 0.7);

    return vec4(baseColor, finalAlpha);
}
void main()
{

    v_local_position = a_Position;
    v_world_position = a_Position; 

    v_normal = a_Normal;
    v_uv = a_UV;
    v_Color= Wave(c_points);
}