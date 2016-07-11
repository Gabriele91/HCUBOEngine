

noperspective in vec3 dist;
uniform vec4 in_color_fill;
uniform vec4 in_color_wire;
out vec4 frag_color;

void main(void)
{
    /* float d = min(dist[0],min(dist[1],dist[2])); */
    float d = min(dist[0],dist[2]); //ignore y axis, is a cube
    float intensity = exp2(-2*d*d);
    frag_color = mix(in_color_fill,in_color_wire,intensity);
}