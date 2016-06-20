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

#ifndef __TIZEN_ATTACH_PANEL_UI_MANAGER_H__
#define __TIZEN_ATTACH_PANEL_UI_MANAGER_H__

#include <Elementary.h>

extern int _ui_manager_append_content_category(Evas_Object *ui_manager, innate_content_s *innate_content_info, bundle *extra_data);
extern void _ui_manager_remove_content_category(Evas_Object *ui_manager, content_s *content_info);

extern Evas_Object *_ui_manager_create(attach_panel_h attach_panel);
extern void _ui_manager_destroy(Evas_Object *ui_manager);
extern Evas_Object *_ui_manager_create_content(Evas_Object *page, content_s *content_info, attach_panel_h attach_panel);
extern void _ui_manager_destroy_content(content_s *content_info, attach_panel_h attach_panel);

#endif /* __TIZEN_ATTACH_PANEL_UI_MANAGER_H__ */