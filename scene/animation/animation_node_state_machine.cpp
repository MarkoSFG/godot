/**************************************************************************/
/*  animation_node_state_machine.cpp                                      */
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

#include "animation_node_state_machine.h"
#include "scene/main/window.h"

/////////////////////////////////////////////////

void AnimationNodeStateMachineTransition::set_switch_mode(SwitchMode p_mode) {
	switch_mode = p_mode;
}

AnimationNodeStateMachineTransition::SwitchMode AnimationNodeStateMachineTransition::get_switch_mode() const {
	return switch_mode;
}

void AnimationNodeStateMachineTransition::set_advance_mode(AdvanceMode p_mode) {
	advance_mode = p_mode;
}

AnimationNodeStateMachineTransition::AdvanceMode AnimationNodeStateMachineTransition::get_advance_mode() const {
	return advance_mode;
}

void AnimationNodeStateMachineTransition::set_advance_condition(const StringName &p_condition) {
	String cs = p_condition;
	ERR_FAIL_COND(cs.contains("/") || cs.contains(":"));
	advance_condition = p_condition;
	if (!cs.is_empty()) {
		advance_condition_name = "conditions/" + cs;
	} else {
		advance_condition_name = StringName();
	}
	emit_signal(SNAME("advance_condition_changed"));
}

StringName AnimationNodeStateMachineTransition::get_advance_condition() const {
	return advance_condition;
}

StringName AnimationNodeStateMachineTransition::get_advance_condition_name() const {
	return advance_condition_name;
}

void AnimationNodeStateMachineTransition::set_advance_expression(const String &p_expression) {
	advance_expression = p_expression;

	String advance_expression_stripped = advance_expression.strip_edges();
	if (advance_expression_stripped == String()) {
		expression.unref();
		return;
	}

	if (expression.is_null()) {
		expression.instantiate();
	}

	expression->parse(advance_expression_stripped);
}

String AnimationNodeStateMachineTransition::get_advance_expression() const {
	return advance_expression;
}

void AnimationNodeStateMachineTransition::set_xfade_time(float p_xfade) {
	ERR_FAIL_COND(p_xfade < 0);
	xfade_time = p_xfade;
	emit_changed();
}

float AnimationNodeStateMachineTransition::get_xfade_time() const {
	return xfade_time;
}

void AnimationNodeStateMachineTransition::set_xfade_curve(const Ref<Curve> &p_curve) {
	xfade_curve = p_curve;
}

Ref<Curve> AnimationNodeStateMachineTransition::get_xfade_curve() const {
	return xfade_curve;
}

void AnimationNodeStateMachineTransition::set_exit_time(float p_exit_time) {
	ERR_FAIL_COND(p_exit_time < 0.0 || p_exit_time > 1.0);
	exit_time = p_exit_time;
	emit_changed();
}

float AnimationNodeStateMachineTransition::get_exit_time() const {
	return exit_time;
}


void AnimationNodeStateMachineTransition::set_reset(bool p_reset) {
	reset = p_reset;
	emit_changed();
}

bool AnimationNodeStateMachineTransition::is_reset() const {
	return reset;
}

void AnimationNodeStateMachineTransition::set_priority(int p_priority) {
	priority = p_priority;
	emit_changed();
}

int AnimationNodeStateMachineTransition::get_priority() const {
	return priority;
}

void AnimationNodeStateMachineTransition::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_switch_mode", "mode"), &AnimationNodeStateMachineTransition::set_switch_mode);
	ClassDB::bind_method(D_METHOD("get_switch_mode"), &AnimationNodeStateMachineTransition::get_switch_mode);

	ClassDB::bind_method(D_METHOD("set_advance_mode", "mode"), &AnimationNodeStateMachineTransition::set_advance_mode);
	ClassDB::bind_method(D_METHOD("get_advance_mode"), &AnimationNodeStateMachineTransition::get_advance_mode);

	ClassDB::bind_method(D_METHOD("set_advance_condition", "name"), &AnimationNodeStateMachineTransition::set_advance_condition);
	ClassDB::bind_method(D_METHOD("get_advance_condition"), &AnimationNodeStateMachineTransition::get_advance_condition);

	ClassDB::bind_method(D_METHOD("set_xfade_time", "secs"), &AnimationNodeStateMachineTransition::set_xfade_time);
	ClassDB::bind_method(D_METHOD("get_xfade_time"), &AnimationNodeStateMachineTransition::get_xfade_time);

	ClassDB::bind_method(D_METHOD("set_xfade_curve", "curve"), &AnimationNodeStateMachineTransition::set_xfade_curve);
	ClassDB::bind_method(D_METHOD("get_xfade_curve"), &AnimationNodeStateMachineTransition::get_xfade_curve);

	ClassDB::bind_method(D_METHOD("set_exit_time", "percent"), &AnimationNodeStateMachineTransition::set_exit_time);
	ClassDB::bind_method(D_METHOD("get_exit_time"), &AnimationNodeStateMachineTransition::get_exit_time);

	ClassDB::bind_method(D_METHOD("set_reset", "reset"), &AnimationNodeStateMachineTransition::set_reset);
	ClassDB::bind_method(D_METHOD("is_reset"), &AnimationNodeStateMachineTransition::is_reset);

	ClassDB::bind_method(D_METHOD("set_priority", "priority"), &AnimationNodeStateMachineTransition::set_priority);
	ClassDB::bind_method(D_METHOD("get_priority"), &AnimationNodeStateMachineTransition::get_priority);

	ClassDB::bind_method(D_METHOD("set_advance_expression", "text"), &AnimationNodeStateMachineTransition::set_advance_expression);
	ClassDB::bind_method(D_METHOD("get_advance_expression"), &AnimationNodeStateMachineTransition::get_advance_expression);

	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "xfade_time", PROPERTY_HINT_RANGE, "0,240,0.01,suffix:s"), "set_xfade_time", "get_xfade_time");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "xfade_curve", PROPERTY_HINT_RESOURCE_TYPE, "Curve"), "set_xfade_curve", "get_xfade_curve");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "exit_time", PROPERTY_HINT_RANGE, "0,1,0.001"), "set_exit_time", "get_exit_time");

	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "reset"), "set_reset", "is_reset");

	ADD_PROPERTY(PropertyInfo(Variant::INT, "priority", PROPERTY_HINT_RANGE, "0,32,1"), "set_priority", "get_priority");
	ADD_GROUP("Switch", "");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "switch_mode", PROPERTY_HINT_ENUM, "Immediate,Sync,At End"), "set_switch_mode", "get_switch_mode");
	ADD_GROUP("Advance", "advance_");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "advance_mode", PROPERTY_HINT_ENUM, "Disabled,Enabled,Auto"), "set_advance_mode", "get_advance_mode");
	ADD_PROPERTY(PropertyInfo(Variant::STRING_NAME, "advance_condition"), "set_advance_condition", "get_advance_condition");
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "advance_expression", PROPERTY_HINT_EXPRESSION, ""), "set_advance_expression", "get_advance_expression");

	BIND_ENUM_CONSTANT(SWITCH_MODE_IMMEDIATE);
	BIND_ENUM_CONSTANT(SWITCH_MODE_SYNC);
	BIND_ENUM_CONSTANT(SWITCH_MODE_AT_END);

	BIND_ENUM_CONSTANT(ADVANCE_MODE_DISABLED);
	BIND_ENUM_CONSTANT(ADVANCE_MODE_ENABLED);
	BIND_ENUM_CONSTANT(ADVANCE_MODE_AUTO);

	ADD_SIGNAL(MethodInfo("advance_condition_changed"));
}

AnimationNodeStateMachineTransition::AnimationNodeStateMachineTransition() {
}

////////////////////////////////////////////////////////

void AnimationNodeStateMachinePlayback::_set_current(AnimationNodeStateMachine *p_state_machine, const StringName &p_state, const StringName &p_state_full_path, AnimationNodeStateMachine *p_sub_state) {
	current = p_state;
	current_full_path = p_state_full_path;
	current_sub_state = p_sub_state;
	if (current == StringName()) {
		group_start_transition = Ref<AnimationNodeStateMachineTransition>();
		group_end_transition = Ref<AnimationNodeStateMachineTransition>();
		return;
	}

	if (p_state_machine->transitions_by_node.has(current_full_path)) {
		current_transitions = p_state_machine->transitions_by_node[current_full_path];
	} else {
		current_transitions = no_transitions;
	}

	// don't need to check start / end transition count for SubState
	if (current_sub_state != nullptr) {
		return;
	}

	Ref<AnimationNodeStateMachine> anodesm = p_state_machine->find_node_by_path(current);
	if (!anodesm.is_valid()) {
		group_start_transition = Ref<AnimationNodeStateMachineTransition>();
		group_end_transition = Ref<AnimationNodeStateMachineTransition>();
		return;
	}

	Vector<int> indices = p_state_machine->find_transition_to(current);
	int group_start_size = indices.size();
	if (group_start_size) {
		group_start_transition = p_state_machine->get_transition(indices[0]);
	} else {
		group_start_transition = Ref<AnimationNodeStateMachineTransition>();
	}

	indices = p_state_machine->find_transition_from(current);
	int group_end_size = indices.size();
	if (group_end_size) {
		group_end_transition = p_state_machine->get_transition(indices[0]);
	} else {
		group_end_transition = Ref<AnimationNodeStateMachineTransition>();
	}

	// Validation.
	if (anodesm->get_state_machine_type() == AnimationNodeStateMachine::STATE_MACHINE_TYPE_GROUPED) {
		indices = anodesm->find_transition_from(anodesm->start_node);
		int anodesm_start_size = indices.size();
		indices = anodesm->find_transition_to(anodesm->end_node);
		int anodesm_end_size = indices.size();
		if (group_start_size > 1) {
			WARN_PRINT_ED("There are two or more transitions to the Grouped AnimationNodeStateMachine in AnimationNodeStateMachine: " + base_path + ", which may result in unintended transitions.");
		}
		if (group_end_size > 1) {
			WARN_PRINT_ED("There are two or more transitions from the Grouped AnimationNodeStateMachine in AnimationNodeStateMachine: " + base_path + ", which may result in unintended transitions.");
		}
		if (anodesm_start_size > 1) {
			WARN_PRINT_ED("There are two or more transitions from the Start of Grouped AnimationNodeStateMachine in AnimationNodeStateMachine: " + base_path + current + ", which may result in unintended transitions.");
		}
		if (anodesm_end_size > 1) {
			WARN_PRINT_ED("There are two or more transitions to the End of Grouped AnimationNodeStateMachine in AnimationNodeStateMachine: " + base_path + current + ", which may result in unintended transitions.");
		}
		if (anodesm_start_size != group_start_size) {
			ERR_PRINT_ED("There is a mismatch in the number of start transitions in and out of the Grouped AnimationNodeStateMachine on AnimationNodeStateMachine: " + base_path + current + ".");
		}
		if (anodesm_end_size != group_end_size) {
			ERR_PRINT_ED("There is a mismatch in the number of end transitions in and out of the Grouped AnimationNodeStateMachine on AnimationNodeStateMachine: " + base_path + current + ".");
		}
	}
}

bool AnimationNodeStateMachinePlayback::has_state(AnimationNodeStateMachine *p_state_machine, const StringName &p_node, AnimationNodeStateMachine* p_sub_state_machine) {
	return p_sub_state_machine != nullptr ? p_sub_state_machine->states.has(p_node) : p_state_machine->states.has(p_node);
}

AnimationNodeStateMachine::State AnimationNodeStateMachinePlayback::get_current_state(AnimationNodeStateMachine *p_state_machine) {
	return current_sub_state != nullptr ? current_sub_state->states[current] : p_state_machine->states[current];
}

AnimationNodeStateMachine::State AnimationNodeStateMachinePlayback::get_fading_state(AnimationNodeStateMachine *p_state_machine, const FadingBlend &p_blend) {
	return p_blend.sub_state_machine != nullptr ? p_blend.sub_state_machine->states[p_blend.fading_from] : p_state_machine->states[p_blend.fading_from];
}

void AnimationNodeStateMachinePlayback::_set_grouped(bool p_is_grouped) {
	is_grouped = p_is_grouped;
}

void AnimationNodeStateMachinePlayback::travel(const StringName &p_state, bool p_reset_on_teleport) {
	ERR_FAIL_COND_EDMSG(is_grouped, "Grouped AnimationNodeStateMachinePlayback must be handled by parent AnimationNodeStateMachinePlayback. You need to retrieve the parent Root/Nested AnimationNodeStateMachine.");
	ERR_FAIL_COND_EDMSG(String(p_state).contains("/Start") || String(p_state).contains("/End"), "Grouped AnimationNodeStateMachinePlayback doesn't allow to play Start/End directly. Instead, play the prev or next state of group in the parent AnimationNodeStateMachine.");
	_travel_main(p_state, p_reset_on_teleport);
}

