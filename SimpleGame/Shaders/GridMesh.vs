#version 330

in vec3 a_Position;
out vec4 v_Color;
uniform float u_Time;

const float c_PI = 3.14159265359;

void Flag()
{
    vec4 newPos = vec4(a_Position, 1.0);

    float value = a_Position.x + 0.5;
    float amp = 0.3;

    float dX = 0.0;
    float dY = value * sin(2.0 * value * c_PI - u_Time * 5.0) * amp;
    float newColorVal = (sin(2.0 * value * c_PI - u_Time * 5.0) + 1.0) * 0.5;

    newPos.y = newPos.y * (1.0 - value);
    newPos += vec4(dX, dY, 0.0, 0.0);
    gl_Position = newPos;

    vec2 colorFactor = (newPos.xy / amp) * 0.5 + 0.5;
    vec3 lowColor = vec3(0.0, 0.2, 0.5);
    vec3 highColor = vec3(0.5, 0.8, 1.0);
    vec3 mixed = mix(lowColor, highColor, colorFactor.y);
    
    // newColorVal(float)을 vec3로 변환하여 mix 함수에 사용
    v_Color = vec4(mix(vec3(newColorVal), mixed, 0.5), 1.0);
}

void Wave()
{
    // GLSL에서는 숫자 뒤에 'f'를 붙이지 않아도 float으로 인식됩니다.
    float AMP = 0.05;
    float FREQ = 30.0;
    float SPEED = 3.0;

    vec4 newPos = vec4(a_Position, 1.0);

    float distance = length(newPos.xy);
    float waveFactor = sin(distance * FREQ - u_Time * SPEED) * AMP;
    vec2 direction = normalize(newPos.xy + 0.00001); // 0으로 나누는 것을 방지

    // 이 부분은 현재 아무런 효과가 없습니다. (dX, dY가 0)
    // 아마 의도한 것은 아래와 같을 수 있습니다.
    // newPos.xy += direction * waveFactor;
    float dX = 0.0;
    float dY = 0.0;
    newPos += vec4(dX, dY, 0.0, 0.0);
    gl_Position = newPos;

    float colorFactor = (sin(distance * FREQ - u_Time * SPEED) + 1.0) / 2.0;
    vec3 lowColor = vec3(0.0, 0.1, 0.2);
    vec3 highColor = vec3(0.5, 0.8, 1.0);

    v_Color = vec4(mix(lowColor, highColor, colorFactor), 1.0);
}

const float c_RENDER_DISTANCE = 10.0;

void WaveMinecraft()
{
    gl_Position = vec4(a_Position, 1.0);

    float distance = length(a_Position.xy);
    float FREQ = 30.0;
    float SPEED = 3.0;

    float colorFactor = (sin(distance * FREQ - u_Time * SPEED) + 1.0) / 2.0;
    vec3 lowColor = vec3(0.0, 0.1, 0.2);
    vec3 highColor = vec3(0.5, 0.8, 1.0);
    vec3 baseColor = mix(lowColor, highColor, colorFactor);

    baseColor += sin(a_Position.x + a_Position.y + a_Position.z) * 0.02;
    float dist_from_origin = length(a_Position.xyz * 3.0);
    float finalAlpha = clamp(dist_from_origin / c_RENDER_DISTANCE, 0.3, 0.7);

    v_Color = vec4(baseColor, finalAlpha);
}

void main()
{
    WaveMinecraft();
}