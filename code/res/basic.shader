#shader vertex
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv_coord;

out vec3 v_pos;
out vec3 v_normal;
out vec2 v_texcoord;

uniform mat4 view;
uniform mat4 proj;

void main()
{
	v_pos = position;
	v_texcoord = uv_coord;
	v_normal = normal;
	gl_Position = proj * view * vec4(position, 1.0f);
};


#shader fragment
#version 330 core

layout(location = 0) out vec4 color;

in vec3 v_pos;
in vec3 v_normal;
in vec2 v_texcoord;

uniform sampler2D u_Texture;

uniform vec3 cameraPos;
uniform vec3 lightPos;
uniform vec4 f_lightColor;

uniform float a_strength;
uniform float d_strength;
uniform float s_strength;

uniform int wireframe;

void main()
{
	vec3 light = lightPos;
	vec3 lightColor = f_lightColor.xyz;

	float ambientStrength = a_strength;
	float specularStrength = s_strength;

	vec3 ambient = ambientStrength * lightColor;

	vec3 normal = normalize(v_normal);
	vec3 lightDir = normalize(light - v_pos);

	float diffuseStrength = d_strength;
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = diffuseStrength * diff * lightColor;

	vec3 viewDir = normalize(cameraPos - v_pos);
	vec3 reflectDir = reflect(-lightDir, normal);

	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
	vec3 specular = specularStrength * spec * lightColor;

	vec4 text = texture2D(u_Texture, v_texcoord);
	vec3 result = ambient + diffuse + specular;
	if(wireframe == 0)
		color = vec4(result * text.xyz, 1.0);
	else if (wireframe == 1)
		color = vec4(0.0, 0.0, 1.0, 1.0);
};