void AnimationNodeStateMachinePlayback::start(const StringName &p_state, bool p_reset) {
	ERR_FAIL_COND_EDMSG(is_grouped, "Grouped AnimationNodeStateMachinePlayback must be handled by parent AnimationNodeStateMachinePlayback. You need to retrieve the parent Root/Nested AnimationNodeStateMachine.");
	ERR_FAIL_COND_EDMSG(String(p_state).contains("/Start") || String(p_state).contains("/End"), "Grouped AnimationNodeStateMachinePlayback doesn't allow to play Start/End directly. Instead, play the prev or next state of group in the parent AnimationNodeStateMachine.");
	_start_main(p_state, p_reset);
}

void AnimationNodeStateMachinePlayback::next() {
	ERR_FAIL_COND_EDMSG(is_grouped, "Grouped AnimationNodeStateMachinePlayback must be handled by parent AnimationNodeStateMachinePlayback. You need to retrieve the parent Root/Nested AnimationNodeStateMachine.");
	_next_main();
}

void AnimationNodeStateMachinePlayback::stop() {
	ERR_FAIL_COND_EDMSG(is_grouped, "Grouped AnimationNodeStateMachinePlayback must be handled by parent AnimationNodeStateMachinePlayback. You need to retrieve the parent Root/Nested AnimationNodeStateMachine.");
	_stop_main();
}

void AnimationNodeStateMachinePlayback::_travel_main(const StringName &p_state, bool p_reset_on_teleport) {
	travel_request = p_state;
	reset_request_on_teleport = p_reset_on_teleport;
	stop_request = false;
}

void AnimationNodeStateMachinePlayback::_start_main(const StringName &p_state, bool p_reset) {
	travel_request = StringName();
	path.clear();
	reset_request = p_reset;
	start_request = p_state;
	stop_request = false;
}

void AnimationNodeStateMachinePlayback::_next_main() {
	next_request = true;
}

void AnimationNodeStateMachinePlayback::_stop_main() {
	stop_request = true;
}

bool AnimationNodeStateMachinePlayback::is_playing() const {
	return playing;
}

bool AnimationNodeStateMachinePlayback::is_end() const {
	return current == "End" && fading_blends.size() == 0;
}

StringName AnimationNodeStateMachinePlayback::get_current_node() const {
	return current;
}

StringName AnimationNodeStateMachinePlayback::get_fading_from_node() const {
	return fading_blends.size() > 0 ? fading_blends.last().fading_from : StringName();
}

AnimationNodeStateMachine* AnimationNodeStateMachinePlayback::get_fading_from_sub_state() const {
	return fading_blends.size() > 0 ? fading_blends.last().sub_state_machine : nullptr;
}

Vector<StringName> AnimationNodeStateMachinePlayback::get_travel_path() const {
	return path;
}

TypedArray<StringName> AnimationNodeStateMachinePlayback::_get_travel_path() const {
	return Variant(get_travel_path()).operator Array();
}

float AnimationNodeStateMachinePlayback::get_current_play_pos() const {
	return pos_current;
}

float AnimationNodeStateMachinePlayback::get_current_length() const {
	return len_current;
}

float AnimationNodeStateMachinePlayback::get_fade_from_play_pos() const {
	return pos_fade_from;
}

float AnimationNodeStateMachinePlayback::get_fade_from_length() const {
	return len_fade_from;
}

float AnimationNodeStateMachinePlayback::get_fading_time() const {
	return fading_blends.size() > 0 ? fading_blends.last().fading_time : 0.0;
}

float AnimationNodeStateMachinePlayback::get_fading_pos() const {
	return fading_blends.size() > 0 ? fading_blends.last().fading_pos : 0.0;
}

void AnimationNodeStateMachinePlayback::_clear_path_children(AnimationTree *p_tree, AnimationNodeStateMachine *p_state_machine, bool p_test_only) {
	List<AnimationNode::ChildNode> child_nodes;
	p_state_machine->get_child_nodes(&child_nodes);
	for (int i = 0; i < child_nodes.size(); i++) {
		Ref<AnimationNodeStateMachine> anodesm = child_nodes[i].node;
		if (anodesm.is_valid() && anodesm->get_state_machine_type() == AnimationNodeStateMachine::STATE_MACHINE_TYPE_GROUPED) {
			Ref<AnimationNodeStateMachinePlayback> playback = p_tree->get(base_path + child_nodes[i].name + "/playback");
			ERR_FAIL_COND(!playback.is_valid());
			playback->_set_base_path(base_path + child_nodes[i].name + "/");
			if (p_test_only) {
				playback = playback->duplicate();
			}
			playback->path.clear();
			playback->_clear_path_children(p_tree, anodesm.ptr(), p_test_only);
			if (current != child_nodes[i].name) {
				playback->_start(anodesm.ptr()); // Can restart.
			}
		}
	}
}

void AnimationNodeStateMachinePlayback::_start_children(AnimationTree *p_tree, AnimationNodeStateMachine *p_state_machine, const String &p_path, bool p_test_only) {
	if (p_state_machine->get_state_machine_type() == AnimationNodeStateMachine::STATE_MACHINE_TYPE_GROUPED ||
		p_state_machine->get_state_machine_type() == AnimationNodeStateMachine::STATE_MACHINE_TYPE_SUBSTATE) {
		return; // This function must be fired only by the top state machine, do nothing in child state machine.
	}
	Vector<String> temp_path = p_path.split("/");
	if (temp_path.size() > 1) {
		for (int i = 1; i < temp_path.size(); i++) {
			String concatenated;
			for (int j = 0; j < i; j++) {
				concatenated += temp_path[j] + (j == i - 1 ? "" : "/");
			}
			Ref<AnimationNodeStateMachine> anodesm = p_state_machine->find_node_by_path(concatenated);
			if (anodesm.is_valid() && anodesm->get_state_machine_type() != AnimationNodeStateMachine::STATE_MACHINE_TYPE_GROUPED) {
				ERR_FAIL_MSG("Root/Nested AnimationNodeStateMachine can't have path from parent AnimationNodeStateMachine.");
			}
			Ref<AnimationNodeStateMachinePlayback> playback = p_tree->get(base_path + concatenated + "/playback");
			ERR_FAIL_COND(!playback.is_valid());
			playback->_set_base_path(base_path + concatenated + "/");
			if (p_test_only) {
				playback = playback->duplicate();
			}
			playback->_start_main(temp_path[i], i == temp_path.size() - 1 ? reset_request : false);
		}
		reset_request = false;
	}
}

bool AnimationNodeStateMachinePlayback::_travel_children(AnimationTree *p_tree, AnimationNodeStateMachine *p_state_machine, const String &p_path, bool p_is_allow_transition_to_self, bool p_is_parent_same_state, bool p_test_only) {
	if (p_state_machine->get_state_machine_type() == AnimationNodeStateMachine::STATE_MACHINE_TYPE_GROUPED ||
		p_state_machine->get_state_machine_type() == AnimationNodeStateMachine::STATE_MACHINE_TYPE_SUBSTATE) {
		return false; // This function must be fired only by the top state machine, do nothing in child state machine.
	}
	Vector<String> temp_path = p_path.split("/");
	Vector<ChildStateMachineInfo> children;

	bool found_route = true;
	bool is_parent_same_state = p_is_parent_same_state;
	if (temp_path.size() > 1) {
		for (int i = 1; i < temp_path.size(); i++) {
			String concatenated;
			for (int j = 0; j < i; j++) {
				concatenated += temp_path[j] + (j == i - 1 ? "" : "/");
			}

			Ref<AnimationNodeStateMachine> anodesm = p_state_machine->find_node_by_path(concatenated);
			if (anodesm.is_valid() && anodesm->get_state_machine_type() != AnimationNodeStateMachine::STATE_MACHINE_TYPE_GROUPED) {
				ERR_FAIL_V_MSG(false, "Root/Nested AnimationNodeStateMachine can't have path from parent AnimationNodeStateMachine.");
			}
			Ref<AnimationNodeStateMachinePlayback> playback = p_tree->get(base_path + concatenated + "/playback");
			ERR_FAIL_COND_V(!playback.is_valid(), false);
			playback->_set_base_path(base_path + concatenated + "/");
			if (p_test_only) {
				playback = playback->duplicate();
			}
			if (!playback->is_playing()) {
				playback->_start(anodesm.ptr());
			}
			ChildStateMachineInfo child_info;
			child_info.playback = playback;

			// Process for the case that parent state is changed.
			bool child_found_route = true;
			bool is_current_same_state = temp_path[i] == playback->get_current_node();
			if (!is_parent_same_state) {
				// Force travel to end current child state machine.
				String child_path = "/" + playback->get_current_node();
				while (true) {
					Ref<AnimationNodeStateMachine> child_anodesm = p_state_machine->find_node_by_path(concatenated + child_path);
					if (!child_anodesm.is_valid() || child_anodesm->get_state_machine_type() != AnimationNodeStateMachine::STATE_MACHINE_TYPE_GROUPED) {
						break;
					}
					Ref<AnimationNodeStateMachinePlayback> child_playback = p_tree->get(base_path + concatenated + child_path + "/playback");
					ERR_FAIL_COND_V(!child_playback.is_valid(), false);
					child_playback->_set_base_path(base_path + concatenated + "/");
					if (p_test_only) {
						child_playback = child_playback->duplicate();
					}
					child_playback->_travel_main("End");
					child_found_route &= child_playback->_travel(p_tree, child_anodesm.ptr(), false, p_test_only);
					child_path += "/" + child_playback->get_current_node();
				}
				// Force restart target state machine.
				playback->_start(anodesm.ptr());
			}
			is_parent_same_state = is_current_same_state;

			bool is_deepest_state = i == temp_path.size() - 1;
			child_info.is_reset = is_deepest_state ? reset_request_on_teleport : false;
			playback->_travel_main(temp_path[i], child_info.is_reset);
			if (playback->_make_travel_path(p_tree, anodesm.ptr(), is_deepest_state ? p_is_allow_transition_to_self : false, child_info.path, p_test_only)) {
				found_route &= child_found_route;
			} else {
				child_info.path.push_back(temp_path[i]);
				found_route = false;
			}
			children.push_back(child_info);
		}
		reset_request_on_teleport = false;
	}

	if (found_route) {
		for (int i = 0; i < children.size(); i++) {
			children.write[i].playback->clear_path();
			for (int j = 0; j < children[i].path.size(); j++) {
				children.write[i].playback->push_path(children[i].path[j]);
			}
		}
	} else {
		for (int i = 0; i < children.size(); i++) {
			children.write[i].playback->_travel_main(StringName(), children[i].is_reset); // Clear travel.
			if (children[i].path.size()) {
				children.write[i].playback->_start_main(children[i].path[children[i].path.size() - 1], children[i].is_reset);
			}
		}
	}
	return found_route;
}

void AnimationNodeStateMachinePlayback::_start(AnimationNodeStateMachine *p_state_machine) {
	playing = true;
	_set_current(p_state_machine, start_request != StringName() ? start_request : p_state_machine->start_node, start_request != StringName() ? start_request : p_state_machine->start_node, nullptr);
	teleport_request = true;
	stop_request = false;
	start_request = StringName();
}

bool AnimationNodeStateMachinePlayback::_travel(AnimationTree *p_tree, AnimationNodeStateMachine *p_state_machine, bool p_is_allow_transition_to_self, bool p_test_only) {
	return _make_travel_path(p_tree, p_state_machine, p_is_allow_transition_to_self, path, p_test_only);
}

String AnimationNodeStateMachinePlayback::_validate_path(AnimationNodeStateMachine *p_state_machine, const String &p_path) {
	if (p_state_machine->get_state_machine_type() == AnimationNodeStateMachine::STATE_MACHINE_TYPE_GROUPED ||
		p_state_machine->get_state_machine_type() == AnimationNodeStateMachine::STATE_MACHINE_TYPE_SUBSTATE) {
		return p_path; // Grouped state machine doesn't allow validat-able request.
	}
	String target = p_path;
	Ref<AnimationNodeStateMachine> anodesm = p_state_machine->find_node_by_path(target);
	while (anodesm.is_valid() && anodesm->get_state_machine_type() == AnimationNodeStateMachine::STATE_MACHINE_TYPE_GROUPED) {
		Vector<int> indices = anodesm->find_transition_from(anodesm->start_node);
		if (indices.size()) {
			target = target + "/" + anodesm->get_transition_to(indices[0]); // Find next state of Start.
		} else {
			break; // There is no transition in Start state of grouped state machine.
		}
		anodesm = p_state_machine->find_node_by_path(target);
	}
	return target;
}

