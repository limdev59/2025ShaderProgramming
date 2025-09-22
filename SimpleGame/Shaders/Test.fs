#version 330

layout(location=0) out vec4 FragColor;

in vec4 v_Color;
in vec3 v_worldPosition;

uniform float u_RenderDistance; // C++에서 전달받을 렌더링 거리
uniform float u_Time; // 코스틱 애니메이션을 위해 시간 uniform 추가

void main()
{
	// --- 이전 단계에서 작성한 그라데이션 코드 ---
	vec3 baseWaterColor = vec3(0.11, 0.72, 1.2);
	vec3 deepWaterColor = vec3(0.02, 0.35, 0.23);

	float distance = length(v_worldPosition.xz);
	float gradientFactor = clamp(distance / u_RenderDistance, 0.0, 1.0);

	vec3 finalColor = mix(baseWaterColor, deepWaterColor, gradientFactor);
	// --- 그라데이션 코드 끝 ---


	// --- 코스틱 효과 계산 시작 ---

	// 1. 코스틱 효과를 조절할 변수들
	float causticSpeed = 1.0;      // 빛이 움직이는 속도
	float causticFrequency = 100000.0;  // 빛의 무늬 크기 (값이 클수록 촘촘해짐)
	float causticBrightness = 0.45; // 빛의 밝기

	// 2. 서로 다른 방향과 속도로 움직이는 두 개의 sin 웨이브를 생성
	float wave1 = sin(v_worldPosition.x * causticFrequency + u_Time * causticSpeed);
	float wave2 = cos(v_worldPosition.z * causticFrequency * 1.2 + u_Time * causticSpeed * 0.8);

	// 3. 두 웨이브를 곱해서 겹치는 패턴을 만들고, pow 함수로 빛 줄기를 날카롭게 만듦
	float causticPattern = pow(abs(wave1 * wave2), 8.0);

	// 4. 계산된 코스틱 패턴을 최종 색상에 더해줌
	finalColor += causticPattern * causticBrightness;

	// --- 코스틱 효과 계산 끝 ---


	FragColor = vec4(finalColor, 0.7);
}