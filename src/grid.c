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

#include "attach_panel.h"
#include "attach_panel_internal.h"
#include "attach_bundle.h"
#include "conf.h"
#include "gesture.h"
#include "grid.h"
#include "log.h"



static const char *const FILE_LAYOUT_EDJ = EDJEDIR"/layout.edj";
static const char *const PRIVATE_DATA_KEY_GRID_LIST = "p_g_lt";
static const char *const PRIVATE_DATA_KEY_GIC = "p_gic";
static const char *const PRIVATE_DATA_KEY_LIST_INDEX = "p_lt_ix";
static const char *const PRIVATE_DATA_KEY_ICON_WIDTH = "p_ic_w";
static const char *const PRIVATE_DATA_KEY_ICON_HEIGHT = "p_ic_h";
static const char *const PRIVATE_DATA_KEY_ITEM_WIDTH = "p_it_w";
static const char *const PRIVATE_DATA_KEY_ITEM_HEIGHT = "p_it_h";
static const char *const PRIVATE_DATA_KEY_ANIMATOR = "p_tm";
static const char *const DEFAULT_ICON = "/usr/share/icons/A01-1_icon_Menu.png";



Eina_Bool _grid_can_flick(attach_panel_h attach_panel)
{
	content_s *current_content_info = NULL;
	int content_h, grid_h;

	if (attach_panel->rotate) {
		if (ATTACH_PANEL_STATE_FULL != attach_panel->attach_panel_land_state) {
			_D("panel is not full state(land mode)");
			return EINA_TRUE;
		}
	} else {
		if (ATTACH_PANEL_STATE_FULL != attach_panel->attach_panel_port_state) {
			_D("panel is not full state(port mode)");
			evas_object_data_set(attach_panel->grid, DATA_KEY_EDGE_TOP, (void *) 1);
			return EINA_TRUE;
		}
	}

	current_content_info = eina_list_nth(attach_panel->content_list, attach_panel->current_page);
	retv_if(!current_content_info, EINA_TRUE);

	if (attach_panel->grid != current_content_info->content) {
		_D("current page is ug page");
		return EINA_TRUE;
	}

	elm_scroller_child_size_get(attach_panel->grid, NULL, &content_h);
	evas_object_geometry_get(attach_panel->grid, NULL, NULL, NULL, &grid_h);

	if (content_h < grid_h) {
		_D("scrolling is not exist(grid size : %d, content size : %d", grid_h, content_h);
		return EINA_TRUE;
	}

	if (!evas_object_data_del(attach_panel->grid, DATA_KEY_EDGE_TOP)) {
		_D("grid can not reach the edge top");
		return EINA_FALSE;
	}

	return EINA_TRUE;
}



static char *__text_get(void *data, Evas_Object *obj, const char *part)
{
	content_s *info = data;
	innate_content_s *innate_content_info = NULL;
	retv_if(!info, NULL);

	innate_content_info = info->innate_content_info;
	retv_if(!innate_content_info, NULL);
	if (!strcmp(part, "elm.text")) {
		return strdup(D_(innate_content_info->name));
	}

	return NULL;
}



