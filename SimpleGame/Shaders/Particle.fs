#version 330

// [중요] 출력이 0번(기본), 1번(밝음) 두 개로 늘어납니다
layout(location=0) out vec4 FragColor;
layout(location=1) out vec4 FragColorHigh;

in vec4 v_Color;
in vec2 v_UV;

uniform sampler2D u_Texture;

void main()
{
    // 텍스처와 버텍스 컬러(파티클 색)를 곱함
    vec4 result = texture(u_Texture, v_UV) * v_Color;

    // 밝기(Luma) 계산 공식
    float brightness = dot(result.rgb, vec3(0.2126, 0.7152, 0.0722));

    // 1. 일반 화면에는 무조건 그립니다.
    FragColor = result;

    // 2. 밝기가 1.0을 넘는 "눈부신" 부분만 따로 1번 타겟에 그립니다.
    if(brightness > 1.0)
    FragColorHigh = result;
    else
    FragColorHigh = vec4(0.0, 0.0, 0.0, 1.0); // 아니면 검은색
}