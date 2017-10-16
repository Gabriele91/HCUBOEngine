#pragma once
#include <sstream>
#include <hcube/render/effect.h>
#include <hcube/math/vector_math.h>
#include <hcube/data/parser/utils_parser.h>

namespace hcube { 
namespace parser {

	class effect_parser : protected utils_parser
	{

		using  map_parameters = effect::map_parameters;
		using  map_techniques = effect::map_techniques;
		using  technique = effect::technique;
		using  parameter = effect::parameter;
		using  parameters = effect::parameters;
		using  parameter_type = effect::parameter_type;
		using  parameter_queue = effect::parameter_queue;


	public:

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

		struct requirement_field
		{
			std::string m_driver_name;
			int         m_driver_major_version;
			int         m_driver_minor_version;
			std::string m_shader_name;
			int         m_shader_version;

			bool test() const
			{
				//get info
				render_driver_info info = render::get_render_driver_info();
				//test name
				if (m_driver_name != render_driver_str[info.m_render_driver]) return false;
				if (m_shader_name != info.m_shader_language)                  return false;
				//test version
				if (m_driver_major_version <= info.m_major_version &&
					m_driver_minor_version <= info.m_minor_version &&
					m_shader_version <= info.m_shader_version)
				{
					return true;
				}
				return false;
			}
		};

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

		enum lights_field
		{
			LF_NONE = 0b00000,
			LF_BASE = 0b00001,
			LF_AMBIENT = 0b00010,
			LF_SPOT = 0b00100,
			LF_POINT = 0b01000,
			LF_DIRECTION = 0b10000,
			LF_SPOT_POINT = LF_SPOT | LF_POINT,
			LF_SPOT_DIRECTION = LF_SPOT | LF_DIRECTION,
			LF_POINT_DIRECTION = LF_POINT | LF_DIRECTION,
			LF_SPOT_POINT_DIRECTION = LF_SPOT | LF_POINT | LF_DIRECTION
		};

		struct pass_field
		{
			cullface_state     m_cullface;
			depth_buffer_state m_depth;
			blend_state        m_blend;
			shader_field       m_shader;
			lights_field	   m_lights{ LF_BASE };
		};

		struct technique_field
		{
			std::string m_name;
			std::vector< pass_field > m_pass;
		};

		struct sub_effect_field
		{
			requirement_field m_requirement;
			parameter_queue m_queue;
			std::vector< technique_field > m_techniques;
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
			std::vector< sub_effect_field > m_sub_effect;
			std::vector< parameter_field >  m_parameters;
			std::list < error_field >	    m_errors;
			size_t						    m_line{ 0 };

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
			m_context->m_sub_effect.clear();
			m_context->m_parameters.clear();
			m_context->m_errors.clear();
			m_context->m_line = 1;
			//get type
			while (*ptr != EOF && *ptr != '\0')
			{
				//skeep line and comments
				skeep_space_end_comment(m_context->m_line,ptr);
				//parsing a block
					  if (cstr_cmp_skip(ptr, "parameters")) { if (!parse_parameters_block(ptr)) return false; }
				else  if (cstr_cmp_skip(ptr, "sub_effect")) { if (!parse_sub_effect_block(ptr)) return false; }
				else { push_error("Not found a valid command"); return false; }
				//skeep line and comments
				skeep_space_end_comment(m_context->m_line, ptr);
			}
			return true;
		}

	protected:

