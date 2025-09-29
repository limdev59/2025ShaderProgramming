#version 330

in vec3 a_Position;
in float a_Radius;
in vec4 a_Color;
in float a_sTime;
in vec3 a_VelLocity;
in float a_LifeTime;
in float a_Mass;

out vec4 v_Color;

uniform float u_Time;
uniform float u_Force;

const vec2 c_G = vec2(0,-9.8);

void main()
{
	float newTime = u_Time - a_sTime;
	float newAlpha = 1.0f;

	vec4 newPosition = vec4(a_Position, 1.0f);

	if(newTime > 0){
		float t = fract(newTime/a_LifeTime)*a_LifeTime; //float t = mod(newTime, 2);
		float tt = t*t;

		float ForceX = u_Force.x ;
		float ForceY = u_Force.y + ;

		vec2 v0 = a_VelLocity.xy;
		vec2 p0 = a_Position.xy;

		vec2 offset = v0 * t + 0.5 * c_G * tt ;

		p0.xy = a_Position.xy * 1/(tt+1);
		
		newPosition.xy = p0 + offset;

		newAlpha = 1.0 - (t / a_LifeTime);
	}else
	{
		newPosition.xy = vec2(-100000,0 );
	}

	
	gl_Position = newPosition;

	v_Color = vec4(a_Color.rgb, newAlpha); 

}