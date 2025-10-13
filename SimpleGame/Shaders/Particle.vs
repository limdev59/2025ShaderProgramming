#version 330

in vec3 a_Position;
in float a_Value;
in vec4 a_Color;
in float a_sTime;
in vec3 a_Velocity;
in float a_LifeTime;
in float a_Mass;
in float a_Period;

out vec4 v_Color;

uniform float u_Time;
uniform vec3 u_Force;

const float c_PI = 3.141592;
const vec2 c_G = vec2(0, -9.8);

void raining()
{
	float lifeTime = a_LifeTime;
	float newAlpha = 1.0f;
	vec4 newPosition = vec4(a_Position, 1.0f);
	float newTime = u_Time - a_sTime;

	if(newTime > 0)
	{
		float t = fract(newTime / lifeTime) * lifeTime;
		float tt = t*t;


		float forceX = u_Force.x + c_G.x * a_Mass;
		float forceY = u_Force.y + c_G.y * a_Mass;

		float aX = forceX / a_Mass * 50;
		float aY = forceY / a_Mass;

		float x = a_Velocity.x * t + 0.5 * aX * 0.2 * tt;
		float y = a_Velocity.y * t + 0.5 * (aY * 3.f) * 0.2 * tt;

		newPosition.xy += vec2(x, y);
		newAlpha = 1.0f - t/lifeTime;	// 1-0
	}
	else
	{
		newPosition.xy = vec2(-1000000,0);
	}

	gl_Position = newPosition;
	v_Color = vec4(a_Color.rgb, newAlpha);
}

void sinParticle()
{
	vec4 centerC = vec4(1,0,0,1);
	vec4 borderC = vec4(1,1,1,0);

	float newTime = u_Time - a_sTime; 
	float lifeTime = a_LifeTime;
	float newAlpha = 1.0f;
	vec4 newColor = a_Color;
	vec4 newPosition = vec4(a_Position, 1.0f);

	if(newTime > 0){
		float period = a_Period;
		float t = fract(newTime/lifeTime) * lifeTime;
		float tt = newTime * newTime;
		float x = t * 2 - 1.f;
		float y = t * sin(t *2 * c_PI * period) * ((a_Value -0.5) * 2.f);
		y *= sin(fract(newTime/lifeTime) * c_PI);
		newPosition.xy += vec2(x,y);
		newAlpha = 1.0f - t/lifeTime;

		newColor = mix(centerC, borderC, abs(y * 8));
	}
	else{
		newPosition.xy = vec2(-1000000,0);
	}


	gl_Position = newPosition;
	v_Color = newColor;	// a_Color
}

void circleParticle()
{
	float newTime = u_Time - a_sTime; 
	float lifeTime = a_LifeTime;
	float newAlpha = 1.0f;
	vec4 newPosition = vec4(a_Position, 1.0f);

	if(newTime > 0)
	{
		float t = fract(newTime/lifeTime) * lifeTime;
		float tt = t * t;

		float x = sin(a_Value * 2 * c_PI);
		float y = cos(a_Value * 2 * c_PI);
		float newX = x + 0.5 * c_G.x * tt;
		float newY = y + 0.5 * c_G.y * tt;

		newPosition.xy += vec2(newX,newY);	//S_O

		newAlpha = 1.0f - t/lifeTime;
	}
	else
	{
		newPosition.xy = vec2(-1000000,0);
	}
	gl_Position = newPosition;
	v_Color = vec4(a_Color.rgb, newAlpha);	
}

void main()
{
	//raining();
	//sinParticle();
	circleParticle();
}