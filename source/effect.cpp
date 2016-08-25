#include <effect.h>
#include <list>
#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <filesystem.h>
#include <resources_manager.h>
namespace hcube
{

#define CSTRCMP( x, y ) ( std::strncmp( x, y, sizeof(y)-1 ) == 0 )
#define CSTRCMP_SKIP( x, y ) ( [&x] () -> bool { if ( CSTRCMP(x,y) ) { x += sizeof(y)-1; return true; } return false; } )()


	using  map_parameters = effect::map_parameters;
	using  map_techniques = effect::map_techniques;
	using  technique      = effect::technique;
	using  parameter      = effect::parameter;
	using  parameters     = effect::parameters;
	using  parameter_type = effect::parameter_type;
	
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
	static blend_type blend_from_string(const std::string& blend, blend_type blend_default)
	{
		//
		if (blend == "one") return BLEND_ONE;
		if (blend == "zero") return BLEND_ZERO;
		//
		if (blend == "one_minus_dst_color") return BLEND_ONE_MINUS_DST_COLOR;
		if (blend == "one_minus_dst_alpha") return BLEND_ONE_MINUS_DST_ALPHA;
		if (blend == "one_minus_src_color") return BLEND_ONE_MINUS_SRC_COLOR;
		if (blend == "one_minus_src_alpha") return BLEND_ONE_MINUS_SRC_ALPHA;
		//
		if (blend == "dst_color") return BLEND_DST_COLOR;
		if (blend == "dst_alpha") return BLEND_DST_ALPHA;
		//
		if (blend == "src_color") return BLEND_SRC_COLOR;
		if (blend == "src_alpha") return BLEND_SRC_ALPHA;
		if (blend == "src_apha_sature") return BLEND_SRC_ALPHA_SATURATE;

		return blend_default;
	}
	
	static std::string depth_to_string(depth_func_type depth)
	{
		switch (depth)
		{
		case hcube::DT_NEVER: return "never";
		case hcube::DT_LESS: return "less";
		case hcube::DT_GREATER:  return "greater";
		case hcube::DT_EQUAL:  return "equal";
		case hcube::DT_LESS_EQUAL:  return "less_equal";
		case hcube::DT_GREATER_EQUAL:  return "greater_equal";
		case hcube::DT_NOT_EQUAL:  return "not_equal";
		case hcube::DT_ALWAYS: return "always";
		default: break;
		}
	}
	static depth_func_type depth_from_string(const std::string& depth, depth_func_type depth_default)
	{
		//
		if (depth == "never") return DT_NEVER;
		if (depth == "less") return DT_LESS;
		if (depth == "greater") return DT_GREATER;
		//
		if (depth == "equal") return DT_EQUAL;
		if (depth == "less_equal") return DT_LESS_EQUAL;
		if (depth == "greater_equal") return DT_GREATER_EQUAL;
		if (depth == "not_equal") return DT_NOT_EQUAL;
		//
		if (depth == "always") return DT_ALWAYS;
		//
		return depth_default;
	}
	
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
	static cullface_type cullface_from_string(const std::string& cullface, cullface_type cullface_default)
	{
		if (cullface == "back")           return CF_BACK;
		if (cullface == "front")          return CF_FRONT;
		if (cullface == "front_and_back") return CF_FRONT_AND_BACK;
		return cullface_default;
	}

	class effect_parser
	{

	public:

		struct parameter_field
		{
			struct values
			{
				float		m_float{ 0.0 };
				int			m_int{ 0 };
				vec2	m_vec2;
				vec3	m_vec3;
				vec4	m_vec4;
				mat4	m_mat4;
				std::string m_texture;
			};

			std::string     m_name;
			parameter_type  m_type;
			values          m_value;
		};

		struct shader_field
		{
			bool        m_name{ false };
			std::string m_text;
			size_t      m_line{ 0 };
		};

		struct lights_field
		{
			bool m_on  { false };
			int  m_n_lights{ 0 };
		};

		struct pass_field
		{
			cullface_state     m_cullface;
			depth_buffer_state m_depth;
			blend_state        m_blend;
			shader_field       m_shader;
			lights_field	   m_lights;
		};

		struct technique_field
		{
			std::string m_name;
			std::vector< pass_field > m_pass;
		};

		struct error_field
		{
			size_t m_line{ 0 };
			std::string m_error;

			error_field() 
			{
			}

			error_field(
				  size_t line,
				  const std::string& error)
				: m_line(line)
				, m_error(error) 
			{
			}
		};

		struct context
		{
			std::vector< technique_field > m_techniques;
			std::vector< parameter_field > m_parameters;
			std::list < error_field >	   m_errors;
			size_t						   m_line{ 0 };

			std::string errors_to_string() const
			{
				std::stringstream sbuffer;

				for (auto& error : m_errors)
				{
					sbuffer << "Error:" << error.m_line << ": " << error.m_error << "\n";
				}

				return sbuffer.str();
			}
		};

