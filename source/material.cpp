//
//  material.cpp
//  HCubo
//
//  Created by Gabriele on 21/07/16.
//  Copyright © 2016 Gabriele. All rights reserved.
//
#include <list>
#include <cstdlib>
#include <cstring>
#include <sstream>
#include <iostream>
#include <material.h>
#include <filesystem.h>
#include <resources_manager.h>

#define CSTRCMP( x, y ) ( std::strncmp( x, y, sizeof(y)-1 ) == 0 )
#define CSTRCMP_SKIP( x, y ) ( [&x] () -> bool { if ( CSTRCMP(x,y) ) { x += sizeof(y)-1; return true; } return false; } )()

class material_parser
{
public:
    
    struct context
    {
        struct ctx_blend
        {
            bool m_enable;
            std::string m_source;
            std::string m_destination;
        }
        m_blend;
        
        struct ctx_cullmode
        {
            bool m_enable;
            std::string m_mode;
        }
        m_cullmode;
        
        struct ctx_shader
        {
            struct ctx_texture_field
            {
                std::string  m_uniform;
                std::string m_texture;
            };
            
            struct ctx_uniform_field
            {
                enum ctx_type
                {
                    T_INT,
                    T_FLOAT,
                    T_VEC2,
                    T_VEC3,
                    T_VEC4,
                    T_MAT4,
                };
                
                struct ctx_values
                {
                    float     m_float;
                    int       m_int;
                    glm::vec2 m_vec2;
                    glm::vec3 m_vec3;
                    glm::vec4 m_vec4;
                    glm::mat4 m_mat4;
                };
                
                std::string  m_uniform;
                ctx_type     m_type;
                ctx_values   m_value;
            };
            std::string m_shader_name;
            std::vector< ctx_texture_field > m_textures;
            std::vector< ctx_uniform_field > m_uniforms;
            
        }
        m_shader;
    };
    
    static std::string cullface_to_string(cullface_type cullface)
    {
        switch (cullface)
        {
            case CF_BACK: return "back";
            case CF_FRONT: return "front";
            case CF_FRONT_AND_BACK: return "front_and_back";
            default: return "";
        }
    }
    
    static cullface_type cullface_from_string(const std::string& cullface,cullface_type cullface_default)
    {
        if(cullface=="back")           return CF_BACK;
        if(cullface=="front")          return CF_FRONT;
        if(cullface=="front_and_back") return CF_FRONT_AND_BACK;
        return cullface_default;
    }
    
    static std::string blend_to_string(blend_type blend)
    {
        switch (blend)
        {
            case BLEND_ONE: return "one";
            case BLEND_ZERO: return "zero";
                
            case BLEND_ONE_MINUS_DST_COLOR: return "one_minus_dst_color";
            case BLEND_ONE_MINUS_DST_ALPHA: return "one_minus_dst_alpha";
            case BLEND_ONE_MINUS_SRC_COLOR: return "one_minus_src_color";
            case BLEND_ONE_MINUS_SRC_ALPHA: return "one_minus_src_alpha";
                
                
            case BLEND_DST_COLOR: return "dst_color";
            case BLEND_DST_ALPHA: return "dst_alpha";
                
            case BLEND_SRC_COLOR: return "src_color";
            case BLEND_SRC_ALPHA: return "src_alpha";
            case BLEND_SRC_ALPHA_SATURATE: return "src_apha_sature";
            default: return "";
        }
    }
    
    static blend_type blend_from_string(const std::string& blend,blend_type blend_default)
    {
        //
        if(blend=="one") return BLEND_ONE;
        if(blend=="zero") return BLEND_ZERO;
        //
        if(blend=="one_minus_dst_color") return BLEND_ONE_MINUS_DST_COLOR;
        if(blend=="one_minus_dst_alpha") return BLEND_ONE_MINUS_DST_ALPHA;
        if(blend=="one_minus_src_color") return BLEND_ONE_MINUS_SRC_COLOR;
        if(blend=="one_minus_src_alpha") return BLEND_ONE_MINUS_SRC_ALPHA;
        //
        if(blend=="dst_color") return BLEND_DST_COLOR;
        if(blend=="dst_alpha") return BLEND_DST_ALPHA;
        //
        if(blend=="src_color") return BLEND_SRC_COLOR;
        if(blend=="src_alpha") return BLEND_SRC_ALPHA;
        if(blend=="src_apha_sature") return BLEND_SRC_ALPHA_SATURATE;
        
        return blend_default;
    }
    