bool AnimationNodeStateMachinePlayback::_make_travel_path(AnimationTree *p_tree, AnimationNodeStateMachine *p_state_machine, bool p_is_allow_transition_to_self, Vector<StringName> &r_path, bool p_test_only) {
	StringName travel = travel_request;
	travel_request = StringName();

	if (!playing) {
		_start(p_state_machine);
	}

	ERR_FAIL_COND_V(!p_state_machine->states.has(travel), false);
	ERR_FAIL_COND_V(!p_state_machine->states.has(current), false);

	if (current == travel) {
		return !p_is_allow_transition_to_self;
	}

	Vector<StringName> new_path;

	Vector2 current_pos = get_current_state(p_state_machine).position;
	Vector2 target_pos = p_state_machine->states[travel].position;

	bool found_route = false;
	HashMap<StringName, AStarCost> cost_map;

	List<int> open_list;

	// Build open list.
	for (int i = 0; i < p_state_machine->transitions.size(); i++) {
		if (p_state_machine->transitions[i].transition->get_advance_mode() == AnimationNodeStateMachineTransition::ADVANCE_MODE_DISABLED) {
			continue;
		}

		if (p_state_machine->transitions[i].from == current) {
			open_list.push_back(i);
			float cost = p_state_machine->states[p_state_machine->transitions[i].to].position.distance_to(current_pos);
			cost *= p_state_machine->transitions[i].transition->get_priority();
			AStarCost ap;
			ap.prev = current;
			ap.distance = cost;
			cost_map[p_state_machine->transitions[i].to] = ap;

			if (p_state_machine->transitions[i].to == travel) { // Prematurely found it! :D
				found_route = true;
				break;
			}
		}
	}

	// Begin astar.
	while (!found_route) {
		if (open_list.size() == 0) {
			break; // No path found.
		}

		// Find the last cost transition.
		List<int>::Element *least_cost_transition = nullptr;
		float least_cost = 1e20;

		for (List<int>::Element *E = open_list.front(); E; E = E->next()) {
			float cost = cost_map[p_state_machine->transitions[E->get()].to].distance;
			cost += p_state_machine->states[p_state_machine->transitions[E->get()].to].position.distance_to(target_pos);

			if (cost < least_cost) {
				least_cost_transition = E;
				least_cost = cost;
			}
		}

		StringName transition_prev = p_state_machine->transitions[least_cost_transition->get()].from;
		StringName transition = p_state_machine->transitions[least_cost_transition->get()].to;

		for (int i = 0; i < p_state_machine->transitions.size(); i++) {
			if (p_state_machine->transitions[i].transition->get_advance_mode() == AnimationNodeStateMachineTransition::ADVANCE_MODE_DISABLED) {
				continue;
			}

			if (p_state_machine->transitions[i].from != transition || p_state_machine->transitions[i].to == transition_prev) {
				continue; // Not interested on those.
			}

			float distance = p_state_machine->states[p_state_machine->transitions[i].from].position.distance_to(p_state_machine->states[p_state_machine->transitions[i].to].position);
			distance *= p_state_machine->transitions[i].transition->get_priority();
			distance += cost_map[p_state_machine->transitions[i].from].distance;

			if (cost_map.has(p_state_machine->transitions[i].to)) {
				// Oh this was visited already, can we win the cost?
				if (distance < cost_map[p_state_machine->transitions[i].to].distance) {
					cost_map[p_state_machine->transitions[i].to].distance = distance;
					cost_map[p_state_machine->transitions[i].to].prev = p_state_machine->transitions[i].from;
				}
			} else {
				// Add to open list.
				AStarCost ac;
				ac.prev = p_state_machine->transitions[i].from;
				ac.distance = distance;
				cost_map[p_state_machine->transitions[i].to] = ac;

				open_list.push_back(i);

				if (p_state_machine->transitions[i].to == travel) {
					found_route = true;
					break;
				}
			}
		}

		if (found_route) {
			break;
		}

		open_list.erase(least_cost_transition);
	}

	// Check child grouped state machine.
	if (found_route) {
		// Make path.
		StringName at = travel;
		while (at != current) {
			new_path.push_back(at);
			at = cost_map[at].prev;
		}
		new_path.reverse();

		// Check internal paths of child grouped state machine.
		// For example:
		// [current - End] - [Start - End] - [Start - End] - [Start - target]
		String current_path = current;
		int len = new_path.size() + 1;
		for (int i = 0; i < len; i++) {
			Ref<AnimationNodeStateMachine> anodesm = p_state_machine->find_node_by_path(current_path);
			if (anodesm.is_valid() && anodesm->get_state_machine_type() == AnimationNodeStateMachine::STATE_MACHINE_TYPE_GROUPED) {
				Ref<AnimationNodeStateMachinePlayback> playback = p_tree->get(base_path + current_path + "/playback");
				ERR_FAIL_COND_V(!playback.is_valid(), false);
				playback->_set_base_path(base_path + current_path + "/");
				if (p_test_only) {
					playback = playback->duplicate();
				}
				if (i > 0) {
					playback->_start(anodesm.ptr());
				}
				if (i >= new_path.size()) {
					break; // Tracing has been finished, needs to break.
				}
				playback->_travel_main("End");
				if (!playback->_travel(p_tree, anodesm.ptr(), false, p_test_only)) {
					found_route = false;
					break;
				}
			}
			if (i >= new_path.size()) {
				break; // Tracing has been finished, needs to break.
			}
			current_path = new_path[i];
		}
	}

	// Finally, rewrite path if route is found.
	if (found_route) {
		r_path = new_path;
		return true;
	} else {
		return false;
	}
}

double AnimationNodeStateMachinePlayback::process(const String &p_base_path, AnimationNodeStateMachine *p_state_machine, const AnimationMixer::PlaybackInfo p_playback_info, bool p_test_only) {
	double rem = _process(p_base_path, p_state_machine, p_playback_info, p_test_only);
	start_request = StringName();
	next_request = false;
	stop_request = false;
	reset_request_on_teleport = false;
	return rem;
}

double AnimationNodeStateMachinePlayback::_process(const String &p_base_path, AnimationNodeStateMachine *p_state_machine, const AnimationMixer::PlaybackInfo p_playback_info, bool p_test_only) {
	_set_base_path(p_base_path);

	AnimationTree *tree = p_state_machine->process_state->tree;

	double p_time = p_playback_info.time;
	bool p_seek = p_playback_info.seeked;
	bool p_is_external_seeking = p_playback_info.is_external_seeking;

	// Check seek to 0 (means reset) by parent AnimationNode.
	if (p_time == 0 && p_seek && !p_is_external_seeking) {
		if (p_state_machine->state_machine_type != AnimationNodeStateMachine::STATE_MACHINE_TYPE_NESTED || is_end() || !playing) {
			// Restart state machine.
			if (p_state_machine->get_state_machine_type() != AnimationNodeStateMachine::STATE_MACHINE_TYPE_GROUPED &&
				p_state_machine->get_state_machine_type() != AnimationNodeStateMachine::STATE_MACHINE_TYPE_SUBSTATE) {
				path.clear();
				_clear_path_children(tree, p_state_machine, p_test_only);
				_start(p_state_machine);
			}
			reset_request = true;
		} else {
			// Reset current state.
			reset_request = true;
			teleport_request = true;
		}
	}

	if (stop_request) {
		start_request = StringName();
		travel_request = StringName();
		path.clear();
		playing = false;
		return 0;
	}

	if (!playing && start_request != StringName() && travel_request != StringName()) {
		return 0;
	}

	// Process start/travel request.
	if (start_request != StringName() || travel_request != StringName()) {
		if (p_state_machine->get_state_machine_type() != AnimationNodeStateMachine::STATE_MACHINE_TYPE_GROUPED &&
			p_state_machine->get_state_machine_type() != AnimationNodeStateMachine::STATE_MACHINE_TYPE_SUBSTATE) {
			_clear_path_children(tree, p_state_machine, p_test_only);
		}

		if (start_request != StringName()) {
			path.clear();
			String start_target = _validate_path(p_state_machine, start_request);
			Vector<String> start_path = String(start_target).split("/");
			start_request = start_path[0];
			if (start_path.size()) {
				_start_children(tree, p_state_machine, start_target, p_test_only);
			}
			// Teleport to start.
			if (p_state_machine->states.has(start_request)) {
				_start(p_state_machine);
			} else {
				StringName node = start_request;
				ERR_FAIL_V_MSG(0, "No such node: '" + node + "'");
			}
		}

		if (travel_request != StringName()) {
			// Fix path.
			String travel_target = _validate_path(p_state_machine, travel_request);
			Vector<String> travel_path = travel_target.split("/");
			travel_request = travel_path[0];
			StringName temp_travel_request = travel_request; // For the case that can't travel.
			// Process children.
			Vector<StringName> new_path;
			bool can_travel = _make_travel_path(tree, p_state_machine, travel_path.size() <= 1 ? p_state_machine->is_allow_transition_to_self() : false, new_path, p_test_only);
			if (travel_path.size()) {
				if (can_travel) {
					can_travel = _travel_children(tree, p_state_machine, travel_target, p_state_machine->is_allow_transition_to_self(), travel_path[0] == current, p_test_only);
				} else {
					_start_children(tree, p_state_machine, travel_target, p_test_only);
				}
			}

			// Process to travel.
			if (can_travel) {
				path = new_path;
			} else {
				// Can't travel, then teleport.
				if (p_state_machine->states.has(temp_travel_request)) {
					path.clear();
					if (current != temp_travel_request || reset_request_on_teleport) {
						_set_current(p_state_machine, temp_travel_request, temp_travel_request, nullptr);
						reset_request = reset_request_on_teleport;
						teleport_request = true;
					}
				} else {
					ERR_FAIL_V_MSG(0, "No such node: '" + temp_travel_request + "'");
				}
			}
		}
	}

	AnimationMixer::PlaybackInfo pi = p_playback_info;

	if (teleport_request) {
		teleport_request = false;
		// Clear fading on teleport.
		for (int i = 0; i < fading_blends.size(); ++i) {
			get_fading_state(p_state_machine, fading_blends[i]).node->blend_end(0);
		}
		fading_blends.clear();
		// Init current length.
		pos_current = 0; // Overwritten suddenly in main process.

		pi.time = 0;
		pi.seeked = true;
		pi.is_external_seeking = false;
		pi.weight = 0;

		len_current = p_state_machine->blend_node(get_current_state(p_state_machine).node, current_full_path, pi, AnimationNode::FILTER_IGNORE, true, true);
		// Don't process first node if not necessary, insteads process next node.
		_transition_to_next_recursive(tree, p_state_machine, p_test_only);
	}

	// Check current node existence.
	if (!has_state(p_state_machine, current, current_sub_state)) {
		playing = false; // Current does not exist.
		_set_current(p_state_machine, StringName(), StringName(), nullptr);
		return 0;
	}

	// Special case for grouped state machine Start/End to make priority with parent blend (means don't treat Start and End states as RESET animations).
	bool is_start_of_group = false;
	bool is_end_of_group = false;
	if (!p_state_machine->are_ends_reset() || p_state_machine->get_state_machine_type() == AnimationNodeStateMachine::STATE_MACHINE_TYPE_GROUPED) {
		is_start_of_group = get_fading_from_node() == p_state_machine->start_node;
		is_end_of_group = current == p_state_machine->end_node;
	}

	// Calc blend amount by cross-fade.
	float fade_blend = 1.0;
	if (fading_blends.size() > 0) {
		if (!has_state(p_state_machine, get_fading_from_node(), get_fading_from_sub_state())) {
			for (int i = 0; i < fading_blends.size(); ++i) {
				get_fading_state(p_state_machine, fading_blends[i]).node->blend_end(0);
			}
			fading_blends.clear();
		} else {
			FadingBlend blend = fading_blends.last();
			if (!p_seek) {
				blend.fading_pos += p_time;
				fading_blends.set(fading_blends.size() - 1, blend);
			}
			fade_blend = MIN(1.0, blend.fading_pos / blend.fading_time);
		}
	}
	if (current_curve.is_valid()) {
		fade_blend = current_curve->sample(fade_blend);
	}
	fade_blend = Math::is_zero_approx(fade_blend) ? CMP_EPSILON : fade_blend;
	if (is_start_of_group) {
		fade_blend = 1.0;
	} else if (is_end_of_group) {
		fade_blend = 0.0;
	}

	// Main process / current state playback
	double rem = 0.0;
	pi = p_playback_info;
	pi.weight = fade_blend;
	if (reset_request) {
		reset_request = false;
		pi.time = 0;
		pi.seeked = true;
		len_current = p_state_machine->blend_node(get_current_state(p_state_machine).node, current_full_path, pi, AnimationNode::FILTER_IGNORE, true, p_test_only);
		rem = len_current;
	} else {
		rem = p_state_machine->blend_node(get_current_state(p_state_machine).node, current_full_path, pi, AnimationNode::FILTER_IGNORE, true, p_test_only); // Blend values must be more than CMP_EPSILON to process discrete keys in edge.
	}

	// any number of ongoing fades are processed here, we should have a list of fading_blends ((A->B)->C)->D
	if (fading_blends.size() > 0) {
		double fade_total = 1.0;
		FadingBlend blend;

		fading_blend_count.clear();
		fading_blend_count[current] = 0;

		// calculate weights for each blend
		for (int i = fading_blends.size() - 1; i >= 0; --i) {
			blend = fading_blends[i];

			if (i < fading_blends.size() - 1) {
				blend.fading_pos += p_time;

				fade_blend = MIN(1.0, blend.fading_pos / blend.fading_time);
				if (current_curve.is_valid()) {
					fade_blend = current_curve->sample(fade_blend);
				}

				fade_blend = fade_blend * fade_total;
			}
			
			fade_total = fade_total - fade_blend;
			blend.fade_total = fade_total;

			fading_blends.set(i, blend);
		}

		// process blends
		for (int i = fading_blends.size() - 1; i >= 0; --i) {
			if (i > 0) {
				blend = fading_blends[i - 1];

				fade_blend = MIN(1.0, blend.fading_pos / blend.fading_time);
				if (current_curve.is_valid()) {
					fade_blend = current_curve->sample(fade_blend);
				}
				fade_blend = fade_blend * fading_blends[i].fade_total;
			} else {
				fade_blend = fading_blends[i].fade_total;
			}

			blend = fading_blends[i];
			pi = p_playback_info;
			
			if (!fading_blend_count.has(blend.fading_from)) {
				fading_blend_count[blend.fading_from] = 0;
			} else {
				fading_blend_count[blend.fading_from]++;
			}
			pi.id = fading_blend_count[blend.fading_from]; // pass in id for which cur_time the AnimationNodeAnimation should use, the greater the index the older the playback

			pi.weight = fade_blend;
			if (_reset_request_for_fading_from) {
				_reset_request_for_fading_from = false;
				pi.time = 0;
				pi.seeked = true;
			}

			float fading_from_rem = p_state_machine->blend_node(get_fading_state(p_state_machine, blend).node, blend.fading_from_full_path, pi, AnimationNode::FILTER_IGNORE, true, p_test_only); // Blend values must be more than CMP_EPSILON to process discrete keys in edge.

			if (i == fading_blends.size() - 1) {
				if (fading_from_rem > len_fade_from) { /// Weird but ok.
					len_fade_from = fading_from_rem;
				}
				pos_fade_from = len_fade_from - fading_from_rem;
			}

			if (blend.fading_pos >= blend.fading_time) {
				// if this blend is done
				while (i >= 0) {
					get_fading_state(p_state_machine, fading_blends[i]).node->blend_end(fading_blend_count[fading_blends[i].fading_from]); // pass in id / index for which cur_time the AnimationNodeAnimation should remove
					fading_blends.remove_at(i); // Finish fading.
					--i;
				}
			}
		}
	}
	
	// Guess playback position.
	if (rem > len_current) { // Weird but ok.
		len_current = rem;
	}
	pos_current = len_current - rem;

	// Find next and see when to transition.
	_transition_to_next_recursive(tree, p_state_machine, p_test_only);

	// Predict remaining time.
	double remain = rem; // If we can't predict the end of state machine, the time remaining must be INFINITY.

	if (p_state_machine->get_state_machine_type() == AnimationNodeStateMachine::STATE_MACHINE_TYPE_NESTED) {
		// There is no next transition.
		if (!p_state_machine->has_transition_from(current)) {
			if (fading_blends.size() > 0) {
				remain = MAX(rem, fading_blends.last().fading_time - fading_blends.last().fading_pos);
			} else {
				remain = rem;
			}
			return remain;
		}
	}

	if (current == p_state_machine->end_node) {
		if (fading_blends.size() > 0) {
			remain = MAX(0, fading_blends.last().fading_time - fading_blends.last().fading_pos);
		} else {
			remain = 0;
		}
		return remain;
	}

	if (!is_end()) {
		return HUGE_LENGTH;
	}

	return remain;
}

