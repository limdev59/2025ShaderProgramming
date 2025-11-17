#version 330

layout(location=0) out vec4 FragColor;

in vec2 v_UV;

uniform float iTime;        
uniform vec2 iResolution;  

// --- 요청된 색상 팔레트 ---
const vec3 color1 = vec3(214.0/255.0, 244.0/255.0, 237.0/255.0); // #D6F4ED
const vec3 color2 = vec3(135.0/255.0, 186.0/255.0, 195.0/255.0); // #87BAC3
const vec3 color3 = vec3(83.0/255.0, 98.0/255.0, 158.0/255.0); // #53629E
const vec3 color4 = vec3(71.0/255.0, 52.0/255.0, 114.0/255.0); // #473472
const vec3 colorWhite = vec3(1.0); // "펑!" 효과

// 4색 그라디언트 계산 헬퍼 함수
vec3 GetSwirlColor(float t)
{
    if (t < 0.333) return mix(color1, color2, smoothstep(0.0, 0.333, t));
    else if (t < 0.666) return mix(color2, color3, smoothstep(0.333, 0.666, t));
    else return mix(color3, color4, smoothstep(0.666, 1.0, t));
}

void main()
{
    // 1. 좌표계 설정 (이전과 동일)
    vec2 uv = v_UV - 0.5;
    uv.x *= iResolution.x / iResolution.y;

    // 2. 주기 설정
    float cycle_duration = 7.0; 
    float cycle_time = mod(iTime, cycle_duration);
    
    // 3. 페이즈(단계) 정의
    float gather_duration = 6.0; // 6초간 "소용돌이"
    float burst_duration = 1.0;  // 1초간 "터뜨리기"

    // 4. 극좌표계 (거리 및 각도)
    float dist = length(uv) * 2.0; 
    float angle = atan(uv.y, uv.x);

    // 5. 공통 파라미터 정의
    
    // [피드백 1] 소용돌이 가닥 6개로 수정
    float num_arms = 6.0;       
    
    // [피드백 2] 회전 속도 상향
    float speed = 3.5;          // (기존 2.0)
    float acceleration = 0.8;   // (기존 0.5)
    
    // (iTime을 써서 회전이 7초마다 튀지 않고 부드럽게 이어지도록)
    float rotation = (iTime * speed) + (iTime * iTime * acceleration);

    vec3 finalColor;

    if (cycle_time < gather_duration)
    {
        // --- 페이즈 1: "소용돌이 치다가" (0.0s ~ 6.0s) ---
        
        // 6. "토네이도" (Tornado) 패턴 정의
        float wave_frequency = 20.0;
        float wave_pattern = (dist * wave_frequency) + (angle * num_arms) - rotation;
        float t = (sin(wave_pattern) + 1.0) * 0.5; // [0, 1]

        // 7. 4색 그라디언트 믹스
        vec3 waveColor = GetSwirlColor(t);
        
        // 8. 6초에 걸쳐 "소용돌이" 효과가 점점 강해집니다 (Fade In)
        float gather_progress = smoothstep(0.0, gather_duration, cycle_time);
        finalColor = mix(color3, waveColor, gather_progress); 
    }
    else
    {
        // --- 페이즈 2: "펑 터져야한다" (6.0s ~ 7.0s) ---

        // 9. 폭발 진행도 (1초 동안 0.0 -> 1.0)
        float burst_progress = (cycle_time - gather_duration) / burst_duration; 
        
        // 10. "파도치는 걸" (Pulse Wave)
        
        // (A) 펄스 확장 (1초간 0.0 -> 1.5 확장)
        float pulse_radius = burst_progress * 1.5;
        
        // (B) 펄스 링의 "두께" (0.3)
        float ring_dist = abs(dist - pulse_radius);
        float pulse_wave_mask = smoothstep(0.3, 0.0, ring_dist);

        // (C) [피드백 3] 충격파가 단색이 아니도록 수정
        // 링과의 거리(ring_dist)를 기반으로 새로운 고주파 4색 그라디언트를 생성합니다.
        // 80.0 -> 링 내부에 촘촘한 띠가 생김
        float ring_t_raw = sin(ring_dist * 80.0);
        float ring_t = (ring_t_raw + 1.0) * 0.5; // [0, 1]
        
        vec3 pulse_color = GetSwirlColor(ring_t);
        
        // (D) 베이스 색상 위에 "화려한 펄스"를 믹스
        finalColor = mix(color3, pulse_color, pulse_wave_mask);

        // 11. "펑!" (Burst Flash) (이전과 동일)
        float flash_progress = burst_progress / 0.2; // 0.2초간 [0, 1]
        float flash = (smoothstep(0.0, 1.0, flash_progress) - smoothstep(1.0, 1.1, flash_progress));
        flash *= (1.0 - smoothstep(0.0, 0.3, dist)); // 중앙에서 강하게
        
        // 펄스 + 베이스 위에 섬광을 덮어씌움
        finalColor = mix(finalColor, colorWhite, flash);
    }
    
    // 12. 최종 출력
    FragColor = vec4(finalColor, 1.0);
}