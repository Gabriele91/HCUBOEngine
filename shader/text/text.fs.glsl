
in vec2 tex_coord;
out vec4 frag_color;

uniform sampler2D image;
uniform vec4 in_color;

void main()
{
    frag_color = vec4(in_color.xyz, in_color.a*texture(image, tex_coord).r);
}
