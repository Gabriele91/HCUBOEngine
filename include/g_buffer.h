//
//  g_buffer.h
//  HCubo
//
//  Created by Gabriele on 19/07/16.
//  Copyright © 2016 Gabriele. All rights reserved.
//
#pragma once
#include <OpenGL4.h>
#include <glm/vec2.hpp>

class g_buffer
{
public:
    
    enum G_BUFFER_TEXTURE_TYPE
    {
        G_BUFFER_TEXTURE_TYPE_POSITION,
		G_BUFFER_TEXTURE_TYPE_NORMAL,
        G_BUFFER_TEXTURE_TYPE_ALBEDO,
        G_BUFFER_NUM_TEXTURES
    };
    
	g_buffer() {}
    
	virtual ~g_buffer() { destoy(); }
	
    bool init(const glm::ivec2& window_size);
    
    bool init(unsigned int width, unsigned int height);

	void destoy();

    void bind_for_writing();

	void bind();

	void unbind();
    
    void bind_for_reading();
    
    void set_read_buffer(G_BUFFER_TEXTURE_TYPE texture_type);
    
    void set_texture_buffer(G_BUFFER_TEXTURE_TYPE texture_type);

	void disable_texture(G_BUFFER_TEXTURE_TYPE texture_type);
    
    void set_read_buffer_depth();
    
    void set_texture_buffer_depth(GLenum n_texture=0);

	void disable_depth_texture();

    unsigned int get_width() const;
    
    unsigned int get_height() const;
    
    glm::ivec2 get_size() const;
    
private:
    
    unsigned int m_width    { 0 };
    unsigned int m_height   { 0 };
    
	GLuint m_fbo{ 0 };
	GLuint m_textures[G_BUFFER_NUM_TEXTURES]{ 0 };
	GLuint m_depth_texture		{ 0 };
	GLenum m_last_depth_n_text  { 0 };
};

