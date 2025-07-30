#ifndef __SHADERS_H__
#define __SHADERS_H__

#define SH_DEFAULT_VERT str_view_make_from_lit( \
"#version 460 core\n" \
"layout (location=0) in vec2  a_position;\n" \
"layout (location=1) in vec2  a_texcoord;\n" \
"layout (location=2) in vec3  a_color;\n" \
"layout (location=3) in float a_opacity;\n" \
"\n" \
"out vec2 v_texcoord;\n" \
"out vec4 v_blend;\n" \
"\n" \
"uniform mat3 u_proj;\n" \
"\n" \
"void\n" \
"main() {\n" \
"  gl_Position = vec4(u_proj * vec3(a_position, 0.0), 1.0);\n" \
"  v_texcoord = a_texcoord;\n" \
"  v_blend = vec4(a_color, a_opacity);\n" \
"}\n")

#define SH_DEFAULT_FRAG str_view_make_from_lit( \
"#version 460 core\n" \
"in vec2 v_texcoord;\n" \
"in vec4 v_blend;\n" \
"\n" \
"out vec4 f_color;\n" \
"\n" \
"uniform sampler2D texture_atlas;\n" \
"\n" \
"void\n" \
"main() {\n" \
"  f_color = texture(texture_atlas, v_texcoord) * v_blend;\n" \
"}\n")

#endif/*__SHADERS_H__*/
