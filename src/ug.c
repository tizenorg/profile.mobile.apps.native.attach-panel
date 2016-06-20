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
#include <bundle.h>
#include <Elementary.h>
#include <ui-gadget.h>

#include "attach_panel.h"
#include "attach_panel_internal.h"
#include "attach_bundle.h"
#include "content_list.h"
#include "gesture.h"
#include "log.h"


//LCOV_EXCL_START
static void __result_cb(ui_gadget_h ui_gadget, app_control_h result, void *priv)
{
	content_s *content_info = priv;
	char *enable = NULL;
	char **select = NULL;

	int i = 0;
	int length = 0;
	int ret = 0;

	ret_if(!content_info);

	if (ATTACH_PANEL_STATE_HIDE == _gesture_get_state())
		return;

	ret = app_control_get_extra_data(result, "__ATTACH_PANEL_FLICK_DOWN__", &enable);
	if (APP_CONTROL_ERROR_NONE == ret) {
		ret_if(!enable);

		_D("attach panel flick %s", enable);
		if (!strcmp(enable, MODE_ENABLE)) {
			content_info->flick = EINA_TRUE;
		} else if (!strcmp(enable, MODE_DISABLE)) {
			content_info->flick = EINA_FALSE;
		} else {
			_E("__ATTACH_PANEL_FLICK_DOWN__ value is wrong type(%s)", enable);
			content_info->flick = EINA_TRUE;
		}
		return;
	}

	ret = app_control_get_extra_data(result, "__ATTACH_PANEL_FULL_MODE__", &enable);
	if (APP_CONTROL_ERROR_NONE == ret) {
		ret_if(!enable);

		_D("attach panel full mode %s", enable);
		if (!strcmp(enable, MODE_ENABLE)) {
			_gesture_set_full_mode(content_info->attach_panel);
		} else {
			_E("__ATTACH_PANEL_FULL_MODE__ value is wrong type(%s)", enable);
		}
		return;
	}

	/* This can be called by clicking the 'Cancel' button on the UGs */
	ret = app_control_get_extra_data(result, "__ATTACH_PANEL_SHOW_PANEL__", &enable);
	if (APP_CONTROL_ERROR_NONE == ret) {
		ret_if(!enable);

		_D("attach panel show panel %s", enable);
		if (!strcmp(enable, MODE_TRUE)) {
			if (ATTACH_PANEL_STATE_HIDE == _gesture_get_state()) {
				/* This is same with attach_panel_show() */
				_content_list_set_resume(content_info->attach_panel->content_list, ATTACH_PANEL_CONTENT_CATEGORY_UG);
				_content_list_send_message(content_info->attach_panel->content_list, "__ATTACH_PANEL_INITIALIZE__", MODE_ENABLE, ATTACH_PANEL_CONTENT_CATEGORY_UG);
				_gesture_show(content_info->attach_panel);
			}
		} else {
			if (ATTACH_PANEL_STATE_HIDE != _gesture_get_state()) {
				/* This is same with attach_panel_hide() */
				_content_list_set_pause(content_info->attach_panel->content_list, ATTACH_PANEL_CONTENT_CATEGORY_UG);
				_gesture_hide(content_info->attach_panel);
			}
		}
		return;
	}

	/* This can be called on the state of recording video. */
	ret = app_control_get_extra_data(result, "__ATTACH_PANEL_SHOW_TOOLBAR__", &enable);
	if (APP_CONTROL_ERROR_NONE == ret) {
		ret_if(!enable);

		_D("attach panel show toolbar %s", enable);
		if (!strcmp(enable, MODE_TRUE)) {
			elm_object_signal_emit(content_info->attach_panel->ui_manager, "toolbar,show", "toolbar");
		} else {
			elm_object_signal_emit(content_info->attach_panel->ui_manager, "toolbar,hide", "toolbar");
		}
		return;
	}

	_D("The core of the panel sends the results to the caller");

	/* This is just for protocol log */
	_D("relay callback is called");
	ret = app_control_get_extra_data_array(result, "http://tizen.org/appcontrol/data/selected", &select, &length);
	if (APP_CONTROL_ERROR_NONE == ret && select) {
		for (i = 0; i < length; i++) {
			_D("selected is %s[%d]", select[i], i);
		}
	}

	ret = app_control_get_extra_data_array(result, "http://tizen.org/appcontrol/data/path", &select, &length);
	if (APP_CONTROL_ERROR_NONE == ret && select) {
		for (i = 0; i < length; i++) {
			_D("path is %s[%d]", select[i], i);
		}
	}

	if (content_info->attach_panel->result_cb) {
		content_info->attach_panel->result_cb(content_info->attach_panel
				, content_info->innate_content_info->content_category
				, result
				, APP_CONTROL_RESULT_SUCCEEDED
				, content_info->attach_panel->result_data);

		if (ATTACH_PANEL_STATE_FULL == _gesture_get_state()) {
			/* This is same with attach_panel_hide() */
			_content_list_set_pause(content_info->attach_panel->content_list, ATTACH_PANEL_CONTENT_CATEGORY_UG);
			_gesture_hide(content_info->attach_panel);
		}
	} else {
		_D("content_info->attach_panel->result_cb is NULL");
	}
}


