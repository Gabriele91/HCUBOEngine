//
//  shader.h
//  OGLHCubeView
//
//  Created by Gabriele on 28/06/16.
//  Copyright © 2016 Gabriele. All rights reserved.
//
#pragma once
#include <memory>
#include <string>
#include <vector>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <texture.h>
#include <smart_pointers.h>

template <class T>
class uniform
{
public:
    
    using ptr = std::shared_ptr< T >;

    ptr shared(){ return ptr((T*)this); }
    
    virtual ~uniform(){};

protected:
    virtual void  set(const void*) = 0;
    virtual void  set(const void*, size_t i,size_t n) = 0;
    virtual void* get() = 0;
    virtual const void* get() const = 0;
};

class uniform_texture : public uniform<uniform_texture>
{
    
public:
    
    uniform_texture(){}
    
    void enable(texture* texture)
    {
        set(texture, (size_t)0, (size_t)0);
    }
    
    void enable(texture::ptr texture)
    {
        set(texture.get(), (size_t)0, (size_t)0);
    }
    
    void set_value(texture::ptr texture)
    {
        set(texture.get(), (size_t)0, (size_t)0);
    }
    
    void set_value(texture* texture)
    {
        set(texture, (size_t)0, (size_t)0);
    }
   
    virtual void disable()=0;
    
    operator const texture*() const
    {
        return (const texture*)(get());
    }
    
};

class uniform_int : public uniform<uniform_int>
{
public:
    
    uniform_int(int i)
    {
        set(&i);
    }
    operator int() const
    {
        return *((int*)(this->get()));
    }
    void set_value(int i)
    {
        set(&i);
    }
    int operator = (int i)
    {
        set(&i);
        return i;
    }
};

class uniform_float : public uniform<uniform_float>
{
public:
    
    uniform_float(float f)
    {
        set(&f);
    }
    
    operator float() const
    {
        return *((float*)(this->get()));
    }
    
    void set_value(float f)
    {
        set(&f);
    }
    
    float operator = (float f)
    {
        set(&f);
        return f;
    }
};

class uniform_vec2 : public uniform<uniform_vec2>
{
public:
    uniform_vec2(const glm::vec2& v2)
    {
        set(&v2.x);
    }
    operator const glm::vec2&() const
    {
        return *((glm::vec2*)(this->get()));
    }
    void set_value(const glm::vec2& v2)
    {
        set(&v2.x);
    }
    const glm::vec2& operator = (const glm::vec2& v2)
    {
        set(&v2.x);
        return v2;
    }
};

class uniform_vec3 : public uniform<uniform_vec3>
{
public:
    
    uniform_vec3(const glm::vec3& v3)
    {
        set(&v3.x);
    }
    
    operator const glm::vec3&() const
    {
        return *((glm::vec3*)(this->get()));
    }
    
    void set_value(const glm::vec3& v3)
    {
        set(&v3.x);
    }
    
    const glm::vec3& operator = (const glm::vec3& v3)
    {
        set(&v3.x);
        return v3;
    }
};

class uniform_vec4 : public uniform<uniform_vec4>
{
public:
    
    uniform_vec4(const glm::vec4& v4)
    {
        set(&v4.x);
    }
    
    operator const glm::vec4&() const
    {
        return *((glm::vec4*)(this->get()));
    }
    
    void set_value(const glm::vec4& v4)
    {
        set(&v4.x);
    }
    
    const glm::vec4& operator = (const glm::vec4& v4)
    {
        set(&v4.x);
        return v4;
    }
};

class uniform_mat4 : public uniform<uniform_mat4>
{
public:
    uniform_mat4(const glm::mat4 & m4)
    {
        set(&m4[0]);
    }
    operator const glm::mat4&() const
    {
        return *((glm::mat4*)(this->get()));
    }
    void set_value(const glm::mat4& m4)
    {
        set(&m4[0]);
    }
    const glm::mat4& operator = (const glm::mat4& m4)
    {
        set(&m4[0]);
        return m4;
    }
};

class uniform_array_int : public uniform<uniform_array_int>
{
public:
    void set(int i, size_t n)
    {
        uniform::set(&i, n, 1);
    }
    
    void set(int* i, size_t n)
    {
        uniform::set(i, 0, n);
    }
    
    int get(size_t n) const
    {
        return ((const int*)uniform::get())[n];
    }
};

class uniform_array_float : public uniform<uniform_array_float>
{
public:
    void set(float f, size_t n)
    {
        uniform::set(&f, n, 1);
    }
    
    void set(float* f, size_t n)
    {
        uniform::set(f, 0, n);
    }
    
    float get(size_t n) const
    {
        return ((const float*)uniform::get())[n];
    }
};

class uniform_array_vec2 : public uniform<uniform_array_vec2>
{
public:
    void set(const glm::vec2& v2, size_t n)
    {
        uniform::set(&v2, n, 1);
    }
    
    void set(glm::vec2* v2, size_t n)
    {
        uniform::set(v2, 0, n);
    }
    
    glm::vec2 get(size_t n) const
    {
        return ((const glm::vec2*)uniform::get())[n];
    }
};

