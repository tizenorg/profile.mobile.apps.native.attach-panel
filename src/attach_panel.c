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
#include <ui-gadget-module.h>
#include <isf_control.h>
#include <system_info.h>
#include <tizen.h>
#include <ui-gadget.h>
#include <cynara-client.h>
#include <fcntl.h>

#include "attach_panel.h"
#include "attach_panel_internal.h"
#include "attach_bundle.h"
#include "conf.h"
#include "content_list.h"
#include "gesture.h"
#include "grid.h"
#include "ui_manager.h"
#include "list.h"
#include "log.h"
#include "scroller.h"
#include "toolbar.h"

static const char *const ATTACH_PANEL_DOMAIN = "attach-panel";
static const char *const PLATFORM_FEATURE_CAMERA = "tizen.org/feature/camera";
static const char *const PLATFORM_FEATURE_MICROPHONE = "tizen.org/feature/microphone";



innate_content_s innate_content_info[ATTACH_PANEL_CONTENT_CATEGORY_VIDEO_RECORDER] = {
	{
		.appid = "attach-panel-gallery",
		.name = "IDS_COM_TAB4_IMAGES_ABB2",
		.operation = APP_CONTROL_OPERATION_PICK,
		.selection_mode = "single",
		.tabbar_name = "IDS_COM_TAB4_IMAGES_ABB2",
		.mime = "image/*",
		.content_category = ATTACH_PANEL_CONTENT_CATEGORY_IMAGE,
		.is_ug = ATTACH_PANEL_CONTENT_CATEGORY_UG,
	},
	{
		.appid = "attach-panel-camera",
		.name = "IDS_COM_TAB4_CAMERA",
		.operation = APP_CONTROL_OPERATION_CREATE_CONTENT,
		.selection_mode = "single",
		.tabbar_name = "IDS_COM_TAB4_CAMERA",
		.content_category = ATTACH_PANEL_CONTENT_CATEGORY_CAMERA,
		.is_ug = ATTACH_PANEL_CONTENT_CATEGORY_UG,
	},
	{
		.appid = "attach-panel-voicerecorder",
		.name = "IDS_COM_TAB4_VOICE",
		.operation = APP_CONTROL_OPERATION_CREATE_CONTENT,
		.selection_mode = "single",
		.tabbar_name = "IDS_COM_TAB4_VOICE",
		.content_category = ATTACH_PANEL_CONTENT_CATEGORY_VOICE,
		.is_ug = ATTACH_PANEL_CONTENT_CATEGORY_UG,
	},
	{
		.appid = "videos",
		.name = "IDS_COM_BUTTON2_VIDEOS",
		.operation = APP_CONTROL_OPERATION_PICK,
		.icon = "/usr/share/icons/video.png",
		.selection_mode = "multiple",
		.tabbar_name = "IDS_COM_TAB4_MORE_ABB",
		.mime = "video/*",
		.content_category = ATTACH_PANEL_CONTENT_CATEGORY_VIDEO,
		.is_ug = ATTACH_PANEL_CONTENT_CATEGORY_APP,
	},
	{
		.appid = "audio",
		.name = "IDS_COM_BUTTON2_AUDIO_FILES",
		.operation = APP_CONTROL_OPERATION_PICK,
		.icon = "/usr/share/icons/music.png",
		.selection_mode = "multiple",
		.tabbar_name = "IDS_COM_TAB4_MORE_ABB",
		.mime = "audio/*",
		.content_category = ATTACH_PANEL_CONTENT_CATEGORY_AUDIO,
		.is_ug = ATTACH_PANEL_CONTENT_CATEGORY_APP,
	},
	{
		.appid = "calendar",
		.name = "IDS_EMAIL_BUTTON2_CALENDAR",
		.operation = APP_CONTROL_OPERATION_PICK,
		.icon = "/usr/share/icons/calendar.png",
		.tabbar_name = "IDS_COM_TAB4_MORE_ABB",
		.mode = 1,
		.type = "vcs",
		.selection_mode = "multiple",
		.mime = "application/vnd.tizen.calendar",
		.max = 1,
		.content_category = ATTACH_PANEL_CONTENT_CATEGORY_CALENDAR,
		.is_ug = ATTACH_PANEL_CONTENT_CATEGORY_APP,
	},
	{
		.appid = "contacts",
		.name = "IDS_COM_BUTTON2_CONTACTS",
		.operation = APP_CONTROL_OPERATION_PICK,
		.icon = "/usr/share/icons/contact.png",
		.tabbar_name = "IDS_COM_TAB4_MORE_ABB",
		.type = "vcf",
		.item_type = "person",
		.selection_mode = "multiple",
		.mime = "application/vnd.tizen.contact",
		.content_category = ATTACH_PANEL_CONTENT_CATEGORY_CONTACT,
		.is_ug = ATTACH_PANEL_CONTENT_CATEGORY_APP,
	},
	{
		.appid = "myfiles",
		.name = "IDS_COM_BUTTON_MY_FILES_ABB",
		.operation = APP_CONTROL_OPERATION_PICK,
		.tabbar_name = "IDS_COM_TAB4_MORE_ABB",
		.icon = "/usr/share/icons/my_files.png",
		.selection_mode = "multiple",
		.content_category = ATTACH_PANEL_CONTENT_CATEGORY_MYFILES,
		.is_ug = ATTACH_PANEL_CONTENT_CATEGORY_APP,
	},
	{
		.appid = "record_video",
		.name = "IDS_IDLE_BUTTON2_RECORD_NVIDEO_ABB",
		.operation = APP_CONTROL_OPERATION_CREATE_CONTENT,
		.icon = "/usr/share/icons/default.png",
		.tabbar_name = "IDS_COM_TAB4_MORE_ABB",
		.selection_mode = "single",
		.mime = "video/3gp",
		.content_category = ATTACH_PANEL_CONTENT_CATEGORY_VIDEO_RECORDER,
		.is_ug = ATTACH_PANEL_CONTENT_CATEGORY_APP,
	},
	{
		.appid = "attach-panel-document",
		.name = "IDS_RCS_TAB3_DOCUMENTS_ABB2",
		.operation = APP_CONTROL_OPERATION_CREATE_CONTENT,
		.selection_mode = "single",
		.tabbar_name = "IDS_RCS_TAB3_DOCUMENTS_ABB2",
		.content_category = ATTACH_PANEL_CONTENT_CATEGORY_DOCUMENT,
		.is_ug = ATTACH_PANEL_CONTENT_CATEGORY_UG,
	},
};



