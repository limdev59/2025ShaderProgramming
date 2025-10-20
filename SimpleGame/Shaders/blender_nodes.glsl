#version 330 core

// --- 입력 변수들 ---
in vec3 v_local_position;
in vec3 v_world_position;
in vec3 v_normal;
in vec2 v_uv;

// --- Uniform 변수들 ---
uniform vec2 u_resolution;
uniform vec3 u_camera_position;

// 1. Generated (로컬 좌표계)
vec3 texCoord_Generated() {
    return v_local_position + 0.5;
}

// 2. Normal (노멀 벡터)
// 시각화를 위해 -1~1 범위를 0~1 범위로 변환
vec3 texCoord_Normal() {
    return normalize(v_normal) * 0.5 + 0.5;
}

// 3. UV (UV 좌표)
vec3 texCoord_UV() {
    return vec3(v_uv, 0.0);
}

// 4. Object (월드 좌표계)
vec3 texCoord_Object() {
    return v_world_position + 0.5;
}

// 5. Camera (뷰 벡터)
// 픽셀에서 카메라로 향하는 정규화된 벡터
vec3 texCoord_Camera() {
    vec3 view_vector = normalize(u_camera_position - v_world_position);
    // 시각화를 위해 0~1 범위로 변환
    return view_vector * 0.5 + 0.5;
}

// 6. Window (스크린 좌표계)
vec3 texCoord_Window() {
    return vec3(gl_FragCoord.xy / u_resolution.xy, 0.0);
}

// 7. Reflection (반사 벡터)
vec3 texCoord_Reflection() {
    vec3 view_vector = normalize(v_world_position - u_camera_position);
    vec3 reflect_vector = reflect(view_vector, normalize(v_normal));
    // 시각화를 위해 0~1 범위로 변환
    return reflect_vector * 0.5 + 0.5;
}
// blender_nodes.glsl 파일에서 아래 코드로 Noise 부분을 교체하세요.

// =======================================================
// Noise Texture Node (Corrected Version)
// Based on a classic 2D Simplex Noise implementation
// =======================================================

vec3 mod289(vec3 x) { return x - floor(x * (1.0 / 289.0)) * 289.0; }
vec2 mod289(vec2 x) { return x - floor(x * (1.0 / 289.0)) * 289.0; }
vec3 permute(vec3 x) { return mod289(((x*34.0)+1.0)*x); }

// 2D Simplex Noise 함수 (수정됨)
float snoise(vec2 v) {
    const vec4 C = vec4(0.211324865405187,   // (3.0-sqrt(3.0))/6.0
                        0.366025403784439,   // 0.5*(sqrt(3.0)-1.0)
                       -0.577350269189626,   // -1.0 + 2.0 * C.x
                        0.024390243902439);  // 1.0 / 41.0
    vec2 i  = floor(v + dot(v, C.yy));
    vec2 x0 = v - i + dot(i, C.xx);
    vec2 i1 = (x0.x > x0.y) ? vec2(1.0, 0.0) : vec2(0.0, 1.0);
    vec2 x1 = x0 - i1 + C.xx;
    vec2 x2 = x0 - 1.0 + 2.0 * C.xx;
    i = mod289(i);
    vec3 p = permute(permute(i.y + vec3(0.0, i1.y, 1.0)) + i.x + vec3(0.0, i1.x, 1.0));
    vec3 m = max(0.5 - vec3(dot(x0,x0), dot(x1,x1), dot(x2,x2)), 0.0);
    m = m*m;
    m = m*m;
    vec3 x = 2.0 * fract(p * C.www) - 1.0;
    vec3 h = abs(x) - 0.5;
    vec3 ox = floor(x + 0.5);
    vec3 a0 = x - ox;
    m *= 1.79284291400159 - 0.85373472095314 * (a0*a0 + h*h);
    vec3 g;
    g.x  = a0.x  * x0.x  + h.x  * x0.y;
    g.yz = a0.yz * vec2(x1.x,x2.x) + h.yz * vec2(x1.y,x2.y);
    return 130.0 * dot(m, g);
}

// 블렌더의 Noise Texture 노드를 모방한 함수 (수정 없음)
float texture_Noise(vec2 uv, float scale, float detail, float roughness, float distortion) {
    float value = 0.0;
    float frequency = scale;
    float amplitude = 1.0;
    for (int i = 0; i < 8; i++) {
        if (i >= int(detail)) break;
        vec2 distorted_uv = uv + (snoise(uv * frequency * 0.5) - 0.5) * distortion;
        value += snoise(distorted_uv * frequency) * amplitude;
        frequency *= 2.0;
        amplitude *= roughness;
    }
    return (value + 1.0) * 0.5;
}