static Evas_Object *__add_icon(Evas_Object *parent, const char *file)
{
	const char *real_icon_file = NULL;
	Evas_Object *icon = NULL;
	Evas_Object *icon_layout = NULL;
	int w, h;

	real_icon_file = file;
	if (access(real_icon_file, R_OK) != 0) {
		_E("Failed to access an icon(%s)", real_icon_file);
		real_icon_file = DEFAULT_ICON;
	}

	icon = elm_icon_add(parent);
	retv_if(!icon, NULL);

	if (elm_image_file_set(icon, real_icon_file, NULL) == EINA_FALSE) {
		_E("Icon file is not accessible (%s)", real_icon_file);
		evas_object_del(icon);
		return NULL;
	}

	w = (int) evas_object_data_get(parent, PRIVATE_DATA_KEY_ICON_WIDTH);
	h = (int) evas_object_data_get(parent, PRIVATE_DATA_KEY_ICON_HEIGHT);
	evas_object_size_hint_min_set(icon, w, h);

	elm_image_preload_disabled_set(icon, EINA_TRUE);
	elm_image_smooth_set(icon, EINA_TRUE);
	elm_image_no_scale_set(icon, EINA_FALSE);
	evas_object_show(icon);

	icon_layout = elm_layout_add(parent);
	retv_if(!icon_layout, NULL);

	elm_layout_file_set(icon_layout, FILE_LAYOUT_EDJ, "grid,icon");
	evas_object_size_hint_weight_set(icon_layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(icon_layout, EVAS_HINT_FILL, EVAS_HINT_FILL);
	evas_object_show(icon_layout);

	elm_object_part_content_set(icon_layout, "icon", icon);

	return icon_layout;
}



static Evas_Object *__content_get(void *data, Evas_Object *obj, const char *part)
{
	content_s *info = data;
	innate_content_s *innate_content_info = NULL;

	retv_if(!info, NULL);
	innate_content_info = info->innate_content_info;
	retv_if(!innate_content_info, NULL);

	if (!strcmp(part, "elm.swallow.end")) {
		Evas_Object *bg = evas_object_rectangle_add(evas_object_evas_get(obj));
		retv_if(!bg, NULL);
		int w, h;

		w = (int) evas_object_data_get(obj, PRIVATE_DATA_KEY_ITEM_WIDTH);
		h = (int) evas_object_data_get(obj, PRIVATE_DATA_KEY_ITEM_HEIGHT);

		evas_object_size_hint_min_set(bg, w, h);
		evas_object_color_set(bg, 0, 0, 0, 0);
		evas_object_show(bg);
		return bg;
	} else if (!strcmp(part, "elm.swallow.icon")) {
		retv_if(!innate_content_info->icon, NULL);
		return __add_icon(obj, innate_content_info->icon);
	} else if (!strcmp(part, "selected")) {

	}
	return NULL;
}



static void __del(void *data, Evas_Object *obj)
{
	ret_if(NULL == data);
}



static void __reply_cb(app_control_h request, app_control_h reply, app_control_result_e result, void *user_data)
{
	content_s *content_info = user_data;

	ret_if(!content_info);
	ret_if(!content_info->attach_panel);

	if (content_info->attach_panel->result_cb) {
		content_info->attach_panel->result_cb(content_info->attach_panel
				, content_info->innate_content_info->content_category
				, reply
				, content_info->attach_panel->result_data);

		if (ATTACH_PANEL_STATE_FULL == _gesture_get_state()) {
			/* This is same with attach_panel_hide */
			//_content_list_set_pause(content_info->attach_panel->content_list, ATTACH_PANEL_CONTENT_CATEGORY_UG);
			_gesture_hide(content_info->attach_panel);
		}
	} else {
		_D("content_info->attach_panel->result_cb is NULL");
	}
}



static void __launch_app(content_s *content_info)
{
	app_control_h app_control = NULL;
	innate_content_s *innate_content_info = NULL;
	char value[BUF_SIZE] = {0, };
	int ret;

	ret_if(!content_info);

	innate_content_info = content_info->innate_content_info;
	ret_if(!innate_content_info);

	ret = app_control_create(&app_control);
	ret_if(APP_CONTROL_ERROR_NONE != ret);

	if (content_info->innate_content_info->operation) {
		app_control_set_operation(app_control, content_info->innate_content_info->operation);
	}

	if (content_info->innate_content_info->mode) {
		snprintf(value, sizeof(value), "%d", content_info->innate_content_info->mode);
		app_control_add_extra_data(app_control, "http://tizen.org/appcontrol/data/mode", value);
	}

	if (content_info->innate_content_info->result_type) {
		app_control_add_extra_data(app_control, "http://tizen.org/appcontrol/data/result_type", content_info->innate_content_info->result_type);
	}

	if (content_info->innate_content_info->item_type) {
		app_control_add_extra_data(app_control, "http://tizen.org/appcontrol/data/item_type", content_info->innate_content_info->item_type);
	}

	if (content_info->innate_content_info->selection_mode) {
		app_control_add_extra_data(app_control, APP_CONTROL_DATA_SELECTION_MODE, content_info->innate_content_info->selection_mode);
	}

	if (content_info->innate_content_info->mime) {
		app_control_set_mime(app_control, content_info->innate_content_info->mime);
	}

	if (content_info->innate_content_info->max) {
		snprintf(value, sizeof(value), "%d", content_info->innate_content_info->max);
		app_control_add_extra_data(app_control, "http://tizen.org/appcontrol/data/max", value);
	}

	if (content_info->extra_data) {
		_D("app control add the extra data for app");
		_bundle_add_to_app_control(content_info->extra_data, app_control);
	}

	app_control_set_launch_mode(app_control, APP_CONTROL_LAUNCH_MODE_GROUP);

	app_control_send_launch_request(app_control, __reply_cb, content_info);
	app_control_destroy(app_control);
}



static void __item_selected(void *data, Evas_Object *obj, void *event_info)
{
	content_s *info = data;
	innate_content_s *innate_content_info = NULL;
	Elm_Object_Item *selected_item = NULL;

	ret_if(!info);
	innate_content_info = info->innate_content_info;
	ret_if(!innate_content_info);

	selected_item = elm_gengrid_selected_item_get(obj);
	ret_if(!selected_item);
	elm_gengrid_item_selected_set(selected_item, EINA_FALSE);

	_D("Selected an item[%s]", innate_content_info->operation);

	__launch_app(info);
}



Eina_Bool __animator_cb(void *data)
{
	Evas_Object *grid = data;
	Elm_Gengrid_Item_Class *gic = NULL;
	Eina_List *list = NULL;
	content_s *info = NULL;

	int index = 0;
	int count = 0;
	int sum = 0;
	int i = 0;
	static int column = 0;
	static int row = 0;

	retv_if(!grid, ECORE_CALLBACK_CANCEL);

	list = evas_object_data_get(grid, PRIVATE_DATA_KEY_GRID_LIST);
	retv_if(!list, ECORE_CALLBACK_CANCEL);

	gic = evas_object_data_get(grid, PRIVATE_DATA_KEY_GIC);
	retv_if(!gic, ECORE_CALLBACK_CANCEL);

	index = (int) evas_object_data_get(grid, PRIVATE_DATA_KEY_LIST_INDEX);

	count = eina_list_count(list);
	if (index == count) goto OUT;

	if (!index) {
		int grid_w;
		int grid_h;
		evas_object_geometry_get(grid, NULL, NULL, &grid_w, &grid_h);
		int w = (int) evas_object_data_get(grid, PRIVATE_DATA_KEY_ITEM_WIDTH);
		int h = (int) evas_object_data_get(grid, PRIVATE_DATA_KEY_ITEM_HEIGHT);
		if (w) {
			column = grid_w / w;
			row = (int) ceil((double) ((double) grid_h / (double) h));
		}
	}

	sum = column * row;
	for (; i < sum; ++i) {
		info = eina_list_nth(list, index);
		elm_gengrid_item_append(grid, gic, info, __item_selected, info);
		index++;
		if (index == count) goto OUT;
	}
	evas_object_data_set(grid, PRIVATE_DATA_KEY_LIST_INDEX, (void *) index);

	return ECORE_CALLBACK_RENEW;

OUT:
	_D("Loading apps is done");

	Elm_Object_Item *first_it = elm_gengrid_first_item_get(grid);
	if (first_it) {
		elm_gengrid_item_bring_in(first_it, ELM_GENGRID_ITEM_SCROLLTO_TOP);
	}
	evas_object_data_del(grid, PRIVATE_DATA_KEY_LIST_INDEX);
	evas_object_data_del(grid, PRIVATE_DATA_KEY_ANIMATOR);

	return ECORE_CALLBACK_CANCEL;
}



static void __edge_top_cb(void *data, Evas_Object *grid, void *event_info)
{
	ret_if(!grid);
	_D("grid reach the edge top");
	evas_object_data_set(grid, DATA_KEY_EDGE_TOP, (void *) 1);
}



static void __scroll_cb(void *data, Evas_Object *grid, void *event_info)
{
	ret_if(!grid);
	evas_object_data_del(grid, DATA_KEY_EDGE_TOP);
}



static void __lang_changed_cb(void *data, Evas_Object *grid, void *event_info)
{
	ret_if(!grid);
	elm_gengrid_realized_items_update(grid);
}



Evas_Object *_grid_create(Evas_Object *page, attach_panel_h attach_panel)
{
	Evas_Object *grid = NULL;
	Elm_Gengrid_Item_Class *gic = NULL;

	int item_w = 0, item_h = 0, icon_size = 0;
	int w, h;

	retv_if(!attach_panel, NULL);

	grid = elm_gengrid_add(page);
	goto_if(!grid, ERROR);

	evas_object_size_hint_weight_set(grid, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(grid, EVAS_HINT_FILL, EVAS_HINT_FILL);

	evas_object_geometry_get(attach_panel->conformant, NULL, NULL, &w, &h);

	if (w < h) {
		item_w = w / GRID_ROW_COUNT;
	} else {
		item_w = h / GRID_ROW_COUNT;
	}
	item_h = item_w * GRID_ITEM_HEIGHT_REL;
	icon_size = item_h * GRID_ICON_SIZE_REL;
	_D("item_size : %d, %d, icon_size : %d", item_w, item_h, icon_size);

	evas_object_data_set(grid, PRIVATE_DATA_KEY_ITEM_WIDTH, (void *) item_w);
	evas_object_data_set(grid, PRIVATE_DATA_KEY_ITEM_HEIGHT, (void *) item_h);
	evas_object_data_set(grid, PRIVATE_DATA_KEY_ICON_WIDTH, (void *) icon_size);
	evas_object_data_set(grid, PRIVATE_DATA_KEY_ICON_HEIGHT, (void *) icon_size);

	elm_gengrid_item_size_set(grid, item_w, item_h);
	elm_gengrid_align_set(grid, 0.0, 0.0);
	elm_gengrid_horizontal_set(grid, EINA_FALSE);
	elm_gengrid_multi_select_set(grid, EINA_FALSE);

	gic = elm_gengrid_item_class_new();
	goto_if(!gic, ERROR);
	gic->func.text_get = __text_get;
	gic->func.content_get = __content_get;
	gic->func.state_get = NULL;
	gic->func.del = __del;
	gic->item_style = "type2";

	evas_object_data_set(grid, PRIVATE_DATA_KEY_GRID_LIST, attach_panel->content_list);
	evas_object_data_set(grid, PRIVATE_DATA_KEY_GIC, gic);
	evas_object_data_set(grid, PRIVATE_DATA_KEY_LIST_INDEX, NULL);
	evas_object_data_set(grid, DATA_KEY_EDGE_TOP, (void *)1);
	evas_object_smart_callback_add(grid, "edge,top", __edge_top_cb, NULL);
	evas_object_smart_callback_add(grid, "scroll", __scroll_cb, NULL);
	evas_object_smart_callback_add(grid, "language,changed", __lang_changed_cb, NULL);

	elm_scroller_movement_block_set(grid, ELM_SCROLLER_MOVEMENT_BLOCK_VERTICAL);

	evas_object_show(grid);

	return grid;

ERROR:
	_grid_destroy(grid);
	return NULL;
}



void _grid_destroy(Evas_Object *grid)
{
	ret_if(!grid);

	Ecore_Animator *animator = NULL;

	evas_object_data_del(grid, PRIVATE_DATA_KEY_GRID_LIST);
	evas_object_data_del(grid, PRIVATE_DATA_KEY_LIST_INDEX);
	evas_object_data_del(grid, PRIVATE_DATA_KEY_ICON_WIDTH);
	evas_object_data_del(grid, PRIVATE_DATA_KEY_ICON_HEIGHT);
	evas_object_data_del(grid, PRIVATE_DATA_KEY_ITEM_WIDTH);
	evas_object_data_del(grid, PRIVATE_DATA_KEY_ITEM_HEIGHT);
	evas_object_data_del(grid, DATA_KEY_EDGE_TOP);

	evas_object_data_del(grid, PRIVATE_DATA_KEY_GIC);

	animator = evas_object_data_del(grid, PRIVATE_DATA_KEY_ANIMATOR);
	if (animator) {
		ecore_animator_del(animator);
		animator = NULL;
	}

	evas_object_del(grid);
}



Elm_Object_Item *_grid_append_item(Evas_Object *grid, content_s *content_info)
{
	Elm_Gengrid_Item_Class *gic = NULL;
	Elm_Object_Item *item = NULL;

	retv_if(!grid, NULL);
	retv_if(!content_info, NULL);

	gic = evas_object_data_get(grid, PRIVATE_DATA_KEY_GIC);
	retv_if(!gic, NULL);

	item = elm_gengrid_item_append(grid, gic, content_info, __item_selected, content_info);
	retv_if(!item, NULL);
	_D("grid append item : %s", content_info->innate_content_info->name);
	content_info->grid_item = item;

	return item;
}



void _grid_remove_item(Evas_Object *grid, content_s *content_info)
{
	Elm_Object_Item *item = NULL;

	ret_if(!grid);
	ret_if(!content_info);

	item = content_info->grid_item;
	ret_if(!item);
	elm_object_item_del(item);
	content_info->grid_item = NULL;
}



int _grid_count_item(Evas_Object *grid)
{
	int count = 0;

	retv_if(!grid, 0);

	count = elm_gengrid_items_count(grid);

	return count;
}
