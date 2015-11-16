/*
 * Samsung API
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the License);
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/license/
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <app.h>
#include <app_control.h>
#include <Elementary.h>
#include <efl_extension.h>

#include "attach_panel.h"
#include "log.h"



const char *const KEY_BACK = "XF86Back";
const char *const LAYOUT = "/usr/share/attach-panel/sample/sample.edj";



static struct {
	Evas_Object *win;
	Evas_Object *layout;
	Evas_Object *bg;
	Evas_Object *conformant;
	attach_panel_h attach_panel;

	int root_w;
	int root_h;
} sample_info = {
	.win = NULL,
	.layout = NULL,
	.bg = NULL,
	.conformant = NULL,
	.attach_panel = NULL,

	.root_w = 0,
	.root_h = 0,
};



static void _rotate_cb(void *data, Evas_Object *obj, void *event)
{
	ret_if(!obj);

	int angle = 0;

	angle = elm_win_rotation_get(obj);

	_D("Angle is %d degree", angle);

	switch (angle) {
	case 0:
	case 180:
		evas_object_size_hint_min_set(obj, sample_info.root_w, sample_info.root_h);
		evas_object_resize(obj, sample_info.root_w, sample_info.root_h);
		evas_object_move(obj, 0, 0);
		break;
	case 90:
	case 270:
		evas_object_size_hint_min_set(obj, sample_info.root_h, sample_info.root_w);
		evas_object_resize(obj, sample_info.root_h, sample_info.root_w);
		evas_object_move(obj, 0, 0);
		break;
	default:
		_E("cannot reach here");
	}
}



static void _result_cb(attach_panel_h attach_panel, attach_panel_content_category_e content_category, app_control_h result, app_control_result_e result_code, void *user_data)
{
	char **select = NULL;
	int i = 0;
	int length = 0;
	int ret = APP_CONTROL_ERROR_NONE;

	ret_if(!result);

	ret = app_control_get_extra_data_array(result, "http://tizen.org/appcontrol/data/selected", &select, &length);
	if (APP_CONTROL_ERROR_NONE == ret) {
		for (i = 0; i < length; i++) {
			_D("file_path is %s[%d]", select[i], i);
			elm_object_part_text_set(sample_info.layout, "result", select[i]);
			free(select[i]);
		}
	}

	free(select);
}



static void _event_cb(attach_panel_h attach_panel, attach_panel_event_e event, void *event_info, void *data)
{
	ret_if(!attach_panel);

	switch (event) {
	case ATTACH_PANEL_EVENT_SHOW_START:
		_D("attach panel : show start");
		break;
	case ATTACH_PANEL_EVENT_SHOW_FINISH:
		_D("attach panel : show finish");
		break;
	case ATTACH_PANEL_EVENT_HIDE_START:
		_D("attach panel : hide start");
		break;
	case ATTACH_PANEL_EVENT_HIDE_FINISH:
		_D("attach panel : hide finish");
		break;
	}
}



static void _win_back_key_cb(void *data, Evas_Object *obj, void *event_info)
{
	bool visible = false;

	if (sample_info.attach_panel) {
		if (attach_panel_get_visibility(sample_info.attach_panel, &visible) != ATTACH_PANEL_ERROR_NONE) {
			_E("Fail to get visible of attach panel");
			return;
		}
		if (visible) {
			_D("attach panel state : show->hide");
			attach_panel_hide(sample_info.attach_panel);
		} else {
			attach_panel_remove_content_category(sample_info.attach_panel, ATTACH_PANEL_CONTENT_CATEGORY_IMAGE);
			attach_panel_remove_content_category(sample_info.attach_panel, ATTACH_PANEL_CONTENT_CATEGORY_CAMERA);
			attach_panel_remove_content_category(sample_info.attach_panel, ATTACH_PANEL_CONTENT_CATEGORY_VOICE);
			attach_panel_remove_content_category(sample_info.attach_panel, ATTACH_PANEL_CONTENT_CATEGORY_VIDEO);
			attach_panel_remove_content_category(sample_info.attach_panel, ATTACH_PANEL_CONTENT_CATEGORY_AUDIO);
			attach_panel_remove_content_category(sample_info.attach_panel, ATTACH_PANEL_CONTENT_CATEGORY_CALENDAR);
			attach_panel_remove_content_category(sample_info.attach_panel, ATTACH_PANEL_CONTENT_CATEGORY_CONTACT);
			attach_panel_remove_content_category(sample_info.attach_panel, ATTACH_PANEL_CONTENT_CATEGORY_MYFILES);
			attach_panel_remove_content_category(sample_info.attach_panel, ATTACH_PANEL_CONTENT_CATEGORY_VIDEO_RECORDER);

			attach_panel_unset_result_cb(sample_info.attach_panel);
			attach_panel_unset_event_cb(sample_info.attach_panel);

			attach_panel_destroy(sample_info.attach_panel);
			sample_info.attach_panel = NULL;
		}
	} else {
		ui_app_exit();
	}
}



static void _attach_panel_cancel_cb(void *data, Evas_Object *obj, const char *emission, const char *source)
{
	_D("");

	if (sample_info.attach_panel) {
		attach_panel_remove_content_category(sample_info.attach_panel, ATTACH_PANEL_CONTENT_CATEGORY_IMAGE);
		attach_panel_remove_content_category(sample_info.attach_panel, ATTACH_PANEL_CONTENT_CATEGORY_CAMERA);
		attach_panel_remove_content_category(sample_info.attach_panel, ATTACH_PANEL_CONTENT_CATEGORY_VOICE);
		attach_panel_remove_content_category(sample_info.attach_panel, ATTACH_PANEL_CONTENT_CATEGORY_VIDEO);
		attach_panel_remove_content_category(sample_info.attach_panel, ATTACH_PANEL_CONTENT_CATEGORY_AUDIO);
		attach_panel_remove_content_category(sample_info.attach_panel, ATTACH_PANEL_CONTENT_CATEGORY_CALENDAR);
		attach_panel_remove_content_category(sample_info.attach_panel, ATTACH_PANEL_CONTENT_CATEGORY_CONTACT);
		attach_panel_remove_content_category(sample_info.attach_panel, ATTACH_PANEL_CONTENT_CATEGORY_MYFILES);
		attach_panel_remove_content_category(sample_info.attach_panel, ATTACH_PANEL_CONTENT_CATEGORY_VIDEO_RECORDER);

		attach_panel_hide(sample_info.attach_panel);
		attach_panel_unset_result_cb(sample_info.attach_panel);
		attach_panel_unset_event_cb(sample_info.attach_panel);

		attach_panel_destroy(sample_info.attach_panel);
		sample_info.attach_panel = NULL;
	}
}



static void _attach_panel_create_cb(void *data, Evas_Object *obj, void *event_info)
{
	attach_panel_h attach_panel = NULL;
	int ret = ATTACH_PANEL_ERROR_NONE;
	bool visible = false;

	if (sample_info.attach_panel) {
		_D("attach panel already existed");
		if (attach_panel_get_visibility(sample_info.attach_panel, &visible) != ATTACH_PANEL_ERROR_NONE) {
			_E("Fail to get visible of attach panel");
			return;
		}
		if (!visible) {
			_D("attach panel state : hide->show");
			attach_panel_show(sample_info.attach_panel);
		}
		return;
	}

	ret = attach_panel_create(sample_info.conformant, &attach_panel);
	ret_if(ATTACH_PANEL_ERROR_NONE != ret);

	attach_panel_add_content_category(attach_panel, ATTACH_PANEL_CONTENT_CATEGORY_IMAGE, NULL);
	attach_panel_add_content_category(attach_panel, ATTACH_PANEL_CONTENT_CATEGORY_CAMERA, NULL);
	attach_panel_add_content_category(attach_panel, ATTACH_PANEL_CONTENT_CATEGORY_VOICE, NULL);
	attach_panel_add_content_category(attach_panel, ATTACH_PANEL_CONTENT_CATEGORY_VIDEO, NULL);
	attach_panel_add_content_category(attach_panel, ATTACH_PANEL_CONTENT_CATEGORY_AUDIO, NULL);
	attach_panel_add_content_category(attach_panel, ATTACH_PANEL_CONTENT_CATEGORY_CALENDAR, NULL);
	attach_panel_add_content_category(attach_panel, ATTACH_PANEL_CONTENT_CATEGORY_CONTACT, NULL);
	attach_panel_add_content_category(attach_panel, ATTACH_PANEL_CONTENT_CATEGORY_MYFILES, NULL);
	attach_panel_add_content_category(attach_panel, ATTACH_PANEL_CONTENT_CATEGORY_VIDEO_RECORDER, NULL);
	attach_panel_set_result_cb(attach_panel, _result_cb, NULL);
	attach_panel_set_event_cb(attach_panel, _event_cb, NULL);

	attach_panel_show(attach_panel);

	sample_info.attach_panel = attach_panel;
}



static Evas_Object *_create_entry(Evas_Object *layout)
{
	Evas_Object *entry = NULL;

	retv_if(!layout, NULL);

	entry = elm_entry_add(layout);
	retv_if(!entry, NULL);

	elm_entry_scrollable_set(entry, EINA_TRUE);
	elm_object_part_text_set(entry, "elm.guide", "search");

	elm_object_part_content_set(layout, "entry", entry);

	elm_entry_cnp_mode_set(entry, ELM_CNP_MODE_PLAINTEXT);
	elm_entry_input_panel_layout_set(entry, ELM_INPUT_PANEL_LAYOUT_NORMAL);
	elm_entry_prediction_allow_set(entry, EINA_FALSE);
	elm_entry_autocapital_type_set(entry, ELM_AUTOCAPITAL_TYPE_NONE);
	elm_entry_input_panel_return_key_type_set(entry, ELM_INPUT_PANEL_RETURN_KEY_TYPE_DONE);
	elm_entry_input_panel_return_key_disabled_set(entry, EINA_FALSE);

	evas_object_show(entry);

	return entry;
}



static void _destroy_entry(Evas_Object *layout)
{
	Evas_Object *entry = NULL;
	_D("");

	entry = elm_object_part_content_unset(layout, "entry");
	if (!entry) {
		return;
	}
	evas_object_del(entry);
}



static Evas_Object *_create_button(Evas_Object *layout)
{
	Evas_Object *button = NULL;

	retv_if(!layout, NULL);

	button = elm_button_add(layout);
	retv_if(!button, NULL);

	elm_object_text_set(button, "attach");
	elm_object_part_content_set(layout, "button", button);
	evas_object_size_hint_weight_set(button, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_show(button);

	evas_object_smart_callback_add(button, "clicked", _attach_panel_create_cb, NULL);

	return button;
}



static void _destroy_button(Evas_Object *layout)
{
	Evas_Object *button = NULL;

	button = elm_object_part_content_unset(layout, "button");
	if (!button) {
		return;
	}

	evas_object_smart_callback_del(button, "clicked", _attach_panel_create_cb);
	evas_object_del(button);
}



static void _destroy_layout(void)
{
	ret_if(!sample_info.layout);

	_destroy_button(sample_info.layout);
	_destroy_entry(sample_info.layout);

	elm_object_signal_callback_del(sample_info.layout, "bg,up", "bg", _attach_panel_cancel_cb);

	if (sample_info.conformant) {
		elm_object_content_unset(sample_info.conformant);
	}
	evas_object_del(sample_info.layout);
	sample_info.layout = NULL;
}



static Evas_Object *_create_layout(Evas_Object *parent)
{
	Evas_Object *layout = NULL;
	Eina_Bool ret = EINA_FALSE;

	retv_if(!parent, NULL);

	layout = elm_layout_add(parent);
	goto_if(!layout, ERROR);

	ret = elm_layout_file_set(layout, LAYOUT, "layout");
	goto_if(EINA_FALSE == ret, ERROR);

	evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(layout, EVAS_HINT_FILL, EVAS_HINT_FILL);
	evas_object_show(layout);
	elm_object_signal_callback_add(layout, "bg,up", "bg", _attach_panel_cancel_cb, NULL);

	goto_if(!_create_entry(layout), ERROR);
	goto_if(!_create_button(layout), ERROR);

	elm_object_content_set(parent, layout);

	return layout;


ERROR:
	_destroy_layout();
	return NULL;
}



static Evas_Object *_create_bg(Evas_Object *parent)
{
	Evas_Object *bg = NULL;
	retv_if(!parent, NULL);

	bg = elm_bg_add(parent);
	retv_if(!bg, NULL);
	evas_object_size_hint_weight_set(bg,  EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_win_resize_object_add(parent, bg);

	evas_object_show(bg);

	return bg;
}



static void _destroy_bg(void)
{
	ret_if(!sample_info.bg);
	evas_object_del(sample_info.bg);
	sample_info.bg = NULL;
}



static void _destroy_conformant(void)
{
	ret_if(!sample_info.conformant);
	evas_object_del(sample_info.conformant);
	sample_info.conformant = NULL;
}



static Evas_Object *_create_conformant(Evas_Object *parent)
{
	Evas_Object *conformant = NULL;
	retv_if(!parent, NULL);

	conformant = elm_conformant_add(parent);
	retv_if(!conformant, NULL);

	elm_win_indicator_mode_set(parent, ELM_WIN_INDICATOR_SHOW);
	elm_win_indicator_opacity_set(parent, ELM_WIN_INDICATOR_OPAQUE);
	evas_object_size_hint_weight_set(conformant,  EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_win_resize_object_add(parent, conformant);
	elm_win_conformant_set(parent, EINA_TRUE);

	evas_object_show(conformant);

	return conformant;
}



static void _create_cb(void)
{
	Evas_Object *layout = NULL;
	Evas_Object *conformant = NULL;
	Evas_Object *bg = NULL;

	sample_info.win = elm_win_add(NULL, "Attach Panel Sample", ELM_WIN_BASIC);
	ret_if(!sample_info.win);

	elm_app_base_scale_set(1.7);

	elm_win_title_set(sample_info.win, "Attach Panel");
	elm_win_alpha_set(sample_info.win, EINA_FALSE);
	elm_win_borderless_set(sample_info.win, EINA_TRUE);
	elm_win_autodel_set(sample_info.win, EINA_TRUE);
	elm_win_raise(sample_info.win);

	evas_object_show(sample_info.win);

	elm_win_screen_size_get(sample_info.win, NULL, NULL, &sample_info.root_w, &sample_info.root_h);
	_D("screen size is (%d, %d)", sample_info.root_w, sample_info.root_h);


	if (elm_win_wm_rotation_supported_get(sample_info.win)) {
		int rots[4] = { 0, 90, 180, 270 };
		elm_win_wm_rotation_available_rotations_set(sample_info.win, rots, 4);
	}

	eext_object_event_callback_add(sample_info.win, EEXT_CALLBACK_BACK, _win_back_key_cb, NULL);
	evas_object_smart_callback_add(sample_info.win, "wm,rotation,changed", _rotate_cb, NULL);

	bg = _create_bg(sample_info.win);
	goto_if(!bg, ERROR);

	conformant = _create_conformant(sample_info.win);
	goto_if(!conformant, ERROR);

	layout = _create_layout(conformant);
	goto_if(!layout, ERROR);

	sample_info.layout = layout;
	sample_info.conformant = conformant;
	sample_info.bg = bg;

	return;

ERROR:
	_D("there is some error");
	if (conformant) {
		_destroy_conformant();
	}
	if (bg) {
		_destroy_bg();
	}

	if (sample_info.win) {
		evas_object_del(sample_info.win);
		eext_object_event_callback_del(sample_info.win, EEXT_CALLBACK_BACK, _win_back_key_cb);
		sample_info.win = NULL;
	}
}



static void _terminate_cb(void)
{
	_D("");

	_destroy_layout();
	_destroy_conformant();
	_destroy_bg();

	if (sample_info.win) {
		eext_object_event_callback_del(sample_info.win, EEXT_CALLBACK_BACK, _win_back_key_cb);
		evas_object_del(sample_info.win);
		sample_info.win = NULL;
	}
}



int main(int argc, char **argv)
{
	elm_init(argc, argv);
	_create_cb();
	elm_run();
	_terminate_cb();
	elm_shutdown();

	return 0;
}
