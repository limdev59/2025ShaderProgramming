layout(location=0) out vec4 FragColor;

in vec4 v_Color;

void main()
{
    vec3 generated_coords = texCoord_Generated();

    vec3 final_color = mix(v_Color.rgb, generated_coords, 0.5);

    FragColor = vec4(final_color, v_Color.a);
}