    bool parse(std::string& source)
    {
        context ctx_default;
        const char* source_ptr = source.c_str();
        return parse(ctx_default,source_ptr);
    }
    
    bool parse(context& default_context,const std::string& source)
    {
        const char* source_ptr = source.c_str();
        return parse(default_context,source_ptr);
    }
    
    bool parse(context& default_context,const char*& ptr)
    {
        //restart
        m_errors.clear();
        m_line = 1;
        m_context = default_context;
        //skeep line and comments
        skeep_space_end_comment(ptr);
        //get type
        while(*ptr != EOF && *ptr != '\0')
        {
            //parsing a block
                 if(is_blend_and_skip(ptr))      { if(!blend(ptr)) return false; }
            else if(is_cullface_and_skip(ptr))   { if(!cullface(ptr)) return false; }
            else if(is_shader_and_skip(ptr))     { if(!shader(ptr)) return false; }
            else { push_error("Not found a valid command"); return false; }
            //skeep line and comments
            skeep_space_end_comment(ptr);
        }
        return true;
    }
    
    
    const context& get_context() const
    {
        return m_context;
    }
    
    std::string errors_to_string() const
    {
        std::stringstream sbuffer;
        
        for(auto& error:m_errors)
        {
            sbuffer << "Error:" << error.m_line << ": " << error.m_error << "\n";
        }
        
        return sbuffer.str();
    }
    
protected:
    
    //////////////////////////////////////////////////////////////
    struct error_field
    {
        size_t m_line;
        std::string m_error;
    };
    size_t                    m_line;
    std::list < error_field > m_errors;
    context m_context;
    //////////////////////////////////////////////////////////////
    bool blend(const char*& ptr)
    {
        //skeep spaces
        skeep_space_end_comment(ptr);
        //get true/false
        if(!parse_bool(ptr,&ptr,m_context.m_blend.m_enable))
        {
            push_error("Not valid blend value");
            return false;
        }
        //skeep spaces
        skeep_space_end_comment(ptr);
        //parse table
        if(is_start_table(*ptr))
        {
            //skeep '{'
            ++ptr;
            //read all values
            while (!is_end_table(*ptr) && *ptr != EOF && *ptr != '\0')
            {
                //skeep spaces
                skeep_space_end_comment(ptr);
                //search source attribute
                if( CSTRCMP_SKIP(ptr, "source") || CSTRCMP_SKIP(ptr, "src") )
                {
                    //skeep "line" space
                    skeep_line_space(ptr);
                    //parse name
                    if(!parse_name(ptr,&ptr,m_context.m_blend.m_source))
                    {
                        push_error("Not valid blend.source value");
                        return false;
                    }
                    //skeep spaces
                    skeep_space_end_comment(ptr);
                }
                else if( CSTRCMP_SKIP(ptr, "destination") || CSTRCMP_SKIP(ptr, "dst") )
                {
                    
                    //skeep "line" space
                    skeep_line_space(ptr);
                    //parse name
                    if(!parse_name(ptr,&ptr,m_context.m_blend.m_destination))
                    {
                        push_error("Not valid blend.destination value");
                        return false;
                    }
                    //skeep spaces
                    skeep_space_end_comment(ptr);
                }
                else
                {
                    push_error("Keyword not valid");
                    return false;
                }
                
            }
            //end while
            if(!is_end_table(*ptr))
            {
                push_error("Not found }");
                return false;
            }
            //skip }
            ++ptr;
        }
        return true;
    }
    
