/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
 *
 * Licensed under the Apache License, Version 2.0 (the License);
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <app_control.h>
#include <Elementary.h>
#include <tizen.h>

#include "attach_panel.h"
#include "attach_panel_internal.h"
#include "conf.h"
#include "content_list.h"
#include "grid.h"
#include "log.h"
#include "page.h"
#include "scroller.h"

#define TRANSIT_DURATION 0.2f


static struct {
	Elm_Transit *transit;
	attach_panel_state_e attach_panel_state;
} gesture_info_s = {
	.transit = NULL,
	.attach_panel_state = ATTACH_PANEL_STATE_HIDE,
};

typedef struct custom_effect {
	Evas_Coord from_h;
	Evas_Coord to_h;
} custom_effect_s;



attach_panel_state_e _gesture_get_state(void)
{
	return gesture_info_s.attach_panel_state;
}



void _gesture_set_state(attach_panel_state_e state)
{
	gesture_info_s.attach_panel_state = state;
}



static Elm_Transit_Effect *__custom_effect_new(Evas_Coord from_h, Evas_Coord to_h)
{
	custom_effect_s *custom_effect = calloc(1, sizeof(custom_effect_s));
	if (!custom_effect) return NULL;
	_D("%s, from_h : %d to_h: %d", __func__, from_h, to_h);
	custom_effect->from_h = from_h;
	custom_effect->to_h = to_h;

	return custom_effect;
}



static void __custom_effect_op(Elm_Transit_Effect *effect, Elm_Transit *transit, double progress)
{
	custom_effect_s *custom_effect = effect;
	const Eina_List *list = NULL;
	const Eina_List *objs = NULL;
	Evas_Object *obj = NULL;
	Evas_Coord h = 0;

	if (!effect) {
		_D("effect is alrealy exist");
		return;
	}

	objs = elm_transit_objects_get(transit);

	h = (Evas_Coord) ((custom_effect->from_h * (1.0 - progress)) + (custom_effect->to_h * progress));

	_D("change the attach-panel height : %d(progress: %f)", h, progress);
	EINA_LIST_FOREACH(objs, list, obj) {
		evas_object_size_hint_min_set(obj, -1, h);
		evas_object_size_hint_max_set(obj, -1, h);
	}
}



static void __custom_effect_free(Elm_Transit_Effect *effect, Elm_Transit *transit)
{
	_D("");
	free(effect);
}



static void __attach_panel_transit_del_cb(void *data, Elm_Transit *transit)
{
	attach_panel_h attach_panel = data;
	_D("%s : transit is ended", __func__);

	gesture_info_s.transit = NULL;
	if (gesture_info_s.attach_panel_state == ATTACH_PANEL_STATE_HIDE) {
		elm_object_signal_emit(attach_panel->conformant, "elm,state,attach_panel,hide,finished", "");
		if (attach_panel->event_cb) {
			attach_panel->event_cb(attach_panel, ATTACH_PANEL_EVENT_HIDE_FINISH, NULL, attach_panel->event_data);
		}
		if (attach_panel->is_delete) {
			_attach_panel_del(attach_panel);
		}
	} else {
		elm_object_signal_emit(attach_panel->conformant, "elm,state,attach_panel,show,finished", "");
		if (attach_panel->event_cb) {
			attach_panel->event_cb(attach_panel, ATTACH_PANEL_EVENT_SHOW_FINISH, NULL, attach_panel->event_data);
		}
	}
}



static void __attach_panel_transit_set(attach_panel_h attach_panel, Evas_Coord from_h, Evas_Coord to_h, double duration)
{
	Elm_Transit_Effect *custom_effect = NULL;

	if (gesture_info_s.transit) {
		_E("Transit is already activating");
		if (attach_panel->cur_event_state == ATTACH_PANEL_EVENT_HIDE_START) {
			if (gesture_info_s.attach_panel_state != ATTACH_PANEL_STATE_HIDE) {
				if (attach_panel->event_cb) {
					attach_panel->event_cb(attach_panel, ATTACH_PANEL_EVENT_HIDE_FINISH, NULL, attach_panel->event_data);
					attach_panel->event_cb(attach_panel, ATTACH_PANEL_EVENT_SHOW_START, NULL, attach_panel->event_data);
				}
			}
		} else {
			if (gesture_info_s.attach_panel_state == ATTACH_PANEL_STATE_HIDE) {
				if (attach_panel->event_cb) {
					attach_panel->event_cb(attach_panel, ATTACH_PANEL_EVENT_SHOW_FINISH, NULL, attach_panel->event_data);
					attach_panel->event_cb(attach_panel, ATTACH_PANEL_EVENT_HIDE_START, NULL, attach_panel->event_data);
				}
			}
		}
		return;
	}

	custom_effect = __custom_effect_new(from_h, to_h);
	ret_if(!custom_effect);

	if (gesture_info_s.attach_panel_state == ATTACH_PANEL_STATE_HIDE) {
		elm_object_signal_emit(attach_panel->conformant, "elm,state,attach_panel,hide,started", "");
		attach_panel->cur_event_state = ATTACH_PANEL_EVENT_HIDE_START;
		if (attach_panel->event_cb) {
			attach_panel->event_cb(attach_panel, ATTACH_PANEL_EVENT_HIDE_START, NULL, attach_panel->event_data);
		}
	} else {
		elm_object_signal_emit(attach_panel->conformant, "elm,state,attach_panel,show,started", "");
		attach_panel->cur_event_state = ATTACH_PANEL_EVENT_SHOW_START;
		if (attach_panel->event_cb) {
			attach_panel->event_cb(attach_panel, ATTACH_PANEL_EVENT_SHOW_START, NULL, attach_panel->event_data);
		}
	}

	gesture_info_s.transit = elm_transit_add();
	if (!gesture_info_s.transit) {
		_E("Fail to create transit");
		__custom_effect_free(custom_effect, NULL);
		return;
	}
	elm_transit_del_cb_set(gesture_info_s.transit, __attach_panel_transit_del_cb, attach_panel);
	elm_transit_object_add(gesture_info_s.transit, attach_panel->attach_panel_rect);
	elm_transit_tween_mode_set(gesture_info_s.transit, ELM_TRANSIT_TWEEN_MODE_DECELERATE);
	elm_transit_smooth_set(gesture_info_s.transit, EINA_FALSE);
	elm_transit_effect_add(gesture_info_s.transit, __custom_effect_op, custom_effect, __custom_effect_free);
	elm_transit_duration_set(gesture_info_s.transit, duration);
	elm_transit_go(gesture_info_s.transit);
}



