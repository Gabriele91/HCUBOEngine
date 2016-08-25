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
#include <effect.h>

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

		effect::ptr			get_effect() const;

		effect::parameters* get_parameters() const;

		material_ptr copy() const;

	protected:

		effect::ptr m_effect;
		effect::parameters* m_parameters;

	};
}