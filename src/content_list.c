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
#include <ui-gadget.h>

#include "attach_panel.h"
#include "attach_panel_internal.h"
#include "conf.h"
#include "content_list.h"
#include "log.h"



void _content_list_send_message(Eina_List *list, const char *key, const char *value, int is_ug)
{
	content_s *content_info = NULL;
	app_control_h app_control = NULL;
	const Eina_List *l = NULL;

	int ret = APP_CONTROL_ERROR_NONE;

	ret_if(!list);
	ret_if(!key);
	ret_if(!value);

	ret = app_control_create(&app_control);
	ret_if(APP_CONTROL_ERROR_NONE != ret);

	ret = app_control_add_extra_data(app_control, "__CALLER_PANEL__", "attach-panel");
	if (APP_CONTROL_ERROR_NONE != ret) {
		_E("Fail to add extra data(__CALLER_PANEL__)");
		app_control_destroy(app_control);
		return;
	}

	ret = app_control_add_extra_data(app_control, key, value);
	if (APP_CONTROL_ERROR_NONE != ret) {
		_E("Fail to add extra data(%s)", key);
		app_control_destroy(app_control);
		return;
	}

	EINA_LIST_FOREACH(list, l, content_info) {
		continue_if(!content_info);
		innate_content_s *innate_content_info = content_info->innate_content_info;
		continue_if(!innate_content_info);

		if (is_ug == innate_content_info->is_ug) {
			ui_gadget_h ui_gadget = NULL;

			ui_gadget = evas_object_data_get(content_info->content, DATA_KEY_UG);
			if (!ui_gadget) {
				_E("Fail to get the ui gadget");
				app_control_destroy(app_control);
				return;
			}
			ug_send_message(ui_gadget, app_control);
		}
	}

	ret = app_control_destroy(app_control);
	ret_if(APP_CONTROL_ERROR_NONE != ret);
}



void _content_list_send_message_to_content(Eina_List *list, const char *key, const char *value, int show_page)
{
	content_s *content_info = NULL;
	app_control_h app_control = NULL;
	ui_gadget_h ui_gadget = NULL;

	int ret = APP_CONTROL_ERROR_NONE;

	ret_if(!list);
	ret_if(!key);
	ret_if(!value);

	content_info = eina_list_nth(list, show_page);
	ret_if(!content_info);

	if (!content_info->innate_content_info->is_ug) {
		_D("this page did not need to send message");
		return;
	}

	ret = app_control_create(&app_control);
	ret_if(APP_CONTROL_ERROR_NONE != ret);

	ret = app_control_add_extra_data(app_control, "__CALLER_PANEL__", "attach-panel");
	if (APP_CONTROL_ERROR_NONE != ret) {
		_E("Fail to add extra data(__CALLER_PANEL__)");
		app_control_destroy(app_control);
		return;
	}

	ret = app_control_add_extra_data(app_control, key, value);
	if (APP_CONTROL_ERROR_NONE != ret) {
		_E("Fail to add extra data(%s)", key);
		app_control_destroy(app_control);
		return;
	}

	ui_gadget = evas_object_data_get(content_info->content, DATA_KEY_UG);
	if (!ui_gadget) {
		_E("Fail to get the ui gadget");
		app_control_destroy(app_control);
		return;
	}
	ug_send_message(ui_gadget, app_control);

	ret = app_control_destroy(app_control);
	ret_if(APP_CONTROL_ERROR_NONE != ret);
}



void _content_list_set_pause(Eina_List *list, int is_ug)
{
	content_s *content_info = NULL;
	const Eina_List *l = NULL;

	ret_if(!list);

	EINA_LIST_FOREACH(list, l, content_info) {
		continue_if(!content_info);
		innate_content_s *innate_content_info = content_info->innate_content_info;
		continue_if(!innate_content_info);
		if (is_ug == innate_content_info->is_ug) {
			ui_gadget_h ui_gadget = NULL;

			ui_gadget = evas_object_data_get(content_info->content, DATA_KEY_UG);
			if (!ui_gadget) {
				_E("Fail to get the ui gadget");
				return;
			}
			//ug_pause_ug(ui_gadget);
		}
	}
}



void _content_list_set_resume(Eina_List *list, int is_ug)
{
	content_s *content_info = NULL;
	const Eina_List *l = NULL;

	ret_if(!list);

	EINA_LIST_FOREACH(list, l, content_info) {
		continue_if(!content_info);
		innate_content_s *innate_content_info = content_info->innate_content_info;
		continue_if(!innate_content_info);
		if (is_ug == innate_content_info->is_ug) {
			ui_gadget_h ui_gadget = NULL;

			ui_gadget = evas_object_data_get(content_info->content, DATA_KEY_UG);
			if (!ui_gadget) {
				_E("Fail to get the ui gadget");
				return;
			}
			//ug_resume_ug(ui_gadget);
		}
	}
}