    bool cullface(const char*& ptr)
    {
        //skeep spaces
        skeep_space_end_comment(ptr);
        //get true/false
        if(!parse_bool(ptr,&ptr,m_context.m_cullmode.m_enable))
        {
            push_error("Not valid cullface value");
            return false;
        }
        //skeep spaces
        skeep_space_end_comment(ptr);
        //parse table
        if(is_start_table(*ptr))
        {
            //skeep '{'
            ++ptr;
            //read all values
            while (!is_end_table(*ptr) && *ptr != EOF && *ptr != '\0')
            {
                //skeep spaces
                skeep_space_end_comment(ptr);
                //search source attribute
                if( CSTRCMP_SKIP(ptr, "mode") )
                {
                    //skeep "line" space
                    skeep_line_space(ptr);
                    //parse name
                    if(!parse_name(ptr,&ptr,m_context.m_cullmode.m_mode))
                    {
                        push_error("Not valid mode value");
                        return false;
                    }
                    //skeep spaces
                    skeep_space_end_comment(ptr);
                }
                else
                {
                    push_error("Keyword not valid");
                    return false;
                }
                
            }
            //end while
            if(!is_end_table(*ptr))
            {
                push_error("Not found }");
                return false;
            }
            //skip }
            ++ptr;
        }
        return true;
    }
    
    bool shader(const char*& ptr)
    {
        //skeep spaces
        skeep_space_end_comment(ptr);
        //get true/false
        if(!parse_cstring(ptr,&ptr,m_context.m_shader.m_shader_name))
        {
            push_error("Not valid shader name");
            return false;
        }
        //skeep spaces
        skeep_space_end_comment(ptr);
        //parse table
        if(is_start_table(*ptr))
        {
            //skeep '{'
            ++ptr;
            //read all values
            while (!is_end_table(*ptr) && *ptr != EOF && *ptr != '\0')
            {
                //skeep spaces
                skeep_space_end_comment(ptr);
                //search source attribute
                if( CSTRCMP_SKIP(ptr, "textures") )
                {
                    //skeep spaces
                    skeep_space_end_comment(ptr);
                    //parse textures
                    if(!textures(ptr)) return false;
                    //skeep spaces
                    skeep_space_end_comment(ptr);
                }
                else if( CSTRCMP_SKIP(ptr, "uniforms") )
                {
                    //skeep spaces
                    skeep_space_end_comment(ptr);
                    //parse textures
                    if(!uniforms(ptr)) return false;
                    //skeep spaces
                    skeep_space_end_comment(ptr);
                }
                else
                {
                    push_error("Keyword not valid");
                    return false;
                }
                
            }
            //end while
            if(!is_end_table(*ptr))
            {
                push_error("Not found }");
                return false;
            }
            //skip }
            ++ptr;
        }
        return true;
    }
    
    bool textures(const char*& ptr)
    {
        //skeep spaces
        skeep_space_end_comment(ptr);
        //parse table
        if(is_start_table(*ptr))
        {
            //skeep '{'
            ++ptr;
            //read all values
            while (!is_end_table(*ptr) && *ptr != EOF && *ptr != '\0')
            {
                //skeep spaces
                skeep_space_end_comment(ptr);
                //uniform name
                std::string uniform_name;
                //parse
                if(!parse_name(ptr, &ptr, uniform_name))
                {
                    push_error("Not valid uniform name");
                    return false;
                }
                //skeep "line" space
                skeep_line_space(ptr);
                //texture name
                std::string texture_name;
                //parse
                if(!parse_cstring(ptr, &ptr, texture_name))
                {
                    push_error("Not valid texture name");
                    return false;
                }
                //push
                m_context.m_shader.m_textures.push_back({ uniform_name,texture_name });
                //skeep spaces
                skeep_space_end_comment(ptr);
            }
            //end while
            if(!is_end_table(*ptr))
            {
                push_error("Not found }");
                return false;
            }
            //skip }
            ++ptr;
        }
        return true;
    }
    
