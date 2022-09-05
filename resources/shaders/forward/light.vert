#version 450

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout(location = 0) in vec3 in_pos;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_uv;

layout(set = 0, binding = 0) uniform CameraData
{
    vec4 world_pos;
    mat4 proj;
    mat4 view;
} u_camera_data;

layout(push_constant) uniform ModelPushConst
{
    mat4 model;
    int  material_idx;
} mpushconst;

layout(location = 0) out vec2 out_uv;
layout(location = 1) out vec3 out_world_pos;
layout(location = 2) out vec3 out_normal;
layout(location = 3) out vec3 out_camera_pos;

out gl_PerVertex
{
    vec4 gl_Position;
};

void main()
{
    out_uv = in_uv;
    out_normal = normalize((mpushconst.model * vec4(in_normal, 0.0)).xyz);
    out_camera_pos = u_camera_data.world_pos.xyz;
    out_world_pos = (mpushconst.model * vec4(in_pos, 1.0)).xyz;
    gl_Position = u_camera_data.proj * u_camera_data.view * vec4(out_world_pos, 1.0);
}
