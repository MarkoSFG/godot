/**************************************************************************/
/*  animation_tree_editor_plugin.cpp                                      */
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

#include "animation_tree_editor_plugin.h"

#include "animation_blend_space_1d_editor.h"
#include "animation_blend_space_2d_editor.h"
#include "animation_blend_tree_editor_plugin.h"
#include "animation_state_machine_editor.h"
#include "core/config/project_settings.h"
#include "core/input/input.h"
#include "core/io/resource_loader.h"
#include "core/math/delaunay_2d.h"
#include "core/os/keyboard.h"
#include "editor/editor_string_names.h"
#include "editor/editor_command_palette.h"
#include "editor/editor_node.h"
#include "editor/editor_undo_redo_manager.h"
#include "editor/gui/editor_bottom_panel.h"
#include "editor/gui/editor_file_dialog.h"
#include "editor/themes/editor_scale.h"
#include "editor/editor_settings.h"
#include "scene/animation/animation_blend_tree.h"
#include "scene/animation/animation_player.h"
#include "scene/gui/button.h"
#include "scene/gui/margin_container.h"
#include "scene/gui/menu_button.h"
#include "scene/gui/panel.h"
#include "scene/gui/scroll_container.h"
#include "scene/gui/separator.h"
#include "scene/main/window.h"
#include "scene/scene_string_names.h"

void AnimationTreeNodeEditorPlugin::_add_standard_context_menu_items(PopupMenu *p_menu, bool p_can_copy, bool p_can_paste) {
	if (p_can_copy || p_can_paste) {
		p_menu->add_separator();
	}
	if (p_can_copy) {
		p_menu->add_item(TTR("Cut"), MENU_CUT);
		p_menu->add_item(TTR("Copy"), MENU_COPY);
	}
	if (p_can_paste) {
		p_menu->add_item(TTR("Paste"), MENU_PASTE);
	}
	if (p_can_copy) {
		p_menu->add_separator();
		p_menu->add_item(TTR("Delete"), MENU_DELETE);
		p_menu->add_separator();
		p_menu->add_item(TTR("Duplicate"), MENU_DUPLICATE);
	}
}

AnimationTreeNodeEditorPlugin::AnimationTreeNodeEditorPlugin() {
	set_process_shortcut_input(true);
	set_shortcut_context(this);

	set_focus_mode(FOCUS_ALL);
}

void AnimationTreeEditor::edit(AnimationTree *p_tree) {
	if (p_tree && !p_tree->is_connected("animation_list_changed", callable_mp(this, &AnimationTreeEditor::_animation_list_changed))) {
		p_tree->connect("animation_list_changed", callable_mp(this, &AnimationTreeEditor::_animation_list_changed), CONNECT_DEFERRED);
	}

	if (tree == p_tree) {
		return;
	}

	if (tree && tree->is_connected("animation_list_changed", callable_mp(this, &AnimationTreeEditor::_animation_list_changed))) {
		tree->disconnect("animation_list_changed", callable_mp(this, &AnimationTreeEditor::_animation_list_changed));
	}

	tree = p_tree;

	Vector<String> path;
	if (tree) {
		edit_path(path);
	}
}

void AnimationTreeEditor::_node_removed(Node *p_node) {
	if (p_node == tree) {
		tree = nullptr;
		_clear_editors();
	}
}

void AnimationTreeEditor::_path_button_pressed(int p_path) {
	edited_path.clear();
	for (int i = 0; i <= p_path; i++) {
		edited_path.push_back(button_path[i]);
	}
}

void AnimationTreeEditor::_animation_list_changed() {
	AnimationNodeBlendTreeEditor *bte = AnimationNodeBlendTreeEditor::get_singleton();
	if (bte) {
		bte->update_graph();
	}
}

void AnimationTreeEditor::_update_path() {
	while (path_hb->get_child_count() > 1) {
		memdelete(path_hb->get_child(1));
	}

	Ref<ButtonGroup> group;
	group.instantiate();

	Button *b = memnew(Button);
	b->set_text(TTR("Root"));
	b->set_toggle_mode(true);
	b->set_button_group(group);
	b->set_pressed(true);
	b->set_focus_mode(FOCUS_NONE);
	b->connect("pressed", callable_mp(this, &AnimationTreeEditor::_path_button_pressed).bind(-1));
	path_hb->add_child(b);
	for (int i = 0; i < button_path.size(); i++) {
		b = memnew(Button);
		b->set_text(button_path[i]);
		b->set_toggle_mode(true);
		b->set_button_group(group);
		path_hb->add_child(b);
		b->set_pressed(true);
		b->set_focus_mode(FOCUS_NONE);
		b->connect("pressed", callable_mp(this, &AnimationTreeEditor::_path_button_pressed).bind(i));
	}
}

