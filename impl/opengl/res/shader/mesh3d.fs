#version 330 core

struct SunLight
{
  vec3 direction;

  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};

struct SpotLight
{
  vec3 position;
  vec3 direction;

  vec3 ambient;
  vec3 diffuse;
  vec3 specular;

  float constant;
  float linear;
  float quadratic;

  float cut_off;
  float outer_cut_off;
};

struct PointLight
{
  vec3 position;

  vec3 ambient;
  vec3 diffuse;
  vec3 specular;

  float constant;
  float linear;
  float quadratic;
};

in vec3 vert_position;
in vec3 vert_normal;

uniform vec3 camera_pos;
uniform vec3 mesh_color;
uniform float mesh_specular;

uniform int sun_lights_size;
uniform SunLight sun_lights[32];

uniform int spot_lights_size;
uniform SpotLight spot_lights[32];

uniform int point_lights_size;
uniform PointLight point_lights[32];

out vec4 FragColor;

vec3 calc_sun_light(SunLight light, vec3 view_dir)
{
  vec3 light_dir = normalize(-light.direction);

  // Diffuse
  vec3 diffuse = light.diffuse * max(dot(vert_normal, light_dir), 0.0);
  // Specular
  vec3 reflect_dir = reflect(-light_dir, vert_normal);
  vec3 specular = light.specular * pow(max(dot(view_dir, reflect_dir), 0.0), mesh_specular);

  return light.ambient + diffuse + specular;
}

vec3 calc_spot_light(SpotLight light, vec3 view_dir)
{
  vec3 light_dir = normalize(light.position - vert_position);

  // Diffuse
  vec3 diffuse = light.diffuse * max(dot(vert_normal, light_dir), 0.0);

  // Specular
  vec3 reflectDir = reflect(-light_dir, vert_normal);
  vec3 specular = light.specular * pow(max(dot(view_dir, reflectDir), 0.0), mesh_specular);

  // Attenuation
  float dis = length(light.position - vert_position);
  float attenuation = 1.0 / (light.constant + light.linear * dis + light.quadratic * (dis * dis));

  // Cut Off
  float theta = dot(light_dir, normalize(-light.direction));
  float epsilon = light.cut_off - light.outer_cut_off;
  float intensity = clamp((theta - light.outer_cut_off) / epsilon, 0.0, 1.0);

  return (light.ambient + (diffuse + specular) * intensity) * attenuation;
}

vec3 calc_point_light(PointLight light, vec3 view_dir)
{
  vec3 light_dir = normalize(light.position - vert_position);

  // Diffuse
  vec3 diffuse = light.diffuse * max(dot(vert_normal, light_dir), 0.0);

  // Specular
  vec3 reflectDir = reflect(-light_dir, vert_normal);
  vec3 specular = light.specular * pow(max(dot(view_dir, reflectDir), 0.0), mesh_specular);

  // Attenuation
  float dis = length(light.position - vert_position);
  float attenuation = 1.0 / (light.constant + light.linear * dis + light.quadratic * (dis * dis));

  return (light.ambient + diffuse + specular) * attenuation;
}

void main()
{
  vec3 view_dir = normalize(camera_pos - vert_position);
  vec3 result = vec3(10);

  // sun lights
  // for (int i = 0; i < sun_lights_size; i++) result += calc_sun_light(sun_lights[i], view_dir);
  // spot lights
  // for (int i = 0; i < spot_lights_size; i++) result += calc_spot_light(spot_lights[i], view_dir);
  // point lights
  // for (int i = 0; i < point_lights_size; i++) result += calc_point_light(point_lights[i], view_dir);

  FragColor = vec4(result * mesh_color, 1.0);
}
