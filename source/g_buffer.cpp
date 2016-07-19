//
//  g_buffer.cpp
//  HCubo
//
//  Created by Gabriele on 19/07/16.
//  Copyright © 2016 Gabriele. All rights reserved.
//
#include <iostream>
#include <string>
#include <g_buffer.h>

static std::string framebuffer_error_to_str(GLenum error)
{
    switch (error)
    {
        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:         return "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT";
//      case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS:         return "GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS";
        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: return "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT";
        case GL_FRAMEBUFFER_UNSUPPORTED:                   return "GL_FRAMEBUFFER_UNSUPPORTED";
        case GL_FRAMEBUFFER_COMPLETE:                      return "GL_FRAMEBUFFER_COMPLETE";
        default:                                           return "GL_UNKNOW";
    }
}
bool g_buffer::init(const glm::ivec2& window_size)
{
    return init(window_size.x,window_size.y);
}

bool g_buffer::init(unsigned int width, unsigned int height)
{
    //save size
    m_width  = width;
    m_height = height;
    
    // Create the FBO
    glGenFramebuffers(1, &m_fbo);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
    
    // Create the gbuffer textures
    glGenTextures(G_BUFFER_NUM_TEXTURES, m_textures);
    glGenTextures(1, &m_depth_texture);
    
    // draw buffers to attach
    GLenum draw_buffers[G_BUFFER_NUM_TEXTURES] = {0};
    
    for (unsigned int i = 0 ; i != G_BUFFER_NUM_TEXTURES ; i++)
    {
        glBindTexture(GL_TEXTURE_2D, m_textures[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, m_textures[i], 0);
        draw_buffers[i] = GL_COLOR_ATTACHMENT0 + i;
    }

	//attach to fbo
	glDrawBuffers(G_BUFFER_NUM_TEXTURES, draw_buffers);
    
    // depth
    glBindTexture(GL_TEXTURE_2D, m_depth_texture);
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_DEPTH_COMPONENT32F,
                 width,
                 height,
                 0,
                 GL_DEPTH_COMPONENT,
                 GL_FLOAT,
                 NULL);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depth_texture, 0);
    
    //status
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    
    //test
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << framebuffer_error_to_str(status) << std::endl;
	}

    // restore default FBO
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    
    //success?
    return status == GL_FRAMEBUFFER_COMPLETE;
}

void g_buffer::bind_for_writing()
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
}

void g_buffer::bind_for_reading()
{
    glBindFramebuffer(GL_READ_FRAMEBUFFER, m_fbo);
}

void g_buffer::bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
}

void g_buffer::unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void g_buffer::set_read_buffer(G_BUFFER_TEXTURE_TYPE texture_type)
{
    glReadBuffer(GL_COLOR_ATTACHMENT0 + texture_type);
}

void g_buffer::set_texture_buffer(G_BUFFER_TEXTURE_TYPE texture_type)
{
	switch (texture_type)
	{
	case G_BUFFER_TEXTURE_TYPE_POSITION:
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_textures[G_BUFFER_TEXTURE_TYPE_POSITION]);
	break;

	case G_BUFFER_TEXTURE_TYPE_NORMAL:
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, m_textures[G_BUFFER_TEXTURE_TYPE_NORMAL]);
	break;

	case G_BUFFER_TEXTURE_TYPE_ALBEDO:
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, m_textures[G_BUFFER_TEXTURE_TYPE_ALBEDO]);
	break;
	
	case g_buffer::G_BUFFER_NUM_TEXTURES:
	default:
		assert(0);
	break;
	}

}

unsigned int g_buffer::get_width() const
{
    return m_width;
}

unsigned int g_buffer::get_height() const
{
    return m_height;
}

glm::ivec2 g_buffer::get_size() const
{
    return glm::ivec2{m_width,m_height};
}