bool AnimationNodeStateMachinePlayback::_transition_to_next_recursive(AnimationTree *p_tree, AnimationNodeStateMachine *p_state_machine, bool p_test_only) {
	_reset_request_for_fading_from = false;

	bool is_state_changed = false;

	AnimationMixer::PlaybackInfo pi;
	NextInfo next;
	Vector<StringName> transition_path;
	transition_path.push_back(current);
	while (true) {
		bool transition_to_same = false;
		next = _find_next(p_tree, p_state_machine);

		if (!_can_transition_to_next(p_tree, p_state_machine, next, p_test_only)) {
			break; // Finish transition.
		}

		if (transition_path.has(next.node)) {
			transition_to_same = true;

			// we only want to disallow transition from current state back to itself if the any state triggers are not active; as a state set to blend back to itself via any state is allowed
			if (!p_tree->any_triggers_active()) {
				WARN_PRINT_ONCE_ED("AnimationNodeStateMachinePlayback: " + base_path + "playback aborts the transition by detecting one or more looped transitions in the same frame to prevent to infinity loop. You may need to check the transition settings.");
				break; // Maybe infinity loop, do nothing more.
			}
		}

		transition_path.push_back(next.node);
		is_state_changed = true;
		bool found_target_state;
		FadingBlend blend;

		// Setting for fading.
		if (next.xfade) {
			// allow multiple playbacks of the same animation / node
			if (next.node == current && next.sub_state_machine == current_sub_state) {
				get_current_state(p_state_machine).node->blend_start();
			} else {
				for (int i = 0; i < fading_blends.size(); ++i) {
					if (fading_blends[i].fading_from == next.node) {
						get_fading_state(p_state_machine, fading_blends[i]).node->blend_start();
						break;
					}
				}
			}

			// Time to fade.
			found_target_state = true;

			// push new fade / blend state to fading_blends vector
			blend.fading_from = current;
			blend.fading_from_full_path = current_full_path;
			blend.fading_time = next.xfade;
			blend.fading_pos = 0;
			blend.sub_state_machine = current_sub_state;
			fading_blends.push_back(blend);
		} else {
			if (reset_request) {
				// There is no possibility of processing doubly. Now we can apply reset actually in here.
				pi.time = 0;
				pi.seeked = true;
				pi.is_external_seeking = false;
				pi.weight = 0;
				p_state_machine->blend_node(get_current_state(p_state_machine).node, current_full_path, pi, AnimationNode::FILTER_IGNORE, true, p_test_only);
			}

			// clear fading_blends
			for (int i = 0; i < fading_blends.size(); ++i) {
				get_fading_state(p_state_machine, fading_blends[i]).node->blend_end(0);
			}
			fading_blends.clear();

			found_target_state = transition_to_same;
		}

		// If it came from path, remove path.
		if (path.size()) {
			path.remove_at(0);
		}

		// Update current status.
		_set_current(p_state_machine, next.node, next.node_full_path, next.sub_state_machine);
		current_curve = next.curve;

		_reset_request_for_fading_from = reset_request && !transition_to_same; // To avoid processing doubly, it must be reset in the fading process within _process().
		reset_request = next.is_reset;

		pos_fade_from = pos_current;
		len_fade_from = len_current;

		if (next.switch_mode == AnimationNodeStateMachineTransition::SWITCH_MODE_SYNC) {
			pi.time = MIN(pos_current, len_current);
			pi.seeked = true;
			pi.is_external_seeking = false;
			pi.weight = 0;
			p_state_machine->blend_node(get_current_state(p_state_machine).node, current_full_path, pi, AnimationNode::FILTER_IGNORE, true, p_test_only);
		}

		// Just get length to find next recursive.
		double rem = 0.0;
		pi.time = 0;
		pi.is_external_seeking = false;
		pi.weight = 0;

		if (next.is_reset) {
			pi.seeked = true;
			len_current = p_state_machine->blend_node(get_current_state(p_state_machine).node, current_full_path, pi, AnimationNode::FILTER_IGNORE, true, true); // Just retrieve remain length, don't process.
			rem = len_current;
		} else {
			pi.seeked = false;
			rem = p_state_machine->blend_node(get_current_state(p_state_machine).node, current_full_path, pi, AnimationNode::FILTER_IGNORE, true, true); // Just retrieve remain length, don't process.
		}

		// Guess playback position.
		if (rem > len_current) { // Weird but ok.
			len_current = rem;
		}
		pos_current = len_current - rem;

		// Fading must be processed.
		if (found_target_state) {
			break;
		}
	}

	return is_state_changed;
}

bool AnimationNodeStateMachinePlayback::_can_transition_to_next(AnimationTree *p_tree, AnimationNodeStateMachine *p_state_machine, NextInfo p_next, bool p_test_only) {
	if (p_next.node == StringName()) {
		return false;
	}

	bool found_next = false;

	if (next_request) {
		// Process request only once.
		next_request = false;
		// Next request must be applied to only deepest state machine.
		Ref<AnimationNodeStateMachine> anodesm = p_state_machine->find_node_by_path(current);
		if (anodesm.is_valid() && anodesm->get_state_machine_type() == AnimationNodeStateMachine::STATE_MACHINE_TYPE_GROUPED) {
			Ref<AnimationNodeStateMachinePlayback> playback = p_tree->get(base_path + current + "/playback");
			ERR_FAIL_COND_V(!playback.is_valid(), false);
			playback->_set_base_path(base_path + current + "/");
			if (p_test_only) {
				playback = playback->duplicate();
			}
			playback->_next_main();
			// Then, fading should end.
			for (int i = 0; i < fading_blends.size(); ++i) {
				get_fading_state(p_state_machine, fading_blends[i]).node->blend_end(0);
			}
			fading_blends.clear();
			found_next = true;
		} else {
			return true;
		}
	}

	if (found_next) {
		return false;
	}

	if (current != p_state_machine->start_node && p_next.switch_mode == AnimationNodeStateMachineTransition::SWITCH_MODE_AT_END) {
		return (pos_current + p_next.xfade) / MAX(len_current, 0.001) >= p_next.exit_time;
		//return pos_current >= len_current - p_next.xfade;
	}
	return true;
}

Ref<AnimationNodeStateMachineTransition> AnimationNodeStateMachinePlayback::_check_group_transition(AnimationTree *p_tree, AnimationNodeStateMachine *p_state_machine, const AnimationNodeStateMachine::Transition &p_transition, Ref<AnimationNodeStateMachine> &r_state_machine, bool &r_bypass) const {
	Ref<AnimationNodeStateMachineTransition> temp_transition;
	Ref<AnimationNodeStateMachinePlayback> parent_playback;
	if (r_state_machine->get_state_machine_type() == AnimationNodeStateMachine::STATE_MACHINE_TYPE_GROUPED) {
		if (p_transition.from == "Start") {
			parent_playback = _get_parent_playback(p_tree);
			if (parent_playback.is_valid()) {
				r_bypass = true;
				temp_transition = parent_playback->_get_group_start_transition();
			}
		} else if (p_transition.to == "End") {
			parent_playback = _get_parent_playback(p_tree);
			if (parent_playback.is_valid()) {
				temp_transition = parent_playback->_get_group_end_transition();
			}
		}
		if (temp_transition.is_valid()) {
			r_state_machine = _get_parent_state_machine(p_tree);
			return temp_transition;
		}
	}
	return p_transition.transition;
}

