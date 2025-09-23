#version 330

in vec3 a_Position;
in float a_Radius;
in vec4 a_Color;
in float a_sTime;
in vec3 a_VelLocity;

out vec4 v_Color;

uniform float u_Time;

const vec2 c_G = vec2(0,-9.8);

void main()
{
	float newTime = u_Time - a_sTime;

	vec4 newPosition = vec4(a_Position, 1.0f);

	if(newTime > 0){
		//float t = fract(newTime*0.5)*2;
		float t = mod(newTime, 2);
		float tt = t*t;

		vec2 v0 = vec2(a_VelLocity.x, a_VelLocity.y);
		vec2 p0 = vec2(a_Position.x, a_Position.y);

		vec2 offset = v0 * t + 0.5 * c_G * t * t;

		p0.x = a_Position.x *1/(tt+1);
		p0.y = a_Position.y * 1/(tt+1);
		
		newPosition.xy = p0 + offset;
	}else
	{
		newPosition.xy = vec2(-100000,0 );
	}

	
	gl_Position = newPosition;

	v_Color = a_Color;
}