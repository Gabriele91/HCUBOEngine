#include <rendering_pass_deferred.h>
#include <basic_meshs.h>


void rendering_pass_deferred::light::get_uniform(int i, shader::ptr shader)
{
	std::string lights_i("lights[" + std::to_string(i) + "]");
	m_uniform_position = shader->get_shader_uniform_vec3((lights_i + ".m_position").c_str());
	m_uniform_diffuse = shader->get_shader_uniform_vec4((lights_i + ".m_diffuse").c_str());
	m_uniform_const = shader->get_shader_uniform_float((lights_i + ".m_const").c_str());
	m_uniform_linear = shader->get_shader_uniform_float((lights_i + ".m_linear").c_str());
	m_uniform_quadratic = shader->get_shader_uniform_float((lights_i + ".m_quadratic").c_str());
}

void rendering_pass_deferred::light::uniform()
{
	m_uniform_position->set_value(m_position);
	m_uniform_diffuse->set_value(m_diffuse);
	m_uniform_const->setValue(m_const);
	m_uniform_linear->setValue(m_linear);
	m_uniform_quadratic->setValue(m_quadratic);
}

rendering_pass_deferred::rendering_pass_deferred(camera::ptr camera, resources_manager& resources)
{
	m_g_buffer.init(camera->get_viewport_size());
	m_square = basic_meshs::square3D({ 2.0,2.0 }, true);
	m_shader = resources.get_shader("deferred_light");
	m_vertex = m_shader->get_shader_uniform_int("g_vertex");
	m_normal = m_shader->get_shader_uniform_int("g_normal");
	m_albedo = m_shader->get_shader_uniform_int("g_albedo_spec");
	//uniforms
	m_view_pos = m_shader->get_shader_uniform_vec3("view_pos");
	m_ambient_light = m_shader->get_shader_uniform_vec4("ambient_light");
	m_uniform_n_lights_used = m_shader->get_shader_uniform_int("n_lights_used");
	//alloc lights
	m_lights.resize(m_max_lights);
	//get unfirom lights
	for (unsigned i = 0; i != m_max_lights; ++i) m_lights[i].get_uniform(i, m_shader);
}


void rendering_pass_deferred::draw_pass(camera::ptr camera, std::vector< entity::ptr >& entities)
{
	const glm::vec4& viewport = camera->get_viewport();
	glViewport(viewport.x, viewport.y, viewport.z, viewport.w);

	m_g_buffer.bind();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (entity::ptr& entity : entities)
	{
		entity->m_material->bind_state();
		entity->m_material->bind(camera.get(), entity->m_model);
		entity->m_renderable->draw();
		entity->m_material->unbind();
		entity->m_material->unbind_state();
	}

	m_g_buffer.unbind();

	m_shader->bind();
	//set texture
	m_g_buffer.set_texture_buffer(g_buffer::G_BUFFER_TEXTURE_TYPE_POSITION);
	m_g_buffer.set_texture_buffer(g_buffer::G_BUFFER_TEXTURE_TYPE_NORMAL);
	m_g_buffer.set_texture_buffer(g_buffer::G_BUFFER_TEXTURE_TYPE_ALBEDO);
	//set uniform id
	m_vertex->setValue(0);
	m_normal->setValue(1);
	m_albedo->setValue(2);
	//add info
	m_view_pos->set_value(camera->get_position());
	m_ambient_light->set_value({ 0.16, 0.16, 0.16, 1.0 });
	m_uniform_n_lights_used->setValue(m_n_lights_used);
	//uniform lights
	for (unsigned i = 0; i != m_max_lights; ++i)
	{
		m_lights[i].uniform();
	}
	//draw squere
	m_square->draw();
	//bind
	m_shader->unbind();
}