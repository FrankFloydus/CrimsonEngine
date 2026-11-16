/**************************************************************************/
/*  test_packed_scene.cpp                                                 */
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

TEST_FORCE_LINK(test_packed_scene)

#include "core/object/callable_mp.h"
#include "core/object/undo_redo.h"
#include "core/variant/typed_array.h"
#include "scene/resources/packed_scene.h"

namespace TestPackedScene {

TEST_CASE("[PackedScene] Pack Scene and Retrieve State") {
	// Create a scene to pack.
	Node *scene = memnew(Node);
	scene->set_name("TestScene");

	// Pack the scene.
	PackedScene packed_scene;
	const Error err = packed_scene.pack(scene);
	CHECK(err == OK);

	// Retrieve the packed state.
	Ref<SceneState> state = packed_scene.get_state();
	CHECK(state.is_valid());
	CHECK(state->get_node_count() == 1);
	CHECK(state->get_node_name(0) == "TestScene");

	memdelete(scene);
}

TEST_CASE("[PackedScene] Signals Preserved when Packing Scene") {
	// Create main scene
	// root
	// `- sub_node (local)
	// `- sub_scene (instance of another scene)
	//    `- sub_scene_node (owned by sub_scene)
	Node *main_scene_root = memnew(Node);
	Node *sub_node = memnew(Node);
	Node *sub_scene_root = memnew(Node);
	Node *sub_scene_node = memnew(Node);

	main_scene_root->add_child(sub_node);
	sub_node->set_owner(main_scene_root);

	sub_scene_root->add_child(sub_scene_node);
	sub_scene_node->set_owner(sub_scene_root);

	main_scene_root->add_child(sub_scene_root);
	sub_scene_root->set_owner(main_scene_root);

	SUBCASE("Signals that should be saved") {
		int main_flags = Object::CONNECT_PERSIST;
		// sub node to a node in main scene
		sub_node->connect("ready", callable_mp(main_scene_root, &Node::is_ready), main_flags);
		// subscene root to a node in main scene
		sub_scene_root->connect("ready", callable_mp(main_scene_root, &Node::is_ready), main_flags);
		//subscene root to subscene root (connected within main scene)
		sub_scene_root->connect("ready", callable_mp(sub_scene_root, &Node::is_ready), main_flags);

		// Pack the scene.
		Ref<PackedScene> packed_scene;
		packed_scene.instantiate();
		const Error err = packed_scene->pack(main_scene_root);
		CHECK(err == OK);

		// Make sure the right connections are in packed scene.
		Ref<SceneState> state = packed_scene->get_state();
		CHECK_EQ(state->get_connection_count(), 3);
	}

	/*
	// FIXME: This subcase requires GH-48064 to be fixed.
	SUBCASE("Signals that should not be saved") {
		int subscene_flags = Object::CONNECT_PERSIST | Object::CONNECT_INHERITED;
		// subscene node to itself
		sub_scene_node->connect("ready", callable_mp(sub_scene_node, &Node::is_ready), subscene_flags);
		// subscene node to subscene root
		sub_scene_node->connect("ready", callable_mp(sub_scene_root, &Node::is_ready), subscene_flags);
		//subscene root to subscene root (connected within sub scene)
		sub_scene_root->connect("ready", callable_mp(sub_scene_root, &Node::is_ready), subscene_flags);

		// Pack the scene.
		Ref<PackedScene> packed_scene;
		packed_scene.instantiate();
		const Error err = packed_scene->pack(main_scene_root);
		CHECK(err == OK);

		// Make sure the right connections are in packed scene.
		Ref<SceneState> state = packed_scene->get_state();
		CHECK_EQ(state->get_connection_count(), 0);
	}
	*/

	memdelete(main_scene_root);
}

TEST_CASE("[PackedScene] Clear Packed Scene") {
	// Create a scene to pack.
	Node *scene = memnew(Node);
	scene->set_name("TestScene");

	// Pack the scene.
	PackedScene packed_scene;
	packed_scene.pack(scene);

	// Clear the packed scene.
	packed_scene.clear();

	// Check if it has been cleared.
	Ref<SceneState> state = packed_scene.get_state();
	CHECK_FALSE(state->get_node_count() == 1);

	memdelete(scene);
}

TEST_CASE("[PackedScene] Can Instantiate Packed Scene") {
	// Create a scene to pack.
	Node *scene = memnew(Node);
	scene->set_name("TestScene");

	// Pack the scene.
	PackedScene packed_scene;
	packed_scene.pack(scene);

	// Check if the packed scene can be instantiated.
	const bool can_instantiate = packed_scene.can_instantiate();
	CHECK(can_instantiate == true);

	memdelete(scene);
}

TEST_CASE("[PackedScene] Instantiate Packed Scene") {
	// Create a scene to pack.
	Node *scene = memnew(Node);
	scene->set_name("TestScene");

	// Pack the scene.
	PackedScene packed_scene;
	packed_scene.pack(scene);

	// Instantiate the packed scene.
	Node *instance = packed_scene.instantiate();
	CHECK(instance != nullptr);
	CHECK(instance->get_name() == "TestScene");

	memdelete(scene);
	memdelete(instance);
}

TEST_CASE("[PackedScene] Instantiate Packed Scene With Children") {
	// Create a scene to pack.
	Node *scene = memnew(Node);
	scene->set_name("TestScene");

	// Add persisting child nodes to the scene.
	Node *child1 = memnew(Node);
	child1->set_name("Child1");
	scene->add_child(child1);
	child1->set_owner(scene);

	Node *child2 = memnew(Node);
	child2->set_name("Child2");
	scene->add_child(child2);
	child2->set_owner(scene);

	// Add non persisting child node to the scene.
	Node *child3 = memnew(Node);
	child3->set_name("Child3");
	scene->add_child(child3);

	// Pack the scene.
	PackedScene packed_scene;
	packed_scene.pack(scene);

	// Instantiate the packed scene.
	Node *instance = packed_scene.instantiate();
	CHECK(instance != nullptr);
	CHECK(instance->get_name() == "TestScene");

	// Validate the child nodes of the instantiated scene.
	CHECK(instance->get_child_count() == 2);
	CHECK(instance->get_child(0)->get_name() == "Child1");
	CHECK(instance->get_child(1)->get_name() == "Child2");
	CHECK(instance->get_child(0)->get_owner() == instance);
	CHECK(instance->get_child(1)->get_owner() == instance);

	memdelete(scene);
	memdelete(instance);
}

TEST_CASE("[PackedScene] Set Path") {
	// Create a scene to pack.
	Node *scene = memnew(Node);
	scene->set_name("TestScene");

	// Pack the scene.
	PackedScene packed_scene;
	packed_scene.pack(scene);

	// Set a new path for the packed scene.
	const String new_path = "NewTestPath";
	packed_scene.set_path(new_path);

	// Check if the path has been set correctly.
	Ref<SceneState> state = packed_scene.get_state();
	CHECK(state.is_valid());
	CHECK(state->get_path() == new_path);

	memdelete(scene);
}

TEST_CASE("[PackedScene] Replace State") {
	// Create a scene to pack.
	Node *scene = memnew(Node);
	scene->set_name("TestScene");

	// Pack the scene.
	PackedScene packed_scene;
	packed_scene.pack(scene);

	// Create another scene state to replace with.
	Ref<SceneState> new_state = memnew(SceneState);
	new_state->set_path("NewPath");

	// Replace the state.
	packed_scene.replace_state(new_state);

	// Check if the state has been replaced.
	Ref<SceneState> state = packed_scene.get_state();
	CHECK(state.is_valid());
	CHECK(state == new_state);

	memdelete(scene);
}

TEST_CASE("[PackedScene] Recreate State") {
	// Create a scene to pack.
	Node *scene = memnew(Node);
	scene->set_name("TestScene");

	// Pack the scene.
	Ref<PackedScene> packed_scene;
	packed_scene.instantiate();
	packed_scene->pack(scene);

	// Recreate the state.
	packed_scene->recreate_state();

	// Check if the state has been recreated.
	Ref<SceneState> state = packed_scene->get_state();
	CHECK(state.is_valid());
	CHECK(state->get_node_count() == 0); // Since the state was recreated, it should be empty.

	memdelete(scene);
}

#ifdef TOOLS_ENABLED
TEST_CASE("[PackedScene] Inspector exposed descendants preserve instance overrides") {
	Node *source = memnew(Node);
	source->set_name("Reusable");
	Node *container = memnew(Node);
	container->set_name("Container");
	source->add_child(container);
	container->set_owner(source);
	container->set_process_priority(20);
	Node *exposed = memnew(Node);
	exposed->set_name("Exposed");
	container->add_child(exposed);
	exposed->set_owner(source);
	exposed->set_process_priority(15);
	Node *hidden = memnew(Node);
	hidden->set_name("Hidden");
	container->add_child(hidden);
	hidden->set_owner(source);
	hidden->set_process_priority(8);
	TypedArray<NodePath> paths;
	paths.push_back(NodePath("Container/Exposed"));
	source->set_inspector_exposed_nodes(paths);

	Ref<PackedScene> reusable = memnew(PackedScene);
	REQUIRE(reusable->pack(source) == OK);
	// A cached path lets packing resolve this instance without filesystem dependencies.
	reusable->set_path("res://test_inspector_exposed_nodes.tscn");
	Node *level = memnew(Node);
	level->set_name("Level");
	Node *instance = reusable->instantiate(PackedScene::GEN_EDIT_STATE_INSTANCE);
	REQUIRE(instance != nullptr);
	level->add_child(instance);
	instance->set_owner(level);
	CHECK_FALSE(level->is_editable_instance(instance));
	CHECK(level->is_node_exposed_in_inspector(instance->get_node(NodePath("Container/Exposed"))));
	CHECK_FALSE(level->is_node_exposed_in_inspector(instance->get_node(NodePath("Container"))));
	CHECK_FALSE(level->is_node_exposed_in_inspector(instance->get_node(NodePath("Container/Hidden"))));

	Ref<PackedScene> packed_level = memnew(PackedScene);
	REQUIRE(packed_level->pack(level) == OK);
	CHECK(packed_level->get_state()->get_node_count() == 2); // No unchanged child overrides.
	instance->get_node(NodePath("Container/Exposed"))->set_process_priority(37);
	instance->get_node(NodePath("Container"))->set_process_priority(999); // Traversal must not save this.
	instance->get_node(NodePath("Container/Hidden"))->set_process_priority(999);
	REQUIRE(packed_level->pack(level) == OK);
	CHECK(packed_level->get_state()->get_node_count() == 3);
	Node *loaded = packed_level->instantiate(PackedScene::GEN_EDIT_STATE_INSTANCE);
	REQUIRE(loaded != nullptr);
	CHECK(loaded->get_node(NodePath("Reusable/Container/Exposed"))->get_process_priority() == 37);
	CHECK(loaded->get_node(NodePath("Reusable/Container"))->get_process_priority() == 20);
	CHECK(loaded->get_node(NodePath("Reusable/Container/Hidden"))->get_process_priority() == 8);

	// Hiding the Inspector controls must keep the already-authored property override.
	instance->set_inspector_exposed_nodes(TypedArray<NodePath>());
	CHECK_FALSE(level->is_node_exposed_in_inspector(instance->get_node(NodePath("Container/Exposed"))));
	REQUIRE(packed_level->pack(level) == OK);
	Node *unexposed = packed_level->instantiate(PackedScene::GEN_EDIT_STATE_INSTANCE);
	REQUIRE(unexposed != nullptr);
	CHECK(unexposed->get_node(NodePath("Reusable"))->get_inspector_exposed_nodes().is_empty());
	CHECK(unexposed->get_node(NodePath("Reusable/Container/Exposed"))->get_process_priority() == 37);
	Ref<PackedScene> repacked = memnew(PackedScene);
	REQUIRE(repacked->pack(unexposed) == OK);
	Node *reloaded = repacked->instantiate(PackedScene::GEN_EDIT_STATE_INSTANCE);
	REQUIRE(reloaded != nullptr);
	CHECK(reloaded->get_node(NodePath("Reusable/Container/Exposed"))->get_process_priority() == 37);
	Node *duplicate = reloaded->get_node(NodePath("Reusable"))->duplicate();
	REQUIRE(duplicate != nullptr);
	CHECK(duplicate->get_node(NodePath("Container/Exposed"))->get_process_priority() == 37);
	CHECK(duplicate->get_inspector_exposed_nodes().is_empty());

	memdelete(duplicate);
	memdelete(reloaded);
	memdelete(unexposed);
	memdelete(loaded);
	memdelete(level);
	memdelete(source);
}

TEST_CASE("[PackedScene] Inspector exposure validates paths without aliases") {
	Node *source = memnew(Node);
	Node *child = memnew(Node);
	child->set_name("Child");
	source->add_child(child);
	child->set_owner(source);
	TypedArray<NodePath> paths;
	paths.push_back(NodePath("Child"));
	paths.push_back(NodePath("Missing"));
	paths.push_back(NodePath("Missing"));
	paths.push_back(NodePath("."));
	paths.push_back(NodePath(".."));
	paths.push_back(NodePath("Child:process_priority"));
	source->set_inspector_exposed_nodes(paths);
	CHECK(source->get_configuration_warnings().size() == 4);
	CHECK(source->is_node_exposed_in_inspector(child));
	paths.clear();
	CHECK(source->get_inspector_exposed_nodes().size() == 6); // The setter owns its array.
	TypedArray<NodePath> copy = source->get_inspector_exposed_nodes();
	copy.clear();
	CHECK(source->is_node_exposed_in_inspector(child)); // The getter also returns a copy.
	memdelete(source);
}

TEST_CASE("[PackedScene] Inspector exposure child checkbox uses its owning scene") {
	Node *level = memnew(Node);
	level->set_name("Level");
	Node *reusable = memnew(Node);
	reusable->set_name("Reusable");
	level->add_child(reusable);
	reusable->set_owner(level);
	Node *container = memnew(Node);
	container->set_name("Container");
	reusable->add_child(container);
	container->set_owner(reusable);
	Node *child = memnew(Node);
	child->set_name("Child");
	container->add_child(child);
	child->set_owner(reusable);

	CHECK_FALSE(child->is_inspector_exposed_in_scene());
	UndoRedo *undo_redo = memnew(UndoRedo);
	undo_redo->create_action("Expose child");
	undo_redo->add_do_property(child, "inspector_exposed_in_scene", true);
	undo_redo->add_undo_property(child, "inspector_exposed_in_scene", false);
	undo_redo->commit_action();
	CHECK(child->is_inspector_exposed_in_scene());
	CHECK(reusable->get_inspector_exposed_nodes().size() == 1);
	CHECK(reusable->get_inspector_exposed_nodes()[0] == NodePath("Container/Child"));
	CHECK(level->get_inspector_exposed_nodes().is_empty()); // Nearest scene only.
	undo_redo->undo();
	CHECK_FALSE(child->is_inspector_exposed_in_scene());
	CHECK(reusable->get_inspector_exposed_nodes().is_empty());
	undo_redo->redo();
	CHECK(child->is_inspector_exposed_in_scene());
	HashSet<StringName> storable;
	child->get_storable_properties(storable);
	CHECK_FALSE(storable.has("inspector_exposed_in_scene")); // No duplicate serialized flag.

	Ref<PackedScene> packed = memnew(PackedScene);
	REQUIRE(packed->pack(reusable) == OK);
	Node *loaded = packed->instantiate(PackedScene::GEN_EDIT_STATE_INSTANCE);
	REQUIRE(loaded != nullptr);
	CHECK(loaded->get_node(NodePath("Container/Child"))->is_inspector_exposed_in_scene());
	loaded->get_node(NodePath("Container/Child"))->set_inspector_exposed_in_scene(false);
	CHECK(loaded->get_inspector_exposed_nodes().is_empty());
	CHECK_FALSE(level->is_inspector_exposed_in_scene());

	memdelete(loaded);
	memdelete(undo_redo);
	memdelete(level);
}
#endif // TOOLS_ENABLED

} // namespace TestPackedScene
