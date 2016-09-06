#pragma vertex
//in
layout(location = ATT_POSITIONT) in vec3 vertex;
layout(location = ATT_NORMAL0)   in vec3 normal;
layout(location = ATT_TEXCOORD0) in vec2 uvcoord;
//out
out vec2 frag_uvcoord;

void main()
{
    gl_Position  = vec4(vertex, 1.0f);
    frag_uvcoord = uvcoord;
}

#pragma fragment
//in
in vec2 frag_uvcoord;
//out
out vec4 frag_color;
//uniform g_buffer
uniform sampler2D g_albedo_spec;
//uniform spot lights
uniform int n_point_lights = 0;
uniform vec4 ambient_light;


void main()
{
    // Retrieve data from gbuffer
    vec3 diffuse = texture(g_albedo_spec, frag_uvcoord).rgb;
    //output
    frag_color = vec4(diffuse*ambient_light.rgb,1.0);
}