void AnimationTreeEditor::_open_shared_parameters() {
	if (!tree) {
		return;
	}

	parameters_tree->clear();
	TreeItem *root = parameters_tree->create_item();
	Dictionary shared_params = tree->get_shared_parameters();

	List<Variant> keys;
	shared_params.get_key_list(&keys);
	for (const Variant &K : keys) {
		TreeItem *item = parameters_tree->create_item(root);
		item->set_text(0, K);
		item->set_metadata(0, item->get_text(0));
		item->set_editable(0, true);

		item->set_text(1, shared_params[K]);
		item->set_editable(1, true);

		item->add_button(1, get_editor_theme_icon("Remove"), PARAM_BUTTON_DELETE, false, TTR("Remove parameter."));
	}

	if (!parameters_window->is_visible()) {
		parameters_window->popup_centered(Vector2(500, 400));
	}
}

void AnimationTreeEditor::_add_shared_parameter() {
	if (!tree) {
		return;
	}

	String text = add_parameter_name->get_text();

	if (!tree->has_shared_parameter(text)) {
		tree->set_shared_parameter(text, Variant(0.0f));

		_open_shared_parameters();
	}
}

void AnimationTreeEditor::_parameter_item_edited() {
	switch (parameters_tree->get_edited_column()) {
		case 0:
			_parameter_renamed();
			break;

		case 1:
			_parameter_value_modified();
			break;
	}
}

void AnimationTreeEditor::_parameter_button_pressed(TreeItem *p_item, int p_column, int p_id, MouseButton p_button) {
	if (tree) {
		String param_name = p_item->get_text(0);
		switch (p_id) {
			case PARAM_BUTTON_DELETE: {
				tree->remove_shared_parameter(param_name);
				_open_shared_parameters();
			} break;
		}
	}
}

void AnimationTreeEditor::_parameter_renamed() {
	TreeItem *ti = parameters_tree->get_edited();
	String text = ti->get_text(0);
	String old_text = ti->get_metadata(0);

	if (!tree || String(text).contains("/") || String(text).contains(":") || String(text).contains(",") || String(text).contains("[") || text == old_text) {
		ti->set_text(0, old_text);
	} else {
		EditorUndoRedoManager *undo_redo = EditorUndoRedoManager::get_singleton();

		while (tree->has_shared_parameter(text)) {
			text = text + " 1";
		}

		ti->set_text(0, text);

		undo_redo->create_action(vformat(TTR("Rename Shared Parameter: %s"), text));
		undo_redo->add_do_method(tree, "rename_shared_parameter", old_text, text);
		undo_redo->add_undo_method(tree, "rename_shared_parameter", text, old_text);
		undo_redo->commit_action();
		ti->set_metadata(0, text);
	}
}

void AnimationTreeEditor::_parameter_value_modified() {
	TreeItem *ti = parameters_tree->get_edited();
	String text = ti->get_text(1);

	if (tree) {
		Variant value = Variant(text.to_float());
		tree->set_shared_parameter(ti->get_text(0), value);
		ti->set_text(1, value);
	}
}

void AnimationTreeEditor::_update_editor(AnimationTree *p_tree) {
	emit_signal("update_editor", p_tree);
}

void AnimationTreeEditor::edit_path(const Vector<String> &p_path) {
	button_path.clear();

	Ref<AnimationNode> node = tree->get_root_animation_node();

	if (node.is_valid()) {
		current_root = node->get_instance_id();

		for (int i = 0; i < p_path.size(); i++) {
			Ref<AnimationNode> child = node->get_child_by_name(p_path[i]);
			ERR_BREAK(child.is_null());
			node = child;
			button_path.push_back(p_path[i]);
		}

		edited_path = button_path;

		for (int i = 0; i < editors.size(); i++) {
			if (editors[i]->can_edit(node)) {
				editors[i]->edit(node);
				editors[i]->show();
			} else {
				editors[i]->edit(Ref<AnimationNode>());
				editors[i]->hide();
			}
		}
	} else {
		current_root = ObjectID();
		edited_path = button_path;
		for (int i = 0; i < editors.size(); i++) {
			editors[i]->edit(Ref<AnimationNode>());
			editors[i]->hide();
		}
	}

	_update_path();
}

