//
//  text_mesh.h
//  OGLHCubeView
//
//  Created by Gabriele on 05/07/16.
//  Copyright © 2016 Gabriele. All rights reserved.
//
#include <string>
#include <renderable.h>
#include <smart_pointers.h>

class text_mesh :  public smart_pointers< text_mesh >,  public renderable
{
    std::u32string m_text;
    size_t         m_text_max_size;
    unsigned int   m_bpoints;
    void           update_mesh();
    
public:
    
    text_mesh(size_t text_max_size=255);
    virtual ~text_mesh();
    
    void set_text(const std::string& text);
    void set_text(const std::u32string& text);
	void draw();
    void draw(camera& cam,
			  const glm::mat4& model,
		      material_ptr material);
};