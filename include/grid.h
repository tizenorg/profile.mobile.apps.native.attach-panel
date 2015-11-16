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

#ifndef __ATTACH_PANEL_GRID_H__
#define __ATTACH_PANEL_GRID_H__

#include <Elementary.h>

extern Evas_Object *_grid_create(Evas_Object *page, attach_panel_h attach_panel);
extern void _grid_destroy(Evas_Object *grid);
extern void _grid_refresh(Evas_Object *grid);

extern Elm_Object_Item *_grid_append_item(Evas_Object *grid, content_s *content_info);
extern void _grid_remove_item(Evas_Object *grid, content_s *content_info);

extern int _grid_count_item(Evas_Object *grid);
extern Eina_Bool _grid_can_flick(attach_panel_h attach_panel);

#endif /* __ATTACH_PANEL_GRID_H__ */
