#if defined( RENDERING_COLOR )

    //output
    out vec4 frag_color;
    //define
    #define output_fragment(position,normal,albedo,specular) forward_color_output_fragment(albedo)
    //compute
    void forward_color_output_fragment(in vec4 albedo)
    {
        //output
        frag_color = albedo;
    }

#elif defined( RENDERING_AMBIENT_LIGHT )

    //uniform
    uniform vec4 ambient_light;
    //output
    out vec4 frag_color;
    //define
    #define output_fragment(position,normal,albedo,specular) forward_ambient_color_output_fragment(albedo)
    //compute
    void forward_ambient_color_output_fragment(in vec4 albedo)
    {
        //output
        frag_color = albedo * ambient_light;
    }

#else

	#pragma include "uniform.glsl"

    #if defined( RENDERING_SPOT_LIGHT )

        //light uniform
        #pragma include "spot_light.glsl"
        //uniform
        uniform spot_light light;
        //macro
        #define in_light light
        #define output_light_computation spot_light_res
        #define compute_light(light,position,view_dir,normal,shininess,light_results)\
            compute_spot_light(light, position, view_dir, normal, shininess, light_results)

    #elif defined( RENDERING_POINT_LIGHT )

        //light uniform
        #pragma include "point_light.glsl"
        //uniform
        uniform point_light light;
        //macro
        #define in_light light
        #define output_light_computation point_light_res
        #define compute_light(light,position,view_dir,normal,shininess,light_results)\
            compute_point_light(light, position, view_dir, normal, shininess, light_results)

    #elif defined( RENDERING_DIRECTION_LIGHT )

        //light uniform
        #pragma include "direction_light.glsl"
        //uniform
        uniform direction_light light;
        //macro
        #define in_light light
        #define output_light_computation direction_light_res
        #define compute_light(light,position,view_dir,normal,shininess,light_results)\
            compute_direction_light(light, view_dir, normal, shininess, light_results)

    #endif

    //output
    out vec4 frag_color;
    //define
    #define output_fragment(position,normal,albedo,specular)\
        forward_light_output_fragment(position,normal,albedo,specular)
    //compute
    void forward_light_output_fragment(in vec4  position,
                                       in vec3  normal,
                                       in vec4  albedo,
                                       in float specular)
    {
        //todo: material
        float shininess = 16.0f;
                
        //view dir
        vec3 view_dir = normalize(camera.m_position - position.xyz);
        
        //result
        output_light_computation light_results;
        
        // Then calculate lighting as usual
        compute_light(in_light,
                      position,
                      view_dir,
                      normal,
                      shininess,
                      light_results);
        //output
        frag_color = vec4(albedo.rgb*(light_results.m_diffuse + light_results.m_specular * specular),albedo.a);
    }
#endif