void _gesture_show(attach_panel_h attach_panel)
{
	gesture_info_s.attach_panel_state = ATTACH_PANEL_STATE_HALF;
	_D("gesture show start");
	elm_config_focus_autoscroll_mode_set(ELM_FOCUS_AUTOSCROLL_MODE_NONE);

	if (attach_panel->rotate) {
		_content_list_send_message(attach_panel->content_list, APP_CONTROL_DATA_SELECTION_MODE, SELECTION_MODE_MULTIPLE, ATTACH_PANEL_CONTENT_CATEGORY_UG);
		attach_panel->attach_panel_land_state = ATTACH_PANEL_STATE_FULL;
		gesture_info_s.attach_panel_state = ATTACH_PANEL_STATE_FULL;
		__attach_panel_transit_set(attach_panel, 0, attach_panel->transit_height, TRANSIT_DURATION);
		elm_object_signal_emit(attach_panel->conformant, "elm,state,attach_panel,show,full", "");


	} else {
		_content_list_send_message(attach_panel->content_list, APP_CONTROL_DATA_SELECTION_MODE, SELECTION_MODE_SINGLE, ATTACH_PANEL_CONTENT_CATEGORY_UG);
		elm_scroller_movement_block_set(attach_panel->grid, ELM_SCROLLER_MOVEMENT_BLOCK_VERTICAL);
		attach_panel->attach_panel_port_state = ATTACH_PANEL_STATE_HALF;
		gesture_info_s.attach_panel_state = ATTACH_PANEL_STATE_HALF;
		__attach_panel_transit_set(attach_panel, 0, attach_panel->transit_height, TRANSIT_DURATION);
	}
}



void _gesture_hide(attach_panel_h attach_panel)
{
	_D("gestrue hide start");
	elm_config_focus_autoscroll_mode_set(ELM_FOCUS_AUTOSCROLL_MODE_BRING_IN);

	if (attach_panel->rotate) {
		attach_panel->attach_panel_land_state = ATTACH_PANEL_STATE_HIDE;
		elm_object_signal_emit(attach_panel->conformant, "elm,state,attach_panel,show,half", "");
	} else {
		if (ATTACH_PANEL_STATE_FULL == attach_panel->attach_panel_port_state) {
			elm_object_signal_emit(attach_panel->conformant, "elm,state,attach_panel,show,half", "");
		}
		attach_panel->attach_panel_port_state = ATTACH_PANEL_STATE_HIDE;
	}
	gesture_info_s.attach_panel_state = ATTACH_PANEL_STATE_HIDE;
	_content_list_set_flick(attach_panel->content_list, EINA_TRUE);
	__attach_panel_transit_set(attach_panel, attach_panel->transit_height, 0, TRANSIT_DURATION);
}



void _gesture_set_full_mode(attach_panel_h attach_panel)
{
	if (attach_panel->rotate) {
		return;
	}
	if (attach_panel->attach_panel_port_state == ATTACH_PANEL_STATE_HALF) {
		attach_panel->attach_panel_port_state = ATTACH_PANEL_STATE_FULL;
		gesture_info_s.attach_panel_state = ATTACH_PANEL_STATE_FULL;
		elm_scroller_movement_block_set(attach_panel->grid, ELM_SCROLLER_MOVEMENT_NO_BLOCK);
		elm_object_signal_emit(attach_panel->conformant, "elm,state,attach_panel,show,full", "");
		_content_list_send_message(attach_panel->content_list, APP_CONTROL_DATA_SELECTION_MODE, SELECTION_MODE_MULTIPLE, ATTACH_PANEL_CONTENT_CATEGORY_UG);
	}
}



