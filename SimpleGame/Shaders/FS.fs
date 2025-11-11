#version 330

layout(location=0) out vec4 FragColor;

in vec2 v_UV;

const float PI = 3.1415928979;

void main()
{
    vec4 newColor = vec4(0);
    float xValue =  pow(abs(sin(v_UV.x * 2 * PI *4)),1);
    float yValue =  pow(abs(cos(v_UV.y * 2 * PI *4)),1);
    float xyValue = xValue + yValue;
    newColor = vec4(
    0.5*xyValue
    ,0.7*xyValue
    ,0.7*xyValue
    ,1
    );
    FragColor = newColor;
}