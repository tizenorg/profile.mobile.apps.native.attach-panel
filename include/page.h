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

#ifndef __ATTACH_PANEL_PAGE_H__
#define __ATTACH_PANEL_PAGE_H__

#include <Elementary.h>

extern Evas_Object *_page_create(attach_panel_h attach_panel, int width, int height);
extern void _page_destroy(Evas_Object *page);

extern void _page_resize(Evas_Object *page, int width, int height);

#endif /* __ATTACH_PANEL_PAGE_H__ */