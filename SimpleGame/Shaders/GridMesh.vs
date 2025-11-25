#version 330
#define MAX_POINTS 100

in vec3 a_Position;
out vec4 v_Color;
out vec2 v_UV;

uniform float u_Time;
uniform vec4 u_Points[MAX_POINTS];
uniform vec2 u_resolution;

const float c_PI = 3.141592;

void flag()
{
    vec4 newPosition = vec4(a_Position,1);
    float value = a_Position.x + 0.5f;    // 0-1
    newPosition.y = newPosition.y * (1-value);
    float dX = 0;
    float dY = value * 0.5 * sin(2 * value * c_PI + u_Time * 10);
    float newColor = (sin(2 * value * c_PI + u_Time * 8) + 1)/2;

    newPosition += vec4(dX,dY,0,0);
    newPosition.xy *= vec2(2,0.5);

    gl_Position = newPosition;
    v_UV.x = (newPosition.x + 1)/2;
	v_UV.y = (1 - newPosition.y)/2;
    v_Color = vec4(newColor);
}

void Wave()
{
    vec4 newPosition = vec4(a_Position, 1);
    float dX = 0;
    float dY = 0;

    vec2 pos = vec2(a_Position.xy);
    vec2 center = vec2(0,0);
    float d = distance(pos, center);
    float y = 2 * clamp(0.5 - d, 0, 1);
    float newColor = y * sin(d *4 * c_PI * 10 - u_Time * 30);

    newPosition += vec4(dX, dY,0,0);
    gl_Position = newPosition;

    v_Color = vec4(newColor);
}

void RainDrop()
{
    vec4 newPosition = vec4(a_Position, 1);
    float dX = 0;
    float dY = 0;

    vec2 pos = vec2(a_Position.xy);
    float newColor = 0;
    
    for(int i  = 0; i< MAX_POINTS; i++)
    {
        float sTime = u_Points[i].z;
        float lTime = u_Points[i].w;
        float newTime = u_Time - u_Points[i].z;
        if(newTime > 0) 
        {
            float baseTime = fract(newTime / lTime);
            float oneMinus = 1 - baseTime;
            float t = baseTime * lTime;
            float range = baseTime * lTime / 5;
            vec2 center = u_Points[i].xy;
            float d = distance(pos, center);
            float y = 10 * clamp(range - d, 0, 1);
            newColor += oneMinus * y * sin(d * 4 * c_PI * 10 - t * 30);
        }
    }
    

    newPosition += vec4(dX, dY, 0, 0);
    gl_Position = newPosition;

    v_Color = vec4(newColor);
}



void main()
{
    flag();
    // Wave();
    // RainDrop();
}