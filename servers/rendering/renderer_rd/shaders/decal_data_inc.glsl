#define DECAL_ORM_CHANNEL_AO (1u << 0)
#define DECAL_ORM_CHANNEL_ROUGHNESS (1u << 1)
#define DECAL_ORM_CHANNEL_METALLIC (1u << 2)

struct DecalData {
	mat4 xform; //to decal transform
	uint orm_channels;
	uint roughness_affects_ssr;
	uint pad;
	float albedo_mix;
	vec4 albedo_rect;
	vec4 normal_rect;
	vec4 orm_rect;
	vec4 emission_rect;
	vec4 modulate;
	float emission_energy;
	uint mask;
	float upper_fade;
	float lower_fade;
	mat3x4 normal_xform;
	vec3 normal;
	float normal_fade;
};
