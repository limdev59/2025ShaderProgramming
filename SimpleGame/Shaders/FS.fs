#version 330
layout(location = 0) out vec4 FragColor;

uniform float u_Time;
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
    // 1. 지역 변수 newUV에 v_UV 값을 복사
    vec2 newUV = v_UV;

    // [추가] 좌우 꿀렁임(Wave) 효과 적용
    // 수식: newX = oldX + sin(y * 주파수 + 시간 * 속도) * 진폭
    float waveAmp = 0.05;   // 진폭 (꿀렁이는 정도, 클수록 많이 휘어짐)
    float waveFreq = 10.0;  // 주파수 (세로 방향으로 물결이 몇 번 치는지)
    float waveSpeed = 5.0;  // 속도 (물결이 흐르는 빠르기)

    // Y좌표에 따라 X좌표를 흔들어 줌
    newUV.x += sin(newUV.y * waveFreq + u_Time * waveSpeed) * waveAmp;

    // 2. 기존 설정 (3x3 타일, 오프셋, 마진)
    float rCnt = 3.0;
    float sAmt = 0.5;
    float margin = 0.05;
    vec4 gapColor = vec4(0.3, 0.3, 0.3, 1.0);

    // 3. x 좌표 계산 (변경된 newUV 사용)
    float a = fract(newUV.x * rCnt);
    float b = floor(newUV.y * rCnt + 1.0);
    float x = a + (b * sAmt);

    // 4. y 좌표 계산
    float y = fract(newUV.y * rCnt);

    // 5. 마진(줄눈) 마스크 계산
    float maskX = step(margin, fract(x)) * step(fract(x), 1.0 - margin);
    float maskY = step(margin, y) * step(y, 1.0 - margin);
    float isBrick = maskX * maskY;

    // 6. 텍스처 샘플링 및 색상 혼합
    vec4 textureColor = texture(u_RGBTexture, vec2(x, y));
    FragColor = mix(gapColor, textureColor, isBrick);
}


//void DigitNum(){
//    vec2 newUV = vec2(v_UV.x, v_UV.y);
//    int digit = int(u_Time)%10;
//    int titleIndex = (digit+9)%10;
//
//    float  offX= float(titleIndex%5)/5;
//    float  offY= float(titleIndex%2)/2;
//
//    FragColor = newColor;
//}
void main()
{
    Brick_Horizontal_AI();
}
