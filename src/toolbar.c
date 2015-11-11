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

#include "attach_panel.h"
#include "attach_panel_internal.h"
#include "conf.h"
#include "log.h"
#include "toolbar.h"



static const char *const PRIVATE_DATA_KEY_EVENT_CALLBACK_LIST = "pdkec";



struct _event_cb {
	int event_type;
	void (*event_cb)(Evas_Object *toolbar, int event_type, void *event_info, void *user_data);
	void *user_data;
};
typedef struct _event_cb event_cb_s;



static void __resize_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
	Evas_Object *toolbar = obj;

	int x, y, w, h;

	ret_if(!toolbar);

	evas_object_geometry_get(toolbar, &x, &y, &w, &h);
	_D("toolbar resize(%d, %d, %d, %d)", x, y, w, h);
}



Evas_Object *_toolbar_create(Evas_Object *ui_manager, attach_panel_h attach_panel)
{
	Evas_Object *toolbar = NULL;

	retv_if(!ui_manager, NULL);
	retv_if(!attach_panel, NULL);

	toolbar = elm_toolbar_add(ui_manager);
	goto_if(!toolbar, ERROR);

	/* This will expand the transverse(horizontal) length of items according to the length of toolbar */
	elm_toolbar_transverse_expanded_set(toolbar, EINA_TRUE);
	elm_toolbar_shrink_mode_set(toolbar, ELM_TOOLBAR_SHRINK_EXPAND);
	elm_toolbar_homogeneous_set(toolbar, EINA_FALSE);
	elm_toolbar_select_mode_set(toolbar, ELM_OBJECT_SELECT_MODE_ALWAYS);
	elm_object_style_set(toolbar, "light_view");
	elm_object_focus_set(toolbar, EINA_TRUE);

	evas_object_show(toolbar);
	evas_object_data_set(toolbar, DATA_KEY_ATTACH_PANEL_INFO, attach_panel);

	evas_object_event_callback_add(toolbar, EVAS_CALLBACK_RESIZE, __resize_cb, NULL);

	return toolbar;

ERROR:
	_toolbar_destroy(toolbar);
	return NULL;
}



void _toolbar_destroy(Evas_Object *toolbar)
{
	attach_panel_h attach_panel = NULL;

	ret_if(!toolbar);

	attach_panel = evas_object_data_del(toolbar, DATA_KEY_ATTACH_PANEL_INFO);
	ret_if(!attach_panel);

	evas_object_event_callback_del(toolbar, EVAS_CALLBACK_RESIZE, __resize_cb);
	evas_object_del(toolbar);
}



static void __view_changed_cb(void *user_data, Evas_Object *obj, void *event_info)
{
	Evas_Object *toolbar = obj;
	Evas_Object *page = user_data;
	Eina_List *event_cb_list = NULL;
	const Eina_List *l = NULL;
	const Eina_List *ln = NULL;
	event_cb_s *event_cb_info = NULL;

	ret_if(!toolbar);
	ret_if(!page);

	event_cb_list = evas_object_data_get(toolbar, PRIVATE_DATA_KEY_EVENT_CALLBACK_LIST);
	ret_if(!event_cb_list);

	EINA_LIST_FOREACH_SAFE(event_cb_list, l, ln, event_cb_info) {
		if (TOOLBAR_EVENT_TYPE_CHANGE_TAB == event_cb_info->event_type) {
			if (event_cb_info->event_cb) {
				event_cb_info->event_cb(toolbar, TOOLBAR_EVENT_TYPE_CHANGE_TAB, page, event_cb_info->user_data);
			}
		}
	}
}



Elm_Object_Item *_toolbar_append_item(Evas_Object *toolbar, const char *tab_name, Evas_Object *page)
{
	Elm_Object_Item *tab_item = NULL;

	retv_if(!toolbar, NULL);
	retv_if(!tab_name, NULL);

	tab_item = elm_toolbar_item_append(toolbar, NULL, tab_name, __view_changed_cb, page);
	elm_object_item_domain_text_translatable_set(tab_item, PACKAGE, EINA_TRUE);
	retv_if(!tab_item, NULL);

	return tab_item;
}



