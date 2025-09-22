#version 330

in vec3 a_Position;
in vec4 a_Color;

out vec4 v_Color;
out vec3 v_worldPosition;

uniform float u_Time;

void main()
{
	vec4 newPosition = vec4(a_Position, 1.0f);

	float speed = 3.0;     // ���� �ӵ� (�����
	float amplitude = 0.08;  // ���� ���� (�����

	float wave = sin(u_Time * speed + a_Position.x * 5.0) * cos(u_Time * speed * 0.5 + a_Position.z * 5.0);

	// 3. wave * amplitude
	newPosition.y += wave * amplitude;

	// --- ���� ȿ�� ��� �� ---

	gl_Position = newPosition;
	v_Color = a_Color;
}