		context* m_context{ nullptr };
		//////////////////////////////////////////////////////
		bool parse_parameters_block(const char*& ptr)
		{
			//skeep spaces
			skeep_space_end_comment(m_context->m_line, ptr);
			//parse table
			if (is_start_table(*ptr))
			{
				//skeep '{'
				++ptr;
				//skeep spaces
				skeep_space_end_comment(m_context->m_line, ptr);
				//read all values
				while (!is_end_table(*ptr) && *ptr != EOF && *ptr != '\0')
				{
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
					skeep_space_end_comment(m_context->m_line, ptr);
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
			skeep_space_end_comment(m_context->m_line, ptr);
			//parse '('
			if (!is_start_arg(*ptr)) return false;
			//jump '('
			++ptr;
			//space
			skeep_space_end_comment(m_context->m_line, ptr);
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
			skeep_space_end_comment(m_context->m_line, ptr);
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
			if (!parse_cstring(m_context->m_line,ptr, &ptr, field.m_value.m_texture))
			{
				return false;
			}
			return true;
		}

		bool parse_vec2(const char*& ptr, parameter_field& field)
		{
			if (!parse_float(ptr, &ptr, field.m_value.m_vec2.x)) return false;

			skeep_space_end_comment(m_context->m_line, ptr);
			if (!is_comm_arg(*ptr)) return false; else ++ptr;
			skeep_space_end_comment(m_context->m_line, ptr);

			if (!parse_float(ptr, &ptr, field.m_value.m_vec2.y)) return false;

			return true;
		}

		bool parse_vec3(const char*& ptr, parameter_field& field)
		{
			if (!parse_float(ptr, &ptr, field.m_value.m_vec3.x)) return false;

			skeep_space_end_comment(m_context->m_line, ptr);
			if (!is_comm_arg(*ptr)) return false; else ++ptr;
			skeep_space_end_comment(m_context->m_line, ptr);

			if (!parse_float(ptr, &ptr, field.m_value.m_vec3.y)) return false;

			skeep_space_end_comment(m_context->m_line, ptr);
			if (!is_comm_arg(*ptr)) return false; else ++ptr;
			skeep_space_end_comment(m_context->m_line, ptr);

			if (!parse_float(ptr, &ptr, field.m_value.m_vec3.z)) return false;

			return true;
		}

		bool parse_vec4(const char*& ptr, parameter_field& field)
		{
			if (!parse_float(ptr, &ptr, field.m_value.m_vec4.x)) return false;

			skeep_space_end_comment(m_context->m_line, ptr);
			if (!is_comm_arg(*ptr)) return false; else ++ptr;
			skeep_space_end_comment(m_context->m_line, ptr);

			if (!parse_float(ptr, &ptr, field.m_value.m_vec4.y)) return false;

			skeep_space_end_comment(m_context->m_line, ptr);
			if (!is_comm_arg(*ptr)) return false; else ++ptr;
			skeep_space_end_comment(m_context->m_line, ptr);

			if (!parse_float(ptr, &ptr, field.m_value.m_vec4.z)) return false;

			skeep_space_end_comment(m_context->m_line, ptr);
			if (!is_comm_arg(*ptr)) return false; else ++ptr;
			skeep_space_end_comment(m_context->m_line, ptr);

			if (!parse_float(ptr, &ptr, field.m_value.m_vec4.w)) return false;

			return true;
		}

		bool parse_mat4(const char*& ptr, parameter_field& field)
		{
			//first
			if (!parse_float(ptr, &ptr, field.m_value.m_mat4[0][0])) return false;
			//skeep spaces
			skeep_space_end_comment(m_context->m_line, ptr);
			// if one param, call mat4 constructor
			if (!is_comm_arg(*ptr))
			{
				// is 'mat4('<float>')'
				field.m_value.m_mat4 = mat4(field.m_value.m_mat4[0][0]);
				//success
				return true;
			}
			//for all
			for (int y = 0; y != 4; ++y)
			for (int x = 0; x != 4; ++x)
			{
				//jmp first
				if ((x + y) == 0) continue;
				//parse ','
				skeep_space_end_comment(m_context->m_line, ptr);
				if (!is_comm_arg(*ptr)) return false; else ++ptr;
				skeep_space_end_comment(m_context->m_line, ptr);
				//parse value
				if (!parse_float(ptr, &ptr, field.m_value.m_mat4[x][y])) return false;
			}

			return true;
		}
		//////////////////////////////////////////////////////
		bool parse_driver_type(const char*& ptr, requirement_field& field)
		{
			if (!parse_name(ptr, &ptr, field.m_driver_name)) return false;
			//skeep spaces
			skeep_space_end_comment(m_context->m_line, ptr);
			//parse '('
			if (!is_start_arg(*ptr)) return false;
			//jump  '('
			++ptr;
			//skeep spaces
			skeep_space_end_comment(m_context->m_line, ptr);
			//major
			if (!parse_int(ptr, &ptr, field.m_driver_major_version)) return false;
			//commond
			skeep_space_end_comment(m_context->m_line, ptr);
			if (!is_comm_arg(*ptr)) return false; else ++ptr;
			skeep_space_end_comment(m_context->m_line, ptr);
			//minor
			if (!parse_int(ptr, &ptr, field.m_driver_minor_version)) return false;
			//skeep spaces
			skeep_space_end_comment(m_context->m_line, ptr);
			//parse ')'
			if (!is_end_arg(*ptr)) return false;
			//jump  ')'
			++ptr;
			//ok
			return true;
		}

		bool parse_shader_type(const char*& ptr, requirement_field& field)
		{
			if (!parse_name(ptr, &ptr, field.m_shader_name)) return false;
			//skeep spaces
			skeep_space_end_comment(m_context->m_line, ptr);
			//parse '('
			if (!is_start_arg(*ptr)) return false;
			//jump  '('
			++ptr;
			//skeep spaces
			skeep_space_end_comment(m_context->m_line, ptr);
			//major
			if (!parse_int(ptr, &ptr, field.m_shader_version)) return false;
			//skeep spaces
			skeep_space_end_comment(m_context->m_line, ptr);
			//parse ')'
			if (!is_end_arg(*ptr)) return false;
			//jump  ')'
			++ptr;
			//skeep spaces
			skeep_space_end_comment(m_context->m_line, ptr);
			//ok
			return true;
		}
		//////////////////////////////////////////////////////
		bool parse_sub_effect_block(const char*& ptr)
		{
			//shader parser state
			enum
			{
				NOT_READ_REQUIREMENT,
				READ_REQUIREMENT
			}
			state = NOT_READ_REQUIREMENT;
			//skeep spaces
			skeep_space_end_comment(m_context->m_line, ptr);
			//skeep spaces
			skeep_space_end_comment(m_context->m_line, ptr);
			//add technique
			m_context->m_sub_effect.resize(m_context->m_sub_effect.size() + 1);
			//ref
			sub_effect_field& t_field = m_context->m_sub_effect[m_context->m_sub_effect.size() - 1];
			//parse table
			if (is_start_table(*ptr))
			{
				//skeep '{'
				++ptr;
				//skeep spaces
				skeep_space_end_comment(m_context->m_line, ptr);
				//read all values
				while (!is_end_table(*ptr) && *ptr != EOF && *ptr != '\0')
				{
					//search source attribute
					if (cstr_cmp_skip(ptr, "requirement"))
					{
						//test
						if (state == READ_REQUIREMENT)
						{
							push_error("Requirement already declared");
							return false;
						}
						//skeep spaces
						skeep_space_end_comment(m_context->m_line, ptr);
						//parse textures
						if (!parse_requirement_block(ptr, t_field.m_requirement)) return false;
						//skeep spaces
						skeep_space_end_comment(m_context->m_line, ptr);
						//change state
						state = READ_REQUIREMENT;
					}
					if (cstr_cmp_skip(ptr, "queue"))
					{
						//test
						if (state == NOT_READ_REQUIREMENT)
						{
							push_error("Requirement must to be declared before");
							return false;
						}
						//..
						parameter_queue p_queue;
						//skeep spaces
						skeep_space_end_comment(m_context->m_line, ptr);
						//parse textures
						if (!parse_queue_block(ptr, t_field.m_queue)) return false;
						//skeep spaces
						skeep_space_end_comment(m_context->m_line, ptr);
					}
					else if (cstr_cmp_skip(ptr, "technique"))
					{
						//test
						if (state == NOT_READ_REQUIREMENT)
						{
							push_error("Requirement must to be declared before");
							return false;
						}
						//skeep spaces
						skeep_space_end_comment(m_context->m_line, ptr);
						//techniques
						if (!parse_techniques_block(ptr, t_field)) return false;
						//skeep spaces
						skeep_space_end_comment(m_context->m_line, ptr);
					}
					else
					{
						push_error("Keyword not valid");
						return false;
					}
					//skeep spaces
					skeep_space_end_comment(m_context->m_line, ptr);

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
		bool parse_requirement_block(const char*& ptr, requirement_field& r_field)
		{
			//parse table
			if (is_start_table(*ptr))
			{
				//skeep '{'
				++ptr;
				//skeep spaces
				skeep_space_end_comment(m_context->m_line, ptr);
				//read all values
				while (!is_end_table(*ptr) && *ptr != EOF && *ptr != '\0')
				{
					//all casses
					if (cstr_cmp_skip(ptr, "driver"))
					{
						//skeep spaces
						skeep_space_end_comment(m_context->m_line, ptr);
						//parse textures
						if (!parse_driver_type(ptr, r_field))
						{
							push_error("Requirement: driver value not valid");
							return false;
						}
						//skeep spaces
						skeep_space_end_comment(m_context->m_line, ptr);
					}
					else if (cstr_cmp_skip(ptr, "shader"))
					{
						//skeep spaces
						skeep_space_end_comment(m_context->m_line, ptr);
						//parse textures
						if (!parse_shader_type(ptr, r_field))
						{
							push_error("Requirement: shader value not valid");
							return false;
						}
						//skeep spaces
						skeep_space_end_comment(m_context->m_line, ptr);
					}
					else
					{
						push_error("Keyword not valid");
						return false;
					}
					//skeep spaces
					skeep_space_end_comment(m_context->m_line, ptr);
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
		bool parse_techniques_block(const char*& ptr, sub_effect_field& subeffect)
		{
			//skeep spaces
			skeep_space_end_comment(m_context->m_line, ptr);
			//name technique
			technique_field new_t_field;
			//get name
			if (!parse_cstring(m_context->m_line,ptr, &ptr, new_t_field.m_name))
			{
				push_error("Not valid technique name");
				return false;
			}
			//skeep spaces
			skeep_space_end_comment(m_context->m_line, ptr);
			//add technique
			subeffect.m_techniques.push_back(new_t_field);
			//ref
			technique_field& t_field = subeffect.m_techniques[subeffect.m_techniques.size() - 1];
			//parse table
			if (is_start_table(*ptr))
			{
				//skeep '{'
				++ptr;
				//skeep spaces
				skeep_space_end_comment(m_context->m_line, ptr);
				//read all values
				while (!is_end_table(*ptr) && *ptr != EOF && *ptr != '\0')
				{
					//search source attribute
					if (cstr_cmp_skip(ptr, "pass"))
					{
						pass_field pass;
						//skeep spaces
						skeep_space_end_comment(m_context->m_line, ptr);
						//parse textures
						if (!parse_pass_block(ptr, pass)) return false;
						//skeep spaces
						skeep_space_end_comment(m_context->m_line, ptr);
						//append
						t_field.m_pass.push_back(pass);
					}
					else
					{
						push_error("Keyword not valid");
						return false;
					}
					//skeep spaces
					skeep_space_end_comment(m_context->m_line, ptr);

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
		bool parse_pass_block(const char*& ptr, pass_field& pass)
		{
			//parse table
			if (is_start_table(*ptr))
			{
				//skeep '{'
				++ptr;
				//skeep spaces
				skeep_space_end_comment(m_context->m_line, ptr);
				//read all values
				while (!is_end_table(*ptr) && *ptr != EOF && *ptr != '\0')
				{
					//all casses
					if (cstr_cmp(ptr, "blend"))
					{
						//skeep spaces
						skeep_space_end_comment(m_context->m_line, ptr);
						//parse textures
						if (!parse_blend(ptr, pass)) return false;
						//skeep spaces
						skeep_space_end_comment(m_context->m_line, ptr);
					}
					else if (cstr_cmp(ptr, "depth") || cstr_cmp(ptr, "zbuffer"))
					{
						//skeep spaces
						skeep_space_end_comment(m_context->m_line, ptr);
						//parse textures
						if (!parse_depth(ptr, pass)) return false;
						//skeep spaces
						skeep_space_end_comment(m_context->m_line, ptr);
					}
					else if (cstr_cmp(ptr, "cullface"))
					{
						//skeep spaces
						skeep_space_end_comment(m_context->m_line, ptr);
						//parse textures
						if (!parse_cullface(ptr, pass)) return false;
						//skeep spaces
						skeep_space_end_comment(m_context->m_line, ptr);
					}
					else if (cstr_cmp(ptr, "lights"))
					{
						//skeep spaces
						skeep_space_end_comment(m_context->m_line, ptr);
						//parse textures
						if (!parse_lights(ptr, pass)) return false;
						//skeep spaces
						skeep_space_end_comment(m_context->m_line, ptr);
					}
					else if (cstr_cmp(ptr, "shader"))
					{
						//skeep spaces
						skeep_space_end_comment(m_context->m_line, ptr);
						//parse textures
						if (!parse_shader(ptr, pass)) return false;
						//skeep spaces
						skeep_space_end_comment(m_context->m_line, ptr);
					}
					else
					{
						push_error("Keyword not valid");
						return false;
					}
					//skeep spaces
					skeep_space_end_comment(m_context->m_line, ptr);
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
		bool parse_queue_block(const char*& ptr, parameter_queue& p_queue)
		{
			//parse table
			if (is_start_table(*ptr))
			{
				//skeep '{'
				++ptr;
				//skeep spaces
				skeep_space_end_comment(m_context->m_line, ptr);
				//read all values
				while (!is_end_table(*ptr) && *ptr != EOF && *ptr != '\0')
				{
					//all casses
					if (cstr_cmp_skip(ptr, "type"))
					{
						//skeep spaces
						skeep_space_end_comment(m_context->m_line, ptr);
						//parse textures
						if (!parse_queue_type(ptr, &ptr, p_queue.m_type))
						{
							push_error("Queue type not valid");
							return false;
						}
						//skeep spaces
						skeep_space_end_comment(m_context->m_line, ptr);
					}
					else if (cstr_cmp_skip(ptr, "order"))
					{
						//skeep spaces
						skeep_space_end_comment(m_context->m_line, ptr);
						//parse textures
						if (!parse_int(ptr, &ptr, p_queue.m_order))
						{
							push_error("Queue order value not valid");
							return false;
						}
						//skeep spaces
						skeep_space_end_comment(m_context->m_line, ptr);
					}
					else
					{
						push_error("Keyword not valid");
						return false;
					}
					//skeep spaces
					skeep_space_end_comment(m_context->m_line, ptr);
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
			if (!cstr_cmp_skip(ptr, "blend"))
			{
				push_error("Blend not found");
				return false;
			}
			//skeep "line" space
			skeep_line_space(ptr);
			//string
			std::string param1, param2;
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
			if (!(cstr_cmp_skip(ptr, "depth") || cstr_cmp_skip(ptr, "zbuffer")))
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
				pass.m_depth = depth_buffer_state({ DM_DISABLE });
				return true;
			}
			//parse
			pass.m_depth.m_mode = DM_ENABLE_AND_WRITE;
			pass.m_depth.m_type = depth_from_string(param1, DT_LESS);
			return true;
		}
		bool parse_cullface(const char*& ptr, pass_field& pass)
		{
			//search source attribute
			if (!(cstr_cmp_skip(ptr, "cullface")))
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
			pass.m_cullface.m_cullface = cullface_from_string(param1, CF_BACK);
			return true;
		}
		bool parse_lights(const char*& ptr, pass_field& pass)
		{
			//search source attribute
			if (!(cstr_cmp_skip(ptr, "lights")))
			{
				push_error("Lights not found");
				return false;
			}
			//skeep "line" space
			skeep_line_space(ptr);
			//is off?
			if ((cstr_cmp_skip(ptr, "off")))
			{
				//default pass.m_lights
				return true;
			}
			//is only_ambient?
			if ((cstr_cmp_skip(ptr, "only_ambient")))
			{
				pass.m_lights = LF_AMBIENT;
				return true;
			}
			//spot point and direction
			if ((cstr_cmp_skip(ptr, "spot_point_direction")))
			{
				pass.m_lights = LF_SPOT_POINT_DIRECTION;
				return true;
			}
			//spot and point
			if ((cstr_cmp_skip(ptr, "spot_point")))
			{
				pass.m_lights = LF_SPOT_POINT;
				return true;
			}
			//spot and direction
			if ((cstr_cmp_skip(ptr, "spot_direction")))
			{
				pass.m_lights = LF_SPOT_DIRECTION;
				return true;
			}
			//point and direction
			if ((cstr_cmp_skip(ptr, "point_direction")))
			{
				pass.m_lights = LF_POINT_DIRECTION;
				return true;
			}
			//spot
			if ((cstr_cmp_skip(ptr, "spot")))
			{
				pass.m_lights = LF_SPOT;
				return true;
			}
			//spot
			if ((cstr_cmp_skip(ptr, "point")))
			{
				pass.m_lights = LF_POINT;
				return true;
			}
			//spot
			if ((cstr_cmp_skip(ptr, "direction")))
			{
				pass.m_lights = LF_DIRECTION;
				return true;
			}
			//error
			push_error("Lights parameter not valid");
			//end
			return false;
		}
		bool parse_shader(const char*& ptr, pass_field& pass)
		{
			//search shader attribute
			if (!cstr_cmp_skip(ptr, "shader"))
			{
				push_error("Shader not found");
				return false;
			}
			//skeep spaces
			skeep_space_end_comment(m_context->m_line, ptr);
			//is a shader name?
			if (is_start_string(*ptr))
			{
				//shader name
				std::string shader_name;
				//get
				if (!parse_cstring(m_context->m_line, ptr, &ptr, shader_name))
				{
					push_error("Not valid shader name");
					return false;
				}
				pass.m_shader.m_name = true;
				pass.m_shader.m_text = shader_name;
				return true;
			}
			//else is a shader source
			if (!cstr_cmp_skip(ptr, "source"))
			{
				push_error("Shader source not found");
				return false;
			}
			//skeep spaces
			skeep_space_end_comment(m_context->m_line, ptr);
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
					//comment test
					if (is_start_multy_line_comment(ptr)) { c_inside = true; }
					else if (is_end_multy_line_comment(ptr)) { c_inside = false; }
					//inc case
					if (is_start_table(*ptr) && !c_inside) ++p_count;
					//dec case
					if (is_end_table(*ptr) && !c_inside) --p_count;
					//exit case
					if (!p_count) break;
					//inc count line
					if (*ptr == '\n') ++m_context->m_line;
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
		bool parse_type(const char* in,
			const char** cout,
			parameter_type& type)
		{
			if (cstr_cmp(in, "int"))
			{
				(*cout) += 3; type = parameter_type::PT_INT;  return true;
			}

			if (cstr_cmp(in, "float"))
			{
				(*cout) += 5; type = parameter_type::PT_FLOAT;  return true;
			}

			if (cstr_cmp(in, "texture"))
			{
				(*cout) += 7; type = parameter_type::PT_TEXTURE;  return true;
			}

			if (cstr_cmp(in, "vec2"))
			{
				(*cout) += 4; type = parameter_type::PT_VEC2;  return true;
			}

			if (cstr_cmp(in, "vec3"))
			{
				(*cout) += 4; type = parameter_type::PT_VEC3;  return true;
			}

			if (cstr_cmp(in, "vec4"))
			{
				(*cout) += 4; type = parameter_type::PT_VEC4;  return true;
			}

			if (cstr_cmp(in, "mat4"))
			{
				(*cout) += 4; type = parameter_type::PT_MAT4;  return true;
			}

			return false;
		}
		bool parse_queue_type(const char* in,
			const char** cout,
			render_scene_queue_type& type)
		{
			if (cstr_cmp(in, "opaque"))
			{
				(*cout) += sizeof("opaque") - 1; type = render_scene_queue_type::RQ_OPAQUE;  return true;
			}

			if (cstr_cmp(in, "translucent"))
			{
				(*cout) += sizeof("translucent") - 1; type = render_scene_queue_type::RQ_TRANSLUCENT;  return true;
			}

			if (cstr_cmp(in, "ui"))
			{
				(*cout) += sizeof("ui") - 1;; type = render_scene_queue_type::RQ_UI;  return true;
			}

			if (cstr_cmp(in, "background"))
			{
				(*cout) += sizeof("background") - 1; type = render_scene_queue_type::RQ_BACKGROUND;  return true;
			}

			return false;
		}
		//////////////////////////////////////////////////////
		void push_error(const std::string& error)
		{
			m_context->m_errors.push_front(error_field{ m_context->m_line,error });
		}

	};

}
}