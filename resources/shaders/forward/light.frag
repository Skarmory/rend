#version 450

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

#define MAX_LIGHTS 256
#define MAX_MATERIALS 256

struct PointLight
{
    vec4  colour;
    float radius;
    float power;
};

layout(push_constant) uniform ModelPushConst
{
    mat4 model;
    int  material_idx;
} mpushconst;

layout(set = 0, binding = 1) uniform PointLightData
{
    int        light_count; 
    vec4       positions[MAX_LIGHTS];
    PointLight point_lights[MAX_LIGHTS]; 
} u_lights;

layout(set = 1, binding = 0) uniform sampler2D u_diffuse;

layout(location = 0) in vec2 in_uv;
layout(location = 1) in vec3 in_world_pos;
layout(location = 2) in vec3 in_normal;
layout(location = 3) in vec3 in_camera_pos;

layout(location = 0) out vec4 out_frag_colour;

vec3 calculate_point_light(vec3 light_pos, PointLight light, vec3 world_pos, vec3 normal, vec3 view_dir, vec2 uv)
{
    vec3 light_dir   = light_pos - world_pos;
    float distance = length(light_dir);
    distance = distance * distance;
    light_dir = normalize(light_dir);

    vec3 texture_sample = texture(u_diffuse, in_uv).rgb;
    vec3 ambient_colour = texture_sample * 0.1;

    float specular = 0.0;
    float lambertian = max(dot(normal, light_dir), 0.0);

    if(lambertian > 0.0)
    {
        vec3 half_dir    = normalize(light_dir + view_dir);
        float spec_angle = max(dot(normal, half_dir), 0.0);
        specular         = pow(spec_angle, 16.0);
    }

    vec3 diffuse_colour  = texture_sample * lambertian * light.colour.rgb * light.power / distance;
    vec3 specular_colour = texture_sample * specular   * light.colour.rgb * light.power / distance;

    return ambient_colour + diffuse_colour + specular_colour;
}

void main()
{
    vec3 colour = vec3(0.0);
    vec3 view_dir = normalize(in_camera_pos - in_world_pos);

    for(int lidx = 0; lidx < u_lights.light_count; ++lidx)
    {
        colour += calculate_point_light(u_lights.positions[lidx].xyz, u_lights.point_lights[lidx], in_world_pos, in_normal, view_dir, in_uv);
    }

    out_frag_colour = vec4(colour, 1.0);
}
