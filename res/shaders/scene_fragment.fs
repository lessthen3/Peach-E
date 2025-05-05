#version 330

#ifndef MAX_POINT_LIGHTS
#define MAX_POINT_LIGHTS 5
#endif

#ifndef MAX_SPOT_LIGHTS
#define MAX_SPOT_LIGHTS 5
#endif

in vec2 outTexCoord;
in vec3 mvVertexNormal;
in vec3 mvVertexPos;

out vec4 fragColor;

struct Attenuation
{
	float constant;
	float linear;
	float exponent;
};

struct PointLight
{
	vec3 colour;
	vec3 position;
	float intensity;
	Attenuation att;
};

struct Material
{
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	int hasTexture;
	float reflectance;
};

struct DirectionalLight
{
	vec3 colour;
	vec3 direction;
	float intensity;
};

struct SpotLight
{
	PointLight pl;
	vec3 conedir;
	float cutoff;
};

struct Fog
{
	int activeFog;
	vec3 colour;
	float density;
};

uniform sampler2D texture_sampler;
uniform vec3 ambientLight;
//uniform vec3 camera_pos;
uniform float specularPower;
uniform Material material;
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform DirectionalLight directionalLight;
uniform SpotLight spotLights[MAX_SPOT_LIGHTS];
uniform Fog fog;

vec4 ambientC;
vec4 diffuseC;
vec4 speculrC;

void setupColours (Material material, vec2 textCoord)
{
	if(material.hasTexture == 1)
	{
		ambientC = texture(texture_sampler, textCoord);
		diffuseC = ambientC;
		speculrC = material.specular;
	}
	else
	{
		ambientC = material.ambient;
		diffuseC = material.diffuse;
		speculrC = material.specular;
	}
}

vec4 calcLightColour(vec3 light_colour, float light_intensity, vec3 position, vec3 to_light_dir, vec3 normal)
{
	vec4 diffuseColour = vec4(0, 0, 0, 0);
	vec4 specColour = vec4(0, 0, 0, 0);

	float diffuseFactor = max(dot(normal, to_light_dir), 0.0);
	diffuseColour = diffuseC * vec4(light_colour, 1.0) * light_intensity * diffuseFactor;

	vec3 camera_direction = normalize(-position);
	vec3 from_light_dir = -to_light_dir;
	vec3 reflected_light = normalize(reflect(from_light_dir, normal));
	float specularFactor = max(dot(camera_direction, reflected_light), 0.0);
	specularFactor = pow(specularFactor, specularPower);
	specColour = speculrC * light_intensity * specularFactor * material.reflectance * vec4(light_colour, 1.0);

	return (diffuseColour + specColour);
}

vec4 calcPointLight(PointLight light, vec3 position, vec3 normal)
{
	vec3 light_direction = light.position - position;
	vec3 to_light_dir = normalize(light_direction); //need normalized vectors for calculations
	vec4 light_colour = calcLightColour(light.colour, light.intensity, position, to_light_dir, normal);

	//Apply Attenuation, mimc light intensity drop off of 1/r^2
	float distance = length(light_direction);
	float attenuationInv = light.att.constant + light.att.linear * distance + light.att.exponent * distance * distance;

	return light_colour / attenuationInv;
}

vec4 calcSpotLight(SpotLight light, vec3 position, vec3 normal)
{
	vec3 light_direction = light.pl.position - position;
	vec3 to_light_dir = normalize(light_direction);
	vec3 from_light_dir = -to_light_dir;
	float spot_alfa = dot(from_light_dir, normalize(light.conedir));

	vec4 colour = vec4(0, 0, 0, 0);

	if(spot_alfa > light.cutoff)
	{
		colour = calcPointLight(light.pl, position, normal);
		colour *= (1.0 - (1.0 - spot_alfa)/(1.0 - light.cutoff));
	}

	return colour;
}

vec4 calcDirectionalLight(DirectionalLight light, vec3 position, vec3 normal)
{
	return calcLightColour(light.colour, light.intensity, position, normalize(light.direction), normal);
}

vec4 calcFog(vec3 pos, vec4 colour, Fog fog, vec3 ambientLight, DirectionalLight directionalLight)
{
	vec3 fogColour = fog.colour * (ambientLight + directionalLight.colour * directionalLight.intensity);
	float distance = length(pos);
	float fogFactor = 1.0 / exp( (distance * fog.density) * (distance * fog.density));
	fogFactor = clamp( fogFactor, 0.0, 1.0 );

	vec3 resultColour = mix(fogColour, colour.xyz, fogFactor);
	return vec4(resultColour.xyz, colour.w);
}

void main()
{
	setupColours(material, outTexCoord);

	vec4 diffuseSpecularComp = calcDirectionalLight(directionalLight, mvVertexPos, mvVertexNormal);
	
	for (int i = 0; i < MAX_POINT_LIGHTS; i++)
	{
		if(pointLights[i].intensity > 0)
		{
			diffuseSpecularComp += calcPointLight(pointLights[i], mvVertexPos, mvVertexNormal);
		}
	}
	
	for (int i = 0; i < MAX_POINT_LIGHTS; i++)
	{
		if(spotLights[i].pl.intensity > 0)
		{
			diffuseSpecularComp += calcSpotLight(spotLights[i], mvVertexPos, mvVertexNormal);
		}
	}
	
	fragColor = ambientC * vec4(ambientLight, 1) + diffuseSpecularComp;

	if(fog.activeFog == 1)
	{
		fragColor = calcFog(mvVertexPos, fragColor, fog, ambientLight, directionalLight);
	}
}