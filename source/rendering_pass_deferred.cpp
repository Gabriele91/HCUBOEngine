#include <rendering_pass_deferred.h>
#include <basic_meshs.h>
#include <transform.h>


void rendering_pass_deferred::uniform_light::get_uniform(int i, shader::ptr shader)
{
    std::string lights_i("lights[" + std::to_string(i) + "]");
    m_uniform_type     = shader->get_shader_uniform_int((lights_i + ".m_type").c_str());
    
    m_uniform_position = shader->get_shader_uniform_vec3((lights_i + ".m_position").c_str());
    m_uniform_direction = shader->get_shader_uniform_vec3((lights_i + ".m_direction").c_str());
    
    m_uniform_diffuse  = shader->get_shader_uniform_vec3((lights_i + ".m_diffuse").c_str());
    m_uniform_specular  = shader->get_shader_uniform_vec3((lights_i + ".m_specular").c_str());
    
    m_uniform_constant  = shader->get_shader_uniform_float((lights_i + ".m_constant").c_str());
    m_uniform_linear    = shader->get_shader_uniform_float((lights_i + ".m_linear").c_str());
	m_uniform_quadratic = shader->get_shader_uniform_float((lights_i + ".m_quadratic").c_str());
    
    m_uniform_inner_cut_off = shader->get_shader_uniform_float((lights_i + ".m_inner_cut_off").c_str());
    m_uniform_outer_cut_off = shader->get_shader_uniform_float((lights_i + ".m_outer_cut_off").c_str());
}

void rendering_pass_deferred::uniform_light::uniform(light_wptr weak_light,const glm::mat4& view,const glm::mat4& model)
{
    auto light = weak_light.lock();
    m_uniform_type->set_value(light->m_type);
    m_uniform_position->set_value((glm::vec3)(view * model * glm::vec4(0,0,0,1.0)));
    m_uniform_direction->set_value((glm::vec3)(view * model * glm::vec4(0,0,1.0,0.0)));
    m_uniform_diffuse->set_value(light->m_diffuse);
    m_uniform_specular->set_value(light->m_specular);
    m_uniform_constant->set_value(light->m_constant);
    m_uniform_linear->set_value(light->m_linear);
	m_uniform_quadratic->set_value(light->m_quadratic);
    m_uniform_inner_cut_off->set_value(light->m_inner_cut_off);
    m_uniform_outer_cut_off->set_value(light->m_outer_cut_off);
}

rendering_pass_deferred::rendering_pass_deferred(const glm::ivec2& w_size, resources_manager& resources)
{
	m_q_size = w_size;
	m_g_buffer.init(w_size);
	m_ssao.init(w_size, resources);
	m_ssao.set_kernel_size(8);
	m_ssao.set_radius(2.0);

	m_square    = basic_meshs::square3D({ 2.0,2.0 }, true);
	m_shader    = resources.get_shader("deferred_light");
	m_position  = m_shader->get_shader_uniform_int("g_vertex");
	m_normal    = m_shader->get_shader_uniform_int("g_normal");
	m_albedo    = m_shader->get_shader_uniform_int("g_albedo_spec");
	m_occlusion = m_shader->get_shader_uniform_int("g_occlusion");
	//uniforms
	m_ambient_light = m_shader->get_shader_uniform_vec4("ambient_light");
	m_uniform_n_lights_used = m_shader->get_shader_uniform_int("n_lights_used");
	//alloc lights
	m_uniform_lights.resize(m_max_lights);
	//get unfirom lights
    for (unsigned i = 0; i != m_max_lights; ++i)
    {
        m_uniform_lights[i].get_uniform(i, m_shader);
    }
}

void rendering_pass_deferred::set_ambient_occlusion(bool enable)
{
	if (m_enable_ambient_occlusion && !enable)
	{
		m_ssao.clear();
	}
	m_enable_ambient_occlusion = enable;
}

bool rendering_pass_deferred::is_enable_ambient_occlusion() const
{
	return m_enable_ambient_occlusion;
}

void rendering_pass_deferred::draw_pass(glm::vec4&  clear_color,
                                        glm::vec4&  ambient_color,
                                        entity::ptr e_camera,
										render_queues& queues)
{
    camera::ptr   c_camera = e_camera->get_component<camera>();
    transform_ptr t_camera = e_camera->get_component<transform>();
	const glm::vec4& viewport = c_camera->get_viewport();
    glViewport(viewport.x, viewport.y, viewport.z, viewport.w);
    glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);

	m_g_buffer.bind();
    
	glClearColor(ambient_color.r, ambient_color.g, ambient_color.b, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//draw
	for (render_queues::element*
		 weak_element = queues.m_cull_opaque;
		 weak_element;
		 weak_element = weak_element->m_next)
	{
		auto entity = weak_element->lock();
		auto t_entity   = entity->get_component<transform>();
		auto r_entity   = entity->get_component<renderable>();
        auto e_material = r_entity->get_material();
        //material
        if (e_material)
        {
            e_material->bind_state();
            e_material->bind(
                viewport,
                c_camera->get_projection(),
                t_camera->get_matrix_inv(),
                t_entity->get_matrix()
            );
        }
        //draw
        entity->get_component<renderable>()->draw();
        //material
        if (e_material)
        {
            e_material->unbind();
            e_material->unbind_state();
        }
	}

	m_g_buffer.unbind();
	////////////////////////////////////////////////////////////////////////////////
	//clear frame buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	////////////////////////////////////////////////////////////////////////////////
	//ssao pass
	if(m_enable_ambient_occlusion)
	{
		m_ssao.applay(e_camera, m_g_buffer, m_square);
	}
	////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////
	m_shader->bind();
	//set texture g_buffer
	m_g_buffer.set_texture_buffer(g_buffer::G_BUFFER_TEXTURE_TYPE_POSITION);
	m_g_buffer.set_texture_buffer(g_buffer::G_BUFFER_TEXTURE_TYPE_NORMAL);
	m_g_buffer.set_texture_buffer(g_buffer::G_BUFFER_TEXTURE_TYPE_ALBEDO);
	//set texture ssao
	m_ssao.set_texture(3);
	//set uniform id
	m_position->set_value(0);
	m_normal->set_value(1);
	m_albedo->set_value(2);
	m_occlusion->set_value(3);
	//add info
	m_ambient_light->set_value(ambient_color);
    //compute max lights
    unsigned max_lights = std::min(m_max_lights,(unsigned)queues.m_lights.size());
	m_uniform_n_lights_used->set_value(max_lights);
	//uniform lights
    for (unsigned i = 0; i != max_lights; ++i)
    {
        auto entity = queues.m_lights[i].lock();
		m_uniform_lights[i].uniform( entity->get_component<light>(),
									 t_camera->get_matrix_inv(),
                                     entity->get_component<transform>()->get_matrix());
	}
	//draw squere
	m_square->draw();
	//unbind texture
	m_g_buffer.disable_texture(g_buffer::G_BUFFER_TEXTURE_TYPE_POSITION);
	m_g_buffer.disable_texture(g_buffer::G_BUFFER_TEXTURE_TYPE_NORMAL);
	m_g_buffer.disable_texture(g_buffer::G_BUFFER_TEXTURE_TYPE_ALBEDO);
	m_ssao.disable_texture();
	//bind
	m_shader->unbind();
	////////////////////////////////////////////////////////////////////////////////
}