static Evas_Object *__attach_panel_rect_add(Evas_Object *conformant, int height)
{
	Evas_Object *rect = NULL;

	retv_if(!conformant, NULL);

	_D("attach panel rect create, height : %d", height);

	rect = evas_object_rectangle_add(evas_object_evas_get(conformant));
	retv_if(!rect, NULL);

	evas_object_size_hint_weight_set(rect, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(rect, EVAS_HINT_FILL, 1.0);
	evas_object_size_hint_min_set(rect, 0, height);
	evas_object_color_set(rect, 0, 0, 0, 0);
	evas_object_show(rect);

	return rect;
}



static void __attach_panel_rect_del(Evas_Object *rect)
{
	ret_if(!rect);
	evas_object_del(rect);
}



static void __rotate_cb(void *data, Evas_Object *obj, void *event)
{
	Evas_Object *win = obj;
	attach_panel_h attach_panel = data;

	int w, h;
	int angle = 0;

	ret_if(!attach_panel);
	ret_if(!attach_panel->conformant);

	angle = elm_win_rotation_get(win);
	elm_win_screen_size_get(win, NULL, NULL, &w, &h);
	_D("Angle is %d degree, win size is %d, %d", angle, w, h);

	switch (angle) {
	case 90:
	case 270:
		_D("resize the attach-panel to land(%d)", angle);
		if (90 == angle)
			ug_send_event(UG_EVENT_ROTATE_LANDSCAPE_UPSIDEDOWN);
		else
			ug_send_event(UG_EVENT_ROTATE_LANDSCAPE);

		w = h;
		attach_panel->rotate = EINA_TRUE;

		if (ATTACH_PANEL_STATE_HIDE == attach_panel->attach_panel_port_state) {
			attach_panel->attach_panel_land_state = ATTACH_PANEL_STATE_HIDE;
		} else if (ATTACH_PANEL_STATE_HALF == attach_panel->attach_panel_port_state) {
			attach_panel->attach_panel_land_state = ATTACH_PANEL_STATE_FULL;
			_gesture_set_state(ATTACH_PANEL_STATE_FULL);
			_content_list_send_message(attach_panel->content_list, APP_CONTROL_DATA_SELECTION_MODE, SELECTION_MODE_MULTIPLE, ATTACH_PANEL_CONTENT_CATEGORY_UG);
			elm_object_signal_emit(attach_panel->conformant, "elm,state,attach_panel,show,full", "");
		} else {
			attach_panel->attach_panel_land_state = ATTACH_PANEL_STATE_FULL;
			_gesture_set_state(ATTACH_PANEL_STATE_FULL);
		}
		break;
	case 0:
	case 180:
		_D("resize the attach-panel to port(%d)", angle);
		if (0 == angle)
			ug_send_event(UG_EVENT_ROTATE_PORTRAIT);
		else
			ug_send_event(UG_EVENT_ROTATE_PORTRAIT_UPSIDEDOWN);

		attach_panel->rotate = EINA_FALSE;

		if (ATTACH_PANEL_STATE_HIDE == attach_panel->attach_panel_land_state) {
			attach_panel->attach_panel_port_state = ATTACH_PANEL_STATE_HIDE;
		} else {
			if (ATTACH_PANEL_STATE_HALF == attach_panel->attach_panel_port_state) {
				_gesture_set_state(ATTACH_PANEL_STATE_HALF);
				_content_list_send_message(attach_panel->content_list, APP_CONTROL_DATA_SELECTION_MODE, SELECTION_MODE_SINGLE, ATTACH_PANEL_CONTENT_CATEGORY_UG);
				elm_object_signal_emit(attach_panel->conformant, "elm,state,attach_panel,show,half", "");
			} else {
				attach_panel->attach_panel_port_state = ATTACH_PANEL_STATE_FULL;
				_gesture_set_state(ATTACH_PANEL_STATE_FULL);
			}
		}
		break;
	default:
		_E("cannot reach here");
		break;
	}

	_scroller_resize(attach_panel->scroller, w, 0);
	elm_scroller_page_show(attach_panel->scroller, attach_panel->cur_page_no, 0);
}



EXPORT_API int attach_panel_create(Evas_Object *conformant, attach_panel_h *attach_panel)
{
	Evas_Object *old_panel = NULL;
	attach_panel_h panel = NULL;
	const char *type = NULL;
	int width = 0;
	int height = 0;
	int base_height = 0;

	retv_if(!attach_panel, ATTACH_PANEL_ERROR_INVALID_PARAMETER);
	retv_if(!conformant, ATTACH_PANEL_ERROR_INVALID_PARAMETER);

	old_panel = elm_object_part_content_get(conformant, "elm.swallow.attach_panel");
	retv_if(old_panel, ATTACH_PANEL_ERROR_ALREADY_EXISTS);

	type = elm_object_widget_type_get(conformant);
	retv_if(!type, ATTACH_PANEL_ERROR_INVALID_PARAMETER);

	if (strcmp(type, "Elm_Conformant")) {
		_E("This is not a conformant, %s", type);
		return ATTACH_PANEL_ERROR_INVALID_PARAMETER;
	}

	bindtextdomain(ATTACH_PANEL_DOMAIN, "/usr/ug/res/locale");

	panel = calloc(1, sizeof(attach_panel_s));
	retv_if(!panel, ATTACH_PANEL_ERROR_OUT_OF_MEMORY);
	panel->conformant = conformant;

	evas_object_geometry_get(conformant, NULL, NULL, &width, &base_height);
	panel->transit_width = width;

	if (width > base_height) {
		base_height = width * BASE_TRANSIT_HEIGHT_REL;
	} else {
		base_height = base_height * BASE_TRANSIT_HEIGHT_REL;
	}

	isf_control_get_recent_ime_geometry(NULL, NULL, NULL, &height);
	if (!height || height < base_height) {
		_E("Fail to get the recent ime height");
		height = base_height;
	}
	panel->transit_height = height;

	panel->attach_panel_rect = __attach_panel_rect_add(conformant, height);
	goto_if(!panel->attach_panel_rect, ERROR);
	elm_object_part_content_set(conformant, "elm.swallow.attach_panel_base", panel->attach_panel_rect);

	panel->ui_manager = _ui_manager_create(panel);
	goto_if(!panel->ui_manager, ERROR);
	elm_object_part_content_set(conformant, "elm.swallow.attach_panel", panel->ui_manager);
	panel->is_delete = EINA_FALSE;
	panel->rotate = EINA_FALSE;
	panel->cur_page_no = 0;
	panel->magic_no = 0x1024;

	panel->gesture = _gesture_create(panel);
	goto_if(!panel->gesture, ERROR);

	evas_object_smart_callback_add(panel->win, "wm,rotation,changed", __rotate_cb, panel);
	elm_scroller_movement_block_set(panel->scroller, ELM_SCROLLER_MOVEMENT_BLOCK_HORIZONTAL);
	__rotate_cb(panel, panel->win, NULL);
	*attach_panel = panel;

	return ATTACH_PANEL_ERROR_NONE;

ERROR:
	if (panel->ui_manager) {
		elm_object_part_content_unset(conformant, "elm.swallow.attach_panel");
		_ui_manager_destroy(panel->ui_manager);
	}

	if (panel->attach_panel_rect) {
		elm_object_part_content_unset(conformant, "elm.swallow.attach_panel_base");
		__attach_panel_rect_del(panel->attach_panel_rect);
	}
	free(panel);

	return ATTACH_PANEL_ERROR_NOT_INITIALIZED;
}



extern void _attach_panel_del(attach_panel_h attach_panel)
{
	content_s *content_info = NULL;
	Eina_List *l = NULL;

	ret_if(!attach_panel);
	_D("%s : attach panel is destroyed", __func__);

	if (0x1024 != attach_panel->magic_no) {
		_D("attach panel is already destroyed");
		return;
	}

	attach_panel->magic_no = 0;

	_gesture_destroy(attach_panel);

	evas_object_smart_callback_del(attach_panel->win, "wm,rotation,changed", __rotate_cb);
	elm_object_part_content_unset(attach_panel->conformant, "elm.swallow.attach_panel_base");
	__attach_panel_rect_del(attach_panel->attach_panel_rect);

	EINA_LIST_FOREACH(attach_panel->content_list, l, content_info) {
		if (content_info->extra_data) {
			bundle_free(content_info->extra_data);
			content_info->extra_data = NULL;
		}
	}

	elm_object_part_content_unset(attach_panel->conformant, "elm.swallow.attach_panel");
	_ui_manager_destroy(attach_panel->ui_manager);

	free(attach_panel);
}



EXPORT_API int attach_panel_destroy(attach_panel_h attach_panel)
{
	retv_if(!attach_panel, ATTACH_PANEL_ERROR_INVALID_PARAMETER);
	_D("%s : attach panel will be destroyed", __func__);

	if (EINA_TRUE == attach_panel->is_delete) {
		_E("Attach panel is already removed");
		return ATTACH_PANEL_ERROR_ALREADY_DESTROYED;
	}

	attach_panel->is_delete = EINA_TRUE;
	if (ATTACH_PANEL_STATE_HIDE == _gesture_get_state()) {
		_attach_panel_del(attach_panel);
	} else {
		attach_panel_hide(attach_panel);
	}

	return ATTACH_PANEL_ERROR_NONE;
}



static void __iter_cb(const char *key, const int type, bundle_keyval_t *kv, void *user_data)
{
	int *bundle_is_op = user_data;

	ret_if(!key);
	ret_if(!bundle_is_op);

	if (!strncmp("__APP_SVC_", key, 10))
		*bundle_is_op = 1;
}



#define SMACK_LABEL_LEN 255
static int __check_privilege(const char *privilege)
{
	cynara *p_cynara;

	int fd = 0;
	int ret = 0;

	char subject_label[SMACK_LABEL_LEN +1] = "";
	char uid[10] = {0, };
	char *client_session = "";

	ret = cynara_initialize(&p_cynara, NULL);
	goto_if(ret != CYNARA_API_SUCCESS, OUT);

	fd = open("/proc/self/attr/current", O_RDONLY);
	goto_if(fd < 0, OUT);

	ret = read(fd, subject_label, SMACK_LABEL_LEN);
	if (ret < 0) {
		_E("read is failed");
		close(fd);
		goto OUT;
	}
	close(fd);

	snprintf(uid, 10, "%d", getuid());

	ret = cynara_check(p_cynara, subject_label, client_session, uid, privilege);
	goto_if(ret != CYNARA_API_ACCESS_ALLOWED, OUT);

	ret = 0;

OUT:
	if (p_cynara)
		cynara_finish(p_cynara);
	return ret;
}



static const char *const PRIVILEGE_CAMERA = "http://tizen.org/privilege/camera";
static const char *const PRIVILEGE_RECORDER = "http://tizen.org/privilege/recorder";
static const char *const PRIVILEGE_APPMANAGER_LAUNCH = "http://tizen.org/privilege/appmanager.launch";
EXPORT_API int attach_panel_add_content_category(attach_panel_h attach_panel, attach_panel_content_category_e content_category, bundle *extra_data)
{
	content_s *content_info = NULL;
	bundle *dup_bundle = NULL;
	Eina_List *l = NULL;
	int ret = SYSTEM_INFO_ERROR_NONE;
	bool value = false;

	retv_if(!attach_panel, ATTACH_PANEL_ERROR_INVALID_PARAMETER);
	retv_if(!attach_panel->ui_manager, ATTACH_PANEL_ERROR_NOT_INITIALIZED);
	retv_if(content_category < ATTACH_PANEL_CONTENT_CATEGORY_IMAGE, ATTACH_PANEL_ERROR_INVALID_PARAMETER);
	retv_if(content_category > ATTACH_PANEL_CONTENT_CATEGORY_VIDEO_RECORDER, ATTACH_PANEL_ERROR_INVALID_PARAMETER);

	if (EINA_TRUE == attach_panel->is_delete) {
		_E("Attach panel is already removed");
		return ATTACH_PANEL_ERROR_ALREADY_DESTROYED;
	}

	switch (content_category) {
	case ATTACH_PANEL_CONTENT_CATEGORY_CAMERA:
		ret = system_info_get_platform_bool(PLATFORM_FEATURE_CAMERA, &value);
		break_if(SYSTEM_INFO_ERROR_NONE != ret);
		retv_if(false == value, ATTACH_PANEL_ERROR_UNSUPPORTED_CONTENT_CATEGORY);

		ret = __check_privilege(PRIVILEGE_CAMERA);
		retv_if(ret < 0, ATTACH_PANEL_ERROR_PERMISSION_DENIED);
		break;
	case ATTACH_PANEL_CONTENT_CATEGORY_VOICE:
		ret = system_info_get_platform_bool(PLATFORM_FEATURE_MICROPHONE, &value);
		break_if(SYSTEM_INFO_ERROR_NONE != ret);
		retv_if(false == value, ATTACH_PANEL_ERROR_UNSUPPORTED_CONTENT_CATEGORY);

		ret = __check_privilege(PRIVILEGE_RECORDER);
		retv_if(ret < 0, ATTACH_PANEL_ERROR_PERMISSION_DENIED);
		break;
	case ATTACH_PANEL_CONTENT_CATEGORY_VIDEO:
	case ATTACH_PANEL_CONTENT_CATEGORY_AUDIO:
	case ATTACH_PANEL_CONTENT_CATEGORY_CALENDAR:
	case ATTACH_PANEL_CONTENT_CATEGORY_CONTACT:
	case ATTACH_PANEL_CONTENT_CATEGORY_MYFILES:
		ret = __check_privilege(PRIVILEGE_APPMANAGER_LAUNCH);
		retv_if(ret < 0, ATTACH_PANEL_ERROR_PERMISSION_DENIED);
		break;
	default:
		break;
	}

	EINA_LIST_FOREACH(attach_panel->content_list, l, content_info) {
		if (content_info->innate_content_info->content_category == content_category) {
			_E("The content category[%d] already exists in the panel", content_category);
			return ATTACH_PANEL_ERROR_ALREADY_EXISTS;
		}
	}

	if (extra_data) {
		int bundle_is_op = 0;

		bundle_foreach(extra_data, (void *) __iter_cb, &bundle_is_op);

		if (bundle_is_op) {
			_E("bundle key is invalid value");
			return ATTACH_PANEL_ERROR_INVALID_PARAMETER;
		}

		dup_bundle = bundle_dup(extra_data);
		retv_if(!dup_bundle, ATTACH_PANEL_ERROR_INVALID_PARAMETER);
	}

	_ui_manager_append_content_category(attach_panel->ui_manager, &innate_content_info[content_category-1], dup_bundle);

	return ATTACH_PANEL_ERROR_NONE;
}



EXPORT_API int attach_panel_remove_content_category(attach_panel_h attach_panel, attach_panel_content_category_e content_category)
{
	content_s *content_info = NULL;
	Eina_List *l = NULL;

	retv_if(!attach_panel, ATTACH_PANEL_ERROR_INVALID_PARAMETER);
	retv_if(!attach_panel->ui_manager, ATTACH_PANEL_ERROR_NOT_INITIALIZED);
	retv_if(content_category < ATTACH_PANEL_CONTENT_CATEGORY_IMAGE, ATTACH_PANEL_ERROR_INVALID_PARAMETER);
	retv_if(content_category > ATTACH_PANEL_CONTENT_CATEGORY_VIDEO_RECORDER, ATTACH_PANEL_ERROR_INVALID_PARAMETER);

	if (EINA_TRUE == attach_panel->is_delete) {
		_E("Attach panel is already removed");
		return ATTACH_PANEL_ERROR_ALREADY_DESTROYED;
	}

	EINA_LIST_FOREACH(attach_panel->content_list, l, content_info) {
		if (content_info->innate_content_info->content_category != content_category) {
			continue;
		}
		bundle_free(content_info->extra_data);
		content_info->extra_data = NULL;
		_ui_manager_remove_content_category(attach_panel->ui_manager, content_info);
		break;
	}

	return ATTACH_PANEL_ERROR_NONE;
}



EXPORT_API int attach_panel_set_extra_data(attach_panel_h attach_panel, attach_panel_content_category_e content_category, bundle *extra_data)
{
	app_control_h app_control = NULL;
	ui_gadget_h ui_gadget = NULL;
	content_s *content_info = NULL;
	Eina_List *l = NULL;

	int ret = ATTACH_PANEL_ERROR_NONE;
	int bundle_is_op = 0;

	retv_if(!attach_panel, ATTACH_PANEL_ERROR_INVALID_PARAMETER);
	retv_if(!attach_panel->ui_manager, ATTACH_PANEL_ERROR_NOT_INITIALIZED);
	retv_if(content_category < ATTACH_PANEL_CONTENT_CATEGORY_IMAGE, ATTACH_PANEL_ERROR_INVALID_PARAMETER);
	retv_if(content_category > ATTACH_PANEL_CONTENT_CATEGORY_VIDEO_RECORDER, ATTACH_PANEL_ERROR_INVALID_PARAMETER);
	retv_if(!extra_data, ATTACH_PANEL_ERROR_INVALID_PARAMETER);

	bundle_foreach(extra_data, (void *) __iter_cb, &bundle_is_op);

	if (bundle_is_op) {
		_E("bundle key is invalid value");
		return ATTACH_PANEL_ERROR_INVALID_PARAMETER;
	}

	EINA_LIST_FOREACH(attach_panel->content_list, l, content_info) {
		if (content_info->innate_content_info->content_category == content_category) {
			_D("add the data in %d content category", content_category);
			break;
		}
	}

	if (content_info->extra_data) {
		ret = _bundle_add_to_bundle(content_info->extra_data, extra_data);
		retv_if(ret != ATTACH_PANEL_ERROR_NONE, ATTACH_PANEL_ERROR_OUT_OF_MEMORY);
	} else {
		bundle *dup_bundle = NULL;

		dup_bundle = bundle_dup(extra_data);
		retv_if(!dup_bundle, ATTACH_PANEL_ERROR_OUT_OF_MEMORY);

		content_info->extra_data = dup_bundle;
	}

	if (innate_content_info[content_category-1].is_ug) {
		if (!content_info->content) {
			_D("Create ug because ug is not created yet");
			content_info->content = _ui_manager_create_content(content_info->page, content_info, attach_panel);
			retv_if(!content_info->content, ATTACH_PANEL_ERROR_OUT_OF_MEMORY);
			if (ATTACH_PANEL_STATE_FULL == _gesture_get_state()) {
				_content_list_send_message(attach_panel->content_list, APP_CONTROL_DATA_SELECTION_MODE, SELECTION_MODE_MULTIPLE, ATTACH_PANEL_CONTENT_CATEGORY_UG);
			} else {
				_content_list_send_message(attach_panel->content_list, APP_CONTROL_DATA_SELECTION_MODE, SELECTION_MODE_SINGLE, ATTACH_PANEL_CONTENT_CATEGORY_UG);
			}
		}

		ret = app_control_create(&app_control);
		retv_if(APP_CONTROL_ERROR_NONE != ret, ATTACH_PANEL_ERROR_OUT_OF_MEMORY);

		ui_gadget = evas_object_data_get(content_info->content, DATA_KEY_UG);
		if (!ui_gadget) {
			_E("Fail to get the ui gadget from content_info");
			app_control_destroy(app_control);
			return ATTACH_PANEL_ERROR_OUT_OF_MEMORY;

		}
		_bundle_add_to_app_control(content_info->extra_data, app_control);
		ug_send_message(ui_gadget, app_control);
		ret = app_control_destroy(app_control);
		retv_if(APP_CONTROL_ERROR_NONE != ret, ATTACH_PANEL_ERROR_OUT_OF_MEMORY);
	} else {
		// In case of app, bundle is not used imediately.
		// when app is launching, bundle is used.
	}

	return ATTACH_PANEL_ERROR_NONE;
}



EXPORT_API int attach_panel_set_result_cb(attach_panel_h attach_panel, attach_panel_result_cb result_cb, void *user_data)
{
	retv_if(!attach_panel, ATTACH_PANEL_ERROR_INVALID_PARAMETER);
	retv_if(!result_cb, ATTACH_PANEL_ERROR_INVALID_PARAMETER);

	if (EINA_TRUE == attach_panel->is_delete) {
		_E("Attach panel is already removed");
		return ATTACH_PANEL_ERROR_ALREADY_DESTROYED;
	}

	attach_panel->result_cb = result_cb;
	attach_panel->result_data = user_data;

	return ATTACH_PANEL_ERROR_NONE;
}



EXPORT_API int attach_panel_unset_result_cb(attach_panel_h attach_panel)
{
	retv_if(!attach_panel, ATTACH_PANEL_ERROR_INVALID_PARAMETER);

	if (EINA_TRUE == attach_panel->is_delete) {
		_E("Attach panel is already removed");
		return ATTACH_PANEL_ERROR_ALREADY_DESTROYED;
	}

	attach_panel->result_cb = NULL;
	attach_panel->result_data = NULL;

	return ATTACH_PANEL_ERROR_NONE;
}



EXPORT_API int attach_panel_set_event_cb(attach_panel_h attach_panel, attach_panel_event_cb event_cb, void *user_data)
{
	retv_if(!attach_panel, ATTACH_PANEL_ERROR_INVALID_PARAMETER);
	retv_if(!event_cb, ATTACH_PANEL_ERROR_INVALID_PARAMETER);

	if (EINA_TRUE == attach_panel->is_delete) {
		_E("Attach panel is already removed");
		return ATTACH_PANEL_ERROR_ALREADY_DESTROYED;
	}

	attach_panel->event_cb = event_cb;
	attach_panel->event_data = user_data;

	return ATTACH_PANEL_ERROR_NONE;
}



EXPORT_API int attach_panel_unset_event_cb(attach_panel_h attach_panel)
{
	retv_if(!attach_panel, ATTACH_PANEL_ERROR_INVALID_PARAMETER);

	if (EINA_TRUE == attach_panel->is_delete) {
		_E("Attach panel is already removed");
		return ATTACH_PANEL_ERROR_ALREADY_DESTROYED;
	}

	attach_panel->event_cb = NULL;
	attach_panel->event_data = NULL;

	return ATTACH_PANEL_ERROR_NONE;
}



EXPORT_API int attach_panel_show(attach_panel_h attach_panel)
{
	Elm_Object_Item *first_it;
	content_s *content_info = NULL;
	int height = 0;

	retv_if(!attach_panel, ATTACH_PANEL_ERROR_INVALID_PARAMETER);
	retv_if(!attach_panel->ui_manager, ATTACH_PANEL_ERROR_INVALID_PARAMETER);

	if (EINA_TRUE == attach_panel->is_delete) {
		_E("Attach panel is already removed");
		return ATTACH_PANEL_ERROR_ALREADY_DESTROYED;
	}

	if (ATTACH_PANEL_STATE_HIDE != _gesture_get_state()) {
		_D("Attach panel is already shown");
		return ATTACH_PANEL_ERROR_NONE;
	}

	isf_control_get_recent_ime_geometry(NULL, NULL, NULL, &height);
	if (height && height > attach_panel->transit_height) {
		_D("change the attach-panel's height(%d)", height);
		evas_object_size_hint_min_set(attach_panel->attach_panel_rect, 0, height);
		attach_panel->transit_height = height;
	}

	content_info = eina_list_nth(attach_panel->content_list, attach_panel->cur_page_no);
	retv_if(!content_info, ATTACH_PANEL_ERROR_OUT_OF_MEMORY);

	_toolbar_bring_in(attach_panel->toolbar, content_info->tabbar_item);

	if (content_info->content) {
		elm_scroller_page_show(attach_panel->scroller, attach_panel->cur_page_no, 0);
	} else {
		content_info->content = _ui_manager_create_content(content_info->page, content_info, attach_panel);
		retv_if(!content_info->content, ATTACH_PANEL_ERROR_OUT_OF_MEMORY);
	}

	_content_list_set_resume(attach_panel->content_list, ATTACH_PANEL_CONTENT_CATEGORY_UG);
	_content_list_send_message(attach_panel->content_list, "__ATTACH_PANEL_INITIALIZE__", MODE_ENABLE, ATTACH_PANEL_CONTENT_CATEGORY_UG);
	evas_object_show(attach_panel->ui_manager);

	_gesture_show(attach_panel);

	attach_panel->content_list = _list_sort_by_rua(attach_panel->content_list);
	_grid_refresh(attach_panel->grid);

	first_it = elm_gengrid_first_item_get(attach_panel->grid);
	if (first_it) {
		elm_gengrid_item_show(first_it, ELM_GENGRID_ITEM_SCROLLTO_TOP);
	}

	return ATTACH_PANEL_ERROR_NONE;
}



EXPORT_API int attach_panel_hide(attach_panel_h attach_panel)
{
	retv_if(!attach_panel, ATTACH_PANEL_ERROR_INVALID_PARAMETER);
	retv_if(!attach_panel->ui_manager, ATTACH_PANEL_ERROR_INVALID_PARAMETER);

	if (EINA_TRUE == attach_panel->is_delete) {
		_E("Attach panel is already removed");
		return ATTACH_PANEL_ERROR_ALREADY_DESTROYED;
	}

	_content_list_set_pause(attach_panel->content_list, ATTACH_PANEL_CONTENT_CATEGORY_UG);
	_gesture_hide(attach_panel);

	return ATTACH_PANEL_ERROR_NONE;
}



EXPORT_API int attach_panel_get_visibility(attach_panel_h attach_panel, bool *visible)
{
	retv_if(!attach_panel, ATTACH_PANEL_ERROR_INVALID_PARAMETER);
	retv_if(!visible, ATTACH_PANEL_ERROR_INVALID_PARAMETER);

	if (EINA_TRUE == attach_panel->is_delete) {
		_E("Attach panel is already removed");
		return ATTACH_PANEL_ERROR_ALREADY_DESTROYED;
	}

	if (attach_panel->rotate) {
		if (ATTACH_PANEL_STATE_HIDE == attach_panel->attach_panel_land_state) {
			*visible = false;
		} else {
			*visible = true;
		}
	} else {
		if (ATTACH_PANEL_STATE_HIDE == attach_panel->attach_panel_port_state) {
			*visible = false;
		} else {
			*visible = true;
		}
	}

	return ATTACH_PANEL_ERROR_NONE;
}
