#version 330 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;
uniform int isZapper;

// texture sampler
uniform sampler2D texture1;

void main()
{
	vec4 temp = texture(texture1, TexCoord);

    float y_align = TexCoord.y;

    vec4 green = vec4(0.0f, 1.0f, 0.0f, 1.0f);
    vec4 orange = vec4(1.0f, 0.5f,0.0f, 1.0f);

    float step1 = 0.5f;     
    float step2 = 0.5f;
    float step3 = 0.5f;

    vec4 color = mix(green, orange, smoothstep(step1, step2, y_align));
    color = mix(color, orange, smoothstep(step2, step3, y_align));
    
    if(isZapper == 1)
        FragColor = color;
	else if(isZapper == 2)
		FragColor = color*temp;
    else
        FragColor =  temp;

}