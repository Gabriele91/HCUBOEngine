
layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform vec2 size;

out vec2 vertex_uv;

void main()
{
    
    vec4 center =  view * model * gl_in[0].gl_Position;
    
    vec4 p[4];
    p[0]=projection * (center + vec4(  size.x,   size.y, 0.0f, 0.0f));
    p[1]=projection * (center + vec4(  size.x,  -size.y, 0.0f, 0.0f));
    p[2]=projection * (center + vec4( -size.x,   size.y, 0.0f, 0.0f));
    p[3]=projection * (center + vec4( -size.x,  -size.y, 0.0f, 0.0f));
    
    vec2 uv[4];
    uv[0] = vec2(  1.0,    0.0 );
    uv[1] = vec2(  1.0,    1.0 );
    uv[2] = vec2(  0.0 ,   0.0 );
    uv[3] = vec2(  0.0,    1.0 );
    
    for(int i=0;i!=4;++i)
    {
        gl_Position = p[i];
        vertex_uv   = uv[i];
        EmitVertex();
    }
}