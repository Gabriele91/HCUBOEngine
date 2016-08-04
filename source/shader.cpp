#include <string>
#include <sstream>
#include <vector>
#include <cstdio>
#include <iostream>
#include <shader.h>
#include <OpenGL4.h>
#include <filesystem.h>


static bool log_error(unsigned int shader, int status,const std::string& type)
{
    if (!status)
    {
        //get len
        GLint info_len = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &info_len);
        //print
        if (info_len > 1)
        {
            char* info_log = (char*)malloc(sizeof(char) * info_len);
            glGetShaderInfoLog(shader, info_len, NULL, info_log);
            std::cout << type<<", error compiling shader:\n" << info_log << std::endl;
            free(info_log);
        }
        return false;
    }
    return true;
}
#if 0
static const char* glsl_type_to_string (GLenum type)
{
    switch (type)
    {
        case GL_BOOL: return "bool";
        case GL_INT: return "int";
        case GL_FLOAT: return "float";
        case GL_FLOAT_VEC2: return "vec2";
        case GL_FLOAT_VEC3: return "vec3";
        case GL_FLOAT_VEC4: return "vec4";
        case GL_FLOAT_MAT2: return "mat2";
        case GL_FLOAT_MAT3: return "mat3";
        case GL_FLOAT_MAT4: return "mat4";
        case GL_SAMPLER_2D: return "sampler2D";
        case GL_SAMPLER_3D: return "sampler3D";
        case GL_SAMPLER_CUBE: return "samplerCube";
        case GL_SAMPLER_2D_SHADOW: return "sampler2DShadow";
        default: break;
    }
    return "other";
}
#endif


inline void jmp_spaces(const char*& str)
{
    //jump space chars
    while((*str) == ' ' || (*str) == '\t' ) ++str;
}

inline bool compare_and_jmp_keyword(const char*& str,const char* keyword)
{
    //get len
    size_t len_str = std::strlen(keyword);
    //compare
    if(std::strncmp(str, keyword, len_str) == 0){ str += len_str; return true; };
    //else
    return false;
}

inline bool parse_cstring(const char* in_out,std::string& out,size_t& line)
{
    const char *tmp=in_out;
    out="";
    //start parse
    if((*tmp)=='\"')  //["...."]
    {
        ++tmp;  //[...."]
        while((*tmp)!='\"'&&(*tmp)!='\n')
        {
            if((*tmp)=='\\') //[\.]
            {
                ++tmp;  //[.]
                switch(*tmp)
                {
                    case 'n':
                        out+='\n';
                        break;
                    case 't':
                        out+='\t';
                        break;
                    case 'b':
                        out+='\b';
                        break;
                    case 'r':
                        out+='\r';
                        break;
                    case 'f':
                        out+='\f';
                        break;
                    case 'a':
                        out+='\a';
                        break;
                    case '\\':
                        out+='\\';
                        break;
                    case '?':
                        out+='\?';
                        break;
                    case '\'':
                        out+='\'';
                        break;
                    case '\"':
                        out+='\"';
                        break;
                    case '\n': /* jump unix */
                        ++line;
                        break;
                    case '\r': /* jump mac */
                        ++line;
                        if((*(tmp+1))=='\n') ++tmp; /* jump window (\r\n)*/
                        break;
                    default:
                        return true;
                        break;
                }
            }
            else
            {
                if((*tmp)!='\0') out+=(*tmp);
                else return false;
            }
            ++tmp;//next char
        }
        if((*tmp)=='\n') return false;
        in_out=tmp+1;
        return true;
    }
    return false;
}