    bool uniforms(const char*& ptr)
    {
        //skeep spaces
        skeep_space_end_comment(ptr);
        //parse table
        if(is_start_table(*ptr))
        {
            //skeep '{'
            ++ptr;
            //read all values
            while (!is_end_table(*ptr) && *ptr != EOF && *ptr != '\0')
            {
                //skeep spaces
                skeep_space_end_comment(ptr);
                //alloc uniform field
                context::ctx_shader::ctx_uniform_field field;
                //parse
                if(!parse_name(ptr, &ptr, field.m_uniform))
                {
                    push_error("Not valid uniform name");
                    return false;
                }
                //skeep "line" space
                skeep_line_space(ptr);
                //value
                if(!parse_value(ptr,field))
                {
                    push_error("Not valid uniform field");
                    return false;
                }
                //push
                m_context.m_shader.m_uniforms.push_back(field);
                //skeep spaces
                skeep_space_end_comment(ptr);
            }
            //end while
            if(!is_end_table(*ptr))
            {
                push_error("Not found }");
                return false;
            }
            //skip }
            ++ptr;
        }
        return true;
    }
    
    bool parse_value(const char*& ptr, context::ctx_shader::ctx_uniform_field& field)
    {
        if(!parse_type(ptr,&ptr,field.m_type)) return false;
        //skeep spaces
        skeep_space_end_comment(ptr);
        //parse '('
        if(!is_start_arg(*ptr)) return false;
        //jump '('
        ++ptr;
        //space
        skeep_space_end_comment(ptr);
        //parse by type
        switch (field.m_type)
        {
            case context::ctx_shader::ctx_uniform_field::ctx_type::T_INT:
                if(!parse_int(ptr, &ptr, field.m_value.m_int)) return false;
            break;
            case context::ctx_shader::ctx_uniform_field::ctx_type::T_FLOAT:
                if(!parse_float(ptr, &ptr, field.m_value.m_float)) return false;
            break;
            case context::ctx_shader::ctx_uniform_field::ctx_type::T_VEC2:
                if(!parse_vec2(ptr, field)) return false;
            break;
            case context::ctx_shader::ctx_uniform_field::ctx_type::T_VEC3:
                if(!parse_vec3(ptr, field)) return false;
            break;
            case context::ctx_shader::ctx_uniform_field::ctx_type::T_VEC4:
                if(!parse_vec4(ptr, field)) return false;
            break;
            case context::ctx_shader::ctx_uniform_field::ctx_type::T_MAT4:
                if(!parse_mat4(ptr, field)) return false;
            break;
            default: return false; break;
        }
        //jump space
        skeep_space_end_comment(ptr);
        //parse ')'
        if(!is_end_arg(*ptr))  return false;
        //jump ')'
        ++ptr;
        //...
        return true;
    }
    
    bool parse_vec2(const char*& ptr, context::ctx_shader::ctx_uniform_field& field)
    {
        if(!parse_float(ptr, &ptr, field.m_value.m_vec2.x)) return false;
        
        skeep_space_end_comment(ptr);
        if(!is_comm_arg(*ptr)) return false; else ++ptr;
        skeep_space_end_comment(ptr);
        
        if(!parse_float(ptr, &ptr, field.m_value.m_vec2.y)) return false;
        
        return true;
    }
    
    bool parse_vec3(const char*& ptr, context::ctx_shader::ctx_uniform_field& field)
    {
        if(!parse_float(ptr, &ptr, field.m_value.m_vec3.x)) return false;
        
        skeep_space_end_comment(ptr);
        if(!is_comm_arg(*ptr)) return false; else ++ptr;
        skeep_space_end_comment(ptr);
        
        if(!parse_float(ptr, &ptr, field.m_value.m_vec3.y)) return false;
        
        skeep_space_end_comment(ptr);
        if(!is_comm_arg(*ptr)) return false; else ++ptr;
        skeep_space_end_comment(ptr);
        
        if(!parse_float(ptr, &ptr, field.m_value.m_vec3.z)) return false;
        
        return true;
    }
    
