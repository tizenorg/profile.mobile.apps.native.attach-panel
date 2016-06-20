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
#include "content_list.h"
#include "log.h"
#include "page.h"
#include "scroller.h"



static const char *const PRIVATE_DATA_KEY_SCROLLER_IS_SCROLLING = "p_is_sc";
static const char *const PRIVATE_DATA_KEY_SCROLLER_DRAG_START = "p_dg_st";
static const char *const PRIVATE_DATA_KEY_EVENT_CALLBACK_LIST = "pdkec";



struct _event_cb {
	int event_type;
	void (*event_cb)(Evas_Object *scroller, int event_type, void *event_info, void *user_data);
	void *user_data;
};
typedef struct _event_cb event_cb_s;


//LCOV_EXCL_START
int _scroller_is_scrolling(Evas_Object *scroller)
{
	retv_if(!scroller, 0);
	return (int)evas_object_data_get(scroller, PRIVATE_DATA_KEY_SCROLLER_IS_SCROLLING);
}



static void __anim_start_cb(void *data, Evas_Object *scroller, void *event_info)
{
	_D("start the scroller(%p) animation", scroller);
	evas_object_data_set(scroller, PRIVATE_DATA_KEY_SCROLLER_IS_SCROLLING, (void *)1);
}



static void __anim_stop_cb(void *data, Evas_Object *scroller, void *event_info)
{
	attach_panel_h attach_panel = data;

	int index = 0;
	int x = 0;
	int page_w = 0;

	ret_if(!attach_panel);
	ret_if(!scroller);

	_D("stop the scroller(%p) animation", scroller);
	evas_object_data_del(scroller, PRIVATE_DATA_KEY_SCROLLER_IS_SCROLLING);
	evas_object_data_del(scroller, PRIVATE_DATA_KEY_SCROLLER_DRAG_START);

	elm_scroller_region_get(scroller, &x, NULL, NULL, NULL);
	elm_scroller_page_size_get(scroller, &page_w, NULL);
	ret_if(!page_w);

	index = x / page_w;
	x = x % page_w;
	if (x != 0) {
		return;
	}

	if (attach_panel->cur_page_no != index) {
		_content_list_send_message_to_content(attach_panel->content_list, "__ATTACH_PANEL_SHOW_CONTENT_CATEGORY__", "false", attach_panel->cur_page_no);
		_content_list_send_message_to_content(attach_panel->content_list, "__ATTACH_PANEL_SHOW_CONTENT_CATEGORY__", "true", index);
		attach_panel->cur_page_no = index;
		_D("change the show page : %d", index);
	}
}



static void __drag_start_cb(void *data, Evas_Object *scroller, void *event_info)
{
	_D("start to drag the scroller(%p)", scroller);
	evas_object_data_set(scroller, PRIVATE_DATA_KEY_SCROLLER_DRAG_START, (void *)1);
}



static void __drag_stop_cb(void *data, Evas_Object *scroller, void *event_info)
{
	_D("stop to drag the scroller(%p)", scroller);
}



static void __scroll_cb(void *data, Evas_Object *scroller, void *event_info)
{
	attach_panel_h attach_panel = data;
	Elm_Object_Item *tabbar_item = NULL;
	Eina_List *event_cb_list = NULL;
	const Eina_List *l = NULL;
	const Eina_List *ln = NULL;
	content_s *info = NULL;
	event_cb_s *event_cb_info = NULL;

	int index = 0;
	int i = 0;
	int x = 0;
	int page_w = 0;

	ret_if(!attach_panel);
	ret_if(!scroller);

	if (!evas_object_data_get(scroller, PRIVATE_DATA_KEY_SCROLLER_DRAG_START))
		return;

	event_cb_list = evas_object_data_get(scroller, PRIVATE_DATA_KEY_EVENT_CALLBACK_LIST);
	ret_if(!event_cb_list);

	elm_scroller_region_get(scroller, &x, NULL, NULL, NULL);
	elm_scroller_page_size_get(scroller, &page_w, NULL);
	ret_if(!page_w);

	index = x / page_w;
	x = x % page_w;
	if (x > (page_w / 2))
		index++;

	EINA_LIST_FOREACH(attach_panel->content_list, l, info) {
		if (index == i) {
			tabbar_item = info->tabbar_item;
			attach_panel->cur_page_no = i;
			break;
		}
		i++;
	}
	ret_if(!tabbar_item);

	EINA_LIST_FOREACH_SAFE(event_cb_list, l, ln, event_cb_info) {
		if (SCROLLER_EVENT_TYPE_SCROLL == event_cb_info->event_type) {
			if (event_cb_info->event_cb) {
				event_cb_info->event_cb(scroller, SCROLLER_EVENT_TYPE_SCROLL, tabbar_item, event_cb_info->user_data);
			}
		}
	}
}



