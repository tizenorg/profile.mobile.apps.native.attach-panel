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

#ifndef __TIZEN_ATTACH_PANEL_CONTENT_LIST_H__
#define __TIZEN_ATTACH_PANEL_CONTENT_LIST_H__

#include <Elementary.h>



void _content_list_send_message(Eina_List *list, const char *key, const char *value, int is_ug);
void _content_list_send_message_to_content(Eina_List *list, const char *key, const char *value, int cur_page_no);
void _content_list_set_pause(Eina_List *list, int is_ug);
void _content_list_set_resume(Eina_List *list, int is_ug);


#endif
