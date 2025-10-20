// C++에 의해 blender_nodes.glsl 파일이 이 코드 앞에 합쳐집니다.

layout(location=0) out vec4 FragColor;
in vec4 v_Color;

void main()
{
    // 1. UV 좌표를 가져옵니다.
    vec2 uv = texCoord_UV().xy;

    // 2. Noise Texture를 사용하여 0~1 사이의 흑백 값을 만듭니다.
    float noise_factor = texture_Noise(uv, 5.0, 4.0, 0.5, 0.0);
    
    // 3. 노이즈 값을 Color Ramp 함수의 입력(Factor)으로 사용합니다.
    vec3 ramp_color = texture_ColorRamp(noise_factor);

    // 4. 기존 웨이브 색상과 컬러 램프 색상을 혼합합니다.
    vec3 final_color = mix(v_Color.rgb, ramp_color, 0.8);

    FragColor = vec4(final_color, v_Color.a);
}