    bool parse_vec4(const char*& ptr, context::ctx_shader::ctx_uniform_field& field)
    {
        if(!parse_float(ptr, &ptr, field.m_value.m_vec4.x)) return false;
        
        skeep_space_end_comment(ptr);
        if(!is_comm_arg(*ptr)) return false; else ++ptr;
        skeep_space_end_comment(ptr);
        
        if(!parse_float(ptr, &ptr, field.m_value.m_vec4.y)) return false;
        
        skeep_space_end_comment(ptr);
        if(!is_comm_arg(*ptr)) return false; else ++ptr;
        skeep_space_end_comment(ptr);
        
        if(!parse_float(ptr, &ptr, field.m_value.m_vec4.z)) return false;
        
        skeep_space_end_comment(ptr);
        if(!is_comm_arg(*ptr)) return false; else ++ptr;
        skeep_space_end_comment(ptr);
        
        if(!parse_float(ptr, &ptr, field.m_value.m_vec4.w)) return false;
        
        return true;
    }
    
    
    
    bool parse_mat4(const char*& ptr, context::ctx_shader::ctx_uniform_field& field)
    {
        //first
        if(!parse_float(ptr, &ptr, field.m_value.m_mat4[0][0])) return false;
        //for all
        for(int x=0;x!=4;++x)
        for(int y=0;y!=4;++y)
        {
            //jmp first
            if((x+y)==0) continue;
            //parse ','
            skeep_space_end_comment(ptr);
            if(!is_comm_arg(*ptr)) return false; else ++ptr;
            skeep_space_end_comment(ptr);
            //parse value
            if(!parse_float(ptr, &ptr, field.m_value.m_mat4[x][y])) return false;
        }
        
        return true;
    }
    
    static bool is_blend_and_skip(const char*& c)
    {
        return CSTRCMP_SKIP(c,"blend");
    }
    
    static bool is_cullface_and_skip(const char*& c)
    {
        return CSTRCMP_SKIP(c,"cullface");
    }
    
    static bool is_shader_and_skip(const char*& c)
    {
        return CSTRCMP_SKIP(c,"shader");
    }
    
    static bool is_textures_and_skip(const char*& c)
    {
        return CSTRCMP_SKIP(c,"textures");
    }
    
    static bool is_uniforms_and_skip(const char*& c)
    {
        return CSTRCMP_SKIP(c,"uniforms");
    }
    //////////////////////////////////////////////////////
    static bool is_line_space(char c)
    {
        return 	 c == ' ' || c == '\t';
    }
    
    static bool is_space(char c)
    {
        return 	 c == ' ' || c == '\t' || c == '\r' || c == '\n';
    }
    
