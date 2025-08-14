#include<sb7.h>
#include<string>
#include<vmath.h>
#include"myOpenGLErrorCheck.h"

class bezier : public sb7::application
{
public:
    bezier();
    ~bezier();

    void init()
    {
        sb7::application::init();
        GLchar title[] = { "" };
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
DECLARE_MAIN(bezier);

void bezier::startup()
{
    std::string ver_src =
        R"(
            
        )";
    std::string tcs_src =
        R"(
            
        )";
    std::string tes_src =
        R"(
            
        )";
    std::string frg_src =
        R"(
            
        )";

    Error::set_shader_source(ver_src, frg_src, tcs_src, tes_src);
    program = Error::compile_program();
    glUseProgram(program);
}

void bezier::render(double time)
{
}

void bezier::shutdown()
{
}

bezier::bezier()
{
}

bezier::~bezier()
{
}
