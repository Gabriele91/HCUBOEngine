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

//texture buffer type
struct texture_type
{
    GLenum m_internal_format = GL_RGBA32F;
    GLenum m_format          = GL_RGBA;
    GLenum m_type            = GL_FLOAT;
    
    texture_type(){}
    
    texture_type(GLenum type_0,GLenum type_1,GLenum type_2)
    {
        m_internal_format = type_0;
        m_format          = type_1;
        m_type            = type_2;
    }
};

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

    //types
    texture_type types[G_BUFFER_NUM_TEXTURES];
    
    //specify type
    types[G_BUFFER_TEXTURE_TYPE_POSITION] = texture_type( GL_RGBA32F, GL_RGBA,  GL_FLOAT );
    types[G_BUFFER_TEXTURE_TYPE_NORMAL]   = texture_type( GL_RGB16F, GL_RGB,  GL_FLOAT );
    types[G_BUFFER_TEXTURE_TYPE_ALBEDO]   = texture_type( GL_RGBA,   GL_RGBA, GL_UNSIGNED_BYTE );
    
    //create texture
    for (unsigned int i = 0 ; i != G_BUFFER_NUM_TEXTURES ; i++)
    {
        glBindTexture(GL_TEXTURE_2D, m_textures[i]);
        glTexImage2D(GL_TEXTURE_2D,
                     0,
                     types[i].m_internal_format,
                     width,
                     height,
                     0,
                     types[i].m_format,
                     types[i].m_type,
                     NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
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

void g_buffer::destoy()
{
	if (m_fbo) glDeleteFramebuffers(1, &m_fbo);
	for (unsigned int i = 0; i != G_BUFFER_NUM_TEXTURES; i++)
	{
		if(m_textures[i]) glDeleteTextures(1, &m_textures[i]);
		m_textures[i] = 0;
	}
	if (m_depth_texture) glDeleteTextures(1, &m_depth_texture);

	m_fbo = 0;
	m_depth_texture = 0;
	m_last_depth_n_text = 0;
	m_width  = 0 ;
	m_height = 0 ;

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

void g_buffer::disable_texture(G_BUFFER_TEXTURE_TYPE texture_type)
{
	switch (texture_type)
	{
	case G_BUFFER_TEXTURE_TYPE_POSITION:
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);
	break;

	case G_BUFFER_TEXTURE_TYPE_NORMAL:
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, 0);
	break;

	case G_BUFFER_TEXTURE_TYPE_ALBEDO:
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, 0);
	break;

	case g_buffer::G_BUFFER_NUM_TEXTURES:
	default:
		assert(0);
	break;
	}

}

void g_buffer::set_read_buffer_depth()
{
    glReadBuffer(GL_DEPTH_ATTACHMENT);
}

void g_buffer::set_texture_buffer_depth(GLenum n_texture)
{
	m_last_depth_n_text = n_texture;
    glActiveTexture(GL_TEXTURE0+n_texture);
    glBindTexture(GL_TEXTURE_2D, m_depth_texture);
}	

void g_buffer::disable_depth_texture()
{
	glActiveTexture(GL_TEXTURE0 + m_last_depth_n_text);
	glBindTexture(GL_TEXTURE_2D, 0);
	m_last_depth_n_text = 0;
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
