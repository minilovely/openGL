#include "sb7.h"
#include <math.h>
#include<vmath.h>


class update : public sb7::application
{
public:
	//初始化
	void startup()
	{
		render_Program = compile_shader();
		//crate VAO
		glCreateVertexArrays(1, &vertex_array_object);
		//create buffer，the second parameter accepts handle
		glCreateBuffers(2, &buffer[0]);

	}
	//渲染函数
	void render(double currentTime)
	{
		//颜色，随着时间而改变。分别是rgba，红绿蓝和透明度
		const GLfloat color[] = { 0.0f,
									0.2f,
									0.0f,
									1.0f };
		GLfloat attrib[] = { (float)sin(currentTime) * 0.5f,
							(float)cos(currentTime) * 0.6f,
							0.0f,0.0f };

		//clear buffer and set the data with the third parameter
		glClearBufferfv(GL_COLOR, 0, color);

		//set active for this VAO 
		glBindVertexArray(vertex_array_object);
		
		//apply memory for each buffer
		glNamedBufferStorage(buffer[0], sizeof(color), color, GL_DYNAMIC_STORAGE_BIT);
		glNamedBufferStorage(buffer[1], sizeof(attrib), attrib, GL_DYNAMIC_STORAGE_BIT);
		//update data for each frame
		glNamedBufferSubData(buffer[1], 0, sizeof(attrib), attrib);
		//bind buffer[0] to VAO
		glVertexArrayVertexBuffer(vertex_array_object, 0, buffer[0], 0, 0);
		glVertexArrayVertexBuffer(vertex_array_object, 1, buffer[1], 0, 0);
		//setup VAO attribution format
		glVertexArrayAttribFormat(vertex_array_object, 0, 4, GL_FLOAT, GL_FALSE, 0);
		glVertexArrayAttribFormat(vertex_array_object, 1, 4, GL_FLOAT, GL_FALSE, 0);
		//match the attribution with binding point
		glVertexArrayAttribBinding(vertex_array_object, 0, 0);
		glVertexArrayAttribBinding(vertex_array_object, 1, 1);
		//active the attribution so that the data can be used
		glEnableVertexArrayAttrib(vertex_array_object, 0);
		glEnableVertexAttribArray(1);

		glUseProgram(render_Program);

		//glVertexAttrib1fv(0, attrib);
		//glPointSize(40.0f);
		glDrawArrays(GL_TRIANGLES, 0, 3);
	}
	//编译·链接着色器
	GLuint compile_shader(void)
	{
		GLuint vertex_shader;
		GLuint fragment_shader;

		GLuint program;

		static const GLchar* vertex_shader_source[] =
		{
			//"#version 450 core	\n"
			//"					\n"
			//"void main(void)	\n"
			R"(
				#version 450 core
				
				layout (location = 1) in vec4 offset;
				void main(void)
				{
					const vec4 vertices[3] = vec4[3](
												vec4(0.25,-0.25,0.5,1.0),
												vec4(-0.25,-0.25,0.5,1.0),
												vec4(0.25,0.25,0.5,1.0)
											);
					gl_Position = vertices[gl_VertexID] + vec4(offset.x, 0.0, 0.0, 0.0);
				}
			)"
		};

		static const GLchar* fragment_shader_source[] =
		{
			R"(
				#version 450 core
				
				out vec4 color;
				
				void main(void)
				{
					color = vec4(0.0,0.8,1.0,1.0);
				}
			)"
		};
		vertex_shader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex_shader, 1, vertex_shader_source, NULL);
		glCompileShader(vertex_shader);


		fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment_shader, 1, fragment_shader_source, NULL);
		glCompileShader(fragment_shader);

		program = glCreateProgram();
		glAttachShader(program, vertex_shader);
		glAttachShader(program, fragment_shader);
		glLinkProgram(program);

		glDeleteShader(vertex_shader);
		glDeleteShader(fragment_shader);

		return program;

	}
	//清理程序对象
	void shutdown() {
		glDeleteVertexArrays(1, &vertex_array_object);
		glDeleteProgram(render_Program);
	}
private:
	GLuint render_Program;
	GLuint vertex_array_object;
	GLuint buffer[2];
};

// 函数实例运行
DECLARE_MAIN(update);
