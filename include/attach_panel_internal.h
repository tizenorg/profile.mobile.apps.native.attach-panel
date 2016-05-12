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

#ifndef __TIZEN_ATTACH_PANEL_INTERNAL_H__
#define __TIZEN_ATTACH_PANEL_INTERNAL_H__

#include <Elementary.h>
#include <app_control.h>

#define DATA_KEY_ATTACH_PANEL "__dkap__"
#define DATA_KEY_ATTACH_PANEL_INFO "__dkapi__"
#define DATA_KEY_EDGE_TOP "__dket__"
#define DATA_KEY_PAGE "__dkpg__"
#define DATA_KEY_UG "__dkug__"

#define SELECTION_MODE_SINGLE "single"
#define SELECTION_MODE_MULTIPLE "multiple"

#define MODE_ENABLE "enable"
#define MODE_DISABLE "disable"
#define MODE_TRUE "true"
#define MODE_FALSE "false"

#define TABBAR_NAME_MORE "More"
#define BUF_SIZE 128

/*
 * attach-panel(Caller) <-> UGs & Apps operations
 *
 * - ATTACH_PANEL_CONTENT_CATEGORY_IMAGE
 *   appid : "attach-panel-gallery"
 *   operation : APP_CONTROL_OPERATION_PICK
 *   mime : "image/(asterisk)"
 *   extra_data : See below
 *   1) APP_CONTROL_DATA_SELECTION_MODE : "single"
 *   2) "http://tizen.org/appcontrol/data/total_count" : any numbers
 *   3) "http://tizen.org/appcontrol/data/total_size" : any bytes
 *   return data : See below
 *   1) "http://tizen.org/appcontrol/data/selected"
 *
 * - ATTACH_PANEL_CONTENT_CATEGORY_CAMERA
 *   appid : "attach-panel-camera"
 *   operation : APP_CONTROL_OPERATION_CREATE_CONTENT
 *   mime : NULL
 *   extra_data : See below
 *   1) APP_CONTROL_DATA_SELECTION_MODE : "single"
 *   2) "http://tizen.org/appcontrol/data/total_size" : any bytes
 *   return data : See below
 *   1) "http://tizen.org/appcontrol/data/selected"
 *
 * - ATTACH_PANEL_CONTENT_CATEGORY_VOICE
 *   appid : "attach-panel-voicerecorder"
 *   operation : APP_CONTROL_OPERATION_CREATE_CONTENT
 *   mime : NULL
 *   extra_data : See below
 *   1) APP_CONTROL_DATA_SELECTION_MODE : "single"
 *   2) "http://tizen.org/appcontrol/data/total_size" : any bytes
 *   return data : See below
 *   1) "http://tizen.org/appcontrol/data/selected"
 *
 * - ATTACH_PANEL_CONTENT_CATEGORY_VIDEO
 *   appid : NULL
 *   operation : APP_CONTROL_OPERATION_PICK
 *   mime : "video/(asterisk)"
 *   extra_data : See below
 *   1) APP_CONTROL_DATA_SELECTION_MODE : "multiple"
 *   2) "http://tizen.org/appcontrol/data/total_count" : any numbers
 *   3) "http://tizen.org/appcontrol/data/total_size" : any bytes
 *   return data : See below
 *   1) "http://tizen.org/appcontrol/data/selected"
 *
 * - ATTACH_PANEL_CONTENT_CATEGORY_AUDIO
 *   appid : NULL
 *   operation : APP_CONTROL_OPERATION_PICK
 *   mime : "audio/(asterisk)"
 *   extra_data : See below
 *   1) APP_CONTROL_DATA_SELECTION_MODE : "multiple"
 *   2) "http://tizen.org/appcontrol/data/total_count" : any numbers
 *   3) "http://tizen.org/appcontrol/data/total_size" : any bytes
 *   return data : See below
 *   1) "http://tizen.org/appcontrol/data/selected"
 *
 * - ATTACH_PANEL_CONTENT_CATEGORY_CALENDAR
 *   appid : NULL
 *   operation : APP_CONTROL_OPERATION_PICK
 *   mime : "application/vnd.tizen.calendar"
 *   extra_data : See below
 *   1) APP_CONTROL_DATA_SELECTION_MODE : "multiple"
 *   2) "http://tizen.org/appcontrol/data/mode" : 1
 *   3) "http://tizen.org/appcontrol/data/type" : "vcs"
 *   4) "http://tizen.org/appcontrol/data/total_count" : any numbers
 *   return data : See below
 *   1) "http://tizen.org/appcontrol/data/selected"
 *
 * - ATTACH_PANEL_CONTENT_CATEGORY_CONTACT
 *   appid : NULL
 *   operation : APP_CONTROL_OPERATION_PICK
 *   mime : "application/vnd.tizen.contact"
 *   extra_data : See below
 *   1) APP_CONTROL_DATA_SELECTION_MODE : "multiple"
 *   2) "http://tizen.org/appcontrol/data/type" : "vcf"
 *   return data : See below
 *   1) "http://tizen.org/appcontrol/data/selected"
 *
 * - ATTACH_PANEL_CONTENT_CATEGORY_MYFILES
 *   appid : NULL
 *   operation : APP_CONTROL_OPERATION_PICK
 *   mime : NULL
 *   extra_data : See below
 *   1) APP_CONTROL_DATA_SELECTION_MODE : "multiple"
 *   2) "http://tizen.org/appcontrol/data/total_count" : any numbers
 *   3) "http://tizen.org/appcontrol/data/total_size" : any bytes
 *   return data : See below
 *   1) "http://tizen.org/appcontrol/data/selected"
 *
 * - ATTACH_PANEL_CONTENT_CATEGORY_VIDEO_RECORDER
 *   appid : NULL
 *   operation : APP_CONTROL_OPERATION_CREATE_CONTENT
 *   mime : "video/3gp"
 *   extra_data : See below
 *   1) APP_CONTROL_DATA_SELECTION_MODE : "single"
 *   2) "http://tizen.org/appcontrol/data/total_count" : any numbers
 *   3) "http://tizen.org/appcontrol/data/total_size" : any bytes
 *   return data : See below
 *   1) "http://tizen.org/appcontrol/data/selected"

 */


