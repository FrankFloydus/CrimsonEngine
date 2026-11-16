/**************************************************************************/
/*  test_decal.cpp                                                        */
/**************************************************************************/
/*                         This file is part of:                          */
/*                             GODOT ENGINE                               */
/*                        https://godotengine.org                         */
/**************************************************************************/
/* Copyright (c) 2014-present Godot Engine contributors (see AUTHORS.md). */
/* Copyright (c) 2007-2014 Juan Linietsky, Ariel Manzur.                  */
/*                                                                        */
/* Permission is hereby granted, free of charge, to any person obtaining  */
/* a copy of this software and associated documentation files (the        */
/* "Software"), to deal in the Software without restriction, including    */
/* without limitation the rights to use, copy, modify, merge, publish,    */
/* distribute, sublicense, and/or sell copies of the Software, and to     */
/* permit persons to whom the Software is furnished to do so, subject to  */
/* the following conditions:                                              */
/*                                                                        */
/* The above copyright notice and this permission notice shall be         */
/* included in all copies or substantial portions of the Software.        */
/*                                                                        */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,        */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF     */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. */
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY   */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,   */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE      */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                 */
/**************************************************************************/

#include "tests/test_macros.h"

TEST_FORCE_LINK(test_decal)

#ifndef _3D_DISABLED

#include "scene/3d/decal.h"
#include "scene/resources/packed_scene.h"

