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
//uniform
uniform sampler2D g_position;
uniform sampler2D g_normal;
uniform sampler2D g_albedo_spec;
uniform sampler2D g_occlusion;
//uniform model camera position
#pragma include "lib/uniform.glsl"
//include light lib
#pragma include "lib/direction_light.glsl"
//uniform light
uniform direction_light light;


void main()
{
    // Retrieve data from gbuffer
    vec4  position  = texture(g_position,    frag_uvcoord).rgba;
    vec3  normal    = texture(g_normal,      frag_uvcoord).rgb;
    vec3  diffuse   = texture(g_albedo_spec, frag_uvcoord).rgb;
    float specular  = texture(g_albedo_spec, frag_uvcoord).a;
    float occlusion = texture(g_occlusion,   frag_uvcoord).r;
    
    //unpack
    normal = normalize(normal * 2.0 - 1.0);
    
    //todo: material
    float shininess = 16.0f;
        
    //view dir
	vec3 view_dir = normalize(camera.position - position.xyz);
    
    //result
    direction_light_res light_results;
    
    // accumulator
    vec3 diff_col = diffuse * occlusion;
    
    // then calculate lighting as usual
    compute_direction_light(light,
							view_dir,
							normal,
							shininess,
							light_results);
    //color
	vec3 lighting = diff_col * (light_results.m_diffuse + light_results.m_specular * specular);
    
    //output
    frag_color = vec4(lighting,1.0);
}