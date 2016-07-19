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
    
	~g_buffer() {}
    
    bool init(const glm::ivec2& window_size);
    
    bool init(unsigned int width, unsigned int height);
    
    void bind_for_writing();

	void bind();

	void unbind();
    
    void bind_for_reading();
    
    void set_read_buffer(G_BUFFER_TEXTURE_TYPE texture_type);
	
	void set_texture_buffer(G_BUFFER_TEXTURE_TYPE texture_type);

    unsigned int get_width() const;
    
    unsigned int get_height() const;
    
    glm::ivec2 get_size() const;
    
private:
    
    unsigned int m_width    { 0 };
    unsigned int m_height   { 0 };
    
    GLuint m_fbo;
    GLuint m_textures[G_BUFFER_NUM_TEXTURES];
    GLuint m_depth_texture;
};

