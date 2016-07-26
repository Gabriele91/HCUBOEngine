#pragma vertex
//in
layout(location = 0) in vec3 vertex;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uvcoord;
//out
out vec2 frag_uvcoord;

void main()
{
	gl_Position  = vec4(vertex, 1.0f);
	frag_uvcoord = uvcoord;
}

#pragma fragment
//consts
const int max_kernel_size = 64;
//in
in vec2 frag_uvcoord;
//out
out float frag_occlusion;
//uniform
uniform sampler2D g_position;
uniform sampler2D g_normal;
uniform sampler2D t_noise;

uniform mat4 projection;
uniform vec3 samples[max_kernel_size];
uniform vec2 noise_scale;
uniform vec2 near_far;
uniform int kernel_size = 64;
uniform float radius = 2.0;

#define USE_ZBUFFER

#ifdef USE_ZBUFFER
float linear_z(float depth)
{
	float near = near_far.x;
	float far = near_far.y;
	float z = depth * 2.0 - 1.0;
    return (2.0 * near * far) / (far + near - z * (far - near));  
}	
#endif

void main()
{
	//get info
	vec3  position = texture(g_position, frag_uvcoord).rgb;
	vec3  normal   = texture(g_normal, frag_uvcoord).rgb;
	vec3  randv    = texture(t_noise, frag_uvcoord*noise_scale).rgb;

	//unpack
	normal = normalize(normal * 2.0 - 1.0);

	// Create TBN change-of-basis matrix: from tangent-space to view-space
	vec3 tangent   = normalize(randv - normal * dot(randv, normal));
	vec3 bitangent = cross(normal, tangent);
	mat3 TBN       = mat3(tangent, bitangent, normal);

	//init occlusion
	float occlusion = 0.0;

	//calc occlusion
	for (int i = 0; i < kernel_size; ++i)
	{
		// get sample position
		vec3 sample_k = normalize(TBN * samples[i]); // From tangent to view-space
		sample_k = position + sample_k * radius;

		// project sample position (to sample texture) (to get position on screen/texture)
		vec4 offset = vec4(sample_k, 1.0);
		offset      = projection * offset;    // from view to clip-space
		offset.xyz /= offset.w;			      // perspective divide
		offset.xyz  = offset.xyz * 0.5 + 0.5; // transform to range 0.0 - 1.0

		// get sample depth
#ifdef USE_ZBUFFER
		float sample_depth = -linear_z(texture(g_position, offset.xy).w); // Get depth value of kernel sample
#else
		float sample_depth = texture(g_position, offset.xy).z;
#endif
		// range check & accumulate
		float range_check = smoothstep(0.0, 1.0, radius / abs(position.z - sample_depth));
		occlusion += (sample_depth >= sample_k.z ? 1.0 : 0.0) * range_check;
	}

	frag_occlusion = 1.0 - (occlusion / kernel_size);
}