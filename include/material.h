//
//  material.h
//  OGLHCubeView
//
//  Created by Gabriele on 29/06/16.
//  Copyright © 2016 Gabriele. All rights reserved.
//
#pragma once
#include <render.h>
#include <camera.h>
#include <vector>
#include <texture.h>
#include <shader.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <smart_pointers.h>

namespace hcube
{
	//class declaration
	class material;
	//pointer declaration
	using material_ptr = std::shared_ptr< material >;
	using material_uptr = std::unique_ptr< material >;
	using material_wptr = std::weak_ptr  < material >;
	template < class... Args >
	static inline material_ptr material_snew(Args&&... args)
	{
		return std::make_shared< material >(args...);
	}
	//class definition
	class material : public resource
	{
	public:

		virtual ~material();

		bool load(resources_manager& resources, const std::string& path);

		void cullface(cullface_state& cfs);

		void blend(const blend_state& bls);

		virtual void bind(const glm::vec4& viewport,
			const glm::mat4& projection,
			const glm::mat4& view,
			const glm::mat4& model);

		virtual void unbind();

		void  bind_state();

		void unbind_state();

		material_ptr copy() const;

	protected:

		cullface_state m_temp_cullface;
		cullface_state m_cullface{ CF_BACK };

		blend_state    m_temp_blend;
		blend_state    m_blend;

		//shader
		shader::ptr  m_shader{ nullptr };
		//standard uniform
		uniform* m_uniform_projection{ nullptr };
		uniform* m_uniform_view{ nullptr };
		uniform* m_uniform_model{ nullptr };
		uniform* m_uniform_viewport{ nullptr };
		//uniform textures
		std::vector< texture::ptr > m_textures;
		std::vector< uniform* >     m_uniform_textures;
		//uniform float
		std::vector< float >    m_floats;
		std::vector< uniform* > m_uniform_floats;
		//uniform int
		std::vector< int >      m_ints;
		std::vector< uniform* > m_uniform_ints;
		//uniform vec2
		std::vector< glm::vec2 > m_vec2s;
		std::vector< uniform* > m_uniform_vec2s;
		//uniform vec3
		std::vector< glm::vec3 > m_vec3s;
		std::vector< uniform* > m_uniform_vec3s;
		//uniform vec4
		std::vector< glm::vec4 > m_vec4s;
		std::vector< uniform* > m_uniform_vec4s;
		//uniform mat4
		std::vector< glm::mat4 > m_mat4s;
		std::vector< uniform* >  m_uniform_mat4s;
	};
}