static void __resize_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
	Evas_Object *scroller = obj;
	attach_panel_h attach_panel = data;

	int x, y, w, h;
	int screen_w = 0, screen_h = 0;

	ret_if(!scroller);
	ret_if(!attach_panel);

	elm_win_screen_size_get(attach_panel->win, NULL, NULL, &screen_w, &screen_h);

	evas_object_geometry_get(scroller, &x, &y, &w, &h);
	_D("scroller resize(%d, %d, %d, %d)", x, y, w, h);

	if (w == screen_w || w == screen_h) {
		_D("screen size(%d, %d)", screen_w, screen_h);
		elm_scroller_page_show(scroller, attach_panel->cur_page_no, 0);
	}
}
//LCOV_EXCL_STOP


Evas_Object *_scroller_create(Evas_Object *ui_manager, attach_panel_h attach_panel)
{
	Evas_Object *box = NULL;
	Evas_Object *scroller = NULL;

	retv_if(!ui_manager, NULL);
	retv_if(!attach_panel, NULL);

	scroller = elm_scroller_add(ui_manager);
	retv_if(!scroller, NULL);

	elm_scroller_bounce_set(scroller, EINA_TRUE, EINA_FALSE);
	elm_scroller_policy_set(scroller, ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_OFF);
	elm_scroller_page_scroll_limit_set(scroller, 1, 1);
	elm_scroller_content_min_limit(scroller, EINA_FALSE, EINA_TRUE);
	elm_scroller_single_direction_set(scroller, ELM_SCROLLER_SINGLE_DIRECTION_HARD);

	elm_scroller_page_size_set(scroller, attach_panel->transit_width, attach_panel->transit_height);
	evas_object_size_hint_weight_set(scroller, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_min_set(scroller, 0, 0);

	elm_object_style_set(scroller, "effect");
	evas_object_show(scroller);
	elm_object_scroll_lock_y_set(scroller, EINA_TRUE);

	evas_object_smart_callback_add(scroller, "scroll,anim,start", __anim_start_cb, NULL);
	evas_object_smart_callback_add(scroller, "scroll,anim,stop", __anim_stop_cb, attach_panel);
	evas_object_smart_callback_add(scroller, "scroll,drag,start", __drag_start_cb, NULL);
	evas_object_smart_callback_add(scroller, "scroll,drag,stop", __drag_stop_cb, NULL);
	evas_object_smart_callback_add(scroller, "scroll", __scroll_cb, attach_panel);
	evas_object_event_callback_add(scroller, EVAS_CALLBACK_RESIZE, __resize_cb, attach_panel);

	box = elm_box_add(scroller);
	goto_if(!box, ERROR);

	elm_box_horizontal_set(box, EINA_TRUE);
	elm_box_align_set(box, 0.5, 0.0);
	evas_object_size_hint_weight_set(box, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_show(box);

	elm_object_content_set(scroller, box);
	attach_panel->cur_page_no = 0;

	return scroller;

ERROR:
	if (scroller) {
		evas_object_del(scroller);
	}
	return NULL;
}



void _scroller_destroy(Evas_Object *scroller)
{
	Evas_Object *box = NULL;
	ret_if(!scroller);

	evas_object_event_callback_del(scroller, EVAS_CALLBACK_RESIZE, __resize_cb);
	box = elm_object_content_unset(scroller);
	if (box) {
		evas_object_del(box);
	}

	evas_object_del(scroller);
}



void _scroller_append_page(Evas_Object *scroller, Evas_Object *page)
{
	Evas_Object *box = NULL;

	box = elm_object_content_get(scroller);
	ret_if(!box);

	elm_box_pack_end(box, page);
}


//LCOV_EXCL_START
void _scroller_remove_page(Evas_Object *scroller, Evas_Object *page)
{
	Evas_Object *box = NULL;
	Evas_Object *tmp = NULL;
	Eina_List *list = NULL;

	int is_page_exist = 0;

	box = elm_object_content_get(scroller);
	ret_if(!box);

	list = elm_box_children_get(box);
	EINA_LIST_FREE(list, tmp) {
		continue_if(!tmp);
		if (page == tmp) {
			is_page_exist = 1;
		}
	}

	if (!is_page_exist) {
		_D("No page to remove");
		return;
	}

	elm_box_unpack(box, page);
}



void _scroller_bring_in_page(Evas_Object *scroller, Evas_Object *page, int *cur_page_no)
{
	Evas_Object *box = NULL;
	Evas_Object *tmp = NULL;
	Eina_List *list = NULL;
	int index = 0;
	int exist = 1;

	ret_if(!scroller);
	ret_if(!page);

	box = elm_object_content_get(scroller);
	ret_if(!box);

	list = elm_box_children_get(box);
	ret_if(!list);

	EINA_LIST_FREE(list, tmp) {
		continue_if(!tmp);
		if (page == tmp) {
			exist = 0;
		}
		index += exist;
	}

	if (exist) {
		index = 0;
	}

	*cur_page_no = index;

	elm_scroller_page_bring_in(scroller, index, 0);
}
//LCOV_EXCL_STOP


void _scroller_resize(Evas_Object *scroller, int width, int height)
{
	Evas_Object *box = NULL;
	Evas_Object *page = NULL;
	Eina_List *list = NULL;

	ret_if(!scroller);

	elm_scroller_page_size_set(scroller, width, height);

	box = elm_object_content_get(scroller);
	ret_if(!box);

	list = elm_box_children_get(box);
	ret_if(!list);

	EINA_LIST_FREE(list, page) {
		_page_resize(page, width, height);
	}
	elm_box_recalculate(box);
}



int _scroller_register_event_cb(Evas_Object *scroller, int event_type, void (*event_cb)(Evas_Object *scroller, int event_type, void *event_info, void *user_data), void *user_data)
{
	Eina_List *event_cb_list = NULL;
	event_cb_s *event_cb_info = NULL;

	retv_if(!scroller, ATTACH_PANEL_ERROR_INVALID_PARAMETER);
	retv_if(event_type <= SCROLLER_EVENT_TYPE_INVALID, ATTACH_PANEL_ERROR_INVALID_PARAMETER);
	retv_if(event_type >= SCROLLER_EVENT_TYPE_MAX, ATTACH_PANEL_ERROR_INVALID_PARAMETER);
	retv_if(!event_cb, ATTACH_PANEL_ERROR_INVALID_PARAMETER);

	event_cb_info = calloc(1, sizeof(event_cb_s));
	retv_if(!event_cb_info, ATTACH_PANEL_ERROR_OUT_OF_MEMORY);

	event_cb_info->event_type = event_type;
	event_cb_info->event_cb = event_cb;
	event_cb_info->user_data = user_data;

	event_cb_list = evas_object_data_get(scroller, PRIVATE_DATA_KEY_EVENT_CALLBACK_LIST);
	event_cb_list = eina_list_append(event_cb_list, event_cb_info);
	evas_object_data_set(scroller, PRIVATE_DATA_KEY_EVENT_CALLBACK_LIST, event_cb_list);

	return ATTACH_PANEL_ERROR_NONE;
}



int _scroller_unregister_event_cb(Evas_Object *scroller, int event_type, void (*event_cb)(Evas_Object *scroller, int event_type, void *event_info, void *user_data))
{
	Eina_List *event_cb_list = NULL;
	const Eina_List *l = NULL;
	const Eina_List *ln = NULL;
	event_cb_s *event_cb_info = NULL;

	retv_if(!scroller, ATTACH_PANEL_ERROR_INVALID_PARAMETER);
	retv_if(event_type <= SCROLLER_EVENT_TYPE_INVALID, ATTACH_PANEL_ERROR_INVALID_PARAMETER);
	retv_if(event_type >= SCROLLER_EVENT_TYPE_MAX, ATTACH_PANEL_ERROR_INVALID_PARAMETER);
	retv_if(!event_cb, ATTACH_PANEL_ERROR_INVALID_PARAMETER);

	event_cb_list = evas_object_data_get(scroller, PRIVATE_DATA_KEY_EVENT_CALLBACK_LIST);
	retv_if(!event_cb_list, ATTACH_PANEL_ERROR_NOT_INITIALIZED);

	EINA_LIST_FOREACH_SAFE(event_cb_list, l, ln, event_cb_info) {
		if (event_cb_info->event_type == event_type
			&& event_cb_info->event_cb == event_cb) {
			event_cb_list = eina_list_remove(event_cb_list, event_cb_info);
			break;
		}
	}

	evas_object_data_set(scroller, PRIVATE_DATA_KEY_EVENT_CALLBACK_LIST, event_cb_list);

	return ATTACH_PANEL_ERROR_NONE;
}
