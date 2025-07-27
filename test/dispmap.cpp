#include<sb7.h>
#include"myOpenGLErrorCheck.h"


class Dispmap : public sb7::application
{
public:
	Dispmap();
	~Dispmap();

	void init()
	{
		sb7::application::init();
		GLchar title[] = { "Dispmap" };
		memcpy(info.title, title, sizeof(title));
	}
	void startup();
	void render(double time);
	void shutdown();

private:
	GLuint vao;
	GLuint buffer;
	GLuint program;
};
DECLARE_MAIN(Dispmap);

void Dispmap::startup()
{

	program = compile_program();
}
void Dispmap::render(double time)
{

}
void Dispmap::shutdown()
{

}

Dispmap::Dispmap()
{
}

Dispmap::~Dispmap()
{
}