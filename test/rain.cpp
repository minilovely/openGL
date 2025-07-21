#include"sb7.h"
#include<vmath.h>
#include<sb7ktx.h>
#include<random>

static unsigned int seed = 0x13371337;

static inline float random_float()
{
	float res;
	unsigned int tmp;

	seed *= 16807;

	tmp = seed ^ (seed >> 4) ^ (seed << 15);

	*((unsigned int*)&res) = (tmp >> 9) | 0x3F800000;

	return (res - 1.0f);
}

class rain: public sb7::application
{
public:
	GLuint compile_shader()
	{
		GLuint vertex_shader;
		GLuint fragment_shader;
		GLuint program;

		static const GLchar* vertex_source[] =
		{
			R"(
				#version 450 core
				layout(location = 0) in int sth_index;
				out VS_OUT
				{
				//flat是禁用插值
					flat int sth;
					vec2 texcoord;
				} vs_out;
				
				struct droplet_t
				{
					float x_offset;
					float y_offset;
					float orientation;
					float unused;
				};
				
				layout (std140) uniform droplets
				{
					droplet_t droplet[256];
				};
				void main()
				{
					const vec2 position[4] = vec2[4] (vec2(-0.5,-0.5),
														vec2(0.5,-0.5),
														vec2(-0.5,0.5),
														vec2(0.5,0.5));
					vs_out.texcoord = position[gl_VertexID].xy + vec2(0.5);
					float co = cos(droplet[sth_index].orientation);
					float so = sin(droplet[sth_index].orientation);
					//这是绕z轴的旋转矩阵
					mat2 rot = mat2(vec2(co,so),vec2(-so,co));
					vec2 pos = 0.25*rot*position[gl_VertexID];
					gl_Position = vec4(pos.x + droplet[sth_index].x_offset, pos.y + droplet[sth_index].y_offset, 0.5, 1.0);
					vs_out.sth = sth_index % 64;
				}
			)"
		};
		static const GLchar* fragment_source[] =
		{
			R"(
				#version 450 core
				layout(location = 0) out vec4 color;
				in VS_OUT
				{
					flat int sth;
					vec2 texcoord;
				} fs_in;
				layout(binding = 0) uniform sampler2DArray tex_sth;
				void main()
				{
					color = texture(tex_sth,vec3(fs_in.texcoord,float(fs_in.sth)));
				}
			)"
		};
		program = glCreateProgram();
		vertex_shader = glCreateShader(GL_VERTEX_SHADER);
		fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

		glShaderSource(vertex_shader, 1, vertex_source, NULL);
		glShaderSource(fragment_shader, 1, fragment_source, NULL);

		glCompileShader(vertex_shader);
		glCompileShader(fragment_shader);

		glAttachShader(program, vertex_shader);
		glAttachShader(program, fragment_shader);

		glLinkProgram(program);

		glDeleteShader(vertex_shader);
		glDeleteShader(fragment_shader);

		return program;
	}

	void init()
	{
		static const char title[] = { "rain" };
		sb7::application::init();
		memcpy(info.title, title, sizeof(title));
	}

	void startup()
	{
		program = compile_shader();
		GLint success;
		glGetProgramiv(program, GL_LINK_STATUS, &success);
		if (!success)
		{
			GLchar infoLog[512];
			glGetProgramInfoLog(program, 512, NULL, infoLog);
			printf("Shader linking failed: %s\n", infoLog);
		}
		else
		{
			GLchar infoLog[512];
			glGetProgramInfoLog(program, 512, NULL, infoLog);
			printf("Shader linking success: %s\n", infoLog);
		}
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		glGenBuffers(1, &rain_buffer);
		glBindBuffer(GL_UNIFORM_BUFFER, rain_buffer);
		glBufferData(GL_UNIFORM_BUFFER, 256 * sizeof(vmath::vec4), NULL, GL_DYNAMIC_DRAW);


		//rain_buffer数据填充
		float random = static_cast<float>(rand()) / RAND_MAX;
		for (int i = 0; i < 256; i++)
		{
			droplet_x_offset[i] = random_float() * 2.0f - 1.0f;
			droplet_rot_speed[i] = (random_float() + 0.5f) * ((i & 1) ? -3.0f : 3.0f);
			droplet_fall_speed[i] = random_float() + 0.2f;
		}
		//纹理setup
		texture = sb7::ktx::file::load("D:/Unity/resourse/OpenGLSuperBible7th/resourse/resourse/sb7code-master/sb7code-master/bin/media/textures/aliens.ktx");
		glBindTexture(GL_TEXTURE_2D_ARRAY, texture);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	void render(double currentTime)
	{
		glUseProgram(program);

		static const GLfloat black[] = { 0.0f, 0.0f , 0.0f, 0.0f };
		glClearBufferfv(GL_COLOR, 0, black);

		glBindBufferBase(GL_UNIFORM_BUFFER, 0, rain_buffer);

		float t = (float)currentTime;

		glViewport(0, 0, info.windowWidth, info.windowHeight);

		vmath::vec4* droplet = (vmath::vec4*)glMapBufferRange(GL_UNIFORM_BUFFER, 0, 256 * sizeof(vmath::vec4), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);

		for (int i = 0; i < 256; i++)
		{
			droplet[i][0] = droplet_x_offset[i];
			droplet[i][1] = 2.0f - fmodf((t + float(i)) * droplet_fall_speed[i], 4.31f);
			droplet[i][2] = t * droplet_rot_speed[i];
			droplet[i][3] = 0.0f;
		}
		glUnmapBuffer(GL_UNIFORM_BUFFER);

		int sth_index;
		for (sth_index = 0;sth_index < 256;sth_index++)
		{
			glVertexAttribI1i(0, sth_index);
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		}
	}

	void shutdown()
	{
		glDeleteProgram(program);
	}

private:
	GLuint program;
	GLuint vao;
	GLuint rain_buffer;
	GLuint texture;

	float droplet_x_offset[256];
	float droplet_fall_speed[256];
	float droplet_rot_speed[256];

};
DECLARE_MAIN(rain);