void _toolbar_remove_item(Evas_Object *toolbar, Elm_Object_Item *tabbar_item)
{
	ret_if(!toolbar);
	ret_if(!tabbar_item);

	elm_object_item_del(tabbar_item);

	/* If the selected item is removed, we have to activate the current */
}



void _toolbar_bring_in(Evas_Object *toolbar, Elm_Object_Item *tabbar_item)
{
	Elm_Object_Item *selected_item = NULL;

	ret_if(!toolbar);
	ret_if(!tabbar_item);

	selected_item = elm_toolbar_selected_item_get(toolbar);
	if (selected_item && selected_item == tabbar_item) {
		return;
	}

	elm_toolbar_item_selected_set(tabbar_item, EINA_TRUE);
	elm_object_focus_set(toolbar, EINA_TRUE);
}



unsigned int _toolbar_count_item(Evas_Object *toolbar)
{
	retv_if(!toolbar, 0);

	return elm_toolbar_items_count(toolbar);
}



int _toolbar_register_event_cb(Evas_Object *toolbar, int event_type, void (*event_cb)(Evas_Object *toolbar, int event_type, void *event_info, void *user_data), void *user_data)
{
	Eina_List *event_cb_list = NULL;
	event_cb_s *event_cb_info = NULL;

	retv_if(!toolbar, ATTACH_PANEL_ERROR_INVALID_PARAMETER);
	retv_if(event_type <= TOOLBAR_EVENT_TYPE_INVALID, ATTACH_PANEL_ERROR_INVALID_PARAMETER);
	retv_if(event_type >= TOOLBAR_EVENT_TYPE_MAX, ATTACH_PANEL_ERROR_INVALID_PARAMETER);
	retv_if(!event_cb, ATTACH_PANEL_ERROR_INVALID_PARAMETER);

	event_cb_info = calloc(1, sizeof(event_cb_s));
	retv_if(!event_cb_info, ATTACH_PANEL_ERROR_OUT_OF_MEMORY);

	event_cb_info->event_type = event_type;
	event_cb_info->event_cb = event_cb;
	event_cb_info->user_data = user_data;

	event_cb_list = evas_object_data_get(toolbar, PRIVATE_DATA_KEY_EVENT_CALLBACK_LIST);
	event_cb_list = eina_list_append(event_cb_list, event_cb_info);
	evas_object_data_set(toolbar, PRIVATE_DATA_KEY_EVENT_CALLBACK_LIST, event_cb_list);

	return ATTACH_PANEL_ERROR_NONE;
}



int _toolbar_unregister_event_cb(Evas_Object *toolbar, int event_type, void (*event_cb)(Evas_Object *toolbar, int event_type, void *event_info, void *user_data))
{
	Eina_List *event_cb_list = NULL;
	const Eina_List *l = NULL;
	const Eina_List *ln = NULL;
	event_cb_s *event_cb_info = NULL;

	retv_if(!toolbar, ATTACH_PANEL_ERROR_INVALID_PARAMETER);
	retv_if(event_type <= TOOLBAR_EVENT_TYPE_INVALID, ATTACH_PANEL_ERROR_INVALID_PARAMETER);
	retv_if(event_type >= TOOLBAR_EVENT_TYPE_MAX, ATTACH_PANEL_ERROR_INVALID_PARAMETER);
	retv_if(!event_cb, ATTACH_PANEL_ERROR_INVALID_PARAMETER);

	event_cb_list = evas_object_data_get(toolbar, PRIVATE_DATA_KEY_EVENT_CALLBACK_LIST);
	retv_if(!event_cb_list, ATTACH_PANEL_ERROR_NOT_INITIALIZED);

	EINA_LIST_FOREACH_SAFE(event_cb_list, l, ln, event_cb_info) {
		if (event_cb_info->event_type == event_type
			&& event_cb_info->event_cb == event_cb) {
			event_cb_list = eina_list_remove(event_cb_list, event_cb_info);
			break;
		}
	}

	evas_object_data_set(toolbar, PRIVATE_DATA_KEY_EVENT_CALLBACK_LIST, event_cb_list);

	return ATTACH_PANEL_ERROR_NONE;
}