static void __resize_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
	Evas_Object *ui_manager = obj;

	int x, y, w, h;

	ret_if(!ui_manager);

	evas_object_geometry_get(ui_manager, &x, &y, &w, &h);
	_D("%s resize(%d, %d, %d, %d)", data, x, y, w, h);
}
//LCOV_EXCL_STOP


Evas_Object *_ug_create(const char *appid, const char *operation, const char *selection_mode, const char *mime, bundle *b, content_s *content_info)
{
	app_control_h app_control = NULL;
	ui_gadget_h ui_gadget = NULL;
	Evas_Object *ug = NULL;
	struct ug_cbs cbs = {
		.layout_cb = NULL,
		.result_cb = __result_cb,
		.destroy_cb = NULL,
		.priv = content_info,
	};
	int ret = APP_CONTROL_ERROR_NONE;

	ret = app_control_create(&app_control);
	goto_if(APP_CONTROL_ERROR_NONE != ret, ERROR);

	ret = app_control_set_operation(app_control, operation);
	goto_if(APP_CONTROL_ERROR_NONE != ret, ERROR);

	ret = app_control_add_extra_data(app_control, "__CALLER_PANEL__", "attach-panel");
	goto_if(APP_CONTROL_ERROR_NONE != ret, ERROR);

	ret = app_control_add_extra_data(app_control, APP_CONTROL_DATA_SELECTION_MODE, selection_mode);
	goto_if(APP_CONTROL_ERROR_NONE != ret, ERROR);

	if (mime) {
		ret = app_control_set_mime(app_control, mime);
		goto_if(APP_CONTROL_ERROR_NONE != ret, ERROR);
	}

	if (b) {
		_D("app control add the extra data for ug");
		_bundle_add_to_app_control(b, app_control);
	}

	ui_gadget = ug_create(NULL, appid, UG_MODE_FRAMEVIEW, app_control, &cbs);
	if (!ui_gadget) {
		_E("The UG of %s is failed to launch", appid);
		goto ERROR;
	}

	ug = ug_get_layout(ui_gadget);
	goto_if(!ug, ERROR);

	evas_object_show(ug);
	evas_object_data_set(ug, DATA_KEY_UG, ui_gadget);
	evas_object_event_callback_add(ug, EVAS_CALLBACK_RESIZE, __resize_cb, "ug");

	ret = app_control_destroy(app_control);
	if (APP_CONTROL_ERROR_NONE != ret) {
		_E("cannot destroy app_control");
	}

	return ug;

ERROR:
	if (app_control) app_control_destroy(app_control);

	return NULL;
}



void _ug_destroy(Evas_Object *ug)
{
	ui_gadget_h ui_gadget = NULL;

	ret_if(!ug);

	evas_object_event_callback_del(ug, EVAS_CALLBACK_RESIZE, __resize_cb);
	ui_gadget = evas_object_data_del(ug, DATA_KEY_UG);
	ret_if(!ui_gadget);

	ug_destroy(ui_gadget);
}