inline bool process_include(std::string& output,
                            const std::string& path_file,
                            size_t& n_files,
                            size_t level = 1)
{
    //cicle test
    if(level > 16)
    {
        std::cout << "effect shader, header inclusion depth limit reached:" << std::endl
                  << "might be caused by cyclic header inclusion"          << std::endl;
        return false;
    }
    //get base dir
    std::string filedir = filesystem::get_directory(path_file);
    //read file
    std::string text_of_file = filesystem::text_file_read_all(path_file);
    text_of_file+="\n";
    //stream
    std::istringstream effect(text_of_file);
    //line buffer
    std::string effect_line;
    //count line
    size_t   line = 0;
    //inc n file
    size_t this_file = n_files++;
    //start line
    output += "#line 1 "+std::to_string(this_file)+"\n";
    //start to parse
    while (std::getline(effect, effect_line))
    {
        //line count
        ++line;
        //ptr to line
        const char* c_effect_line = effect_line.c_str();
        //pragma
        if(compare_and_jmp_keyword(c_effect_line,"#pragma"))
        {
            //jmp space
            jmp_spaces(c_effect_line);
            //include?
            if(compare_and_jmp_keyword(c_effect_line,"include"))
            {
                //jmp space
                jmp_spaces(c_effect_line);
                //path
                std::string path_include;
                //path
                std::string output_include;
                //get path
                if(!parse_cstring(c_effect_line, path_include,line))
                {
                    return false;
                }
                //add include
                if(!process_include(output_include,filedir+"/"+path_include,n_files,level+1))
                {
                    return false;
                }
                output+= output_include;
                output+= "#line "+std::to_string(line+1)+" "+std::to_string(this_file)+"\n";
                continue;
            }
        }
        //lines
        output += effect_line; output+="\n";
    }
    return true;
}

inline bool process_import(const std::string& path_effect_file,
						   const std::vector<std::string>& defines,
						   std::string& out_vertex,
						   std::string& out_fragment,
						   std::string& out_geometry,
						   size_t& n_files,
						   size_t level = 0)
{
	//cicle test
	if (level > 16)
	{
		std::cout << "effect shader, header inclusion depth limit reached:" << std::endl
			      << "might be caused by cyclic header inclusion" << std::endl;
		return false;
	}
	//state
	enum parsing_state
	{
		P_ALL,
		P_VERTEX,
		P_FRAGMENT,
		P_GEOMETRY,
		P_SIZE
	};
	//strings
	std::string buffers[P_SIZE];
	//get base dir
	std::string filedir = filesystem::get_directory(path_effect_file);
	//read file
	std::istringstream effect(filesystem::text_file_read_all(path_effect_file));
	//parsing
	std::string& all      = buffers[P_ALL];
	std::string& vertex   = buffers[P_VERTEX];
	std::string& fragment = buffers[P_FRAGMENT];
	std::string& geometry = buffers[P_GEOMETRY];
	//start state
	parsing_state state = P_ALL;
	//line buffer
	std::string effect_line;
	//count line
	size_t  line = 0;
	//count files
	size_t  this_file = n_files++;
	//put line of "all" buffer
	all += "#line 1 " + std::to_string(this_file) + "\n";
	//start to parse
	while (std::getline(effect, effect_line))
	{
		//line count
		++line;
		//ptr to line
		const char* c_effect_line = effect_line.c_str();
		//jmp space
		jmp_spaces(c_effect_line);
		//is pragma?
		if (compare_and_jmp_keyword(c_effect_line, "#pragma"))
		{
			//jmp space
			jmp_spaces(c_effect_line);
			//type
			if (compare_and_jmp_keyword(c_effect_line, "vertex"))
			{
				state = P_VERTEX;
				vertex += "#line " + std::to_string(line + 1) + " " + std::to_string(this_file) + "\n";
				continue;
			}
			else if (compare_and_jmp_keyword(c_effect_line, "fragment"))
			{
				state = P_FRAGMENT;
				fragment += "#line " + std::to_string(line + 1) + " " + std::to_string(this_file) + "\n";
				continue;
			}
			else if (compare_and_jmp_keyword(c_effect_line, "geometry"))
			{
				state = P_GEOMETRY;
				geometry += "#line " + std::to_string(line + 1) + " " + std::to_string(this_file) + "\n";
				continue;
			}
			else if (compare_and_jmp_keyword(c_effect_line, "include"))
			{
				//jmp space
				jmp_spaces(c_effect_line);
				//path
				std::string path_include;
				//path
				std::string output_include;
				//get path
				if (!parse_cstring(c_effect_line, path_include, line))
				{
					return false;
				}
				//add include
				if (!process_include(output_include, filedir + "/" + path_include, n_files, level+1))
				{
					return false;
				}
				//lines
				buffers[state] += output_include;
				buffers[state] += "#line " + std::to_string(line + 1) + " " + std::to_string(this_file) + "\n";
				continue;
			}
			else if (compare_and_jmp_keyword(c_effect_line, "import"))
			{
				//jmp space
				jmp_spaces(c_effect_line);
				//path
				std::string path_include;
				//get path
				if (!parse_cstring(c_effect_line, path_include, line))
				{
					return false;
				}
				//add include
				if (!process_import(filedir + "/" + path_include, defines, vertex, fragment, geometry, n_files, level+1))
				{
					return false;
				}
				//lines
				std::string current_line = "#line " + std::to_string(line + 1) + " " + std::to_string(this_file) + "\n";
				buffers[P_VERTEX]   += current_line;
				buffers[P_FRAGMENT] += current_line;
				buffers[P_GEOMETRY] += geometry.size() ? current_line : "";
				continue;
			}
		}
		//append to a buffer
		buffers[state] += effect_line; buffers[state] += "\n";
	}
	//copy output
	out_vertex   = all + vertex;
	out_fragment = all + fragment;
	out_geometry = geometry.size() ? all + geometry : "";

	return true;
}

