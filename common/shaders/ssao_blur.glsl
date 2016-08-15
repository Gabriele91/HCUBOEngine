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
out float frag_occlusion_blur;
//uniform
uniform sampler2D g_ssao_input;


void main() 
{
	vec2 tp_size = 1.0 / vec2(textureSize(g_ssao_input, 0));
	float result = 0.0;
	for (int x = -2; x < 2; ++x)
	{
		for (int y = -2; y < 2; ++y)
		{
			vec2 offset = vec2(float(x), float(y)) * tp_size;
			result += texture(g_ssao_input, frag_uvcoord + offset).r;
		}
	}
	frag_occlusion_blur = result / (4.0 * 4.0);
}