		bool parse(context& default_context, const std::string& source)
		{
			const char* c_ptr = source.c_str();
			return parse(default_context, c_ptr);
		}
		bool parse(context& default_context, const char*& ptr)
		{
			m_context = &default_context;
			//restart
			m_context->m_techniques.clear();
			m_context->m_parameters.clear();
			m_context->m_errors.clear();
			m_context->m_line = 1;
			//get type
			while (*ptr != EOF && *ptr != '\0')
			{
				//skeep line and comments
				skeep_space_end_comment(ptr);
				//parsing a block
					 if (CSTRCMP_SKIP(ptr,"parameters")) { if (!parse_parameters_block(ptr)) return false; }
				else if (CSTRCMP_SKIP(ptr,"technique"))  { if (!parse_techniques_block(ptr)) return false; }
				else { push_error("Not found a valid command"); return false; }
				//skeep line and comments
				skeep_space_end_comment(ptr);
			}
			return true;
		}

	protected:

		context* m_context{ nullptr };
		//////////////////////////////////////////////////////
		bool parse_parameters_block(const char*& ptr)
		{
			//skeep spaces
			skeep_space_end_comment(ptr);
			//parse table
			if (is_start_table(*ptr))
			{
				//skeep '{'
				++ptr;
				//read all values
				while (!is_end_table(*ptr) && *ptr != EOF && *ptr != '\0')
				{
					//skeep spaces
					skeep_space_end_comment(ptr);
					//alloc uniform field
					parameter_field field;
					//parse
					if (!parse_name(ptr, &ptr, field.m_name))
					{
						push_error("Not valid uniform name");
						return false;
					}
					//skeep "line" space
					skeep_line_space(ptr);
					//value
					if (!parse_value(ptr, field))
					{
						push_error("Not valid uniform field");
						return false;
					}
					//push
					m_context->m_parameters.push_back(field);
					//skeep spaces
					skeep_space_end_comment(ptr);
				}
				//end while
				if (!is_end_table(*ptr))
				{
					push_error("Not found }");
					return false;
				}
				//skip }
				++ptr;
			}
			return true;
		}

		bool parse_value(const char*& ptr, parameter_field& field)
		{
			if (!parse_type(ptr, &ptr, field.m_type)) return false;
			//skeep spaces
			skeep_space_end_comment(ptr);
			//parse '('
			if (!is_start_arg(*ptr)) return false;
			//jump '('
			++ptr;
			//space
			skeep_space_end_comment(ptr);
			//parse by type
			switch (field.m_type)
			{
			case parameter_type::PT_INT:
				if (!parse_int(ptr, &ptr, field.m_value.m_int)) return false;
				break;
			case parameter_type::PT_FLOAT:
				if (!parse_float(ptr, &ptr, field.m_value.m_float)) return false;
				break;
			case parameter_type::PT_TEXTURE:
				if (!parse_texture(ptr, field)) return false;
				break; 
			case parameter_type::PT_VEC2:
				if (!parse_vec2(ptr, field)) return false;
				break;
			case parameter_type::PT_VEC3:
				if (!parse_vec3(ptr, field)) return false;
				break;
			case parameter_type::PT_VEC4:
				if (!parse_vec4(ptr, field)) return false;
				break;
			case parameter_type::PT_MAT4:
				if (!parse_mat4(ptr, field)) return false;
				break;
			default: return false; break;
			}
			//jump space
			skeep_space_end_comment(ptr);
			//parse ')'
			if (!is_end_arg(*ptr))  return false;
			//jump ')'
			++ptr;
			//...
			return true;
		}

		bool parse_texture(const char*& ptr, parameter_field& field)
		{
			//texture name
			std::string texture_name;
			//parse
			if (!parse_cstring(ptr, &ptr, field.m_value.m_texture))
			{
				return false;
			}
			return true;
		}

		bool parse_vec2(const char*& ptr, parameter_field& field)
		{
			if (!parse_float(ptr, &ptr, field.m_value.m_vec2.x)) return false;

			skeep_space_end_comment(ptr);
			if (!is_comm_arg(*ptr)) return false; else ++ptr;
			skeep_space_end_comment(ptr);

			if (!parse_float(ptr, &ptr, field.m_value.m_vec2.y)) return false;

			return true;
		}

		bool parse_vec3(const char*& ptr, parameter_field& field)
		{
			if (!parse_float(ptr, &ptr, field.m_value.m_vec3.x)) return false;

			skeep_space_end_comment(ptr);
			if (!is_comm_arg(*ptr)) return false; else ++ptr;
			skeep_space_end_comment(ptr);

			if (!parse_float(ptr, &ptr, field.m_value.m_vec3.y)) return false;

			skeep_space_end_comment(ptr);
			if (!is_comm_arg(*ptr)) return false; else ++ptr;
			skeep_space_end_comment(ptr);

			if (!parse_float(ptr, &ptr, field.m_value.m_vec3.z)) return false;

			return true;
		}

