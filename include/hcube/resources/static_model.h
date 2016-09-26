//
//  static_model.h
//  OGLHCubeView
//
//  Created by Gabriele on 05/07/16.
//  Copyright © 2016 Gabriele. All rights reserved.
//
#pragma once
#include <vector>
#include <hcube/config.h>
#include <hcube/core/smart_pointers.h>
#include <hcube/resources/prefab.h>
#include <hcube/components/mesh.h>

namespace hcube
{
	class HCUBE_API static_model : public prefab, public smart_pointers< static_model >
	{
	public:


		struct sub_model
		{
			mesh::ptr m_mesh;
		};

		std::vector< sub_model > m_sub_models;

		bool load(resources_manager& resources, const std::string& path);

		virtual entity::ptr instantiate();

		virtual prefab_ptr copy() const;
	};
}