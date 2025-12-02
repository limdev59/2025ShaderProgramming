#version 330
layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 FragColor1;

uniform float u_Time;
uniform sampler2D u_DigitTexture;
uniform sampler2D u_RGBTexture;
in vec2 v_UV;

void Q1(){
    vec2 newUV = vec2(v_UV.x, v_UV.y);
    float  x= newUV.x;
    float  y= 1-abs(2*(newUV.y-0.5));

    vec4 newColor = texture(u_RGBTexture, vec2(x,y));
    FragColor = newColor;
}

void Q2(){
    vec2 newUV = vec2(v_UV.x, v_UV.y);
    float  x= fract(newUV.x*3);
    float  y= newUV.y/3 + (2-floor(newUV.x*3))/3;
    vec4 newColor = texture(u_RGBTexture, vec2(x,y));
    FragColor = newColor;
}

void Q3(){
    vec2 newUV = vec2(v_UV.x, v_UV.y);
    float  x= fract(newUV.x*3);
    float  y= newUV.y/3 + (floor(newUV.x*3))/3;

    vec4 newColor = texture(u_RGBTexture, vec2(x,y));
    FragColor = newColor;
}

void Brick_Horizontal(){
    vec2 newUV = vec2(v_UV.x, v_UV.y);
    float rCnt = 2;
    float sAmt = 0.5;
    float  x= fract(newUV.x*rCnt) 
            + floor(newUV.y*rCnt+1)*sAmt;
    float  y= fract(newUV.y*rCnt);

    vec4 newColor = texture(u_RGBTexture, vec2(x,y));
    FragColor = newColor;
}

void Brick_Vertical(){
    vec2 newUV = vec2(v_UV.x, v_UV.y);

    float rCnt = 2;
    float sAmt = 0.5;
    float x = fract(newUV.x * rCnt);
    float y = fract(newUV.y *rCnt)
            + floor(rCnt*newUV.x-rCnt)*sAmt;
    vec4 newColor = texture(u_RGBTexture, vec2(x,y));
    FragColor = newColor;
}

void Brick_Horizontal_AI() {
    vec2 newUV = v_UV;

    float waveAmp = 0.05;
    float waveFreq = 10.0;
    float waveSpeed = 5.0;

    newUV.x += sin(newUV.y * waveFreq + u_Time * waveSpeed) * waveAmp;

    float rCnt = 3.0;
    float sAmt = 0.5;
    float margin = 0.05;
    vec4 gapColor = vec4(0.3, 0.3, 0.3, 1.0);

    float a = fract(newUV.x * rCnt);
    float b = floor(newUV.y * rCnt + 1.0);
    float x = a + (b * sAmt);

    float y = fract(newUV.y * rCnt);
    float maskX = step(margin, fract(x)) * step(fract(x), 1.0 - margin);
    float maskY = step(margin, y) * step(y, 1.0 - margin);
    float isBrick = maskX * maskY;

    vec4 textureColor = texture(u_RGBTexture, vec2(x, y));
    FragColor = mix(gapColor, textureColor, isBrick);
}

void Digit_Num()
{
    int digit = int(u_Time * 10)%10;

    int titleIndex = (digit + 9) % 10;
    float offX = float(titleIndex % 5) / 5;
    float offY = float(titleIndex / 5) / 2;
    float tx = v_UV.x / 5 + offX;
    float ty = v_UV.y / 2 + offY;
    FragColor = texture(u_DigitTexture, vec2(tx, ty));
}

void Lens(){
    int digit = int(u_Time * 10)%10;
    int titleIndex = (digit + 9) % 10;

    float offX = float(titleIndex % 5) / 5;
    float offY = float(titleIndex / 5) / 2;

    float tx = v_UV.x / 5 + offX;
    float ty = v_UV.y / 2 + offY;
    FragColor = texture(u_DigitTexture, vec2(tx, ty));
}

void main()
{
    // 여기서 원하는 함수를 호출하면, 해당 효과가 FBO에 가득 차게 그려집니다.
    // 예: Digit_Num(); 또는 Q1();
    Digit_Num(); 
    FragColor1 = vec4(1,0,0,1);
}