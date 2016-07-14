#pragma vertex
//input
layout(location = 0) in vec3 vertex;
layout(location = 1) in vec2 uvcoord;
//out coord
out vec2 frag_uvcoord;
//uniform
uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{
    frag_uvcoord= uvcoord;
    gl_Position = projection * view * model * vec4(vertex,1.0);
}

#pragma fragment
//in coord
in vec2 frag_uvcoord;
//out color
out vec4 frag_color;
//uniform
uniform vec4      in_color;
uniform sampler2D texture_id;

void main()
{
    frag_color = texture(texture_id, frag_uvcoord)*in_color;
}