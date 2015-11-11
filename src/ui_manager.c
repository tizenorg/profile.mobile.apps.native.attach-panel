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

#include <Elementary.h>
#include <bundle.h>
#include <ui-gadget.h>
#include <ui-gadget-module.h>

#include "attach_panel.h"
#include "attach_panel_internal.h"
#include "conf.h"
#include "content_list.h"
#include "gesture.h"
#include "log.h"
#include "page.h"
#include "scroller.h"
#include "toolbar.h"
#include "ug.h"
#include "grid.h"



static const char *const FILE_LAYOUT_EDJ = EDJEDIR"/layout.edj";
static const char *const GROUP_LAYOUT = "layout";



static Evas_Object *__create_content(Evas_Object *page, content_s *content_info, attach_panel_h attach_panel)
{
	Evas_Object *content = NULL;
	Elm_Object_Item *item = NULL;

	retv_if(!page, NULL);
	retv_if(!content_info, NULL);
	retv_if(!attach_panel, NULL);

	switch (content_info->innate_content_info->content_category) {
	case ATTACH_PANEL_CONTENT_CATEGORY_IMAGE:
	case ATTACH_PANEL_CONTENT_CATEGORY_CAMERA:
	case ATTACH_PANEL_CONTENT_CATEGORY_VOICE:
		content = _ug_create(content_info->innate_content_info->appid
						, content_info->innate_content_info->operation
						, content_info->innate_content_info->selection_mode
						, content_info->innate_content_info->mime
						, content_info->extra_data
						, content_info);
		evas_object_show(content);
		content_info->grid_item = NULL;
		break;
	default:
		/* This function can return NULL, if there is already a grid content */
		if (attach_panel->grid) {
			content = attach_panel->grid;
		} else {
			content = _grid_create(page, attach_panel);
			attach_panel->grid = content;
		}

		item = _grid_append_item(attach_panel->grid, content_info);
		break_if(!item);

		break;
	}

	evas_object_size_hint_weight_set(content, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

	evas_object_data_set(content, DATA_KEY_PAGE, page);

	return content;
}



static void __destroy_content(content_s *content_info, attach_panel_h attach_panel)
{
	ret_if(!content_info->content);
	ret_if(!attach_panel);

	evas_object_data_del(content_info->content, DATA_KEY_PAGE);

	switch (content_info->innate_content_info->content_category) {
	case ATTACH_PANEL_CONTENT_CATEGORY_IMAGE:
	case ATTACH_PANEL_CONTENT_CATEGORY_CAMERA:
	case ATTACH_PANEL_CONTENT_CATEGORY_VOICE:
		_ug_destroy(content_info->content);
		break;
	default:
		_grid_remove_item(attach_panel->grid, content_info);

		if (!_grid_count_item(attach_panel->grid)) {
			_grid_destroy(attach_panel->grid);
			attach_panel->grid = NULL;
		}

		break;
	}
}



int _ui_manager_append_content_category(Evas_Object *ui_manager, innate_content_s *innate_content_info, bundle *extra_data)
{
	attach_panel_h attach_panel = NULL;
	content_s *content_info = NULL;
	Evas_Object *page = NULL;

	retv_if(!ui_manager, ATTACH_PANEL_ERROR_INVALID_PARAMETER);
	retv_if(!innate_content_info, ATTACH_PANEL_ERROR_INVALID_PARAMETER);

	attach_panel = evas_object_data_get(ui_manager, DATA_KEY_ATTACH_PANEL_INFO);
	retv_if(!attach_panel, ATTACH_PANEL_ERROR_INVALID_PARAMETER);

	content_info = calloc(1, sizeof(content_s));
	retv_if(!content_info, ATTACH_PANEL_ERROR_OUT_OF_MEMORY);
	content_info->attach_panel = attach_panel;
	content_info->innate_content_info = innate_content_info;
	content_info->extra_data = extra_data;

	if (innate_content_info->is_ug) {
		/* UG side */
		page = _page_create(attach_panel, attach_panel->transit_width, 0);
		goto_if(!page, ERROR);
		_scroller_append_page(attach_panel->scroller, page);

		content_info->tabbar_item =
			_toolbar_append_item(attach_panel->toolbar, innate_content_info->name, page);
		goto_if(!content_info->tabbar_item, ERROR);
	} else {
		/* App side */
		if (attach_panel->grid) {
			/* Grid */
			page = attach_panel->grid_page;
			content_info->tabbar_item = attach_panel->grid_tabbar_item;
		} else {
			/* No grid */
			Evas_Object *page_bg = NULL;
			page = _page_create(attach_panel, attach_panel->transit_width, 0);
			page_bg = elm_object_part_content_get(page, "bg");
			if (page_bg) {
				evas_object_color_set(page_bg, 250, 250, 250, 255);
			}
			attach_panel->grid_page = page;
			goto_if(!page, ERROR);
			_scroller_append_page(attach_panel->scroller, page);

			content_info->tabbar_item = _toolbar_append_item(attach_panel->toolbar, content_info->innate_content_info->tabbar_name, page);
			attach_panel->grid_tabbar_item = content_info->tabbar_item;
			goto_if(!content_info->tabbar_item, ERROR);
		}
	}

	content_info->content = __create_content(page, content_info, attach_panel);
	goto_if(!content_info->content, ERROR);
	elm_object_part_content_set(page, "content", content_info->content);

	if (1 == _toolbar_count_item(attach_panel->toolbar)) {
		_toolbar_bring_in(attach_panel->toolbar, content_info->tabbar_item);
	}

	attach_panel->content_list = eina_list_append(attach_panel->content_list, content_info);
	content_info->index = eina_list_count(attach_panel->content_list) - 1;

	return ATTACH_PANEL_ERROR_NONE;

ERROR:
	if (content_info->tabbar_item) _toolbar_remove_item(attach_panel->toolbar, content_info->tabbar_item);
	if (content_info->content) __destroy_content(content_info, attach_panel);
	if (page) _page_destroy(page);
	free(content_info);

	return ATTACH_PANEL_ERROR_NOT_INITIALIZED;
}



void _ui_manager_remove_content_category(Evas_Object *ui_manager, content_s *content_info)
{
	attach_panel_h attach_panel = NULL;

	ret_if(!ui_manager);
	ret_if(!content_info);
	ret_if(!content_info->content);

	attach_panel = evas_object_data_get(ui_manager, DATA_KEY_ATTACH_PANEL_INFO);
	ret_if(!attach_panel);

	__destroy_content(content_info, attach_panel);

	if (content_info->innate_content_info->is_ug
		|| !attach_panel->grid) {
		Evas_Object *page = evas_object_data_get(content_info->content, DATA_KEY_PAGE);
		if (page) {
			_page_destroy(page);
		}
		_toolbar_remove_item(attach_panel->toolbar, content_info->tabbar_item);
	}

	attach_panel->content_list = eina_list_remove(attach_panel->content_list, content_info);
	free(content_info);
}



static void __remove_content_categories(Evas_Object *ui_manager)
{
	attach_panel_h attach_panel = NULL;
	content_s *content_info = NULL;

	ret_if(!ui_manager);

	attach_panel = evas_object_data_get(ui_manager, DATA_KEY_ATTACH_PANEL_INFO);
	ret_if(!attach_panel);

	EINA_LIST_FREE(attach_panel->content_list, content_info) {
		Evas_Object *page = NULL;

		__destroy_content(content_info, attach_panel);
		page= evas_object_data_get(content_info->content, DATA_KEY_PAGE);
		if (page) {
			elm_object_part_content_unset(page, "content");
			_page_destroy(page);
		}
		_toolbar_remove_item(attach_panel->toolbar, content_info->tabbar_item);
		free(content_info);
	}
}



static void __resize_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
	Evas_Object *ui_manager = obj;
	Evas_Coord x = 0;
	Evas_Coord y = 0;
	Evas_Coord w = 0;
	Evas_Coord h = 0;

	ret_if(!ui_manager);

	evas_object_geometry_get(ui_manager, &x, &y, &w, &h);
	_D("ui_manager resize(%d, %d, %d, %d)", x, y, w, h);
}