AnimationNodeStateMachinePlayback::NextInfo AnimationNodeStateMachinePlayback::_find_next(AnimationTree *p_tree, AnimationNodeStateMachine *p_state_machine) const {
	NextInfo next;
	next.exit_time = 1.0;
	if (path.size()) {
		for (int i = 0; i < p_state_machine->transitions.size(); i++) {
			Ref<AnimationNodeStateMachine> anodesm = p_state_machine;
			bool bypass = false;
			Ref<AnimationNodeStateMachineTransition> ref_transition = _check_group_transition(p_tree, p_state_machine, p_state_machine->transitions[i], anodesm, bypass);
			if (ref_transition->get_advance_mode() == AnimationNodeStateMachineTransition::ADVANCE_MODE_DISABLED) {
				continue;
			}
			if (p_state_machine->transitions[i].from == current && p_state_machine->transitions[i].to == path[0]) {
				next.node = path[0];
				next.xfade = ref_transition->get_xfade_time();
				next.curve = ref_transition->get_xfade_curve();
				next.exit_time = ref_transition->get_exit_time();
				next.switch_mode = ref_transition->get_switch_mode();
				next.is_reset = ref_transition->is_reset();
			}
		}
	} else {
		int auto_advance_to = -1;
		float priority_best = 1e20;
		double exit_time_best = 2;
		for (int i = 0; i < current_transitions.size(); i++) {
			Ref<AnimationNodeStateMachine> anodesm = p_state_machine;
			bool bypass = false;
			Ref<AnimationNodeStateMachineTransition> ref_transition = _check_group_transition(p_tree, p_state_machine, current_transitions[i], anodesm, bypass);
			if (ref_transition->get_advance_mode() == AnimationNodeStateMachineTransition::ADVANCE_MODE_DISABLED) {
				continue;
			}
			if (_check_advance_condition(anodesm, ref_transition) || bypass) {
				if (ref_transition->get_priority() < priority_best ||
					(ref_transition->get_priority() == priority_best && ref_transition->get_exit_time() < exit_time_best)) {
					priority_best = ref_transition->get_priority();
					exit_time_best = ref_transition->get_exit_time();
					auto_advance_to = i;
				}
			}
		}

		StringName tr_to;
		StringName tr_to_sub_state;
		AnimationNodeStateMachine::Transition tr_transition;
		if (auto_advance_to != -1) {
			tr_to = current_transitions[auto_advance_to].to;
			tr_to_sub_state = current_transitions[auto_advance_to].to_sub_state;
			tr_transition = current_transitions[auto_advance_to];
		}

		if (p_tree->any_triggers_active()) {
			// check AnyState transitions (these always take priority over current_transitions)
			if (p_state_machine->transitions_by_node.has("Any State")) {
				Vector<AnimationNodeStateMachine::Transition> any_transitions = p_state_machine->transitions_by_node["Any State"];
				float any_priority_best = 1e20;

				for (int i = 0; i < any_transitions.size(); i++) {
					Ref<AnimationNodeStateMachine> anodesm = p_state_machine;
					bool bypass = false;
					Ref<AnimationNodeStateMachineTransition> ref_transition = _check_group_transition(p_tree, p_state_machine, any_transitions[i], anodesm, bypass);
					if (ref_transition->get_advance_mode() == AnimationNodeStateMachineTransition::ADVANCE_MODE_DISABLED) {
						continue;
					}
					if (_check_any_state_condition(anodesm, ref_transition) || bypass) {
						if (ref_transition->get_priority() < any_priority_best) {
							any_priority_best = ref_transition->get_priority();
							auto_advance_to = i;
						}
					}
				}

				if (auto_advance_to != -1) {
					tr_to = any_transitions[auto_advance_to].to;
					tr_to_sub_state = any_transitions[auto_advance_to].to_sub_state;
					tr_transition = any_transitions[auto_advance_to];
				}
			}
		}

		if (auto_advance_to != -1) {
			next.sub_state_machine = nullptr;

			if (p_state_machine->sub_states.has(tr_to_sub_state)) {
				AnimationNodeStateMachine *sub_state_machine = p_state_machine->sub_states[tr_to_sub_state];
				next.sub_state_machine = sub_state_machine;
				next.node_full_path = tr_transition.to;
				tr_to = tr_transition.to_sub_state_node;
			} else if (p_state_machine->sub_states.has(tr_to)) {
				AnimationNodeStateMachine *sub_state_machine = p_state_machine->sub_states[tr_to];

				if (sub_state_machine->get_state_machine_type() == AnimationNodeStateMachine::STATE_MACHINE_TYPE_SUBSTATE) {
					Vector<AnimationNodeStateMachine::Transition> start_transitions = p_state_machine->transitions_by_node[sub_state_machine->sub_state_start_node];
					priority_best = 1e20;
					int auto_advance_to_sub = -1;

					// select which state in the SubState we want to transition to (based on "SubState/Start -> ..." transitions) since "Start" and "End" in SubState is purely UI / editor
					// and the SubState parent is the state machine that actually plays its states
					for (int i = 0; i < start_transitions.size(); i++) {
						Ref<AnimationNodeStateMachine> anodesm = p_state_machine;
						bool bypass = false;
						Ref<AnimationNodeStateMachineTransition> ref_transition = _check_group_transition(p_tree, p_state_machine, start_transitions[i], anodesm, bypass);
						if (ref_transition->get_advance_mode() == AnimationNodeStateMachineTransition::ADVANCE_MODE_DISABLED) {
							continue;
						}
						if (_check_advance_condition(anodesm, ref_transition) || bypass) {
							if (ref_transition->get_priority() < priority_best) {
								priority_best = ref_transition->get_priority();
								auto_advance_to_sub = i;
								next.sub_state_machine = sub_state_machine;
								next.node_full_path = start_transitions[i].to;
								// set the to state without "SubState\" at the start
								tr_to = start_transitions[i].to_sub_state_node;
							}
						}
					}

					// not allowed, there must be a valid transition from "SubState\Start" node to something else, so just do the first one by default if possible
					if (auto_advance_to_sub == -1) {
						if (start_transitions.size() > 0) {
							ERR_PRINT(String(sub_state_machine->sub_state_start_node) + " must have at least one always valid transition / entry into the SubState");
							next.sub_state_machine = sub_state_machine;
							next.node_full_path = start_transitions[0].to;
							// set the to state without "SubState\" at the start
							tr_to = start_transitions[0].to_sub_state_node;
						} else {
							ERR_PRINT(String(sub_state_machine->sub_state_start_node) + " must have at least one transition / entry into the SubState");
						}
					}
				}
			}

			if (next.node_full_path == StringName()) {
				next.node_full_path = tr_to;
			}
			next.node = tr_to;
			Ref<AnimationNodeStateMachine> anodesm = p_state_machine;
			bool bypass = false;
			Ref<AnimationNodeStateMachineTransition> ref_transition = _check_group_transition(p_tree, p_state_machine, tr_transition, anodesm, bypass);
			next.xfade = ref_transition->get_xfade_time();
			next.curve = ref_transition->get_xfade_curve();
			next.exit_time = ref_transition->get_exit_time();
			next.switch_mode = ref_transition->get_switch_mode();
			next.is_reset = ref_transition->is_reset();
		}
	}

	return next;
}

bool AnimationNodeStateMachinePlayback::_check_advance_condition(const Ref<AnimationNodeStateMachine> state_machine, const Ref<AnimationNodeStateMachineTransition> transition) const {
	if (transition->get_advance_mode() != AnimationNodeStateMachineTransition::ADVANCE_MODE_AUTO) {
		return false;
	}

	StringName advance_condition_name = transition->get_advance_condition_name();

	if (advance_condition_name != StringName() && !bool(state_machine->get_parameter(advance_condition_name))) {
		return false;
	}

	if (transition->expression.is_valid()) {
		AnimationTree *tree_base = state_machine->get_animation_tree();
		ERR_FAIL_NULL_V(tree_base, false);

		NodePath advance_expression_base_node_path = tree_base->get_advance_expression_base_node();
		Node *expression_base = tree_base->get_node_or_null(advance_expression_base_node_path);

		if (expression_base) {
			Ref<Expression> exp = transition->expression;
			bool ret = exp->execute(Array(), expression_base, false, Engine::get_singleton()->is_editor_hint()); // Avoids allowing the user to crash the system with an expression by only allowing const calls.
			if (exp->has_execute_failed() || !ret) {
				return false;
			}
		} else {
			WARN_PRINT_ONCE("Animation transition has a valid expression, but no expression base node was set on its AnimationTree.");
		}
	}

	return true;
}

bool AnimationNodeStateMachinePlayback::_check_any_state_condition(const Ref<AnimationNodeStateMachine> state_machine, const Ref<AnimationNodeStateMachineTransition> transition) const {
	if (transition->get_advance_mode() != AnimationNodeStateMachineTransition::ADVANCE_MODE_AUTO) {
		return false;
	}

	StringName trigger = transition->get_advance_condition();
	AnimationTree *tree_base = state_machine->get_animation_tree();
	ERR_FAIL_NULL_V(tree_base, false);

	if (!tree_base->has_trigger(trigger)) {
		return false;
	}

	if (transition->expression.is_valid()) {
		NodePath advance_expression_base_node_path = tree_base->get_advance_expression_base_node();
		Node *expression_base = tree_base->get_node_or_null(advance_expression_base_node_path);

		if (expression_base) {
			Ref<Expression> exp = transition->expression;
			bool ret = exp->execute(Array(), expression_base, false, Engine::get_singleton()->is_editor_hint()); // Avoids allowing the user to crash the system with an expression by only allowing const calls.
			if (exp->has_execute_failed() || !ret) {
				return false;
			}
		} else {
			WARN_PRINT_ONCE("Animation transition has a valid expression, but no expression base node was set on its AnimationTree.");
		}
	}

	return true;
}

void AnimationNodeStateMachinePlayback::clear_path() {
	path.clear();
}

void AnimationNodeStateMachinePlayback::push_path(const StringName &p_state) {
	path.push_back(p_state);
}

void AnimationNodeStateMachinePlayback::_set_base_path(const String &p_base_path) {
	base_path = p_base_path;
}

Ref<AnimationNodeStateMachinePlayback> AnimationNodeStateMachinePlayback::_get_parent_playback(AnimationTree *p_tree) const {
	if (base_path.is_empty()) {
		return Ref<AnimationNodeStateMachinePlayback>();
	}
	Vector<String> split = base_path.split("/");
	ERR_FAIL_COND_V_MSG(split.size() < 2, Ref<AnimationNodeStateMachinePlayback>(), "Path is too short.");
	StringName self_path = split[split.size() - 2];
	split.remove_at(split.size() - 2);
	String playback_path = String("/").join(split) + "playback";
	Ref<AnimationNodeStateMachinePlayback> playback = p_tree->get(playback_path);
	if (!playback.is_valid()) {
		ERR_PRINT_ONCE("Can't get parent AnimationNodeStateMachinePlayback with path: " + playback_path + ". Maybe there is no Root/Nested AnimationNodeStateMachine in the parent of the Grouped AnimationNodeStateMachine.");
		return Ref<AnimationNodeStateMachinePlayback>();
	}
	if (playback->get_current_node() != self_path) {
		return Ref<AnimationNodeStateMachinePlayback>();
	}
	return playback;
}

Ref<AnimationNodeStateMachine> AnimationNodeStateMachinePlayback::_get_parent_state_machine(AnimationTree *p_tree) const {
	if (base_path.is_empty()) {
		return Ref<AnimationNodeStateMachine>();
	}
	Vector<String> split = base_path.split("/");
	ERR_FAIL_COND_V_MSG(split.size() < 3, Ref<AnimationNodeStateMachine>(), "Path is too short.");
	split = split.slice(1, split.size() - 2);
	Ref<AnimationNode> root = p_tree->get_root_animation_node();
	ERR_FAIL_COND_V_MSG(root.is_null(), Ref<AnimationNodeStateMachine>(), "There is no root AnimationNode in AnimationTree: " + String(p_tree->get_name()));
	String anodesm_path = String("/").join(split);
	Ref<AnimationNodeStateMachine> anodesm = !anodesm_path.size() ? root : root->find_node_by_path(anodesm_path);
	ERR_FAIL_COND_V_MSG(anodesm.is_null(), Ref<AnimationNodeStateMachine>(), "Can't get state machine with path: " + anodesm_path);
	return anodesm;
}

Ref<AnimationNodeStateMachineTransition> AnimationNodeStateMachinePlayback::_get_group_start_transition() const {
	ERR_FAIL_COND_V_MSG(group_start_transition.is_null(), Ref<AnimationNodeStateMachineTransition>(), "Group start transition is null.");
	return group_start_transition;
}

Ref<AnimationNodeStateMachineTransition> AnimationNodeStateMachinePlayback::_get_group_end_transition() const {
	ERR_FAIL_COND_V_MSG(group_end_transition.is_null(), Ref<AnimationNodeStateMachineTransition>(), "Group end transition is null.");
	return group_end_transition;
}