void AnimationTreeEditor::_clear_editors() {
	button_path.clear();
	current_root = ObjectID();
	edited_path = button_path;
	for (int i = 0; i < editors.size(); i++) {
		editors[i]->edit(Ref<AnimationNode>());
		editors[i]->hide();
	}
	_update_path();
}

Vector<String> AnimationTreeEditor::get_edited_path() const {
	return button_path;
}

void AnimationTreeEditor::enter_editor(const String &p_path) {
	Vector<String> path = edited_path;
	path.push_back(p_path);
	edit_path(path);
}

void AnimationTreeEditor::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_ENTER_TREE: {
			get_tree()->connect("node_removed", callable_mp(this, &AnimationTreeEditor::_node_removed));
		} break;
		case NOTIFICATION_PROCESS: {
			ObjectID root;
			if (tree && tree->get_root_animation_node().is_valid()) {
				root = tree->get_root_animation_node()->get_instance_id();
			}

			if (root != current_root) {
				edit_path(Vector<String>());
			}

			if (button_path.size() != edited_path.size()) {
				edit_path(edited_path);
			}
		} break;
		case NOTIFICATION_EXIT_TREE: {
			get_tree()->disconnect("node_removed", callable_mp(this, &AnimationTreeEditor::_node_removed));
		} break;
	}
}

void AnimationTreeEditor::_bind_methods() {
}

AnimationTreeEditor *AnimationTreeEditor::singleton = nullptr;

void AnimationTreeEditor::add_plugin(AnimationTreeNodeEditorPlugin *p_editor) {
	ERR_FAIL_COND(p_editor->get_parent());
	editor_base->add_child(p_editor);
	editors.push_back(p_editor);
	p_editor->set_h_size_flags(SIZE_EXPAND_FILL);
	p_editor->set_v_size_flags(SIZE_EXPAND_FILL);
	p_editor->hide();
}

void AnimationTreeEditor::remove_plugin(AnimationTreeNodeEditorPlugin *p_editor) {
	ERR_FAIL_COND(p_editor->get_parent() != editor_base);
	editor_base->remove_child(p_editor);
	editors.erase(p_editor);
}

String AnimationTreeEditor::get_base_path() {
	String path = SceneStringNames::get_singleton()->parameters_base_path;
	for (int i = 0; i < edited_path.size(); i++) {
		path += edited_path[i] + "/";
	}
	return path;
}

bool AnimationTreeEditor::can_edit(const Ref<AnimationNode> &p_node) const {
	for (int i = 0; i < editors.size(); i++) {
		if (editors[i]->can_edit(p_node)) {
			return true;
		}
	}
	return false;
}

Vector<String> AnimationTreeEditor::get_animation_list() {
	if (!singleton->tree || !singleton->is_visible_unsafe()) {
		// When tree is empty, singleton not in the main thread.
		return Vector<String>();
	}

	AnimationTree *tree = singleton->tree;
	if (!tree) {
		return Vector<String>();
	}

	List<StringName> anims;
	tree->get_animation_list(&anims);
	Vector<String> ret;
	for (const StringName &E : anims) {
		ret.push_back(E);
	}

	return ret;
}

