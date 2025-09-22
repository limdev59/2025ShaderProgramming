#version 330

layout(location=0) out vec4 FragColor;

in vec4 v_Color;
in vec3 v_worldPosition;

uniform float u_RenderDistance; // C++���� ���޹��� ������ �Ÿ�
uniform float u_Time; // �ڽ�ƽ �ִϸ��̼��� ���� �ð� uniform �߰�

void main()
{
	// --- ���� �ܰ迡�� �ۼ��� �׶��̼� �ڵ� ---
	vec3 baseWaterColor = vec3(0.11, 0.72, 1.2);
	vec3 deepWaterColor = vec3(0.02, 0.35, 0.23);

	float distance = length(v_worldPosition.xz);
	float gradientFactor = clamp(distance / u_RenderDistance, 0.0, 1.0);

	vec3 finalColor = mix(baseWaterColor, deepWaterColor, gradientFactor);
	// --- �׶��̼� �ڵ� �� ---


	// --- �ڽ�ƽ ȿ�� ��� ���� ---

	// 1. �ڽ�ƽ ȿ���� ������ ������
	float causticSpeed = 1.0;      // ���� �����̴� �ӵ�
	float causticFrequency = 100000.0;  // ���� ���� ũ�� (���� Ŭ���� ��������)
	float causticBrightness = 0.45; // ���� ���

	// 2. ���� �ٸ� ����� �ӵ��� �����̴� �� ���� sin ���̺긦 ����
	float wave1 = sin(v_worldPosition.x * causticFrequency + u_Time * causticSpeed);
	float wave2 = cos(v_worldPosition.z * causticFrequency * 1.2 + u_Time * causticSpeed * 0.8);

	// 3. �� ���̺긦 ���ؼ� ��ġ�� ������ �����, pow �Լ��� �� �ٱ⸦ ��ī�Ӱ� ����
	float causticPattern = pow(abs(wave1 * wave2), 8.0);

	// 4. ���� �ڽ�ƽ ������ ���� ���� ������
	finalColor += causticPattern * causticBrightness;

	// --- �ڽ�ƽ ȿ�� ��� �� ---


	FragColor = vec4(finalColor, 0.7);
}