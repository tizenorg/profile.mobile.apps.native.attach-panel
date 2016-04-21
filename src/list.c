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

#include <aul.h>
#include <Elementary.h>
#include <tizen.h>
#include <rua.h>
#include <rua_stat.h>

#include "attach_panel.h"
#include "attach_panel_internal.h"
#include "conf.h"
#include "log.h"



static struct {
	int ordering;
} list_info = {
	.ordering = 1,
};

static int __rua_stat_tag_iter_cb(const char *rua_stat_tag, void *data)
{
	Eina_List *content_list = data;
	Eina_List *l = NULL;
	Eina_List *ln = NULL;
	content_s *content_info = NULL;

	retv_if(!content_list, -1);
	retv_if(!rua_stat_tag, -1);

	_D("[Rua] %d : %s", list_info.ordering, rua_stat_tag);
	EINA_LIST_FOREACH_SAFE(content_list, l, ln, content_info) {
		continue_if(!content_info);
		continue_if(!content_info->innate_content_info);
		if (!strcmp(rua_stat_tag, content_info->innate_content_info->appid)) {
			content_info->order = list_info.ordering;
			list_info.ordering++;
		}
	}

	return 0;
}



static int __sort_cb(const void *d1, const void *d2)
{
	content_s *content_info1 = (content_s *) d1;
	content_s *content_info2 = (content_s *) d2;

	if (!content_info1 || !content_info1->innate_content_info) {
		return 1;
	}
	if (!content_info2 || !content_info2->innate_content_info) {
		return -1;
	}
	if (content_info1->innate_content_info->is_ug || content_info2->innate_content_info->is_ug) {
		return -1;
	}
	if (content_info1->order < content_info2->order) {
		return -1;
	} else if (content_info1->order > content_info2->order) {
		return 1;
	} else {
		return (strcmp(content_info1->innate_content_info->appid, content_info2->innate_content_info->appid));
	}
}



Eina_List * _list_sort_by_rua(Eina_List *content_list)
{
	int ret = 0;
	retv_if(!content_list, NULL);

	list_info.ordering = 1;
	ret = rua_stat_get_stat_tags("attach-panel", __rua_stat_tag_iter_cb, content_list);
	retv_if(0 != ret, content_list);

	content_list = eina_list_sort(content_list, eina_list_count(content_list), __sort_cb);

	return content_list;
}
