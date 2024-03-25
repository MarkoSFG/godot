/**************************************************************************/
/*  animation_tree_editor_plugin.h                                        */
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

#ifndef ANIMATION_TREE_EDITOR_PLUGIN_H
#define ANIMATION_TREE_EDITOR_PLUGIN_H

#include "editor/editor_plugin.h"
#include "scene/animation/animation_tree.h"
#include "scene/gui/graph_edit.h"
#include "scene/gui/tree.h"
#include "scene/gui/dialogs.h"

class Button;
class EditorFileDialog;
class ScrollContainer;

class AnimationTreeNodeEditorPlugin : public VBoxContainer {
	GDCLASS(AnimationTreeNodeEditorPlugin, VBoxContainer);

protected:
	enum {
		MENU_LOAD_FILE = 1000,
		MENU_PASTE = 1001,
		MENU_LOAD_FILE_CONFIRM = 1002,
		MENU_DUPLICATE = 1003,
		MENU_CUT = 1004,
		MENU_COPY = 1005,
		MENU_DELETE = 1006,
		MENU_USER_ID
	};

	void _add_standard_context_menu_items(PopupMenu *p_menu, bool p_can_copy, bool p_can_paste);

public:
	virtual bool can_edit(const Ref<AnimationNode> &p_node) = 0;
	virtual void edit(const Ref<AnimationNode> &p_node) = 0;

	AnimationTreeNodeEditorPlugin();
};

class AnimationTreeEditor : public VBoxContainer {
	GDCLASS(AnimationTreeEditor, VBoxContainer);

	enum {
		PARAM_BUTTON_DELETE,
	};

	HBoxContainer *top_hb = nullptr;
	ScrollContainer *path_edit = nullptr;
	HBoxContainer *path_hb = nullptr;

	AnimationTree *tree = nullptr;
	MarginContainer *editor_base = nullptr;

	Button *btn_parameters = nullptr;
	AcceptDialog *parameters_window = nullptr;
	Tree *parameters_tree = nullptr;
	LineEdit *add_parameter_name = nullptr;

	Vector<String> button_path;
	Vector<String> edited_path;
	Vector<AnimationTreeNodeEditorPlugin *> editors;

	void _update_path();
	void _clear_editors();
	ObjectID current_root;

	void _path_button_pressed(int p_path);
	void _animation_list_changed();

	static Vector<String> get_animation_list();

protected:
	void _notification(int p_what);
	void _node_removed(Node *p_node);
	static void _bind_methods();

	static AnimationTreeEditor *singleton;

public:
	AnimationTree *get_animation_tree() { return tree; }
	void add_plugin(AnimationTreeNodeEditorPlugin *p_editor);
	void remove_plugin(AnimationTreeNodeEditorPlugin *p_editor);

	String get_base_path();

	bool can_edit(const Ref<AnimationNode> &p_node) const;

	void _add_shared_parameter();
	void _open_shared_parameters();
	void _parameter_item_edited();
	void _parameter_button_pressed(TreeItem *p_item, int p_column, int p_id, MouseButton p_button);
	void _parameter_renamed();
	void _parameter_value_modified();

	void _update_editor(AnimationTree *p_tree);

	void edit_path(const Vector<String> &p_path);
	Vector<String> get_edited_path() const;

	void enter_editor(const String &p_path = "");
	static AnimationTreeEditor *get_singleton() { return singleton; }
	void edit(AnimationTree *p_tree);
	AnimationTreeEditor();
};

class AnimationTreeEditorPlugin : public EditorPlugin {
	GDCLASS(AnimationTreeEditorPlugin, EditorPlugin);

	AnimationTreeEditor *anim_tree_editor = nullptr;
	Button *button = nullptr;

public:
	virtual String get_name() const override { return "AnimationTree"; }
	bool has_main_screen() const override { return false; }
	virtual void edit(Object *p_object) override;
	virtual bool handles(Object *p_object) const override;
	virtual void make_visible(bool p_visible) override;

	AnimationTreeEditorPlugin();
	~AnimationTreeEditorPlugin();
};

#endif // ANIMATION_TREE_EDITOR_PLUGIN_H
