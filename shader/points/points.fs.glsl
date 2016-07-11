uniform vec4 in_color_center;
uniform vec4 in_color_border;
in  vec2 vertex_uv;
out vec4 frag_color;


void main()
{
    float radius = length(vertex_uv-vec2(0.5,0.5));
    if(radius > 0.5)
    {
        discard;
    }
    frag_color  = mix(in_color_center,in_color_border,abs(radius*2.0));
}