AnimationTreeEditor::AnimationTreeEditor() {
	AnimationNodeAnimation::get_editable_animation_list = get_animation_list;

	top_hb = memnew(HBoxContainer);
	add_child(top_hb);

	path_edit = memnew(ScrollContainer);
	top_hb->add_child(path_edit);
	path_edit->set_vertical_scroll_mode(ScrollContainer::SCROLL_MODE_DISABLED);
	path_edit->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	path_hb = memnew(HBoxContainer);
	path_edit->add_child(path_hb);
	path_hb->add_child(memnew(Label(TTR("Path:"))));

	btn_parameters = memnew(Button);
	btn_parameters->set_theme_type_variation("FlatButton");
	btn_parameters->set_text(TTR("Parameters"));
	top_hb->add_child(btn_parameters);
	btn_parameters->set_tooltip_text(TTR("Edit shared parameters."));
	btn_parameters->connect("pressed", callable_mp(this, &AnimationTreeEditor::_open_shared_parameters), CONNECT_DEFERRED);

	add_child(memnew(HSeparator));

	singleton = this;
	editor_base = memnew(MarginContainer);
	editor_base->set_v_size_flags(SIZE_EXPAND_FILL);
	add_child(editor_base);

	add_plugin(memnew(AnimationNodeBlendTreeEditor));
	add_plugin(memnew(AnimationNodeBlendSpace1DEditor));
	add_plugin(memnew(AnimationNodeBlendSpace2DEditor));
	add_plugin(memnew(AnimationNodeStateMachineEditor));

	ED_SHORTCUT("blend_tree_editor/cut", TTR("Cut"), KeyModifierMask::CMD_OR_CTRL | Key::X);
	ED_SHORTCUT("blend_tree_editor/copy", TTR("Copy"), KeyModifierMask::CMD_OR_CTRL | Key::C);
	ED_SHORTCUT("blend_tree_editor/paste", TTR("Paste"), KeyModifierMask::CMD_OR_CTRL | Key::V);
	ED_SHORTCUT("blend_tree_editor/duplicate", TTR("Duplicate"), KeyModifierMask::CMD_OR_CTRL | Key::D);

	parameters_window = memnew(AcceptDialog);
	parameters_window->set_title(TTR("Parameters"));
	add_child(parameters_window);

	VBoxContainer *vb = memnew(VBoxContainer);
	HBoxContainer *hb = memnew(HBoxContainer);

	parameters_tree = memnew(Tree);
	parameters_tree->set_column_expand(0, true);
	parameters_tree->set_columns(2);
	parameters_tree->set_column_custom_minimum_width(0, EDSCALE * 250);
	parameters_tree->set_hide_root(true);
	parameters_tree->set_hide_folding(true);
	parameters_tree->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	parameters_tree->connect("item_edited", callable_mp(this, &AnimationTreeEditor::_parameter_item_edited), CONNECT_DEFERRED);
	parameters_tree->connect("button_clicked", callable_mp(this, &AnimationTreeEditor::_parameter_button_pressed));

	add_parameter_name = memnew(LineEdit);
	add_parameter_name->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	hb->add_child(add_parameter_name);

	Button *add_btn = memnew(Button);
	add_btn->set_text("+");
	add_btn->connect("pressed", callable_mp(this, &AnimationTreeEditor::_add_shared_parameter), CONNECT_DEFERRED);
	hb->add_child(add_btn);

	vb->add_child(hb);
	vb->add_child(parameters_tree);

	parameters_window->add_child(vb);
}

void AnimationTreeEditorPlugin::edit(Object *p_object) {
	anim_tree_editor->edit(Object::cast_to<AnimationTree>(p_object));
}

bool AnimationTreeEditorPlugin::handles(Object *p_object) const {
	return p_object->is_class("AnimationTree");
}

void AnimationTreeEditorPlugin::make_visible(bool p_visible) {
	if (p_visible) {
		//editor->hide_animation_player_editors();
		//editor->animation_panel_make_visible(true);
		button->show();
		EditorNode::get_bottom_panel()->make_item_visible(anim_tree_editor);
		anim_tree_editor->set_process(true);
	} else {
		if (anim_tree_editor->is_visible_in_tree()) {
			EditorNode::get_bottom_panel()->hide_bottom_panel();
		}
		button->hide();
		anim_tree_editor->set_process(false);
	}
}

AnimationTreeEditorPlugin::AnimationTreeEditorPlugin() {
	anim_tree_editor = memnew(AnimationTreeEditor);
	anim_tree_editor->set_custom_minimum_size(Size2(0, 300) * EDSCALE);

	button = EditorNode::get_bottom_panel()->add_item(TTR("AnimationTree"), anim_tree_editor, ED_SHORTCUT_AND_COMMAND("bottom_panels/toggle_animation_tree_bottom_panel", TTR("Toggle AnimationTree Bottom Panel")));
	button->hide();
}

AnimationTreeEditorPlugin::~AnimationTreeEditorPlugin() {
}
