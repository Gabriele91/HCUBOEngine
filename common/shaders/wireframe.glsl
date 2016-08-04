#pragma vertex
//unifroms
uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

//in
layout(location = 0) in vec3 vertex;
layout(location = 1) in vec3 normal;

//out
out vec3 geom_normal;

void main()
{
    mat3 normal_matrix = transpose(inverse(mat3(model)));
    geom_normal = normal_matrix * normal;
    gl_Position = projection*view*model*vec4(vertex,1.0);
}


#pragma geometry

//uniforms
uniform vec4 viewport;
uniform vec2 line_size;

//struct geometry input
layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

//in from vertex shader
in vec3 geom_normal[3];

//out
out vec3 frag_vertex;
out vec3 frag_normal;
noperspective out vec3 dist;

void main(void)
{
    vec2 viewport_size = viewport.zw - viewport.xy;
    vec2 window_scale  = viewport_size * (1.0 / (2.0*line_size));
    
    vec2 p0 = window_scale * gl_in[0].gl_Position.xy/gl_in[0].gl_Position.w;
    vec2 p1 = window_scale * gl_in[1].gl_Position.xy/gl_in[1].gl_Position.w;
    vec2 p2 = window_scale * gl_in[2].gl_Position.xy/gl_in[2].gl_Position.w;
    
    vec2 v0 = p2-p1;
    vec2 v1 = p2-p0;
    vec2 v2 = p1-p0;
    float area = abs(v1.x*v2.y - v1.y * v2.x);
    
    dist = vec3(area/length(v0),0,0);
    frag_normal = geom_normal[0];
    frag_vertex = gl_in[0].gl_Position.xyz;
    gl_Position = gl_in[0].gl_Position;
    EmitVertex();
    
    dist = vec3(0,area/length(v1),0);
    frag_normal = geom_normal[1];
    frag_vertex = gl_in[1].gl_Position.xyz;
    gl_Position = gl_in[1].gl_Position;
    EmitVertex();
    
    dist = vec3(0,0,area/length(v2));
    frag_normal = geom_normal[2];
    frag_vertex = gl_in[2].gl_Position.xyz;
    gl_Position = gl_in[2].gl_Position;
    EmitVertex();
    
    EndPrimitive();
}


#pragma fragment
//uniform
uniform vec4 color_lines;
//in
in vec3 frag_vertex;
in vec3 frag_normal;
noperspective in vec3 dist;
//out
layout(location = 0) out vec3 g_vertex;
layout(location = 1) out vec3 g_normal;
layout(location = 2) out vec4 g_albedo_spec;

void main(void)
{
    float d = min(dist[0],min(dist[1],dist[2]));
    if(exp2(-2*d*d) < 0.5) discard;
    
    g_vertex          = frag_vertex;
    g_normal          = normalize(frag_normal) * 0.5 + 0.5 ;
    g_albedo_spec     = vec4(color_lines.xyz,1.0);
}