		bool parse_vec4(const char*& ptr, parameter_field& field)
		{
			if (!parse_float(ptr, &ptr, field.m_value.m_vec4.x)) return false;

			skeep_space_end_comment(ptr);
			if (!is_comm_arg(*ptr)) return false; else ++ptr;
			skeep_space_end_comment(ptr);

			if (!parse_float(ptr, &ptr, field.m_value.m_vec4.y)) return false;

			skeep_space_end_comment(ptr);
			if (!is_comm_arg(*ptr)) return false; else ++ptr;
			skeep_space_end_comment(ptr);

			if (!parse_float(ptr, &ptr, field.m_value.m_vec4.z)) return false;

			skeep_space_end_comment(ptr);
			if (!is_comm_arg(*ptr)) return false; else ++ptr;
			skeep_space_end_comment(ptr);

			if (!parse_float(ptr, &ptr, field.m_value.m_vec4.w)) return false;

			return true;
		}

		bool parse_mat4(const char*& ptr, parameter_field& field)
		{
			//first
			if (!parse_float(ptr, &ptr, field.m_value.m_mat4[0][0])) return false;
			//skeep spaces
			skeep_space_end_comment(ptr);
			// if one param, call mat4 constructor
			if (!is_comm_arg(*ptr)) 
			{ 			
				// is 'mat4('<float>')'
				field.m_value.m_mat4 = mat4(field.m_value.m_mat4[0][0]); 
				//success
				return true;
			}
			//for all
			for (int x = 0; x != 4; ++x)
				for (int y = 0; y != 4; ++y)
				{
					//jmp first
					if ((x + y) == 0) continue;
					//parse ','
					skeep_space_end_comment(ptr);
					if (!is_comm_arg(*ptr)) return false; else ++ptr;
					skeep_space_end_comment(ptr);
					//parse value
					if (!parse_float(ptr, &ptr, field.m_value.m_mat4[x][y])) return false;
				}

			return true;
		}
		//////////////////////////////////////////////////////
		bool parse_techniques_block(const char*& ptr)
		{
			//skeep spaces
			skeep_space_end_comment(ptr);
			//name technique
			technique_field new_t_field;
			//get name
			if (!parse_cstring(ptr, &ptr, new_t_field.m_name))
			{
				push_error("Not valid technique name");
				return false;
			}
			//skeep spaces
			skeep_space_end_comment(ptr);
			//add technique
			m_context->m_techniques.push_back(new_t_field);
			//ref
			technique_field& t_field = m_context->m_techniques[m_context->m_techniques.size() - 1];
			//parse table
			if (is_start_table(*ptr))
			{
				//skeep '{'
				++ptr;
				//read all values
				while (!is_end_table(*ptr) && *ptr != EOF && *ptr != '\0')
				{
					//skeep spaces
					skeep_space_end_comment(ptr);
					//search source attribute
					if (CSTRCMP_SKIP(ptr, "pass"))
					{
						pass_field pass;
						//skeep spaces
						skeep_space_end_comment(ptr);
						//parse textures
						if (!parse_pass_block(ptr, pass)) return false;
						//skeep spaces
						skeep_space_end_comment(ptr);
						//append
						t_field.m_pass.push_back(pass);
					}
					else
					{
						push_error("Keyword not valid");
						return false;
					}

				}
				//end while
				if (!is_end_table(*ptr))
				{
					push_error("Not found }");
					return false;
				}
				//skip }
				++ptr;
			}
			//end
			return true;
		}
		//////////////////////////////////////////////////////
		bool parse_pass_block(const char*& ptr,pass_field& pass)
		{
			//parse table
			if (is_start_table(*ptr))
			{
				//skeep '{'
				++ptr;
				//read all values
				while (!is_end_table(*ptr) && *ptr != EOF && *ptr != '\0')
				{
					//skeep spaces
					skeep_space_end_comment(ptr);
					//all casses
					if (CSTRCMP(ptr, "blend"))
					{
						//skeep spaces
						skeep_space_end_comment(ptr);
						//parse textures
						if (!parse_blend(ptr, pass)) return false;
						//skeep spaces
						skeep_space_end_comment(ptr);
					}
					else if (CSTRCMP(ptr, "depth") || CSTRCMP(ptr, "zbuffer"))
					{
						//skeep spaces
						skeep_space_end_comment(ptr);
						//parse textures
						if (!parse_depth(ptr, pass)) return false;
						//skeep spaces
						skeep_space_end_comment(ptr);
					}
					else if (CSTRCMP(ptr, "cullface"))
					{
						//skeep spaces
						skeep_space_end_comment(ptr);
						//parse textures
						if (!parse_cullface(ptr, pass)) return false;
						//skeep spaces
						skeep_space_end_comment(ptr);
					}
					else if (CSTRCMP(ptr, "lights"))
					{
						//skeep spaces
						skeep_space_end_comment(ptr);
						//parse textures
						if (!parse_lights(ptr, pass)) return false;
						//skeep spaces
						skeep_space_end_comment(ptr);
					}
					else if (CSTRCMP(ptr, "shader"))
					{
						//skeep spaces
						skeep_space_end_comment(ptr);
						//parse textures
						if (!parse_shader(ptr, pass)) return false;
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
				if (!is_end_table(*ptr))
				{
					push_error("Not found }");
					return false;
				}
				//skip }
				++ptr;
			}
            return true;
		}
		//////////////////////////////////////////////////////
		bool parse_blend(const char*& ptr, pass_field& pass)
		{
			//search source attribute
			if (!CSTRCMP_SKIP(ptr, "blend"))
			{
				push_error("Blend not found");
				return false;
			}
			//skeep "line" space
			skeep_line_space(ptr);
			//string
			std::string param1,param2;
			//parse name
			if (!parse_name(ptr, &ptr, param1))
			{
				push_error("First blend parameter not valid");
				return false;
			}
			//param test
			if (param1 == "off")
			{
				pass.m_blend = blend_state();
				return true;
			}
			//skeep "line" space
			skeep_line_space(ptr);
			//parse name
			if (!parse_name(ptr, &ptr, param2))
			{
				push_error("Second blend parameter not valid");
				return false;
			}
			//parse
			pass.m_blend.m_enable = true;
			pass.m_blend.m_src = blend_from_string(param1, BLEND_ONE);
			pass.m_blend.m_dst = blend_from_string(param2, BLEND_ZERO);
			return true;
		}
		bool parse_depth(const char*& ptr, pass_field& pass)
		{
			//search source attribute
			if (!(CSTRCMP_SKIP(ptr, "depth") || CSTRCMP_SKIP(ptr, "zbuffer")))
			{
				push_error("Depth not found");
				return false;
			}
			//skeep "line" space
			skeep_line_space(ptr);
			//string
			std::string param1;
			//parse name
			if (!parse_name(ptr, &ptr, param1))
			{
				push_error("Depth parameter not valid");
				return false;
			}
			//param test
			if (param1 == "off")
			{
				pass.m_depth = depth_buffer_state(false);
				return true;
			}
			//parse
			pass.m_depth.m_depth = true;
			pass.m_depth.m_type = depth_from_string(param1, DT_LESS);
			return true;
		}
		bool parse_cullface(const char*& ptr, pass_field& pass)
		{
			//search source attribute
			if (!(CSTRCMP_SKIP(ptr, "cullface")))
			{
				push_error("Cullface not found");
				return false;
			}
			//skeep "line" space
			skeep_line_space(ptr);
			//string
			std::string param1;
			//parse name
			if (!parse_name(ptr, &ptr, param1))
			{
				push_error("Cullface parameter not valid");
				return false;
			}
			//param test
			if (param1 == "off")
			{
				pass.m_cullface = cullface_state(CF_DISABLE);
				return true;
			}
			//parse
			pass.m_cullface.m_cullface = cullface_from_string(param1,CF_BACK);
			return true;
		}
		bool parse_lights(const char*& ptr, pass_field& pass)
		{
			//search source attribute
			if (!(CSTRCMP_SKIP(ptr, "lights")))
			{
				push_error("Lights not found");
				return false;
			}
			//skeep "line" space
			skeep_line_space(ptr);
			//is off?
			if ((CSTRCMP_SKIP(ptr, "off")))
			{
				pass.m_lights.m_on = false;
				pass.m_lights.m_n_lights = 0;
				return true;
			}
			//is only_ambient?
			if ((CSTRCMP_SKIP(ptr, "only_ambient")))
			{
				pass.m_lights.m_on = true;
				pass.m_lights.m_n_lights = 0;
				return true;
			}
			//number
			if (!is_uint_number(*ptr))
			{
				push_error("Lights command required int parameter");
				return false;
			}
			//enable lights
			pass.m_lights.m_on = true;
			//parse int
			if(!parse_int(ptr, &ptr, pass.m_lights.m_n_lights))
			{
				push_error("Lights command required valid int parameter");
				return false;
			}
			//end
			return true;
		}
		bool parse_shader(const char*& ptr, pass_field& pass)
		{
			//search shader attribute
			if (!CSTRCMP_SKIP(ptr, "shader"))
			{
				push_error("Shader not found");
				return false;
			}
			//skeep spaces
			skeep_space_end_comment(ptr);
			//is a shader name?
			if (is_start_string(*ptr))
			{
				//shader name
				std::string shader_name;
				//get
				if (!parse_cstring(ptr, &ptr, shader_name))
				{
					push_error("Not valid shader name");
					return false;
				}
				pass.m_shader.m_name = true;
				pass.m_shader.m_text = shader_name;
				return true;
			}
			//else is a shader source
			if (!CSTRCMP_SKIP(ptr, "source"))
			{
				push_error("Shader source not found");
				return false;
			}
			//skeep spaces
			skeep_space_end_comment(ptr);
			//parse source
			pass.m_shader.m_name = false;
			//count parenthesis
			size_t p_count = 0;
			//inside a comment
			bool   c_inside = false;
			//parse table
			if (is_start_table(*ptr))
			{
				//skeep '{'
				++ptr;
				++p_count;
				//save start line
				pass.m_shader.m_line = m_context->m_line;
				//read all values
				while (*ptr != EOF && *ptr != '\0')
				{
					//break case
					if (is_end_table(*ptr) && !c_inside)
					{ 
						--p_count; 
						if (!p_count) break; 
					}
					//comment test
						    if (is_start_multy_line_comment(ptr)) { c_inside = true;  }
					else if (is_end_multy_line_comment(ptr))   { c_inside = false; }
					//append
					pass.m_shader.m_text += *ptr; 
					//next
					++ptr;
				}
				//end while
				if (!is_end_table(*ptr))
				{
					push_error("Not found }");
					return false;
				}
				//skip }
				++ptr;
			}
			return true;
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
			return 	 (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '_';
		}

		static bool is_uint_number(char c)
		{
			return 	 (c >= '0' && c <= '9');
		}

		static bool is_start_int_number(char c)
		{
			return 	 (c >= '0' && c <= '9') || c == '-';
		}

		static bool is_start_float_number(char c)
		{
			return 	 (c >= '0' && c <= '9') || c == '-' || c == '.';
		}

		static bool is_start_string(char c)
		{
			return c == '\"';
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
			return (*c) == '/' && (*(c + 1)) == '/';
		}

		static bool is_start_multy_line_comment(const char* c)
		{
			return (*c) == '/' && (*(c + 1)) == '*';
		}

		static bool is_end_multy_line_comment(const char* c)
		{
			return (*c) == '*' && (*(c + 1)) == '/';
		}

		void skeep_line_comment(const char*& inout)
		{
			if (is_line_comment(inout))
			{
				while (*(inout) != EOF &&
					*(inout) != '\0'&&
					*(inout) != '\n')
					++(inout);
			}
		}

		void skeep_multy_line_comment(const char*& inout)
		{
			if (is_start_multy_line_comment(inout))
			{
				while (*(inout) != EOF &&
					*(inout) != '\0' &&
					!is_end_multy_line_comment(inout))
				{
					m_context->m_line += (*(inout)) == '\n';
					++(inout);
				}
				if ((*(inout)) == '*') ++(inout);
			}
		}

		void skeep_space_end_comment(const char*& inout)
		{
			while (is_space(*(inout)) ||
				is_line_comment(inout) ||
				is_start_multy_line_comment(inout))
			{
				skeep_line_comment(inout);
				skeep_multy_line_comment(inout);
				m_context->m_line += (*(inout)) == '\n';
				++(inout);
			}
		}

		void skeep_line_space(const char*& inout)
		{
			while (is_line_space(*inout)) ++(inout);
		}
		//////////////////////////////////////////////////////
		bool parse_bool(const char* in, const char** cout, bool& out)
		{
			if (CSTRCMP(in, "true")) { (*cout) += 4; out = true;  return true; }
			if (CSTRCMP(in, "false")) { (*cout) += 5; out = false; return true; };
			return false;
		}

		bool parse_int(const char* in, const char** cout, int& out)
		{
			out = (int)std::strtod(in, (char**)cout);
			return in != (*cout);
		}

		bool parse_float(const char* in, const char** cout, float& out)
		{
			out = (float)std::strtod(in, (char**)cout);
			return in != (*cout);
		}

		bool parse_name(const char* in, const char** cout, std::string& out)
		{
			if (!is_start_name(*in)) return false;
			out = "";
			out += *in;
			++in;
			while (is_char_name(*in))
			{
				out += *in;
				++in;
			}
			(*cout) = in;
			return true;
		}

		bool parse_cstring(const char* in, const char** cout, std::string& out)
		{
			const char *tmp = in;
			out = "";
			//start parse
			if ((*tmp) == '\"')  //["...."]
			{
				++tmp;  //[...."]
				while ((*tmp) != '\"' && (*tmp) != '\n')
				{
					if ((*tmp) == '\\') //[\.]
					{
						++tmp;  //[.]
						switch (*tmp)
						{
						case 'n':
							out += '\n';
							break;
						case 't':
							out += '\t';
							break;
						case 'b':
							out += '\b';
							break;
						case 'r':
							out += '\r';
							break;
						case 'f':
							out += '\f';
							break;
						case 'a':
							out += '\a';
							break;
						case '\\':
							out += '\\';
							break;
						case '?':
							out += '\?';
							break;
						case '\'':
							out += '\'';
							break;
						case '\"':
							out += '\"';
							break;
						case '\n': /* jump unix */
							++m_context->m_line;
							break;
						case '\r': /* jump mac */
							++m_context->m_line;
							if ((*(tmp + 1)) == '\n') ++tmp; /* jump window (\r\n)*/
							break;
						default:
							return true;
							break;
						}
					}
					else
					{
						if ((*tmp) != '\0') out += (*tmp);
						else return false;
					}
					++tmp;//next char
				}
				if ((*tmp) == '\n') return false;
				if (cout) (*cout) = tmp + 1;
				return true;
			}
			return false;
		}
		//////////////////////////////////////////////////////
		bool parse_type(const char* in,
						const char** cout,
						parameter_type& type)
		{
			if (CSTRCMP(in, "int"))
			{
				(*cout) += 3; type = parameter_type::PT_INT;  return true;
			}

			if (CSTRCMP(in, "float"))
			{
				(*cout) += 5; type = parameter_type::PT_FLOAT;  return true;
			}

			if (CSTRCMP(in, "texture"))
			{
				(*cout) += 7; type = parameter_type::PT_TEXTURE;  return true;
			}

			if (CSTRCMP(in, "vec2"))
			{
				(*cout) += 4; type = parameter_type::PT_VEC2;  return true;
			}

			if (CSTRCMP(in, "vec3"))
			{
				(*cout) += 4; type = parameter_type::PT_VEC3;  return true;
			}

			if (CSTRCMP(in, "vec4"))
			{
				(*cout) += 4; type = parameter_type::PT_VEC4;  return true;
			}

			if (CSTRCMP(in, "mat4"))
			{
				(*cout) += 4; type = parameter_type::PT_MAT4;  return true;
			}

			return false;
		}
		//////////////////////////////////////////////////////
		void push_error(const std::string& error)
		{
			m_context->m_errors.push_front(error_field{ m_context->m_line,error });
		}

	};
	
	//struct by type
	struct parameter_texture : public effect::parameter
	{
		//value
		texture::ptr m_value;
		//init
		parameter_texture()
		{
			m_value = texture::ptr(nullptr);
			m_type = effect::PT_TEXTURE;
		}
		parameter_texture(texture::ptr value)
		{
			m_value = value;
			m_type = effect::PT_TEXTURE;
		}
		//set value
		virtual void set_value(texture::ptr value)
		{
			m_value = value;
		}
		//get value
		virtual texture::ptr get_texture() 
		{ 
			return m_value;
		}
		//copy
		virtual parameter* copy() const
		{
			return new parameter_texture(m_value);
		}
	};
	struct parameter_int : public effect::parameter 
	{
		//value
		int m_value;
		//init
		parameter_int(int value = 0)
		{
			m_value = value;
			m_type  = effect::PT_INT;
		}
		//set value
		virtual void set_value(int i)
		{
			m_value = i;
		}
		//get value
		virtual int get_int()
		{
			return m_value;
		}
		//copy
		virtual parameter* copy() const
		{
			return new parameter_int(m_value);
		}
	};
	struct parameter_float : public effect::parameter
	{
		//value
		float m_value;
		//init
		parameter_float(float value = 0.0f)
		{
			m_value = value;
			m_type  = effect::PT_FLOAT;
		}
		//set value
		virtual void set_value(float f)
		{
			m_value = f;
		}
		//get value
		virtual float get_float()
		{
			return m_value;
		}
		//copy
		virtual parameter* copy() const
		{
			return new parameter_float(m_value);
		}
	};
	struct parameter_vec2 : public effect::parameter
	{
		//value
		vec2 m_value;
		//init
		parameter_vec2()
		{
			m_type = effect::PT_VEC2;
		}
		parameter_vec2(const vec2& value)
		{
			m_value = value;
			m_type  = effect::PT_VEC2;
		}
		//set value
		virtual void set_value(const vec2& value)
		{
			m_value = value;
		}
		//get value
		virtual const vec2& get_vec2()
		{
			return m_value;
		}
		//copy
		virtual parameter* copy() const
		{
			return new parameter_vec2(m_value);
		}
	};
	struct parameter_vec3 : public effect::parameter
	{
		//value
		vec3 m_value;
		//init
		parameter_vec3()
		{
			m_type = effect::PT_VEC3;
		}
		parameter_vec3(const vec3& value)
		{
			m_value = value;
			m_type  = effect::PT_VEC3;
		}
		//set value
		virtual void set_value(const vec3& value)
		{
			m_value = value;
		}
		//get value
		virtual const vec3& get_vec3()
		{
			return m_value;
		}
		//copy
		virtual parameter* copy() const
		{
			return new parameter_vec3(m_value);
		}
	};
	struct parameter_vec4 : public effect::parameter
	{
		//value
		vec4 m_value;
		//init
		parameter_vec4()
		{
			m_type = effect::PT_VEC4;
		}
		parameter_vec4(const vec4& value)
		{
			m_value = value;
			m_type  = effect::PT_VEC4;
		}
		//set value
		virtual void set_value(const vec4& value)
		{
			m_value = value;
		}
		//get value
		virtual const vec4& get_vec4()
		{
			return m_value;
		}
		//copy
		virtual parameter* copy() const
		{
			return new parameter_vec4(m_value);
		}
	};
	struct parameter_mat4 : public effect::parameter
	{
		//value
		mat4 m_value;
		//init
		parameter_mat4()
		{
			m_type = effect::PT_MAT4;
		}
		parameter_mat4(const mat4& value)
		{
			m_value = value;
			m_type  = effect::PT_MAT4;
		}
		//set value
		virtual void set_value(const mat4& value)
		{
			m_value = value;
		}
		//get value
		virtual const mat4& get_mat4()
		{
			return m_value;
		}
		//copy
		virtual parameter* copy() const
		{
			return new parameter_mat4(m_value);
		}
	};



	//enable pass effect
	void effect::pass::bind(
		const vec4& viewport,
		const mat4& projection,
		const mat4& view,
		const mat4& model, 
		parameters* params
	)
	{
		render::set_blend_state(m_blend);
		render::set_cullface_state(m_cullface);
		render::set_depth_buffer_state(m_depth);
		m_shader->bind();
		//default uniforms
		if (m_uniform_projection) m_uniform_projection->set_value(projection);
		if (m_uniform_view)       m_uniform_view->set_value(view);
		if (m_uniform_model)      m_uniform_model->set_value(model);
		if (m_uniform_viewport)   m_uniform_viewport->set_value(viewport);
		//if null use default
		if (!params) params = &m_effect->m_parameters;
		//uniform
		for (size_t i = 0; i != m_uniform.size(); ++i)
		{
			auto& param = (*params)[m_param_id[i]];
			//uniform value
			switch (param->get_type())
			{
			default:
			case PT_TEXTURE_ARRAY:
			case PT_NONE:
				/* void */
			break;
			//uniform
			case PT_INT: m_uniform[i]->set_value(param->get_int()); break;
			case PT_FLOAT: m_uniform[i]->set_value(param->get_float()); break;
			case PT_TEXTURE: m_uniform[i]->set_value(param->get_texture()); break;
			case PT_VEC2: m_uniform[i]->set_value(param->get_vec2()); break;
			case PT_VEC3: m_uniform[i]->set_value(param->get_vec3()); break;
			case PT_VEC4: m_uniform[i]->set_value(param->get_vec4()); break;
			case PT_MAT4: m_uniform[i]->set_value(param->get_mat4()); break;
			case PT_INT_ARRAY: m_uniform[i]->set_value(param->get_int_array()); break;
			case PT_FLOAT_ARRAY: m_uniform[i]->set_value(param->get_float_array()); break;
			case PT_VEC2_ARRAY: m_uniform[i]->set_value(param->get_vec2_array()); break;
			case PT_VEC3_ARRAY: m_uniform[i]->set_value(param->get_vec3_array()); break;
			case PT_VEC4_ARRAY: m_uniform[i]->set_value(param->get_vec4_array()); break;
			case PT_MAT4_ARRAY: m_uniform[i]->set_value(param->get_mat4_array()); break;
			}
		}
	}

	//disable pass effect
	void effect::pass::unbind()
	{
		m_shader->unbind();
	}

	//safe enable pass effect
	render_state effect::pass::safe_bind(
		const vec4& viewport,
		const mat4& projection,
		const mat4& view,
		const mat4& model, 
		parameters* params
	)
	{
		render_state state = render::get_render_state();
		bind(viewport, projection,view,model,params);
		return state;
	}

	//safe disable pass effect
	void effect::pass::safe_unbind(const render_state& state)
	{
		render::set_render_state(state);
		unbind();
	}

	//load effect
	bool effect::load(resources_manager& resources, const std::string& path)
	{
		//parser
		effect_parser		   e_parser;
		effect_parser::context e_context;
		//do parsing
		if(!e_parser.parse(e_context, filesystem::text_file_read_all(path)))
		{
			std::cout << "Effect: "
					  << path
					  << std::endl
					  << e_context.errors_to_string()
					  << std::endl;
			return false;
		}
		//alloc params
		m_parameters.resize(e_context.m_parameters.size());
		//add params
		for (size_t i = 0; i != m_parameters.size(); ++i)
		{
			switch (e_context.m_parameters[i].m_type)
			{
			case parameter_type::PT_INT:    m_parameters[i] = std::unique_ptr< parameter >(new parameter_int(e_context.m_parameters[i].m_value.m_int));     break;
			case parameter_type::PT_FLOAT:  m_parameters[i] = std::unique_ptr< parameter >(new parameter_float(e_context.m_parameters[i].m_value.m_float)); break;
			case parameter_type::PT_VEC2:   m_parameters[i] = std::unique_ptr< parameter >(new parameter_vec2(e_context.m_parameters[i].m_value.m_vec2));   break;
			case parameter_type::PT_VEC3:   m_parameters[i] = std::unique_ptr< parameter >(new parameter_vec3(e_context.m_parameters[i].m_value.m_vec3));   break;
			case parameter_type::PT_VEC4:   m_parameters[i] = std::unique_ptr< parameter >(new parameter_vec4(e_context.m_parameters[i].m_value.m_vec4));   break;
			case parameter_type::PT_MAT4:   m_parameters[i] = std::unique_ptr< parameter >(new parameter_mat4(e_context.m_parameters[i].m_value.m_mat4));   break;
			case parameter_type::PT_TEXTURE:m_parameters[i] = std::unique_ptr< parameter >(new parameter_texture(resources.get_texture(e_context.m_parameters[i].m_value.m_texture))); break;
			default: assert(0); break;
			}
			//save id
			m_parameters[i]->m_id = i;
			//add into map
			m_map_parameters[e_context.m_parameters[i].m_name] = i;
		}
		//add tech
		for (size_t t = 0; t != e_context.m_techniques.size(); ++t)
		{
			//add into map
			technique& this_technique = m_map_techniques[e_context.m_techniques[t].m_name];
			//alloc pass
			this_technique.resize(e_context.m_techniques[t].m_pass.size());
			//add pass
			for (size_t p = 0; p != this_technique.size(); ++p)
			{
				//ref
				effect_parser::pass_field& parser_pass = e_context.m_techniques[t].m_pass[p];
				//get all values
				this_technique[p].m_effect   = this;
				this_technique[p].m_blend    = parser_pass.m_blend;
				this_technique[p].m_cullface = parser_pass.m_cullface;
				this_technique[p].m_depth    = parser_pass.m_depth;
				//shader
				if (parser_pass.m_shader.m_name)
				{
					this_technique[p].m_shader = resources.get_shader(parser_pass.m_shader.m_text);
				}
				else
				{
					this_technique[p].m_shader = shader::snew();
					if (!this_technique[p].m_shader->load_effect(
						parser_pass.m_shader.m_text,
						path,
						parser_pass.m_shader.m_line - 1,
						{}))
					{
						std::cout << "Effect: "
								  << path
								  << std::endl
								  << "Error from technique: " << e_context.m_techniques[t].m_name << ", pass["<< p+1 << "]"
								  << std::endl;

					}
				}
				//default uniform
				this_technique[p].m_uniform_model      = this_technique[p].m_shader->get_uniform("model");
				this_technique[p].m_uniform_view       = this_technique[p].m_shader->get_uniform("view");
				this_technique[p].m_uniform_projection = this_technique[p].m_shader->get_uniform("projection");
				this_technique[p].m_uniform_viewport   = this_technique[p].m_shader->get_uniform("viewport");
				//lights uniforms
				if (parser_pass.m_lights.m_on)
				{
					//add support lights
					this_technique[p].m_support_light = true;
					//ambient
					this_technique[p].m_uniform_ambient_light = this_technique[p].m_shader->get_uniform("ambient_light");
					//n lights
					this_technique[p].m_uniform_n_lights_used = this_technique[p].m_shader->get_uniform("n_lights_used");
					//alloc
					this_technique[p].m_uniform_lights.resize(parser_pass.m_lights.m_n_lights);
					//add uniforms
					for (size_t l = 0; l != parser_pass.m_lights.m_n_lights; ++l)
					{
						this_technique[p].m_uniform_lights[l].get_uniform(l, this_technique[p].m_shader);
					}
				}
				//get uniforms
				for (auto it : m_map_parameters)
				{
					//get
					uniform* u_shader = this_technique[p].m_shader->get_uniform(it.first.c_str());
					//test
					if (u_shader)
					{
						//push
						this_technique[p].m_param_id.push_back(it.second);
						this_technique[p].m_uniform.push_back(u_shader);
					}
				}

			}
		}
		
		return true;
	}

	//get technique
	effect::technique* effect::get_technique(const std::string& technique_name)
	{
		auto it_tech = m_map_techniques.find(technique_name);
		if (it_tech != m_map_techniques.end()) return &it_tech->second;
		return nullptr;
	}

	//get parameter
	effect::parameter* effect::get_parameter(int parameter_id)
	{
		if(m_parameters.size() > parameter_id)
			return m_parameters[parameter_id].get();
		return nullptr;
	}

	effect::parameter* effect::get_parameter(const std::string& parameter_name)
	{
		auto it_param = m_map_parameters.find(parameter_name);
		if(it_param != m_map_parameters.end()) return get_parameter(it_param->second);
		return nullptr;
	}

	effect::parameters* effect::copy_all_parameters()
	{
		//alloc vector
		auto new_params = new parameters(m_parameters.size());
		//copy all
		for (size_t i = 0; i != m_parameters.size(); ++i)
		{
			(*new_params)[i] = std::unique_ptr< parameter >( m_parameters[i]->copy() );
		}
		return new_params;
	}

	//get id
	int effect::get_parameter_id(const std::string& parameter_name)
	{
		auto it_param = m_map_parameters.find(parameter_name);
		if (it_param != m_map_parameters.end()) return it_param->second;
		return -1;
	}

}