bool shader::load(const std::string& effect_file,
                  const std::vector<std::string>& defines)
{
	//id file
	size_t  this_file = 0;
	//buffers
	std::string vertex, fragment, geometry;
	//process
	if (!process_import(effect_file, defines, vertex, fragment, geometry, this_file)) return false;
    //load shader
    return load_shader  ( vertex, 0,
                          fragment, 0,
                          geometry, 0,
                          defines  );
}


bool shader::load(const std::string& vs_file,
                  const std::string& fs_file,
                  const std::string& gs_file,
                  const std::vector<std::string>& defines)
{
    return load_shader
    (
        filesystem::text_file_read_all(vs_file), 0,
        filesystem::text_file_read_all(fs_file), 0,
        gs_file.size() ?
        filesystem::text_file_read_all(gs_file) : "", 0,
        defines
    );
}

bool shader::load_shader(const std::string& vs_str, size_t line_vs,
                         const std::string& fs_str, size_t line_fs,
                         const std::string& gs_str, size_t line_gs,
                         const std::vector<std::string>& defines)
{
    //delete last shader
    delete_program();
    //int variables
    GLint compiled = 0, linked = 0;
    //list define
    std::string defines_string;
    for (const std::string& define : defines) defines_string += "#define " + define + "\n";
    ////////////////////////////////////////////////////////////////////////////////
    // load shaders files
    // vertex
    std::string file_vs =
    "#version 400\n" +
    defines_string +
    "#define saturate(x) clamp( x, 0.0, 1.0 )       \n"
    "#define lerp        mix                        \n"
    "#line "+std::to_string(line_vs)+"\n" +
    vs_str;
    //create a vertex shader
    m_shader_vs = glCreateShader(GL_VERTEX_SHADER);
    const char* c_vs_source = file_vs.c_str();
    glShaderSource(m_shader_vs, 1, &c_vs_source, 0);
    //compiling
    compiled = 0;
    glCompileShader(m_shader_vs);
    glGetShaderiv(m_shader_vs, GL_COMPILE_STATUS, &compiled);
    if (!log_error(m_shader_vs, compiled,"vertex shader")){ glDeleteShader(m_shader_vs); }
    ////////////////////////////////////////////////////////////////////////////////
    //fragmentx
    std::string file_fs =
    "#version 400\n" +
    defines_string +
    "#define saturate(x) clamp( x, 0.0, 1.0 )       \n"
    "#define lerp        mix                        \n"
    "#define bestp\n"
    "#define highp\n"
    "#define mediump\n"
    "#define lowp\n"
    "#line "+std::to_string(line_fs)+"\n" +
    fs_str;
    //create a fragment shader
    m_shader_fs = glCreateShader(GL_FRAGMENT_SHADER);
    const char* c_fs_source = file_fs.c_str();
    glShaderSource(m_shader_fs, 1, &c_fs_source, 0);
    //compiling
    compiled = 0;
    glCompileShader(m_shader_fs);
    glGetShaderiv(m_shader_fs, GL_COMPILE_STATUS, &compiled);
    if (!log_error(m_shader_fs, compiled,"fragment shader")){ glDeleteShader(m_shader_fs); }
    ////////////////////////////////////////////////////////////////////////////////
    if(gs_str.size())
    {
        //geometrfs_stry
        std::string file_gs =
        "#version 400\n" +
        defines_string +
        "#define saturate(x) clamp( x, 0.0, 1.0 )       \n"
        "#define lerp        mix                        \n"
        "#line "+std::to_string(line_gs)+"\n" +
        gs_str;
        //geometry
        m_shader_gs = glCreateShader(GL_GEOMETRY_SHADER);
        const char* c_gs_source = file_gs.c_str();
        glShaderSource(m_shader_gs, 1, &(c_gs_source), 0);
        //compiling
        compiled = 0;
        glCompileShader(m_shader_gs);
        glGetShaderiv(m_shader_gs, GL_COMPILE_STATUS, &compiled);
        if (!log_error(m_shader_gs, compiled,"Geometry")){ glDeleteShader(m_shader_gs); }
    }
    ////////////////////////////////////////////////////////////////////////////////
    //made a shader program
    m_shader_id = glCreateProgram();
    glAttachShader(m_shader_id, m_shader_vs);
    glAttachShader(m_shader_id, m_shader_fs);
    if(m_shader_gs) glAttachShader(m_shader_id, m_shader_gs);
    glLinkProgram(m_shader_id);
    //get link status
    glGetProgramiv(m_shader_id, GL_LINK_STATUS, &linked);
    //out errors
    if (!linked)
    {
        GLint info_len = 0;
        glGetProgramiv(m_shader_id, GL_INFO_LOG_LENGTH, &info_len);
        if (info_len)
        {
            char* info_log = (char*)malloc(sizeof(char) * info_len);
            glGetProgramInfoLog(m_shader_id, info_len, NULL, info_log);
            std::cout << "error linking program: \n" << info_log << std::endl;
            free(info_log);
        }
        std::cout << "error linking" <<std::endl;
        //"stacca" gli schader dal shader program
        glDetachShader(m_shader_id, m_shader_fs);
        glDetachShader(m_shader_id, m_shader_vs);
        if(m_shader_gs)  glDetachShader(m_shader_id, m_shader_gs);
        //cancella gli shader
        glDeleteShader(m_shader_fs);
        glDeleteShader(m_shader_vs);
        if(m_shader_gs) glDeleteShader(m_shader_gs);
        //cancella lo shader program
        glDeleteProgram(m_shader_id);
        //all to 0
        m_shader_vs = 0;
        m_shader_fs = 0;
        m_shader_gs = 0;
        m_shader_id = 0;
        //fail
        return false;
    }
    return true;
}