static void _change_tab_cb(Evas_Object *toolbar, int event_type, void *event_info, void *data)
{
	attach_panel_h attach_panel = data;
	Evas_Object *page = event_info;

	ret_if(!attach_panel);
	ret_if(!attach_panel->toolbar);
	ret_if(!attach_panel->scroller);
	ret_if(!page);

	_scroller_bring_in_page(attach_panel->scroller, page, &attach_panel->current_page);
}



static void _scroll_cb(Evas_Object *scroller, int event_type, void *event_info, void *data)
{
	attach_panel_h attach_panel = data;
	Elm_Object_Item *tabbar_item = event_info;

	ret_if(!attach_panel);
	ret_if(!attach_panel->toolbar);
	ret_if(!tabbar_item);

	_toolbar_bring_in(attach_panel->toolbar, tabbar_item);
}



static void __window_resume_cb(void *data, Evas_Object *scroller, void *event_info)
{
	attach_panel_h attach_panel = data;
	ret_if(!attach_panel);

	if (ATTACH_PANEL_STATE_HIDE == _gesture_get_state()) {
		_D("attach panel is already hide");
		return;
	}

	_D("caller window is resumed");
	//_content_list_set_resume(attach_panel->content_list, ATTACH_PANEL_CONTENT_CATEGORY_UG);
}



