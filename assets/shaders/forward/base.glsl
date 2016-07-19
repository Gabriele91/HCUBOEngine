#pragma vertex

layout(location = 0) in vec3 vertex;
uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{
    gl_Position = projection * view * model * vec4(vertex,1.0);
}

#pragma fragment

uniform vec4 in_color;
out vec4 frag_color;

void main()
{
    frag_color = in_color;
}