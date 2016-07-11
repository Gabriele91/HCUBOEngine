layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;


uniform vec2 window_scale;
uniform vec2 line_size;
noperspective out vec3 dist;

void main(void)
{
    vec2 window_scale = vec2(1280, 768) * (1.0 / (2.0*line_size));
    
    vec2 p0 = window_scale * gl_in[0].gl_Position.xy/gl_in[0].gl_Position.w;
    vec2 p1 = window_scale * gl_in[1].gl_Position.xy/gl_in[1].gl_Position.w;
    vec2 p2 = window_scale * gl_in[2].gl_Position.xy/gl_in[2].gl_Position.w;
    
    vec2 v0 = p2-p1;
    vec2 v1 = p2-p0;
    vec2 v2 = p1-p0;
    float area = abs(v1.x*v2.y - v1.y * v2.x);
    
    dist = vec3(area/length(v0),0,0);
    gl_Position = gl_in[0].gl_Position;
    EmitVertex();
    
    dist = vec3(0,area/length(v1),0);
    gl_Position = gl_in[1].gl_Position;
    EmitVertex();
    
    dist = vec3(0,0,area/length(v2));
    gl_Position = gl_in[2].gl_Position;
    EmitVertex();
    
    EndPrimitive();
}