/*
 * attach-panel(Caller) <-> ui-gadgets(Callee) protocol
 *
 * - Caller send
 *   key : "__CALLER_PANEL__"
 *   value : "attach-panel"
 *   when : ug_create()
 *
 *   key : APP_CONTROL_DATA_SELECTION_MODE
 *   value : SELECTION_MODE_MULTIPLE
 *   when : full mode
 *   how : ug_send_message()
 *
 *   key : APP_CONTROL_DATA_SELECTION_MODE
 *   value : SELECTION_MODE_SINGLE
 *   when : half mode
 *   how : ug_send_message()
 *
 *   key : "__ATTACH_PANEL_INITIALIZE__"
 *   value : "enable"
 *   when : attach_panel_show()
 *   how : ug_send_message()
 *
 *   key : "__ATTACH_PANEL_SHOW_CONTENT_CATEGORY__"
 *   value : "true" or "false"
 *   when : set as true when the content category is shown, and false when it is hidden.
 *   how : ug_send_message()
 *
 * - Callee send
 *   key : "__ATTACH_PANEL_FLICK_DOWN__"
 *   value : "enable" or "disable"
 *   when : ug is full, then ug has events.
 *   how : ug_send_result()
 *
 *   key : "__ATTACH_PANEL_FULL_MODE__"
 *   value : "enable"
 *   when : ug needs to be shown on the full mode
 *   how : ug_send_result()
 *
 *   key : "__ATTACH_PANEL_SHOW_TOOLBAR__"
 *   value : "true" or "false"
 *   when : set as true when the panel's tabbar needs to be shown, and false when it needs to be hidden.
 *   how : ug_send_result()
 *
 *   key : "__ATTACH_PANEL_SHOW_PANEL__"
 *   value : "true" or "false"
 *   when : set as true when the panel it needs to be shown, and false when it needs to be hidden.
 *   how : ug_send_result()
 */



enum {
	ATTACH_PANEL_CONTENT_CATEGORY_APP = 0,
	ATTACH_PANEL_CONTENT_CATEGORY_UG,
};



typedef enum {
	ATTACH_PANEL_STATE_HIDE,
	ATTACH_PANEL_STATE_HALF,
	ATTACH_PANEL_STATE_FULL,
} attach_panel_state_e;



struct _attach_panel {
	Evas_Object *win;
	Evas_Object *attach_panel_rect;
	Evas_Object *conformant;
	Evas_Object *ui_manager;
	Evas_Object *toolbar;
	Evas_Object *scroller;
	Evas_Object *gesture;
	Evas_Object *grid;
	Evas_Object *grid_page;
	Elm_Object_Item *grid_tabbar_item;

	Eina_List *content_list;

	Ecore_Idler *append_idler;

	attach_panel_state_e attach_panel_port_state;
	attach_panel_state_e attach_panel_land_state;
	attach_panel_result_cb result_cb;
	void *result_data;
	attach_panel_event_cb event_cb;
	void *event_data;

	Evas_Coord transit_width;
	Evas_Coord transit_height;

	Eina_Bool is_delete;
	Eina_Bool rotate;
	int cur_page_no;
	int cur_event_state;
	int magic_no;
};
typedef struct _attach_panel attach_panel_s;



struct _innate_content {
	/* default information */
	const char *appid;
	const char *name;
	const char *icon;
	const char *tabbar_name;
	int content_category;
	int is_ug;

	/* launching features */
	const char *operation;
	const char *type;
	const char *item_type;
	const char *selection_mode;
	const char *mime;
	int mode;
	int max;
};
typedef struct _innate_content innate_content_s;



struct _content {
	attach_panel_s *attach_panel;
	innate_content_s *innate_content_info;
	Elm_Object_Item *tabbar_item;
	Elm_Object_Item *grid_item;
	Evas_Object *content;
	Evas_Object *page;
	bundle *extra_data;
	int index;
	int order;
	Eina_Bool flick;
};
typedef struct _content content_s;



extern void _attach_panel_del(attach_panel_s *attach_panel);



#endif // __TIZEN_ATTACH_PANEL_INTERNAL_H__