void AnimationNodeStateMachinePlayback::_bind_methods() {
	ClassDB::bind_method(D_METHOD("travel", "to_node", "reset_on_teleport"), &AnimationNodeStateMachinePlayback::travel, DEFVAL(true));
	ClassDB::bind_method(D_METHOD("start", "node", "reset"), &AnimationNodeStateMachinePlayback::start, DEFVAL(true));
	ClassDB::bind_method(D_METHOD("next"), &AnimationNodeStateMachinePlayback::next);
	ClassDB::bind_method(D_METHOD("stop"), &AnimationNodeStateMachinePlayback::stop);
	ClassDB::bind_method(D_METHOD("is_playing"), &AnimationNodeStateMachinePlayback::is_playing);
	ClassDB::bind_method(D_METHOD("get_current_node"), &AnimationNodeStateMachinePlayback::get_current_node);
	ClassDB::bind_method(D_METHOD("get_current_play_position"), &AnimationNodeStateMachinePlayback::get_current_play_pos);
	ClassDB::bind_method(D_METHOD("get_current_length"), &AnimationNodeStateMachinePlayback::get_current_length);
	ClassDB::bind_method(D_METHOD("get_fading_from_node"), &AnimationNodeStateMachinePlayback::get_fading_from_node);
	ClassDB::bind_method(D_METHOD("get_travel_path"), &AnimationNodeStateMachinePlayback::_get_travel_path);
}

AnimationNodeStateMachinePlayback::AnimationNodeStateMachinePlayback() {
	current_sub_state = nullptr;
	set_local_to_scene(true); // Only one per instantiated scene.
	no_transitions.clear();
	fading_blends.clear();
	fading_blend_count.clear();
	current_transitions = no_transitions;
	default_transition.instantiate();
	default_transition->set_xfade_time(0);
	default_transition->set_reset(true);
	default_transition->set_advance_mode(AnimationNodeStateMachineTransition::ADVANCE_MODE_AUTO);
	default_transition->set_switch_mode(AnimationNodeStateMachineTransition::SWITCH_MODE_IMMEDIATE);
}

///////////////////////////////////////////////////////

void AnimationNodeStateMachine::get_parameter_list(List<PropertyInfo> *r_list) const {
	r_list->push_back(PropertyInfo(Variant::OBJECT, playback, PROPERTY_HINT_RESOURCE_TYPE, "AnimationNodeStateMachinePlayback", PROPERTY_USAGE_EDITOR | PROPERTY_USAGE_ALWAYS_DUPLICATE)); // Don't store this object in .tres, it always needs to be made as unique object.
	List<StringName> advance_conditions;
	for (int i = 0; i < transitions.size(); i++) {
		StringName ac = transitions[i].transition->get_advance_condition_name();
		if (ac != StringName() && advance_conditions.find(ac) == nullptr && transitions[i].from != any_state_node) {
			advance_conditions.push_back(ac);
		}
	}

	advance_conditions.sort_custom<StringName::AlphCompare>();
	for (const StringName &E : advance_conditions) {
		r_list->push_back(PropertyInfo(Variant::BOOL, E));
	}
}

Variant AnimationNodeStateMachine::get_parameter_default_value(const StringName &p_parameter) const {
	if (p_parameter == playback) {
		Ref<AnimationNodeStateMachinePlayback> p;
		p.instantiate();
		return p;
	} else {
		return false; // Advance condition.
	}
}

bool AnimationNodeStateMachine::is_parameter_read_only(const StringName &p_parameter) const {
	if (p_parameter == playback) {
		return true;
	}
	return false;
}

void AnimationNodeStateMachine::add_node(const StringName &p_name, Ref<AnimationNode> p_node, const Vector2 &p_position) {
	ERR_FAIL_COND(states.has(p_name));
	ERR_FAIL_COND(p_node.is_null());
	ERR_FAIL_COND(String(p_name).contains("/"));

	State state_new;
	state_new.node = p_node;
	state_new.position = p_position;

	states[p_name] = state_new;

	if (p_node->is_state_machine()) { // seems like the most performant way to check this
		AnimationNodeStateMachine *state_machine = Object::cast_to<AnimationNodeStateMachine>(p_node.ptr());
		state_machine->parent = this;
		state_machine->node_name = p_name;
		state_machine->sub_state_start_node = state_machine->get_sub_state_parent_path() + state_machine->start_node;
	}

	emit_changed();
	emit_signal(SNAME("tree_changed"));

	p_node->connect("tree_changed", callable_mp(this, &AnimationNodeStateMachine::_tree_changed), CONNECT_REFERENCE_COUNTED);
	p_node->connect("animation_node_renamed", callable_mp(this, &AnimationNodeStateMachine::_animation_node_renamed), CONNECT_REFERENCE_COUNTED);
	p_node->connect("animation_node_removed", callable_mp(this, &AnimationNodeStateMachine::_animation_node_removed), CONNECT_REFERENCE_COUNTED);
}

void AnimationNodeStateMachine::replace_node(const StringName &p_name, Ref<AnimationNode> p_node) {
	ERR_FAIL_COND(states.has(p_name) == false);
	ERR_FAIL_COND(p_node.is_null());
	ERR_FAIL_COND(String(p_name).contains("/"));

	{
		Ref<AnimationNode> node = states[p_name].node;
		if (node.is_valid()) {
			node->disconnect("tree_changed", callable_mp(this, &AnimationNodeStateMachine::_tree_changed));
			node->disconnect("animation_node_renamed", callable_mp(this, &AnimationNodeStateMachine::_animation_node_renamed));
			node->disconnect("animation_node_removed", callable_mp(this, &AnimationNodeStateMachine::_animation_node_removed));
		}
	}

	states[p_name].node = p_node;

	emit_changed();
	emit_signal(SNAME("tree_changed"));

	p_node->connect("tree_changed", callable_mp(this, &AnimationNodeStateMachine::_tree_changed), CONNECT_REFERENCE_COUNTED);
	p_node->connect("animation_node_renamed", callable_mp(this, &AnimationNodeStateMachine::_animation_node_renamed), CONNECT_REFERENCE_COUNTED);
	p_node->connect("animation_node_removed", callable_mp(this, &AnimationNodeStateMachine::_animation_node_removed), CONNECT_REFERENCE_COUNTED);
}

void AnimationNodeStateMachine::set_state_machine_type(StateMachineType p_state_machine_type) {
	state_machine_type = p_state_machine_type;
	emit_changed();
	emit_signal(SNAME("tree_changed"));
	notify_property_list_changed();
}

AnimationNodeStateMachine::StateMachineType AnimationNodeStateMachine::get_state_machine_type() const {
	return state_machine_type;
}

void AnimationNodeStateMachine::set_allow_transition_to_self(bool p_enable) {
	allow_transition_to_self = p_enable;
}

bool AnimationNodeStateMachine::is_allow_transition_to_self() const {
	return allow_transition_to_self;
}

void AnimationNodeStateMachine::set_reset_ends(bool p_enable) {
	reset_ends = p_enable;
}

bool AnimationNodeStateMachine::are_ends_reset() const {
	return reset_ends;
}

bool AnimationNodeStateMachine::can_playback_node(const StringName &p_name) const {
	if (get_state_machine_type() == AnimationNodeStateMachine::STATE_MACHINE_TYPE_SUBSTATE) {
		return false; // don't play anything inside SubState
	}
	if (states.has(p_name) && states[p_name].node->is_state_machine()) {
		AnimationNodeStateMachine *state_machine = Object::cast_to<AnimationNodeStateMachine>(states[p_name].node.ptr());
		return state_machine->get_state_machine_type() != AnimationNodeStateMachine::STATE_MACHINE_TYPE_SUBSTATE;
	}
	return true;
}

bool AnimationNodeStateMachine::can_edit_node(const StringName &p_name) const {
	if (states.has(p_name)) {
		return !(states[p_name].node->is_class("AnimationNodeStartState") || states[p_name].node->is_class("AnimationNodeEndState") || states[p_name].node->is_class("AnimationNodeAnyState"));
	}

	return true;
}

Ref<AnimationNode> AnimationNodeStateMachine::get_node(const StringName &p_name) const {
	ERR_FAIL_COND_V_EDMSG(!states.has(p_name), Ref<AnimationNode>(), String(p_name) + " is not found current state.");

	return states[p_name].node;
}

StringName AnimationNodeStateMachine::get_node_name(const Ref<AnimationNode> &p_node) const {
	for (const KeyValue<StringName, State> &E : states) {
		if (E.value.node == p_node) {
			return E.key;
		}
	}

	ERR_FAIL_V(StringName());
}

void AnimationNodeStateMachine::get_child_nodes(List<ChildNode> *r_child_nodes) {
	Vector<StringName> nodes;

	for (const KeyValue<StringName, State> &E : states) {
		nodes.push_back(E.key);
	}

	nodes.sort_custom<StringName::AlphCompare>();

	for (int i = 0; i < nodes.size(); i++) {
		ChildNode cn;
		cn.name = nodes[i];
		cn.node = states[cn.name].node;
		r_child_nodes->push_back(cn);
	}
}

AnimationNodeStateMachine* AnimationNodeStateMachine::get_sub_state_parent() {
	if (state_machine_type == STATE_MACHINE_TYPE_SUBSTATE) {
		if (parent) {
			return parent->get_sub_state_parent();
		}
	}
	return this;
}

String AnimationNodeStateMachine::get_sub_state_parent_path() {
	if (state_machine_type == STATE_MACHINE_TYPE_SUBSTATE) {
		if (parent) {
			String parent_path = parent->get_sub_state_parent_path();
			return parent_path + node_name + "/";
		}
	}
	return "";
}

bool AnimationNodeStateMachine::has_node(const StringName &p_name) const {
	return states.has(p_name);
}

void AnimationNodeStateMachine::remove_node(const StringName &p_name) {
	ERR_FAIL_COND(!states.has(p_name));

	if (!can_edit_node(p_name)) {
		return;
	}

	Ref<AnimationNode> node = get_node(p_name);
	AnimationNodeStateMachine *transition_owner = get_sub_state_parent();

	if (node.is_valid() && node->is_state_machine()) {
		AnimationNodeStateMachine* state_machine = Object::cast_to<AnimationNodeStateMachine>(node.ptr());
		String sub_state_path = state_machine->get_sub_state_parent_path();

		if (sub_state_path.length() > 0) {
			for (int i = 0; i < transition_owner->transitions.size(); i++) {
				if (String(transition_owner->transitions[i].from).begins_with(sub_state_path) || String(transition_owner->transitions[i].to).begins_with(sub_state_path)) {
					transition_owner->remove_transition_by_index(i);
					i--;
				}
			}
		}
	}

	String node_path = get_sub_state_parent_path() + p_name;
	for (int i = 0; i < transition_owner->transitions.size(); i++) {
		if (transition_owner->transitions[i].from == node_path || transition_owner->transitions[i].to == node_path) {
			transition_owner->remove_transition_by_index(i);
			i--;
		}
	}

	{
		Ref<AnimationNode> node = states[p_name].node;
		ERR_FAIL_COND(node.is_null());
		node->disconnect("tree_changed", callable_mp(this, &AnimationNodeStateMachine::_tree_changed));
		node->disconnect("animation_node_renamed", callable_mp(this, &AnimationNodeStateMachine::_animation_node_renamed));
		node->disconnect("animation_node_removed", callable_mp(this, &AnimationNodeStateMachine::_animation_node_removed));
	}

	states.erase(p_name);

	emit_signal(SNAME("animation_node_removed"), get_instance_id(), p_name);
	emit_changed();
	emit_signal(SNAME("tree_changed"));
}

void AnimationNodeStateMachine::rename_node(const StringName &p_name, const StringName &p_new_name) {
	ERR_FAIL_COND(!states.has(p_name));
	ERR_FAIL_COND(states.has(p_new_name));
	ERR_FAIL_COND(!can_edit_node(p_name));

	states[p_new_name] = states[p_name];
	states.erase(p_name);

	if (states[p_new_name].node->is_state_machine()) {
		AnimationNodeStateMachine *state_machine = Object::cast_to<AnimationNodeStateMachine>(states[p_new_name].node.ptr());
		state_machine->node_name = p_new_name;
	}

	_rename_transitions(p_name, p_new_name);

	emit_signal(SNAME("animation_node_renamed"), get_instance_id(), p_name, p_new_name);
	emit_changed();
	emit_signal(SNAME("tree_changed"));
}

