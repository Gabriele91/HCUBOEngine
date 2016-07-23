//
//  static_model.h
//  OGLHCubeView
//
//  Created by Gabriele on 05/07/16.
//  Copyright © 2016 Gabriele. All rights reserved.
//
#pragma once
#include <vector>
#include <mesh.h>
#include <material.h>
#include <resource.h>

class static_model : public smart_pointers< static_model >,
				     public resource,
					 public renderable
{
public:


	struct sub_model
	{
		material_ptr m_material;
		mesh::ptr m_mesh;
	};

	std::vector< sub_model > m_sub_models;

	void draw(camera& cam, const glm::mat4& model, material_ptr material);
	bool load(resources_manager& resources, const std::string& path);
};