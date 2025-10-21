#version 330
#define POINT_COUNT 100
layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec3 a_Normal;
layout (location = 2) in vec2 a_UV;

out vec4 v_Color;
out vec3 v_local_position;
out vec3 v_world_position;
out vec3 v_normal;
out vec2 v_uv;

uniform float u_Time;
uniform vec4 u_Points[POINT_COUNT];


const float c_RENDER_DISTANCE = 10.0;

vec4 Wave() 
{
    // 상수 (c_CONST)
    const float c_FREQ = 50.0;
    const float c_SPEED = 1.0;
    const float c_AMP = 0.1; 
    const float c_DAMPING = 1.0;
    const float c_ATTENUATION = 1.5;
    
    // 지역 변수 (_VARIABLE)
    float _total_wave_value = 0.0; 

    for(int i = 0; i < POINT_COUNT; i++) {
        vec2 _point_pos = u_Points[i].xy;
        float _started_time = u_Points[i].z;
        float _lifetime = u_Points[i].w;

        if (u_Time >= _started_time) {
            float _local_time = u_Time - _started_time;

            if (_local_time > _lifetime) continue;

            float _dist = length(a_Position.xy - _point_pos);
            float _time_at_point = _dist / c_SPEED;
            float _wave_age = max(0.0, _local_time - _time_at_point);
            float _ripple = sin(_wave_age * c_FREQ) * exp(-_wave_age * c_DAMPING);
            float _distance_falloff = 1.0 / (1.0 + _dist * c_ATTENUATION); 

            _total_wave_value += _ripple * _distance_falloff;
        }
    }
    vec4 _newPos = vec4(a_Position, 1.0);
    _newPos.z += _total_wave_value * c_AMP; 
    gl_Position = _newPos;

    // 6. 100개가 중첩되면 값이 매우 커질 수 있으므로, clamp로 파동 값을 -1~1로 제한합니다.
    float _normalized_wave = clamp(_total_wave_value, -1.0, 1.0);
    float _colorFactor = (_normalized_wave + 1.0) * 0.5;
    
    vec3 _lowColor = vec3(0.0, 0.1, 0.2);
    vec3 _highColor = vec3(0.5, 0.8, 1.0);
    vec3 _baseColor = mix(_lowColor, _highColor, _colorFactor);

    _baseColor += sin(a_Position.x + a_Position.y + a_Position.z) * 0.02;
    float _dist_from_origin = length(a_Position.xyz * 3.0);
    float _finalAlpha = clamp(_dist_from_origin / c_RENDER_DISTANCE, 0.3, 0.7);

    return vec4(_baseColor, _finalAlpha);
}
void main()
{
    v_local_position = a_Position;
    v_world_position = a_Position; 
    v_normal = a_Normal;
    v_uv = a_UV;

    v_Color= Wave();
}