void AnimationNodeStateMachine::_rename_transitions(const StringName &p_name, const StringName &p_new_name) {
	if (updating_transitions) {
		return;
	}

	updating_transitions = true;
	String node_name = get_sub_state_parent_path() + p_name;
	String new_node_name = get_sub_state_parent_path() + p_new_name;
	String node_path = node_name + "/";
	AnimationNodeStateMachine *transition_owner = get_sub_state_parent();

	for (int i = 0; i < transition_owner->transitions.size(); i++) {
		if (transition_owner->transitions[i].from == node_name || String(transition_owner->transitions[i].from).begins_with(node_path)) {
			transition_owner->transitions.write[i].from = String(transition_owner->transitions.write[i].from).replace(node_name, new_node_name);
		}
		if (transition_owner->transitions[i].to == node_name || String(transition_owner->transitions[i].to).begins_with(node_path)) {
			transition_owner->transitions.write[i].to = String(transition_owner->transitions.write[i].to).replace(node_name, new_node_name);
		}
	}

	if (transition_owner->transitions_by_node.has(p_name)) {
		transition_owner->transitions_by_node[p_new_name] = transition_owner->transitions_by_node[p_name];
		transition_owner->transitions_by_node.erase(p_name);
	}

	for (KeyValue<StringName, Vector<Transition>> &E : transition_owner->transitions_by_node) {
		for (int i = 0; i < E.value.size(); i++) {
			Transition tr = E.value[i];
			if (tr.from == node_name || String(tr.from).begins_with(node_path)) {
				tr.from = String(tr.from).replace(node_name, new_node_name);
			}
			if (tr.to == node_name || String(tr.to).begins_with(node_path)) {
				tr.to = String(tr.from).replace(node_name, new_node_name);
			}
			E.value.set(i, tr);
		}
	}

	updating_transitions = false;
}

void AnimationNodeStateMachine::get_node_list(List<StringName> *r_nodes) const {
	List<StringName> nodes;
	for (const KeyValue<StringName, State> &E : states) {
		nodes.push_back(E.key);
	}
	nodes.sort_custom<StringName::AlphCompare>();

	for (const StringName &E : nodes) {
		r_nodes->push_back(E);
	}
}

int AnimationNodeStateMachine::get_multi_transition_count(const StringName &p_from, const StringName &p_to) const {
	int index = 0;
	for (int i = 0; i < transitions.size(); i++) {
		if (transitions[i].from == p_from && transitions[i].to == p_to) {
			++index;
		}
	}
	return index;
}

bool AnimationNodeStateMachine::has_transition(const StringName &p_from, const StringName &p_to) const {
	for (int i = 0; i < transitions.size(); i++) {
		if (transitions[i].from == p_from && transitions[i].to == p_to) {
			return true;
		}
	}
	return false;
}

bool AnimationNodeStateMachine::has_transition_from(const StringName &p_from) const {
	for (int i = 0; i < transitions.size(); i++) {
		if (transitions[i].from == p_from) {
			return true;
		}
	}
	return false;
}

bool AnimationNodeStateMachine::has_transition_to(const StringName &p_to) const {
	for (int i = 0; i < transitions.size(); i++) {
		if (transitions[i].to == p_to) {
			return true;
		}
	}
	return false;
}

int AnimationNodeStateMachine::find_transition(const StringName &p_from, const StringName &p_to) const {
	for (int i = 0; i < transitions.size(); i++) {
		if (transitions[i].from == p_from && transitions[i].to == p_to) {
			return i;
		}
	}
	return -1;
}

Vector<int> AnimationNodeStateMachine::find_transition_from(const StringName &p_from) const {
	Vector<int> ret;
	for (int i = 0; i < transitions.size(); i++) {
		if (transitions[i].from == p_from) {
			ret.push_back(i);
		}
	}
	return ret;
}

Vector<int> AnimationNodeStateMachine::find_transition_to(const StringName &p_to) const {
	Vector<int> ret;
	for (int i = 0; i < transitions.size(); i++) {
		if (transitions[i].to == p_to) {
			ret.push_back(i);
		}
	}
	return ret;
}

bool AnimationNodeStateMachine::_can_connect(const StringName &p_name) {
	if (states.has(p_name)) {
		return true;
	}

	String node_name = p_name;
	AnimationNodeStateMachine* state_machine = this;

	if (node_name.contains("/")) {
		// any full paths are relative to the SubState parent, not the SubState itself
		state_machine = get_sub_state_parent();

		Vector<String> path = node_name.split("/");
		int i = 0;

		while (i < path.size()) {
			if (!state_machine->states.has(path[i])) {
				return false;
			}

			if (i < path.size() - 1) {
				state_machine = Object::cast_to<AnimationNodeStateMachine>(state_machine->states[path[i]].node.ptr());

				// if node is not an AnimationNodeStateMachine of type SubState then return false
				if (!state_machine || state_machine->get_state_machine_type() != STATE_MACHINE_TYPE_SUBSTATE) {
					return false;
				}
			}
			++i;
		}

		return true;
	}

	return false;
}

void AnimationNodeStateMachine::add_multi_transition(const StringName &p_from, const StringName &p_to, int p_idx, const Ref<AnimationNodeStateMachineTransition> &p_transition) {
	if (updating_transitions) {
		return;
	}

	ERR_FAIL_COND(p_from == end_node || p_to == start_node);
	ERR_FAIL_COND(p_from == p_to);
	ERR_FAIL_COND(!_can_connect(p_from));
	ERR_FAIL_COND(!_can_connect(p_to));
	ERR_FAIL_COND(p_transition.is_null());

	updating_transitions = true;
	String to = p_to;

	Transition tr;
	tr.from = p_from;
	tr.to = p_to;
	if (to.contains("/")) {
		tr.to_sub_state = to.substr(0, to.rfind("/"));
		tr.to_sub_state_node = to.substr(to.rfind("/") + 1);
	}
	tr.transition = p_transition;

	tr.transition->connect("advance_condition_changed", callable_mp(this, &AnimationNodeStateMachine::_tree_changed), CONNECT_REFERENCE_COUNTED);

	if (!transitions_by_node.has(p_from)) {
		transitions_by_node[p_from] = Vector<Transition>();
	}

	// cycle through the list of transitions and compare from / to and insert the transition based on the passed p_index and how many from / to duplicates we find
	if (transitions.size() == 0) {
		transitions.push_back(tr);
		transitions_by_node[p_from].push_back(tr);
	} else if (p_idx <= 0) {
		transitions.insert(0, tr);
		transitions_by_node[p_from].insert(0, tr);
	} else {
		int index = p_idx;
		for (int i = 0; i < transitions.size() - 1; i++) {
			if (transitions[i].from == p_from && transitions[i].to == p_to) {
				--index;
				if (index <= 0) {
					transitions.insert(i + 1, tr);
					transitions_by_node[p_from].insert(p_idx, tr);
					break;
				}
			}
		}
		if (index > 0) {
			transitions.push_back(tr);
			transitions_by_node[p_from].push_back(tr);
		}
	}

	updating_transitions = false;
}

void AnimationNodeStateMachine::add_transition(const StringName &p_from, const StringName &p_to, const Ref<AnimationNodeStateMachineTransition> &p_transition) {
	if (updating_transitions) {
		return;
	}

	ERR_FAIL_COND(p_from == end_node || p_to == start_node);
	ERR_FAIL_COND(p_from == p_to);
	ERR_FAIL_COND(!_can_connect(p_from));
	ERR_FAIL_COND(!_can_connect(p_to));
	ERR_FAIL_COND(p_transition.is_null());

	updating_transitions = true;
	String to = p_to;

	Transition tr;
	tr.from = p_from;
	tr.to = p_to;
	if (to.contains("/")) {
		tr.to_sub_state = to.substr(0, to.rfind("/"));
		tr.to_sub_state_node = to.substr(to.rfind("/") + 1);
	}
	tr.transition = p_transition;

	tr.transition->connect("advance_condition_changed", callable_mp(this, &AnimationNodeStateMachine::_tree_changed), CONNECT_REFERENCE_COUNTED);
	transitions.push_back(tr);

	if (!transitions_by_node.has(p_from)) {
		transitions_by_node[p_from] = Vector<Transition>();
	}
	transitions_by_node[p_from].push_back(tr);

	updating_transitions = false;
}

Ref<AnimationNodeStateMachineTransition> AnimationNodeStateMachine::get_transition(int p_transition) const {
	ERR_FAIL_INDEX_V(p_transition, transitions.size(), Ref<AnimationNodeStateMachineTransition>());
	return transitions[p_transition].transition;
}

StringName AnimationNodeStateMachine::get_transition_from(int p_transition) const {
	ERR_FAIL_INDEX_V(p_transition, transitions.size(), StringName());
	return transitions[p_transition].from;
}

StringName AnimationNodeStateMachine::get_transition_to(int p_transition) const {
	ERR_FAIL_INDEX_V(p_transition, transitions.size(), StringName());
	return transitions[p_transition].to;
}

bool AnimationNodeStateMachine::is_transition_across_group(int p_transition) const {
	ERR_FAIL_INDEX_V(p_transition, transitions.size(), false);
	if (get_state_machine_type() == AnimationNodeStateMachine::STATE_MACHINE_TYPE_GROUPED) {
		if (transitions[p_transition].from == "Start" || transitions[p_transition].to == "End") {
			return true;
		}
	}
	return false;
}

bool AnimationNodeStateMachine::is_transition_across_group(const StringName &p_from, const StringName &p_to) const {
	if (get_state_machine_type() == AnimationNodeStateMachine::STATE_MACHINE_TYPE_GROUPED) {
		if (p_from == "Start" || p_to == "End") {
			return true;
		}
	}
	return false;
}

int AnimationNodeStateMachine::get_transition_count() const {
	return transitions.size();
}

void AnimationNodeStateMachine::remove_multi_transition(const StringName &p_from, const StringName &p_to, int p_idx) {
	int index = p_idx;
	for (int i = 0; i < transitions.size(); i++) {
		if (transitions[i].from == p_from && transitions[i].to == p_to) {
			if (index <= 0) {
				remove_transition_by_index_node(i, false); // we want to remove the one at the desired index so don't let it be removed here
				break;
			} else {
				--index;
			}
		}
	}

	index = p_idx;
	if (transitions_by_node.has(p_from)) {
		Vector<Transition> trans_list = transitions_by_node[p_from];
		for (int i = 0; i < trans_list.size(); i++) {
			if (trans_list[i].to == p_to) {
				if (index <= 0) {
					trans_list.remove_at(i);
					break;
				} else {
					--index;
				}
			}
		}
	}
}

void AnimationNodeStateMachine::remove_transition(const StringName &p_from, const StringName &p_to) {
	for (int i = 0; i < transitions.size(); i++) {
		if (transitions[i].from == p_from && transitions[i].to == p_to) {
			remove_transition_by_index(i);
			return;
		}
	}
}

void AnimationNodeStateMachine::reorder_transition(const int p_from, const int p_to) {
	Transition tr = transitions[p_from];
	transitions.insert(p_to, tr);
	transitions.remove_at(p_from > p_to ? p_from + 1 : p_from);
}

void AnimationNodeStateMachine::remove_transition_by_index(const int p_transition) {
	remove_transition_by_index_node(p_transition, true);
}

void AnimationNodeStateMachine::remove_transition_by_index_node(const int p_transition, bool remove_by_node) {
	ERR_FAIL_INDEX(p_transition, transitions.size());
	Transition tr = transitions[p_transition];
	transitions.write[p_transition].transition->disconnect("advance_condition_changed", callable_mp(this, &AnimationNodeStateMachine::_tree_changed));
	transitions.remove_at(p_transition);

	Vector<String> path_from = String(tr.from).split("/");
	Vector<String> path_to = String(tr.to).split("/");

	List<Vector<String>> paths;
	paths.push_back(path_from);
	paths.push_back(path_to);

	if (remove_by_node && transitions_by_node.has(tr.from)) {
		bool found = false;
		Vector<Transition> trans_list = transitions_by_node[tr.from];
		for (int i = 0; i < trans_list.size(); i++) {
			if (trans_list[i].to == tr.to) {
				ERR_FAIL_COND_MSG(found, "Don't call remove_transition_by_index_node with remove_by_node set to true if there is more than 1 transition from one node to another.");
				trans_list.remove_at(i);
				found = true;
				--i;
				// let the loop continue for error detection (to see if there are multiple transitions from one state to the other)
			}
		}
	}
}

void AnimationNodeStateMachine::_remove_transition(const Ref<AnimationNodeStateMachineTransition> p_transition) {
	for (int i = 0; i < transitions.size(); i++) {
		if (transitions[i].transition == p_transition) {
			remove_transition_by_index(i);
			return;
		}
	}
}

void AnimationNodeStateMachine::set_graph_offset(const Vector2 &p_offset) {
	graph_offset = p_offset;
}

Vector2 AnimationNodeStateMachine::get_graph_offset() const {
	return graph_offset;
}

