//
//  material.h
//  OGLHCubeView
//
//  Created by Gabriele on 29/06/16.
//  Copyright © 2016 Gabriele. All rights reserved.
//
#pragma once
#include <vector>
#include <hcube/config.h>
#include <hcube/math/vector_math.h>
#include <hcube/core/smart_pointers.h>
#include <hcube/core/resource.h>
#include <hcube/render/effect.h>

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
	class HCUBE_API material : public resource
	{
	public:

		virtual ~material();

		bool load(resources_manager& resources, const std::string& path);

		effect::ptr			get_effect() const;

		effect::parameters* get_parameters() const;

		material_ptr copy() const;

		enum default_parameters
		{
			MAT_DEFAULT_DIFFUSE_MAP,
			MAT_DEFAULT_NORMAL_MAP,
			MAT_DEFAULT_SPECULAR_MAP,
			MAT_DEFAULT_COLOR,
            MAT_DEFAULT_MASK,
			MAT_DEFAULT_MAX
		};

		const effect::parameter* get_default_parameter(default_parameters dp) const;
		const effect::parameter* get_parameter_by_name(const std::string& name) const;

		effect::parameter* get_default_parameter(default_parameters dp);
		effect::parameter* get_parameter_by_name(const std::string& name);

	protected:

		//effect class
		effect::ptr m_effect;
		//effect parameters
		effect::parameters* m_parameters;
		//dafault params
		int m_default_parameters[MAT_DEFAULT_MAX]{ -1 };
	};
}