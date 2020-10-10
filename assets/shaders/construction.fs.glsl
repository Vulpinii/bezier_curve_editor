#version 410 core

// Output data
out vec4 FragColor;
in vec3 color;


void main()
{
    FragColor = vec4(color, 1.0);
}
