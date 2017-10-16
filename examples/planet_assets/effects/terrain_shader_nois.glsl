#define USE_VERTEX_SHADER
#pragma import <base_texture_normal_specular>
/////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////
float hash( const in float n ) 
{
    return fract(sin(n)*43758.5453123);
}

float hash( const in vec2 p ) 
{
	float h = dot(p,vec2(127.1,311.7));	
    return fract(sin(h)*43758.5453123);
}

float hash( const in vec3 p ) 
{
	float h = dot(p,vec3(127.1,311.7,758.5453123));	
    return fract(sin(h)*43758.5453123);
}

float noise( const in  float p ) 
{    
    float i = floor( p );
    float f = fract( p );	
	float u = f*f*(3.0-2.0*f);
    return -1.0+2.0* mix( hash( i + 0. ), hash( i + 1. ), u);
}

float noise2( const in vec2 p ) 
{    
    vec2 i = floor( p );
    vec2 f = fract( p );	
	vec2 u = f*f*(3.0-2.0*f);
    return -1.0+2.0*mix( mix( hash( i + vec2(0.0,0.0) ), 
                     hash( i + vec2(1.0,0.0) ), u.x),
                mix( hash( i + vec2(0.0,1.0) ), 
                     hash( i + vec2(1.0,1.0) ), u.x), u.y);
}

float noise3( const in  vec3 x ) 
{
    vec3 p = floor(x);
    vec3 f = fract(x);
    f = f*f*(3.0-2.0*f);
    float n = p.x + p.y*157.0 + 113.0*p.z;
    return mix(mix(mix( hash(n+  0.0), hash(n+  1.0),f.x),
                   mix( hash(n+157.0), hash(n+158.0),f.x),f.y),
               mix(mix( hash(n+113.0), hash(n+114.0),f.x),
                   mix( hash(n+270.0), hash(n+271.0),f.x),f.y),f.z);
}

float terrain_map_height(const in vec3 n)
{
  float h=0.0;
  float f=640000.0;
  
  while(f>40.0f)	
  {
	h=h*0.5+ noise3(n*f);
	f/=2.0f;
  }
  
  return ((1.0-clamp(h*0.5,0.0,1.0)))*0.5;
}
/////////////////////////////////////////////////////////
#pragma vertex

uniform vec2 uv_height_offset;
uniform vec2 uv_height_step;
uniform sampler2D height_map;

//define function
void vertex_shader(out vertex_output output)
{
	//compute uv
	output.m_uvmap = uv_height_offset + vertex.xy * uv_height_step;
	//compute height
	float height = 
	  terrain_map_height(vec3(output.m_uvmap.x,1.0,output.m_uvmap.y)) 
	* texture( height_map, output.m_uvmap ).r
    -0.5;
	//compute vertex
	vec4  position = vec4(output.m_uvmap.x - 0.5, height, output.m_uvmap.y - 0.5, 1.0);
	//compute vertex in model space
	output.m_world_position = transform.m_model * position;
	//compute vertex in projection space
	output.m_position = camera.m_projection * camera.m_view * output.m_world_position;
}   