void shader::delete_program()
{
    if (m_shader_id)
    {
        glUseProgram(0);
        //"stacca" gli schader dal shader program
        glDetachShader(m_shader_id, m_shader_fs);
        glDetachShader(m_shader_id, m_shader_vs);
        if(m_shader_gs) glDetachShader(m_shader_id, m_shader_gs);
        //cancella gli shader
        glDeleteShader(m_shader_fs);
        glDeleteShader(m_shader_vs);
        if(m_shader_gs) glDeleteShader(m_shader_gs);
        //cancella lo shader program
        glDeleteProgram(m_shader_id);
        //to null
        m_shader_gs = 0;
        m_shader_fs = 0;
        m_shader_vs = 0;
        m_shader_id = 0;
    }
}

shader::~shader()
{
    delete_program();
}

void shader::bind()
{
    //start texture uniform
    m_uniform_ntexture = -1;
    //uniform parogram shaders
    glUseProgram(m_shader_id);
}

//get uniform id
int shader::get_uniform_id(const char *name)
{
    return glGetUniformLocation(m_shader_id, name);
}

//disabilita shader
void shader::unbind()
{
    //disable textures
    while( m_uniform_ntexture >= 0 )
    {
        glActiveTexture((GLenum)(GL_TEXTURE0+m_uniform_ntexture));
        glBindTexture(GL_TEXTURE_2D, 0);
        --m_uniform_ntexture;
    }
    //disable program
    glUseProgram(0);
    //unbind vbo
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    //unbind ibo
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
//id programma
unsigned int shader::program_id() const
{
    return m_shader_id;
}

//start uniform
void shader::uniform()
{
    //restart texture uniform
    m_uniform_ntexture = -1;
}

//uniform

#define uniform_gl_class(class_name,T,uniform_gl_call)\
class class_name : public uniform<class_name> {\
\
GLint m_id{0};\
\
    public:\
    class_name( GLint id ): m_id(id){}\
    virtual void  set(const void* value, size_t s, size_t n)\
    {\
        uniform_gl_call ((GLuint)(m_id+s), (GLuint)n, (const T*)value); \
    }\
    virtual void  set(const void* value)\
    {\
        uniform_gl_call (m_id, 1, (const T*)value);\
    }\
    virtual void* get(){ return nullptr;}\
    virtual const void* get() const { return nullptr; }\
    virtual ~class_name(){}\
};

uniform_gl_class(uniform_gl_int, int, glUniform1iv)
uniform_gl_class(uniform_gl_float, float, glUniform1fv)
uniform_gl_class(uniform_gl_vec2, float, glUniform2fv)
uniform_gl_class(uniform_gl_vec3, float, glUniform3fv)
uniform_gl_class(uniform_gl_vec4, float, glUniform4fv)

class uniform_gl_texture : public uniform_texture
{
    GLint     m_unform_id{-1};
    GLint    m_id{ 0 };
    shader*   m_shader{ nullptr };
    
public:
    
    uniform_gl_texture(GLint id, shader* shader)
    :m_id(id)
    ,m_shader(shader)
    {}
    
    virtual void disable()
    {
        glActiveTexture((GLenum)(GL_TEXTURE0+m_id));
        glBindTexture(GL_TEXTURE_2D, 0);
        m_unform_id = -1;
    }
    
    virtual void  set(const void* value, size_t s, size_t n)
    {
        //uniform
        m_unform_id = (GLint)++m_shader->m_uniform_ntexture;
        //texture type
        glActiveTexture((GLenum)(GL_TEXTURE0+m_unform_id));
        glBindTexture(GL_TEXTURE_2D, ((texture*)value)->get_id());
        //bind id
        glUniform1i(m_id, m_unform_id);
    }
    
    virtual void  set(const void* value)
    {
        //void
    }
    
    virtual void* get(){ return NULL; }
    virtual const void* get() const { return NULL; }
    virtual ~uniform_gl_texture(){}
};

class uniform_gl_mat4 : public uniform < uniform_gl_mat4 >
{
    GLint m_id{ 0 };
public:
    
    uniform_gl_mat4(GLint id) : m_id(id){}
    
    virtual void  set(const void* value, size_t s, size_t n)
    {
        glUniformMatrix4fv((GLuint)(m_id + s), (GLuint)n, false, (const float*)value);
    }
    
    virtual void  set(const void* value)
    {
        glUniformMatrix4fv(m_id, 1, false, (const float*)value);
    }
    virtual void* get(){ return NULL; }
    virtual const void* get() const { return NULL; }
    virtual ~uniform_gl_mat4(){}
    
};


uniform_texture* shader::get_uniform_texture(const char *name)
{
    return (uniform_texture*)new uniform_gl_texture(get_uniform_id(name),this);
}

uniform_int* shader::get_uniform_int(const char *name)
{
    return (uniform_int*)new uniform_gl_int(get_uniform_id(name));
}

uniform_float* shader::get_uniform_float(const char *name)
{
    return (uniform_float*)new uniform_gl_float(get_uniform_id(name));
}

uniform_vec2* shader::get_uniform_vec2(const char *name)
{
    return (uniform_vec2*)new uniform_gl_vec2(get_uniform_id(name));
}

uniform_vec3* shader::get_uniform_vec3(const char *name)
{
    return (uniform_vec3*)new uniform_gl_vec3(get_uniform_id(name));
}

uniform_vec4* shader::get_uniform_vec4(const char *name)
{
    return (uniform_vec4*)new uniform_gl_vec4(get_uniform_id(name));
}

uniform_mat4* shader::get_uniform_mat4(const char *name)
{
    return (uniform_mat4*)new uniform_gl_mat4(get_uniform_id(name));
}

uniform_array_int* shader::get_uniform_array_int(const char *name)
{
    return (uniform_array_int*)new uniform_gl_int(get_uniform_id(name));
}

uniform_array_float* shader::get_uniform_array_float(const char *name)
{
    return (uniform_array_float*)new uniform_gl_float(get_uniform_id(name));
}

uniform_array_vec2* shader::get_uniform_array_vec2(const char *name)
{
    return (uniform_array_vec2*)new uniform_gl_vec2(get_uniform_id(name));
}

uniform_array_vec3* shader::get_uniform_array_vec3(const char *name)
{
    return (uniform_array_vec3*)new uniform_gl_vec3(get_uniform_id(name));
}

uniform_array_vec4* shader::get_uniform_array_vec4(const char *name)
{
    return (uniform_array_vec4*)new uniform_gl_vec4(get_uniform_id(name));
}

uniform_array_mat4* shader::get_uniform_array_mat4(const char *name)
{
    return (uniform_array_mat4*)new uniform_gl_mat4(get_uniform_id(name));
}



uniform_texture::ptr shader::get_shader_uniform_texture(const char *name){ return get_uniform_texture(name)->shared(); }
uniform_int::ptr shader::get_shader_uniform_int(const char *name){ return get_uniform_int(name)->shared(); }
uniform_float::ptr shader::get_shader_uniform_float(const char *name){ return get_uniform_float(name)->shared(); }
uniform_vec2::ptr shader::get_shader_uniform_vec2(const char *name){ return get_uniform_vec2(name)->shared(); }
uniform_vec3::ptr shader::get_shader_uniform_vec3(const char *name){ return get_uniform_vec3(name)->shared(); }
uniform_vec4::ptr shader::get_shader_uniform_vec4(const char *name){ return get_uniform_vec4(name)->shared(); }
uniform_mat4::ptr shader::get_shader_uniform_mat4(const char *name){ return get_uniform_mat4(name)->shared(); }

uniform_array_int::ptr shader::get_shader_uniform_array_int(const char *name){ return get_uniform_array_int(name)->shared(); }
uniform_array_float::ptr shader::get_shader_uniform_array_float(const char *name){ return get_uniform_array_float(name)->shared(); }
uniform_array_vec2::ptr shader::get_shader_uniform_array_vec2(const char *name){ return get_uniform_array_vec2(name)->shared(); }
uniform_array_vec3::ptr shader::get_shader_uniform_array_vec3(const char *name){ return get_uniform_array_vec3(name)->shared(); }
uniform_array_vec4::ptr shader::get_shader_uniform_array_vec4(const char *name){ return get_uniform_array_vec4(name)->shared(); }
uniform_array_mat4::ptr shader::get_shader_uniform_array_mat4(const char *name){ return get_uniform_array_mat4(name)->shared(); }