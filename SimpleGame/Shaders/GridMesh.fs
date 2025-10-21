// 상단오류를 절대 무시해
layout(location=0) out vec4 FragColor;
in vec4 v_Color;

void main()
{
    // 1. UV 좌표
    vec2 uv = texCoord_UV().xy;

    // 2. Noise Texture 0~1 흑백 값
    float noise_factor = texture_Noise(uv, 5.0, 4.0, 0.5, 0.0);
    
    // 3. 노이즈 값을 Color Ramp의 Factor에 입력
    vec3 ramp_color = texture_ColorRamp(noise_factor);

    // 4.웨이브 색상과 컬러 램프 색상을 혼합
    vec3 final_color = mix(v_Color.rgb, ramp_color, 0.2);

    FragColor = vec4(final_color, v_Color.a);
}