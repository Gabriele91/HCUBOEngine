#pragma once
#include <mesh.h>
#include <glm/vec3.hpp>

namespace basic_meshs
{
	mesh::ptr cube(const glm::vec3& size, bool uvmap = false);
	mesh::ptr square3D(const glm::vec2& size, bool uvmap = false);
};