static void __window_pause_cb(void *data, Evas_Object *scroller, void *event_info)
{
	attach_panel_h attach_panel = data;
	ret_if(!attach_panel);

	if (ATTACH_PANEL_STATE_HIDE == _gesture_get_state()) {
		_D("attach panel is already hide");
		return;
	}

	_D("caller window is paused");
	//_content_list_set_pause(attach_panel->content_list, ATTACH_PANEL_CONTENT_CATEGORY_UG);
}



static void __lang_changed_cb(void *data, Evas_Object *obj, void *event_info)
{
	_D("language changed");
	ug_send_event(UG_EVENT_LANG_CHANGE);
}



static void __keypad_on_cb(void *data, Evas_Object *obj, void *event_info)
{
	attach_panel_h attach_panel = data;
	ret_if(!attach_panel);

	_D("keypad state on");

	/* This is same with attach_panel_hide */
	//_content_list_set_pause(attach_panel->content_list, ATTACH_PANEL_CONTENT_CATEGORY_UG);
	_gesture_hide(attach_panel);
}



Evas_Object *_ui_manager_create(attach_panel_h attach_panel)
{
	Evas_Object *ui_manager = NULL;
	Evas_Object *parent = NULL;
	int ret = ATTACH_PANEL_ERROR_NONE;

	retv_if(!attach_panel, NULL);
	retv_if(!attach_panel->conformant, NULL);

	ui_manager = elm_layout_add(attach_panel->conformant);
	retv_if(!ui_manager, NULL);
	elm_layout_file_set(ui_manager, FILE_LAYOUT_EDJ, GROUP_LAYOUT);
	evas_object_size_hint_weight_set(ui_manager, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(ui_manager, EVAS_HINT_FILL, 1.0);
	evas_object_show(ui_manager);

	evas_object_data_set(ui_manager, DATA_KEY_ATTACH_PANEL_INFO, attach_panel);
	evas_object_event_callback_add(ui_manager, EVAS_CALLBACK_RESIZE, __resize_cb, NULL);

	attach_panel->toolbar = _toolbar_create(ui_manager, attach_panel);
	goto_if(!attach_panel->toolbar, ERROR);
	elm_object_part_content_set(ui_manager, "toolbar", attach_panel->toolbar);

	ret = _toolbar_register_event_cb(attach_panel->toolbar, TOOLBAR_EVENT_TYPE_CHANGE_TAB, _change_tab_cb, attach_panel);
	goto_if(ATTACH_PANEL_ERROR_NONE != ret, ERROR);

	attach_panel->scroller = _scroller_create(ui_manager, attach_panel);
	goto_if(!attach_panel->scroller, ERROR);
	elm_object_part_content_set(ui_manager, "scroller", attach_panel->scroller);

	ret = _scroller_register_event_cb(attach_panel->scroller, SCROLLER_EVENT_TYPE_SCROLL, _scroll_cb, attach_panel);
	goto_if(ATTACH_PANEL_ERROR_NONE != ret, ERROR);

	parent = attach_panel->conformant;
	do {
		const char *type = NULL;

		parent = elm_object_parent_widget_get(parent);
		break_if(!parent);

		type = elm_object_widget_type_get(parent);
		continue_if(!type);
		if (!strcmp(type, "Elm_Win")) {
			UG_INIT_EFL(parent, UG_OPT_INDICATOR_DISABLE);
			attach_panel->win = parent;
			break;
		}
	} while (parent);

	evas_object_smart_callback_add(parent, "normal", __window_resume_cb, attach_panel);
	evas_object_smart_callback_add(parent, "iconified", __window_pause_cb, attach_panel);
	evas_object_smart_callback_add(attach_panel->conformant, "language,changed", __lang_changed_cb, NULL);
	evas_object_smart_callback_add(attach_panel->conformant, "virtualkeypad,state,on", __keypad_on_cb, attach_panel);

	return ui_manager;

ERROR:
	if (attach_panel->toolbar) {
		_toolbar_destroy(attach_panel->toolbar);
	}

	if (ui_manager) {
		evas_object_del(ui_manager);
	}

	return NULL;
}



void _ui_manager_destroy(Evas_Object *ui_manager)
{
	attach_panel_h attach_panel = NULL;
	int ret = ATTACH_PANEL_ERROR_NONE;

	__remove_content_categories(ui_manager);

	/* attach_panel_h is used in the __remove_content_categories */
	attach_panel = evas_object_data_del(ui_manager, DATA_KEY_ATTACH_PANEL_INFO);
	ret_if(!attach_panel);

	evas_object_smart_callback_del(attach_panel->conformant, "virtualkeypad,state,on", __keypad_on_cb);
	evas_object_smart_callback_del(attach_panel->conformant, "language,changed", __lang_changed_cb);
	evas_object_event_callback_del(ui_manager, EVAS_CALLBACK_RESIZE, __resize_cb);
	if (attach_panel->win) {
		evas_object_smart_callback_del(attach_panel->win, "normal", __window_resume_cb);
		evas_object_smart_callback_del(attach_panel->win, "iconified", __window_pause_cb);
	}

	if (attach_panel->scroller) {
		ret = _scroller_unregister_event_cb(attach_panel->scroller, SCROLLER_EVENT_TYPE_SCROLL, _scroll_cb);
		if (ATTACH_PANEL_ERROR_NONE != ret) {
			_E("cannot unregiter event_cb for scroller");
		}
		elm_object_part_content_unset(ui_manager, "scroller");
		_scroller_destroy(attach_panel->scroller);
	}

	if (attach_panel->toolbar) {
		ret = _toolbar_unregister_event_cb(attach_panel->toolbar, TOOLBAR_EVENT_TYPE_CHANGE_TAB, _change_tab_cb);
		if (ATTACH_PANEL_ERROR_NONE != ret) {
			_E("cannot unregiter event_cb for toolbar");
		}
		elm_object_part_content_unset(ui_manager, "toolbar");
		_toolbar_destroy(attach_panel->toolbar);
	}

	if (attach_panel->ui_manager) {
		evas_object_del(ui_manager);
	}
}