static Evas_Event_Flags __flick_end_cb(void *data, void *event_info)
{
	attach_panel_h attach_panel = data;
	content_s *content_info = NULL;
	Elm_Gesture_Line_Info *line_info = (Elm_Gesture_Line_Info *) event_info;

	retv_if(!attach_panel, EVAS_EVENT_FLAG_ON_HOLD);
	retv_if(!line_info, EVAS_EVENT_FLAG_ON_HOLD);
	_D("flick end event");

	if (EINA_TRUE == attach_panel->is_delete) {
		_E("Attach panel is already removed");
		return EVAS_EVENT_FLAG_ON_HOLD;
	}

	if (_scroller_is_scrolling(attach_panel->scroller)) {
		return EVAS_EVENT_FLAG_ON_HOLD;
	}
	if (!_grid_can_flick(attach_panel)) {
		return EVAS_EVENT_FLAG_ON_HOLD;
	}

	content_info = eina_list_nth(attach_panel->content_list, attach_panel->cur_page_no);
	retv_if(!content_info, EVAS_EVENT_FLAG_ON_HOLD);

	if (!content_info->flick && line_info->momentum.my >= 0) {
		_D("flick of current page(%d) is disable", attach_panel->cur_page_no);
		return EVAS_EVENT_FLAG_ON_HOLD;
	}

	if ((line_info->angle > 45 && line_info->angle < 135) || (line_info->angle > 225 && line_info->angle < 315)) {
		_D("it is not flick event");
		return EVAS_EVENT_FLAG_ON_HOLD;
	}

	/* Flick Up */
	if (line_info->momentum.my < 0) {
		if (attach_panel->rotate) {
			return EVAS_EVENT_FLAG_ON_HOLD;
		}
		if (attach_panel->attach_panel_port_state == ATTACH_PANEL_STATE_HALF) {
			attach_panel->attach_panel_port_state = ATTACH_PANEL_STATE_FULL;
			gesture_info_s.attach_panel_state = ATTACH_PANEL_STATE_FULL;
			elm_scroller_movement_block_set(attach_panel->grid, ELM_SCROLLER_MOVEMENT_NO_BLOCK);
			elm_object_signal_emit(attach_panel->conformant, "elm,state,attach_panel,show,full", "");
			_content_list_send_message(attach_panel->content_list, APP_CONTROL_DATA_SELECTION_MODE, SELECTION_MODE_MULTIPLE, ATTACH_PANEL_CONTENT_CATEGORY_UG);
		}
	}
	/* Flick Down */
	else {
		if (attach_panel->rotate) {
			if (attach_panel->attach_panel_land_state == ATTACH_PANEL_STATE_FULL) {
				_gesture_hide(attach_panel);
			}
		} else {
			if (attach_panel->attach_panel_port_state == ATTACH_PANEL_STATE_FULL) {
				attach_panel->attach_panel_port_state = ATTACH_PANEL_STATE_HALF;
				gesture_info_s.attach_panel_state = ATTACH_PANEL_STATE_HALF;
				elm_scroller_movement_block_set(attach_panel->grid, ELM_SCROLLER_MOVEMENT_BLOCK_VERTICAL);
				elm_object_signal_emit(attach_panel->conformant, "elm,state,attach_panel,show,half", "");
				_content_list_send_message(attach_panel->content_list, APP_CONTROL_DATA_SELECTION_MODE, SELECTION_MODE_SINGLE, ATTACH_PANEL_CONTENT_CATEGORY_UG);
			} else if (attach_panel->attach_panel_port_state == ATTACH_PANEL_STATE_HALF) {
				_gesture_hide(attach_panel);
				_content_list_send_message(attach_panel->content_list, APP_CONTROL_DATA_SELECTION_MODE, SELECTION_MODE_SINGLE, ATTACH_PANEL_CONTENT_CATEGORY_UG);
			}
		}
	}

	return EVAS_EVENT_FLAG_ON_HOLD;
}



Evas_Object *_gesture_create(attach_panel_h attach_panel)
{
	Evas_Object *gesture_layer = NULL;

	retv_if(!attach_panel, NULL);
	retv_if(!attach_panel->ui_manager, NULL);

	_D("gesture create");

	gesture_layer = elm_gesture_layer_add(attach_panel->scroller);
	retv_if(!gesture_layer, NULL);
	elm_gesture_layer_attach(gesture_layer, attach_panel->scroller);
	evas_object_show(gesture_layer);

	elm_gesture_layer_cb_set(gesture_layer, ELM_GESTURE_N_FLICKS, ELM_GESTURE_STATE_END, __flick_end_cb, attach_panel);

	return gesture_layer;

}



void _gesture_destroy(attach_panel_h attach_panel)
{
	ret_if(!attach_panel);
	ret_if(!attach_panel->gesture);

	evas_object_del(attach_panel->gesture);
	attach_panel->gesture = NULL;
}