    static bool is_start_name(char c)
    {
        return 	 (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
    }
    
    static bool is_char_name(char c)
    {
        return 	 (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c=='_' ;
    }
    
    static bool is_uint_number(char c)
    {
        return 	 (c >= '0' && c <= '9');
    }
    
    static bool is_start_int_number(char c)
    {
        return 	 (c >= '0' && c <= '9')|| c=='-' ;
    }
    
    static bool is_start_float_number(char c)
    {
        return 	 (c >= '0' && c <= '9')|| c=='-' || c=='.' ;
    }
    
    static bool is_start_table(char c)
    {
        return (c == '{');
    }
    
    static bool is_end_table(char c)
    {
        return (c == '}');
    }
    
    static bool is_start_arg(char c)
    {
        return (c == '(');
    }
    
    static bool is_comm_arg(char c)
    {
        return (c == ',');
    }
    
    static bool is_end_arg(char c)
    {
        return (c == ')');
    }
    
    static bool is_start_obj_list(char c)
    {
        return (c == '|');
    }
    
    static bool is_line_comment(const char* c)
    {
        return (*c)=='/' && (*(c+1))=='/';
    }
    
    static bool is_start_multy_line_comment(const char* c)
    {
        return (*c)=='/' && (*(c+1))=='*';
    }
    
    static bool is_end_multy_line_comment(const char* c)
    {
        return (*c)=='*' && (*(c+1))=='/';
    }
    
    void skeep_line_comment(const char*& inout)
    {
        if(is_line_comment(inout))
        {
            while(*(inout)!=EOF &&
                  *(inout)!='\0'&&
                  *(inout)!='\n')
                ++(inout);
        }
    }
    
    void skeep_multy_line_comment(const char*& inout)
    {
        if(is_start_multy_line_comment(inout))
        {
            while(*(inout)!=EOF &&
                  *(inout)!='\0'&&
                  !is_end_multy_line_comment(inout))
            {
                m_line+=(*(inout))=='\n';
                ++(inout);
            }
            if((*(inout))=='*') ++(inout);
        }
    }
    
    void skeep_space_end_comment(const char*& inout)
    {
        while(is_space(*(inout))||
              is_line_comment(inout)||
              is_start_multy_line_comment(inout))
        {
            skeep_line_comment(inout);
            skeep_multy_line_comment(inout);
            m_line+=(*(inout))=='\n';
            ++(inout);
        }
    }
    
    void skeep_line_space(const char*& inout)
    {
        while(is_line_space(*inout)) ++(inout);
    }
    //////////////////////////////////////////////////////
    bool parse_bool(const char* in,const char** cout,bool& out)
    {
        if( CSTRCMP(in,"true") )  { (*cout)+=4; out = true;  return true; }
        if( CSTRCMP(in,"false") ) { (*cout)+=5; out = false; return true; };
        return false;
    }
    
    bool parse_int(const char* in,const char** cout,int& out)
    {
        out = (int)std::strtod(in,(char**)cout);
        return in!=(*cout);
    }
    
    bool parse_float(const char* in,const char** cout,float& out)
    {
        out = (float)std::strtod(in,(char**)cout);
        return in!=(*cout);
    }
    
    bool parse_name(const char* in,const char** cout,std::string& out)
    {
        if (!is_start_name(*in)) return false;
		out = "";
        out+=*in;
        ++in;
        while(is_char_name(*in))
        {
            out+=*in;
            ++in;
        }
        (*cout)=in;
        return true;
    }
    
    bool parse_cstring(const char* in,const char** cout,std::string& out)
    {
        const char *tmp=in;
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
                            ++m_line;
                            break;
                        case '\r': /* jump mac */
                            ++m_line;
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
            if(cout) (*cout)=tmp+1;
            return true;
        }
        return false;
    }
    //////////////////////////////////////////////////////
    bool parse_type(const char* in,
                    const char** cout,
                    context::ctx_shader::ctx_uniform_field::ctx_type& type)
    {
        if( CSTRCMP(in,"int") )
        { (*cout)+=3; type = context::ctx_shader::ctx_uniform_field::ctx_type::T_INT;  return true; }
        
        if( CSTRCMP(in,"float") )
        { (*cout)+=5; type = context::ctx_shader::ctx_uniform_field::ctx_type::T_FLOAT;  return true; }
        
        if( CSTRCMP(in,"vec2") )
        { (*cout)+=4; type = context::ctx_shader::ctx_uniform_field::ctx_type::T_VEC2;  return true; }
        
        if( CSTRCMP(in,"vec3") )
        { (*cout)+=4; type = context::ctx_shader::ctx_uniform_field::ctx_type::T_VEC3;  return true; }
        
        if( CSTRCMP(in,"vec4") )
        { (*cout)+=4; type = context::ctx_shader::ctx_uniform_field::ctx_type::T_VEC4;  return true; }
        
        if( CSTRCMP(in,"mat4") )
        { (*cout)+=4; type = context::ctx_shader::ctx_uniform_field::ctx_type::T_MAT4;  return true; }
        
        return false;
    }
    //////////////////////////////////////////////////////
    void push_error(const std::string& error)
    {
        m_errors.push_front(error_field{m_line,error});
    }
    
};

material::~material() {};

bool material::load(resources_manager& resources,const std::string& path)
{
    
    //parse
    material_parser parser;
    //default
    material_parser::context context;
    context.m_blend.m_enable      = m_blend.m_enable;
    context.m_blend.m_source      = material_parser::blend_to_string( m_blend.m_src );
    context.m_blend.m_destination = material_parser::blend_to_string( m_blend.m_dst );
    context.m_cullmode.m_enable   = m_cullface.m_cullface != CF_DISABLE;
    context.m_cullmode.m_mode     = material_parser::cullface_to_string( m_cullface.m_cullface );
    //do parsing
    if(!parser.parse(context,filesystem::text_file_read_all(path)))
    {
        std::cout << "Material: "
                  << path
                  << std::endl
                  << parser.errors_to_string()
                  << std::endl;
        return false;
    }
    //set blend
    if(parser.get_context().m_blend.m_enable)
        m_blend = blend_state(material_parser::blend_from_string(parser.get_context().m_blend.m_source,m_blend.m_src),
                              material_parser::blend_from_string(parser.get_context().m_blend.m_destination,m_blend.m_dst));
    else
        m_blend = blend_state();
    //set cullface
    if(parser.get_context().m_cullmode.m_enable)
        m_cullface = cullface_state(material_parser::cullface_from_string(parser.get_context().m_cullmode.m_mode, m_cullface.m_cullface));
    else
        m_cullface = cullface_state(CF_DISABLE);
    //shader name
    std::string shader_name = parser.get_context().m_shader.m_shader_name;
    //set shader
    if(shader_name.size())
    {
        //shader
        m_shader = resources.get_shader(shader_name);
        //default uniform
        if(m_shader->get_uniform_id("model") > -1)
            m_uniform_model     = m_shader->get_shader_uniform_mat4("model");
        if(m_shader->get_uniform_id("view") > -1)
            m_uniform_view      = m_shader->get_shader_uniform_mat4("view");
        if(m_shader->get_uniform_id("projection") > -1)
            m_uniform_projection= m_shader->get_shader_uniform_mat4("projection");
        if(m_shader->get_uniform_id("viewport") > -1)
            m_uniform_viewport= m_shader->get_shader_uniform_vec4("viewport");
        //texture
        for(auto& utexture : parser.get_context().m_shader.m_textures)
        {
            m_textures.push_back(resources.get_texture(utexture.m_texture));
            m_uniform_textures.push_back(m_shader->get_shader_uniform_texture(utexture.m_uniform.c_str()));
        }
        //const uniform
        for(auto& uniform : parser.get_context().m_shader.m_uniforms)
        {
            switch (uniform.m_type)
            {
                case material_parser::context::ctx_shader::ctx_uniform_field::ctx_type::T_INT:
                    m_ints.push_back(uniform.m_value.m_int);
                    m_uniform_ints.push_back(m_shader->get_shader_uniform_int(uniform.m_uniform.c_str()));
                    break;
                case material_parser::context::ctx_shader::ctx_uniform_field::ctx_type::T_FLOAT:
                    m_floats.push_back(uniform.m_value.m_float);
                    m_uniform_floats.push_back(m_shader->get_shader_uniform_float(uniform.m_uniform.c_str()));
                    break;
                case material_parser::context::ctx_shader::ctx_uniform_field::ctx_type::T_VEC2:
                    m_vec2s.push_back(uniform.m_value.m_vec2);
                    m_uniform_vec2s.push_back(m_shader->get_shader_uniform_vec2(uniform.m_uniform.c_str()));
                    break;
                case material_parser::context::ctx_shader::ctx_uniform_field::ctx_type::T_VEC3:
                    m_vec3s.push_back(uniform.m_value.m_vec3);
                    m_uniform_vec3s.push_back(m_shader->get_shader_uniform_vec3(uniform.m_uniform.c_str()));
                    break;
                case material_parser::context::ctx_shader::ctx_uniform_field::ctx_type::T_VEC4:
                    m_vec4s.push_back(uniform.m_value.m_vec4);
                    m_uniform_vec4s.push_back(m_shader->get_shader_uniform_vec4(uniform.m_uniform.c_str()));
                    break;
                case material_parser::context::ctx_shader::ctx_uniform_field::ctx_type::T_MAT4:
                    m_mat4s.push_back(uniform.m_value.m_mat4);
                    m_uniform_mat4s.push_back(m_shader->get_shader_uniform_mat4(uniform.m_uniform.c_str()));
                    break;
                default: break;
            }
        }
    }
    return true;
}

void material::cullface(cullface_state& cfs)
{
    m_cullface = cfs;
}

void material::blend(const blend_state& bls)
{
    m_blend = bls;
}

void material::bind(const glm::vec4& viewport,
					const glm::mat4& projection,
					const glm::mat4& view,
					const glm::mat4& model)
{
	if (!m_shader)
		return;

	//bind shader
	m_shader->bind();

	//uniforms
	if (m_uniform_projection)
		m_uniform_projection->set_value(projection);

	if (m_uniform_view)
		m_uniform_view->set_value(view);

	if (m_uniform_model)
		m_uniform_model->set_value(model);

	if (m_uniform_viewport)
		m_uniform_viewport->set_value(viewport);

	for (size_t i = 0; i != m_ints.size(); ++i)
	{
		m_uniform_ints[i]->set_value(m_ints[i]);
	}

	for (size_t i = 0; i != m_floats.size(); ++i)
	{
		m_uniform_floats[i]->set_value(m_floats[i]);
	}

	for (size_t i = 0; i != m_textures.size(); ++i)
	{
		m_uniform_textures[i]->set_value(m_textures[i]);
	}

	for (size_t i = 0; i != m_vec2s.size(); ++i)
	{
		m_uniform_vec2s[i]->set_value(m_vec2s[i]);
	}

	for (size_t i = 0; i != m_vec3s.size(); ++i)
	{
		m_uniform_vec3s[i]->set_value(m_vec3s[i]);
	}

	for (size_t i = 0; i != m_vec4s.size(); ++i)
	{
		m_uniform_vec4s[i]->set_value(m_vec4s[i]);
	}

	for (size_t i = 0; i != m_mat4s.size(); ++i)
	{
		m_uniform_mat4s[i]->set_value(m_mat4s[i]);
	}
}

void material::unbind()
{
	if (!m_shader) return;
	//unbind shader
	m_shader->unbind();
}


void material::bind_state()
{
    m_temp_blend    = render::get_blend_state();
    m_temp_cullface = render::get_cullface_state();
    render::set_cullface_state(m_cullface);
    render::set_blend_state(m_blend);
}

void material::unbind_state()
{
    render::set_blend_state(m_temp_blend);
    render::set_cullface_state(m_temp_cullface);
}

material_ptr material::copy() const
{
	auto omaterial = material_snew();

	//cullface
    omaterial->m_temp_cullface = m_temp_cullface;
	omaterial->m_cullface = m_cullface;
    //blend
    omaterial->m_temp_blend = m_temp_blend;
    omaterial->m_blend = m_blend;
	//shader
	omaterial->m_shader = m_shader;
	//standard uniform
	omaterial->m_uniform_projection = m_uniform_projection;
	omaterial->m_uniform_view = m_uniform_view;
	omaterial->m_uniform_model = m_uniform_model;
	omaterial->m_uniform_viewport = m_uniform_viewport;
	//uniform textures
	omaterial->m_textures = m_textures;
	omaterial->m_uniform_textures = m_uniform_textures;
	//uniform float
	omaterial->m_floats = m_floats;
	omaterial->m_uniform_textures = m_uniform_textures;
	//uniform int
	omaterial->m_ints = m_ints;
	omaterial->m_uniform_ints = m_uniform_ints;
	//uniform vec2
	omaterial->m_vec2s = m_vec2s;
	omaterial->m_uniform_vec2s = m_uniform_vec2s;
	//uniform vec3
	omaterial->m_vec3s = m_vec3s;
	omaterial->m_uniform_vec3s = m_uniform_vec3s;
	//uniform vec4
	omaterial->m_vec4s = m_vec4s;
	omaterial->m_uniform_vec4s = m_uniform_vec4s;
	//uniform mat4
	omaterial->m_mat4s = m_mat4s;
	omaterial->m_uniform_mat4s = m_uniform_mat4s;

	return omaterial;
}