namespace TestDecal {

TEST_CASE("[SceneTree][Decal] Getters/Setters") {
	Decal *decal = memnew(Decal);
	AABB default_aabb{
		Vector3(-1.0f, -1.0f, -1.0f),
		Vector3(2.0f, 2.0f, 2.0f)
	};
	SUBCASE("Default values") {
		CHECK_MESSAGE(decal->get_aabb() == default_aabb, "get_aabb() returns the expected values for the default.");
	}

	SUBCASE("Set and Get Size") {
		Vector3 size = Vector3(2.0f, 3.0f, 4.0f);

		decal->set_size(size);
		AABB expected_aabb{
			Vector3(-1.0f, -1.5f, -2.0f),
			Vector3(2.0f, 3.0f, 4.0f)
		};
		CHECK_MESSAGE(decal->get_aabb() == expected_aabb, "get_aabb() returns the expected values after setting.");
		//default back the size in order to use the default_aabb for future assertions
		decal->set_size(Vector3(2.0f, 2.0f, 2.0f));
	}

	SUBCASE("Set and Get Texture") {
		Ref<Texture2D> albedo_texture;
		albedo_texture.instantiate();
		decal->set_texture(Decal::TEXTURE_ALBEDO, albedo_texture);
		CHECK_MESSAGE(decal->get_texture(Decal::TEXTURE_ALBEDO) == albedo_texture, "get_texture() returns the expected texture after setting.");
		CHECK_MESSAGE(default_aabb == decal->get_aabb(), "get_aabb() remains unchanged after setting texture.");
	}

	SUBCASE("Set and Get Emission Energy") {
		decal->set_emission_energy(2.0f);
		CHECK_MESSAGE(decal->get_emission_energy() == 2.0, "get_emission_energy() returns the expected value after setting.");
		CHECK_MESSAGE(default_aabb == decal->get_aabb(), "get_aabb() remains unchanged after setting emission energy.");
	}

	SUBCASE("Set and Get Albedo Mix") {
		decal->set_albedo_mix(0.5);
		CHECK_MESSAGE(decal->get_albedo_mix() == 0.5f, "get_albedo_mix() returns the expected value after setting.");
		CHECK_MESSAGE(default_aabb == decal->get_aabb(), "get_aabb() remains unchanged after setting albedo mix.");
	}

	SUBCASE("Set and Get Modulate") {
		Color new_color(0.5f, 0.5f, 0.5f, 1.0f);
		decal->set_modulate(new_color);
		CHECK_MESSAGE(decal->get_modulate() == new_color, "get_modulate() returns the expected value after setting.");
		CHECK_MESSAGE(default_aabb == decal->get_aabb(), "get_aabb() remains unchanged after setting modulate.");
	}

	SUBCASE("Set and Get Upper Fade") {
		decal->set_upper_fade(0.4f);
		CHECK_MESSAGE(decal->get_upper_fade() == 0.4f, "get_upper_fade() returns the expected value after setting.");
		CHECK_MESSAGE(default_aabb == decal->get_aabb(), "get_aabb() remains unchanged after setting upper fade.");
	}

	SUBCASE("Set and Get Lower Fade") {
		decal->set_lower_fade(0.2f);
		CHECK_MESSAGE(decal->get_lower_fade() == 0.2f, "get_lower_fade() returns the expected value after setting.");
		CHECK_MESSAGE(default_aabb == decal->get_aabb(), "get_aabb() remains unchanged after setting lower fade.");
	}

	SUBCASE("Set and Get Normal Fade") {
		decal->set_normal_fade(0.1f);
		CHECK_MESSAGE(decal->get_normal_fade() == 0.1f, "get_normal_fade() returns the expected value after setting.");
		CHECK_MESSAGE(default_aabb == decal->get_aabb(), "get_aabb() remains unchanged after setting normal fade.");
	}

	SUBCASE("Enable and Check Distance Fade") {
		decal->set_enable_distance_fade(true);
		CHECK_MESSAGE(decal->is_distance_fade_enabled() == true, "is_distance_fade_enabled() returns the expected value after setting.");
		CHECK_MESSAGE(default_aabb == decal->get_aabb(), "get_aabb() remains unchanged after enabling distance fade.");
	}

	SUBCASE("Set and Get Distance Fade Begin") {
		decal->set_distance_fade_begin(50.0f);
		CHECK_MESSAGE(decal->get_distance_fade_begin() == 50.0f, "get_distance_fade_begin() returns the expected value after setting.");
		CHECK_MESSAGE(default_aabb == decal->get_aabb(), "get_aabb() remains unchanged after setting distance fade begin.");
	}

	SUBCASE("Set and Get Distance Fade Length") {
		decal->set_distance_fade_length(15.0f);
		CHECK_MESSAGE(decal->get_distance_fade_length() == 15.0f, "get_distance_fade_length() returns the expected value after setting.");
		CHECK_MESSAGE(default_aabb == decal->get_aabb(), "get_aabb() remains unchanged after setting distance fade length.");
	}

	SUBCASE("Set and Get Cull Mask") {
		decal->set_cull_mask(0xFF);
		CHECK_MESSAGE(decal->get_cull_mask() == 0xFF, "get_cull_mask() returns the expected value after setting.");
		CHECK_MESSAGE(default_aabb == decal->get_aabb(), "get_aabb() remains unchanged after setting cull mask.");
	}

	memdelete(decal);
}

TEST_CASE("[SceneTree][Decal] ORM channels persist in a packed scene") {
	Decal *decal = memnew(Decal);
	decal->set_name("ORMDecal");
	CHECK(uint32_t(decal->get_orm_channels()) == Decal::ORM_CHANNEL_ALL);

	decal->set_orm_channels(0);
	CHECK(decal->get_orm_channels().is_empty());

	// Exercise the bound property used by the Inspector and scene serialization.
	bool property_valid = false;
	decal->set("orm_channels", Decal::ORM_CHANNEL_ROUGHNESS, &property_valid);
	CHECK(property_valid);
	CHECK(uint32_t(decal->get_orm_channels()) == Decal::ORM_CHANNEL_ROUGHNESS);

	PackedScene scene;
	const Error error = scene.pack(decal);
	memdelete(decal);
	REQUIRE(error == OK);

	Node *instance = scene.instantiate();
	Decal *copy = Object::cast_to<Decal>(instance);
	CHECK(copy != nullptr);
	if (copy) {
		CHECK(uint32_t(copy->get_orm_channels()) == Decal::ORM_CHANNEL_ROUGHNESS);
		copy->set_orm_channels(Decal::ORM_CHANNEL_ROUGHNESS | (1 << 10));
		CHECK(uint32_t(copy->get_orm_channels()) == Decal::ORM_CHANNEL_ROUGHNESS);
	}
	if (instance) {
		memdelete(instance);
	}
}

TEST_CASE("[SceneTree][Decal] Screen-space reflection roughness is opt-in") {
	Decal *decal = memnew(Decal);
	decal->set_name("SSRDecal");
	CHECK_FALSE(decal->is_roughness_affecting_ssr());

	bool property_valid = false;
	decal->set("roughness_affects_ssr", true, &property_valid);
	CHECK(property_valid);
	CHECK(decal->is_roughness_affecting_ssr());

	PackedScene scene;
	const Error error = scene.pack(decal);
	memdelete(decal);
	REQUIRE(error == OK);

	Node *instance = scene.instantiate();
	Decal *copy = Object::cast_to<Decal>(instance);
	CHECK(copy != nullptr);
	if (copy) {
		CHECK(copy->is_roughness_affecting_ssr());
		copy->set_roughness_affects_ssr(false);
		CHECK_FALSE(copy->is_roughness_affecting_ssr());
	}
	if (instance) {
		memdelete(instance);
	}
}

} // namespace TestDecal

#endif // _3D_DISABLED
