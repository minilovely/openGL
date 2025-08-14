#include<sb7.h>
#include<string>
#include<vmath.h>
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
	std::string ver_src = 
		R"(
			#version 420 core

            out VS_OUT
            {
                vec2 tc;
            } vs_out;
 
            void main(void)
            {
                const vec4 vertices[] = vec4[](vec4(-0.5, 0.0, -0.5, 1.0),
                                               vec4( 0.5, 0.0, -0.5, 1.0),
                                               vec4(-0.5, 0.0,  0.5, 1.0),
                                               vec4( 0.5, 0.0,  0.5, 1.0));

                int x = gl_InstanceID & 63;
                int y = gl_InstanceID >> 6;
                vec2 offs = vec2(x, y);
 
                vs_out.tc = (vertices[gl_VertexID].xz + offs + vec2(0.5)) / 64.0;
                gl_Position = vertices[gl_VertexID] + vec4(float(x - 32), 0.0,
                                                           float(y - 32), 0.0);
            }
		)";
	std::string tcs_src =
		R"(
			#version 420 core

            layout (vertices = 4) out;

            in VS_OUT
            {
                vec2 tc;
            } tcs_in[];

            out TCS_OUT
            {
                vec2 tc;
            } tcs_out[];

            uniform mat4 mvp_matrix;

            void main(void)
            {
                if (gl_InvocationID == 0)
                {
                    vec4 p0 = mvp_matrix * gl_in[0].gl_Position;
                    vec4 p1 = mvp_matrix * gl_in[1].gl_Position;
                    vec4 p2 = mvp_matrix * gl_in[2].gl_Position;
                    vec4 p3 = mvp_matrix * gl_in[3].gl_Position;
                    p0 /= p0.w;
                    p1 /= p1.w;
                    p2 /= p2.w;
                    p3 /= p3.w;
                    if (p0.z <= 0.0 ||
                        p1.z <= 0.0 ||
                        p2.z <= 0.0 ||
                        p3.z <= 0.0)
                     {
                          gl_TessLevelOuter[0] = 0.0;
                          gl_TessLevelOuter[1] = 0.0;
                          gl_TessLevelOuter[2] = 0.0;
                          gl_TessLevelOuter[3] = 0.0;
                     }
                     else
                     {
                        float l0 = length(p2.xy - p0.xy) * 16.0 + 1.0;
                        float l1 = length(p3.xy - p2.xy) * 16.0 + 1.0;
                        float l2 = length(p3.xy - p1.xy) * 16.0 + 1.0;
                        float l3 = length(p1.xy - p0.xy) * 16.0 + 1.0;
                        gl_TessLevelOuter[0] = l0;
                        gl_TessLevelOuter[1] = l1;
                        gl_TessLevelOuter[2] = l2;
                        gl_TessLevelOuter[3] = l3;
                        gl_TessLevelInner[0] = min(l1, l3);
                        gl_TessLevelInner[1] = min(l0, l2);
                    }
                }
                gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
                tcs_out[gl_InvocationID].tc = tcs_in[gl_InvocationID].tc;
            }

		)";
	std::string tes_src =
		R"(
			#version 420 core

            layout (quads, fractional_odd_spacing) in;

            uniform sampler2D tex_displacement;

            uniform mat4 mv_matrix;
            uniform mat4 proj_matrix;
            uniform float dmap_depth;

            in TCS_OUT
            {
                vec2 tc;
            } tes_in[];

            out TES_OUT
            {
                vec2 tc;
                vec3 world_coord;
                vec3 eye_coord;
            } tes_out;

            void main(void)
            {
                vec2 tc1 = mix(tes_in[0].tc, tes_in[1].tc, gl_TessCoord.x);
                vec2 tc2 = mix(tes_in[2].tc, tes_in[3].tc, gl_TessCoord.x);
                vec2 tc = mix(tc2, tc1, gl_TessCoord.y);

                vec4 p1 = mix(gl_in[0].gl_Position, gl_in[1].gl_Position, gl_TessCoord.x);
                vec4 p2 = mix(gl_in[2].gl_Position, gl_in[3].gl_Position, gl_TessCoord.x);
                vec4 p = mix(p2, p1, gl_TessCoord.y);
                p.y += texture(tex_displacement, tc).r * dmap_depth;

                vec4 P_eye = mv_matrix * p;

                tes_out.tc = tc;
                tes_out.world_coord = p.xyz;
                tes_out.eye_coord = P_eye.xyz;

                gl_Position = proj_matrix * P_eye;
            }

		)";
	std::string frg_src =
		R"(
			#version 420 core

            out vec4 color;

            layout (binding = 1) uniform sampler2D tex_color;

            uniform bool enable_fog = true;
            uniform vec4 fog_color = vec4(0.7, 0.8, 0.9, 0.0);

            in TES_OUT
            {
                vec2 tc;
                vec3 world_coord;
                vec3 eye_coord;
            } fs_in;

            vec4 fog(vec4 c)
            {
                float z = length(fs_in.eye_coord);

                float de = 0.025 * smoothstep(0.0, 6.0, 10.0 - fs_in.world_coord.y);
                float di = 0.045 * (smoothstep(0.0, 40.0, 20.0 - fs_in.world_coord.y));

                float extinction   = exp(-z * de);
                float inscattering = exp(-z * di);

                return c * extinction + fog_color * (1.0 - inscattering);
            }

            void main(void)
            {
                vec4 landscape = texture(tex_color, fs_in.tc);

                if (enable_fog)
                {
                    color = fog(landscape);
                }
                else
                {
                    color = landscape;
                }
            }

		)";


	Error::set_shader_source(ver_src,frg_src,tcs_src,tes_src);
	program = Error::compile_program();
	glUseProgram(program);
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