double AnimationNodeStateMachine::_process(const AnimationMixer::PlaybackInfo p_playback_info, bool p_test_only) {
	Ref<AnimationNodeStateMachinePlayback> playback_new = get_parameter(playback);
	ERR_FAIL_COND_V(playback_new.is_null(), 0.0);
	playback_new->_set_grouped(state_machine_type == STATE_MACHINE_TYPE_GROUPED);
	if (p_test_only) {
		playback_new = playback_new->duplicate(); // Don't process original when testing.
	}
	return playback_new->process(node_state.base_path, this, p_playback_info, p_test_only);
}

void AnimationNodeStateMachine::blend_start() {
	WARN_PRINT("Blending from a state machine back to itself in a parent state machine using the AnyState is not recommended, strange behaviour will arise.");
	for (const KeyValue<StringName, State> &E : states) {
		E.value.node->blend_start();
	}
}

void AnimationNodeStateMachine::blend_end(const int p_index) {
	WARN_PRINT("Blending from a state machine back to itself in a parent state machine using the AnyState is not recommended, strange behaviour will arise.");
	for (const KeyValue<StringName, State> &E : states) {
		E.value.node->blend_end(p_index);
	}
}

String AnimationNodeStateMachine::get_caption() const {
	return "StateMachine";
}

Ref<AnimationNode> AnimationNodeStateMachine::get_child_by_name(const StringName &p_name) const {
	return get_node(p_name);
}

bool AnimationNodeStateMachine::_set(const StringName &p_name, const Variant &p_value) {
	String prop_name = p_name;
	if (prop_name.begins_with("states/")) {
		String node_name = prop_name.get_slicec('/', 1);
		String what = prop_name.get_slicec('/', 2);

		if (what == "node") {
			Ref<AnimationNode> anode = p_value;
			if (anode.is_valid()) {
				add_node(node_name, p_value);
			}
			return true;
		}

		if (what == "position") {
			if (states.has(node_name)) {
				states[node_name].position = p_value;
			}
			return true;
		}
	} else if (prop_name == "transitions") {
		Array trans = p_value;
		ERR_FAIL_COND_V(trans.size() % 3 != 0, false);

		for (int i = 0; i < trans.size(); i += 3) {
			add_transition(trans[i], trans[i + 1], trans[i + 2]);
		}
		return true;
	} else if (prop_name == "graph_offset") {
		set_graph_offset(p_value);
		return true;
	}

	return false;
}

bool AnimationNodeStateMachine::_get(const StringName &p_name, Variant &r_ret) const {
	String prop_name = p_name;
	if (prop_name.begins_with("states/")) {
		String node_name = prop_name.get_slicec('/', 1);
		String what = prop_name.get_slicec('/', 2);

		if (what == "node") {
			if (states.has(node_name) && can_edit_node(node_name)) {
				r_ret = states[node_name].node;
				return true;
			}
		}

		if (what == "position") {
			if (states.has(node_name)) {
				r_ret = states[node_name].position;
				return true;
			}
		}
	} else if (prop_name == "transitions") {
		Array trans;
		for (int i = 0; i < transitions.size(); i++) {
			String from = transitions[i].from;
			String to = transitions[i].to;

			trans.push_back(from);
			trans.push_back(to);
			trans.push_back(transitions[i].transition);
		}

		r_ret = trans;
		return true;
	} else if (prop_name == "graph_offset") {
		r_ret = get_graph_offset();
		return true;
	}

	return false;
}

void AnimationNodeStateMachine::_get_property_list(List<PropertyInfo> *p_list) const {
	List<StringName> names;
	for (const KeyValue<StringName, State> &E : states) {
		names.push_back(E.key);
	}
	names.sort_custom<StringName::AlphCompare>();

	for (const StringName &prop_name : names) {
		p_list->push_back(PropertyInfo(Variant::OBJECT, "states/" + prop_name + "/node", PROPERTY_HINT_RESOURCE_TYPE, "AnimationNode", PROPERTY_USAGE_NO_EDITOR));
		p_list->push_back(PropertyInfo(Variant::VECTOR2, "states/" + prop_name + "/position", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NO_EDITOR));
	}

	p_list->push_back(PropertyInfo(Variant::ARRAY, "transitions", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NO_EDITOR));
	p_list->push_back(PropertyInfo(Variant::VECTOR2, "graph_offset", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NO_EDITOR));

	for (PropertyInfo &E : *p_list) {
		_validate_property(E);
	}
}

void AnimationNodeStateMachine::_validate_property(PropertyInfo &p_property) const {
	if (p_property.name == "allow_transition_to_self" || p_property.name == "reset_ends") {
		if (state_machine_type == STATE_MACHINE_TYPE_GROUPED ||
			state_machine_type == STATE_MACHINE_TYPE_SUBSTATE) {
			p_property.usage = PROPERTY_USAGE_NONE;
		}
	}
}

void AnimationNodeStateMachine::post_enter_tree() {
	AnimationNodeStateMachine *parent_state = parent;
	String sub_state_path = node_name;

	while (parent_state != nullptr) {
		parent_state->sub_states[sub_state_path] = this;
		sub_state_path = parent_state->node_name + "/" + sub_state_path;
		parent_state = parent_state->parent;
	}
}

void AnimationNodeStateMachine::reset_state() {
	states.clear();
	sub_states.clear();
	transitions.clear();
	transitions_by_node.clear();

	playback = "playback";
	start_node = "Start";
	end_node = "End";
	graph_offset = Vector2();

	Ref<AnimationNodeStartState> s;
	s.instantiate();
	State start;
	start.node = s;
	start.position = Vector2(200, 100);
	states[start_node] = start;

	Ref<AnimationNodeAnyState> a;
	a.instantiate();
	State any_state;
	any_state.node = a;
	any_state.position = Vector2(200, 0);
	states[any_state_node] = any_state;

	Ref<AnimationNodeEndState> e;
	e.instantiate();
	State end;
	end.node = e;
	end.position = Vector2(900, 100);
	states[end_node] = end;

	emit_changed();
	emit_signal(SNAME("tree_changed"));
}

void AnimationNodeStateMachine::set_node_position(const StringName &p_name, const Vector2 &p_position) {
	ERR_FAIL_COND(!states.has(p_name));
	states[p_name].position = *(new Vector2(round(p_position.x / 10.0) * 10, round(p_position.y / 10.0) * 10));
}

Vector2 AnimationNodeStateMachine::get_node_position(const StringName &p_name) const {
	ERR_FAIL_COND_V(!states.has(p_name), Vector2());
	return states[p_name].position;
}

void AnimationNodeStateMachine::_tree_changed() {
	emit_changed();
	AnimationRootNode::_tree_changed();
}

void AnimationNodeStateMachine::_animation_node_renamed(const ObjectID &p_oid, const String &p_old_name, const String &p_new_name) {
	AnimationRootNode::_animation_node_renamed(p_oid, p_old_name, p_new_name);
}

void AnimationNodeStateMachine::_animation_node_removed(const ObjectID &p_oid, const StringName &p_node) {
	AnimationRootNode::_animation_node_removed(p_oid, p_node);
}

#ifdef TOOLS_ENABLED
void AnimationNodeStateMachine::get_argument_options(const StringName &p_function, int p_idx, List<String> *r_options) const {
	const String pf = p_function;
	bool add_state_options = false;
	if (p_idx == 0) {
		add_state_options = (pf == "get_node" || pf == "has_node" || pf == "rename_node" || pf == "remove_node" || pf == "replace_node" || pf == "set_node_position" || pf == "get_node_position");
	} else if (p_idx <= 1) {
		add_state_options = (pf == "has_transition" || pf == "add_transition" || pf == "remove_transition");
	}
	if (add_state_options) {
		for (const KeyValue<StringName, State> &E : states) {
			r_options->push_back(String(E.key).quote());
		}
	}
	AnimationRootNode::get_argument_options(p_function, p_idx, r_options);
}
#endif

void AnimationNodeStateMachine::_bind_methods() {
	ClassDB::bind_method(D_METHOD("add_node", "name", "node", "position"), &AnimationNodeStateMachine::add_node, DEFVAL(Vector2()));
	ClassDB::bind_method(D_METHOD("replace_node", "name", "node"), &AnimationNodeStateMachine::replace_node);
	ClassDB::bind_method(D_METHOD("get_node", "name"), &AnimationNodeStateMachine::get_node);
	ClassDB::bind_method(D_METHOD("remove_node", "name"), &AnimationNodeStateMachine::remove_node);
	ClassDB::bind_method(D_METHOD("rename_node", "name", "new_name"), &AnimationNodeStateMachine::rename_node);
	ClassDB::bind_method(D_METHOD("has_node", "name"), &AnimationNodeStateMachine::has_node);
	ClassDB::bind_method(D_METHOD("get_node_name", "node"), &AnimationNodeStateMachine::get_node_name);

	ClassDB::bind_method(D_METHOD("set_node_position", "name", "position"), &AnimationNodeStateMachine::set_node_position);
	ClassDB::bind_method(D_METHOD("get_node_position", "name"), &AnimationNodeStateMachine::get_node_position);

	ClassDB::bind_method(D_METHOD("has_transition", "from", "to"), &AnimationNodeStateMachine::has_transition);
	ClassDB::bind_method(D_METHOD("add_transition", "from", "to", "transition"), &AnimationNodeStateMachine::add_transition);
	ClassDB::bind_method(D_METHOD("add_multi_transition", "from", "to", "idx", "transition"), &AnimationNodeStateMachine::add_multi_transition);
	ClassDB::bind_method(D_METHOD("get_transition", "idx"), &AnimationNodeStateMachine::get_transition);
	ClassDB::bind_method(D_METHOD("get_transition_from", "idx"), &AnimationNodeStateMachine::get_transition_from);
	ClassDB::bind_method(D_METHOD("get_transition_to", "idx"), &AnimationNodeStateMachine::get_transition_to);
	ClassDB::bind_method(D_METHOD("get_transition_count"), &AnimationNodeStateMachine::get_transition_count);
	ClassDB::bind_method(D_METHOD("remove_transition_by_index", "idx"), &AnimationNodeStateMachine::remove_transition_by_index);
	ClassDB::bind_method(D_METHOD("remove_transition", "from", "to"), &AnimationNodeStateMachine::remove_transition);
	ClassDB::bind_method(D_METHOD("remove_multi_transition", "from", "to", "idx"), &AnimationNodeStateMachine::remove_multi_transition);

	ClassDB::bind_method(D_METHOD("set_graph_offset", "offset"), &AnimationNodeStateMachine::set_graph_offset);
	ClassDB::bind_method(D_METHOD("get_graph_offset"), &AnimationNodeStateMachine::get_graph_offset);

	ClassDB::bind_method(D_METHOD("set_state_machine_type", "state_machine_type"), &AnimationNodeStateMachine::set_state_machine_type);
	ClassDB::bind_method(D_METHOD("get_state_machine_type"), &AnimationNodeStateMachine::get_state_machine_type);

	ClassDB::bind_method(D_METHOD("set_allow_transition_to_self", "enable"), &AnimationNodeStateMachine::set_allow_transition_to_self);
	ClassDB::bind_method(D_METHOD("is_allow_transition_to_self"), &AnimationNodeStateMachine::is_allow_transition_to_self);

	ClassDB::bind_method(D_METHOD("set_reset_ends", "enable"), &AnimationNodeStateMachine::set_reset_ends);
	ClassDB::bind_method(D_METHOD("are_ends_reset"), &AnimationNodeStateMachine::are_ends_reset);

	ADD_PROPERTY(PropertyInfo(Variant::INT, "state_machine_type", PROPERTY_HINT_ENUM, "Root,Nested,Grouped,SubState"), "set_state_machine_type", "get_state_machine_type");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "allow_transition_to_self"), "set_allow_transition_to_self", "is_allow_transition_to_self");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "reset_ends"), "set_reset_ends", "are_ends_reset");

	BIND_ENUM_CONSTANT(STATE_MACHINE_TYPE_ROOT);
	BIND_ENUM_CONSTANT(STATE_MACHINE_TYPE_NESTED);
	BIND_ENUM_CONSTANT(STATE_MACHINE_TYPE_GROUPED);
	BIND_ENUM_CONSTANT(STATE_MACHINE_TYPE_SUBSTATE);
}

AnimationNodeStateMachine::AnimationNodeStateMachine() {
	parent = nullptr;
	sub_states.clear();

	Ref<AnimationNodeStartState> s;
	s.instantiate();
	State start;
	start.node = s;
	start.position = Vector2(200, 100);
	states[start_node] = start;

	Ref<AnimationNodeAnyState> a;
	a.instantiate();
	State any_state;
	any_state.node = a;
	any_state.position = Vector2(200, 0);
	states[any_state_node] = any_state;

	Ref<AnimationNodeEndState> e;
	e.instantiate();
	State end;
	end.node = e;
	end.position = Vector2(900, 100);
	states[end_node] = end;
}