class uniform_array_vec3 : public uniform<uniform_array_vec3>
{
public:
    void set(const glm::vec3& v3, size_t n)
    {
        uniform::set(&v3, n, 1);
    }
    void set(glm::vec3* v3, size_t n)
    {
        uniform::set(v3, 0, n);
    }
    glm::vec3 get(size_t n) const
    {
        return ((const glm::vec3*)uniform::get())[n];
    }
};

class uniform_array_vec4 : public uniform<uniform_array_vec3>
{
public:
    void set(const glm::vec4& v4, size_t n)
    {
        uniform::set(&v4, n, 1);
    }
    void set(glm::vec4* v4, size_t n)
    {
        uniform::set(v4, 0, n);
    }
    glm::vec4 get(size_t n) const
    {
        return ((const glm::vec4*)uniform::get())[n];
    }
};
class uniform_array_mat4 : public uniform<uniform_array_mat4>
{
public:
    void set(const glm::mat4& v4, size_t n)
    {
        uniform::set(&v4, n, 1);
    }
    
    void set(glm::mat4* v4, size_t n)
    {
        uniform::set(v4, 0, n);
    }
    
    glm::mat4 get(size_t n) const
    {
        return ((const glm::mat4*)uniform::get())[n];
    }
};

class shader : public smart_pointers<shader>, public resource
{

public:

    shader(){}
    virtual ~shader();
    
    bool load(resources_manager& resources,const std::string& path)
    {
        return load(path);
    }
    
    bool load(const std::string& effect)
    {
        std::vector<std::string> defines;
        return load(effect, defines);
    }
    
    
    bool load(const std::string& vs, const std::string& fs)
    {
        std::vector<std::string> defines;
        return load(vs, fs, "", defines);
    }
    
    bool load(const std::string& vs, const std::string& fs, const std::string& gs)
    {
        std::vector<std::string> defines;
        return load(vs, fs, gs, defines);
    }
    
    bool load(const std::string& vs, const std::string& fs,  const std::vector<std::string>& defines)
    {
        return load(vs, fs, "", defines);
    }
    
    bool load(const std::string& effect, const std::vector<std::string>& defines);
    
    bool load(const std::string& vs, const std::string& fs, const std::string& gs, const std::vector<std::string>& defines);
    
    bool load_shader(const std::string& vs, size_t line_vs,
                     const std::string& fs, size_t line_fs,
                     const std::string& gs, size_t line_gs,
                     const std::vector<std::string>& defines);
    
    //get consts
    uniform_texture*   get_uniform_texture(const char *name);
    uniform_int*       get_uniform_int(const char *name);
    uniform_float*     get_uniform_float(const char *name);
    uniform_vec2*      get_uniform_vec2(const char *name);
    uniform_vec3*      get_uniform_vec3(const char *name);
    uniform_vec4*      get_uniform_vec4(const char *name);
    uniform_mat4*      get_uniform_mat4(const char *name);
    
    uniform_array_int*   get_uniform_array_int(const char *name);
    uniform_array_float* get_uniform_array_float(const char *name);
    uniform_array_vec2*  get_uniform_array_vec2(const char *name);
    uniform_array_vec3*  get_uniform_array_vec3(const char *name);
    uniform_array_vec4*  get_uniform_array_vec4(const char *name);
    uniform_array_mat4*  get_uniform_array_mat4(const char *name);
    
    uniform_texture::ptr  get_shader_uniform_texture(const char *name);
    uniform_int::ptr      get_shader_uniform_int(const char *name);
    uniform_float::ptr    get_shader_uniform_float(const char *name);
    uniform_vec2::ptr     get_shader_uniform_vec2(const char *name);
    uniform_vec3::ptr     get_shader_uniform_vec3(const char *name);
    uniform_vec4::ptr     get_shader_uniform_vec4(const char *name);
    uniform_mat4::ptr     get_shader_uniform_mat4(const char *name);
    
    uniform_array_int::ptr get_shader_uniform_array_int(const char *name);
    uniform_array_float::ptr get_shader_uniform_array_float(const char *name);
    uniform_array_vec2::ptr get_shader_uniform_array_vec2(const char *name);
    uniform_array_vec3::ptr get_shader_uniform_array_vec3(const char *name);
    uniform_array_vec4::ptr get_shader_uniform_array_vec4(const char *name);
    uniform_array_mat4::ptr get_shader_uniform_array_mat4(const char *name);
    
    //imposta shader
    virtual void bind();
    virtual void unbind();
    virtual void uniform();
    
    //get uniform id
    int get_uniform_id(const char *name);
  
    //id programma
    unsigned int program_id() const;

protected:
    
    //count uniforms texture
    long m_uniform_ntexture{ -1 };
    
    //friend class
    friend class uniform_gl_texture;
    
    
private:
    
    unsigned int m_shader_id{ 0 };
    unsigned int m_shader_vs{ 0 };
    unsigned int m_shader_fs{ 0 };
    unsigned int m_shader_gs{ 0 };
    void delete_program();

};
