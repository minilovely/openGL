#include "myOpenGLErrorCheck.h"

namespace Error
{
	void set_shader_source(std::string ver_src, std::string frg_src)
	{
		ver_str = ver_src;
		frg_str = frg_src;

		vertex_source = ver_str.c_str();
		fragment_source = frg_str.c_str();
	}
	void set_shader_source(std::string ver_src, std::string frg_src,std::string tcs_src,std::string tes_src)
	{
		ver_str = ver_src;
		frg_str = frg_src;
		tcs_str = tcs_src;
		tes_str = tes_src;

		vertex_source = ver_str.c_str();
		fragment_source = frg_str.c_str();
		tessellation_control_source = tcs_str.c_str();
		tessellation_evaluation_source = tes_str.c_str();

	}

	GLuint compile_program()
	{

		if (!ver_str.empty())
		{
			vs_isActive = true;
			vertex_shader = glCreateShader(GL_VERTEX_SHADER);
			glShaderSource(vertex_shader, 1, &vertex_source, NULL);
			glCompileShader(vertex_shader);
			if (!frg_str.empty())
			{
				fs_isActive = true;
				fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
				glShaderSource(fragment_shader, 1, &fragment_source, NULL);
				glCompileShader(fragment_shader);
				if (!tcs_str.empty())
				{
					tcs_isActive = true;
					tessellation_control_shader = glCreateShader(GL_TESS_CONTROL_SHADER);
					glShaderSource(tessellation_control_shader, 1, &tessellation_control_source, NULL);
					glCompileShader(tessellation_control_shader);
					if (!tes_str.empty())
					{
						tes_isActive = true;
						tessellation_evluation_shader = glCreateShader(GL_TESS_EVALUATION_SHADER);
						glShaderSource(tessellation_evluation_shader, 1, &tessellation_control_source, NULL);
						glCompileShader(tessellation_evluation_shader);
					}
				}
			}
		}		
		program = glCreateProgram();

		check_shader_error(vertex_shader, fragment_shader,
							tessellation_control_shader,tessellation_evluation_shader);

		attachShader(vertex_shader, fragment_shader,
			tessellation_control_shader, tessellation_evluation_shader);
		glLinkProgram(program);

		check_program_error(program);

		glDeleteShader(vertex_shader);
		glDeleteShader(fragment_shader);

		return program;
	}
	void attachShader(GLuint vertex_shader, GLuint fragment_shader,
						GLuint	tessellation_control_shader, GLuint tessellation_evluation_shader)
	{
		if (vs_isActive)
		{
			glAttachShader(program, vertex_shader);
			if (fs_isActive)
			{
				glAttachShader(program, fragment_shader);
				if (tcs_isActive)
				{
					glAttachShader(program, tessellation_control_shader);
					if (tes_isActive)
					{
						glAttachShader(program, tessellation_evluation_shader);
					}
				}
			}
		}
	}
	void check_shader_error(GLuint vertex_shader, GLuint fragment_shader,
							GLuint	tessellation_control_shader, GLuint tessellation_evluation_shader)
	{
		GLint success;
		GLint Log_length;
		std::vector<GLchar> Log_info;

		glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
		if (vs_isActive)
		{
			if (!success)
			{
				glGetShaderiv(vertex_shader, GL_INFO_LOG_LENGTH, &Log_length);
				Log_info.resize(Log_length);
				glGetShaderInfoLog(vertex_shader, Log_length, NULL, Log_info.data());
				std::cerr << RED << "Vertex Shader Compilation Error:\n" << RESET << Log_info.data() << std::endl;
			}
			glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
			if (fs_isActive)
			{
				if (!success)
				{
					glGetShaderiv(fragment_shader, GL_INFO_LOG_LENGTH, &Log_length);
					Log_info.resize(Log_length);
					glGetShaderInfoLog(fragment_shader, Log_length, NULL, Log_info.data());
					std::cerr << RED << "Fragment Shader Compilation Error:\n" << RESET << Log_info.data() << std::endl;
				}
				glGetShaderiv(tessellation_control_shader, GL_COMPILE_STATUS, &success);
				if (tcs_isActive)
				{
					if (!success)
					{
						glGetShaderiv(tessellation_control_shader, GL_INFO_LOG_LENGTH, &Log_length);
						Log_info.resize(Log_length);
						glGetShaderInfoLog(tessellation_control_shader, Log_length, NULL, Log_info.data());
						std::cerr << RED << "Tessellation Control Shader Compilation Error:\n" << RESET << Log_info.data() << std::endl;
					}
					glGetShaderiv(tessellation_evluation_shader, GL_COMPILE_STATUS, &success);
					if (tes_isActive)
					{
						if (!success)
						{
							glGetShaderiv(tessellation_evluation_shader, GL_INFO_LOG_LENGTH, &Log_length);
							Log_info.resize(Log_length);
							glGetShaderInfoLog(tessellation_evluation_shader, Log_length, NULL, Log_info.data());
							std::cerr << RED << "tessellation Evluation Shader Compilation Error:\n" << RESET << Log_info.data() << std::endl;
						}
					}
				}
			}
		}
	}
	void check_program_error(GLuint program)
	{
		GLint success;
		GLint Log_length;
		std::vector<GLchar> Log_info;
		glGetProgramiv(program, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &Log_length);
			Log_info.resize(Log_length);
			glGetProgramInfoLog(program, Log_length, NULL, Log_info.data());
			std::cerr << RED << "Program Linking Error:\n" << RESET << Log_info.data() << std::endl;
		}
	}

}