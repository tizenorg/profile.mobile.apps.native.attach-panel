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
#include <bundle.h>
#include <Elementary.h>
#include <ui-gadget.h>
#include <tizen.h>

#include "attach_panel.h"
#include "attach_panel_internal.h"
#include "log.h"



static void __foreach_bundle_to_add_cb(const char *key, const int type, bundle_keyval_t *kv, void *data)
{
	app_control_h app_control = data;
	char *basic_val = NULL;
	void **arr = NULL;

	size_t size = 0;
	size_t *array_element_size = NULL;
	unsigned int array_length = 0;
	int ret = BUNDLE_ERROR_NONE;

	ret_if(!key);
	ret_if(!app_control);

	_D("key: %s", key);

	switch(type) {
	case BUNDLE_TYPE_STR:
		ret = bundle_keyval_get_basic_val(kv, (void *) &basic_val, &size);
		ret_if(ret != BUNDLE_ERROR_NONE);
		ret_if(!basic_val);

		ret = app_control_add_extra_data(app_control, key, basic_val);
		ret_if(ret != APP_CONTROL_ERROR_NONE);
		break;

	case BUNDLE_TYPE_STR_ARRAY:
		ret = bundle_keyval_get_array_val(kv, &arr, &array_length, &array_element_size);
		ret_if(ret != BUNDLE_ERROR_NONE);
		ret_if(!arr);

		ret = app_control_add_extra_data_array(app_control, key, (const char **) arr, array_length);
		ret_if(ret != APP_CONTROL_ERROR_NONE);
		break;

	case BUNDLE_TYPE_BYTE:
		ret = bundle_keyval_get_basic_val(kv, (void *) &basic_val, &size);
		ret_if(ret != BUNDLE_ERROR_NONE);
		ret_if(!basic_val);

		ret = app_control_add_extra_data(app_control, key, basic_val);
		ret_if(ret != APP_CONTROL_ERROR_NONE);
		break;

	case BUNDLE_TYPE_BYTE_ARRAY:
		ret = bundle_keyval_get_array_val(kv, &arr, &array_length, &array_element_size);
		ret_if(ret != BUNDLE_ERROR_NONE);
		ret_if(!arr);

		ret = app_control_add_extra_data_array(app_control, key, (const char **) arr, array_length);
		ret_if(ret != APP_CONTROL_ERROR_NONE);
		break;

	default:
		_E("There is wrong type");
		break;
	}
}



int _bundle_add_to_app_control(bundle *b, app_control_h app_control)
{
	retv_if(!b, ATTACH_PANEL_ERROR_INVALID_PARAMETER);
	retv_if(!app_control, ATTACH_PANEL_ERROR_INVALID_PARAMETER);

	bundle_foreach(b, (void *) __foreach_bundle_to_add_cb, app_control);
	retv_if(get_last_result() != BUNDLE_ERROR_NONE, ATTACH_PANEL_ERROR_OUT_OF_MEMORY);

	return ATTACH_PANEL_ERROR_NONE;
}



static void __foreach_bundle_to_change_cb(const char *key, const int type, bundle_keyval_t *kv, void *data)
{
	bundle *origin = data;
	char *basic_val = NULL;
	void **arr = NULL;

	size_t size;
	size_t *array_element_size = NULL;
	unsigned int array_length = 0;
	int ret = BUNDLE_ERROR_NONE;

	ret_if(!key);
	ret_if(!origin);

	switch(type) {
	case BUNDLE_TYPE_STR:
		ret = bundle_keyval_get_basic_val(kv, (void *) &basic_val, &size);
		ret_if(ret != BUNDLE_ERROR_NONE);
		ret_if(!basic_val);

		ret = bundle_del(origin, key);
		ret_if(ret != BUNDLE_ERROR_NONE && ret != BUNDLE_ERROR_KEY_NOT_AVAILABLE);

		ret = bundle_add_str(origin, key, basic_val);
		ret_if(ret != BUNDLE_ERROR_NONE);
		break;

	case BUNDLE_TYPE_STR_ARRAY:
		ret = bundle_keyval_get_array_val(kv, &arr, &array_length, &array_element_size);
		ret_if(ret != BUNDLE_ERROR_NONE);
		ret_if(!arr);

		ret = bundle_del(origin, key);
		ret_if(ret != BUNDLE_ERROR_NONE && ret != BUNDLE_ERROR_KEY_NOT_AVAILABLE);

		ret = bundle_add_str_array(origin, key, (const char **) arr, array_length);
		ret_if(ret != BUNDLE_ERROR_NONE);
		break;

	case BUNDLE_TYPE_BYTE:
		ret = bundle_keyval_get_basic_val(kv, (void *) &basic_val, &size);
		ret_if(ret != BUNDLE_ERROR_NONE);
		ret_if(!basic_val);

		ret = bundle_del(origin, key);
		ret_if(ret != BUNDLE_ERROR_NONE && ret != BUNDLE_ERROR_KEY_NOT_AVAILABLE);

		ret = bundle_add_byte(origin, key, basic_val, size);
		ret_if(ret != BUNDLE_ERROR_NONE);
		break;

	case BUNDLE_TYPE_BYTE_ARRAY:
		ret = bundle_keyval_get_array_val(kv, &arr, &array_length, &array_element_size);
		ret_if(ret != BUNDLE_ERROR_NONE);
		ret_if(!arr);

		ret = bundle_del(origin, key);
		ret_if(ret != BUNDLE_ERROR_NONE && ret != BUNDLE_ERROR_KEY_NOT_AVAILABLE);

		ret = bundle_add_str_array(origin, key, (const char **) arr, array_length);
		ret_if(ret != BUNDLE_ERROR_NONE);
		break;

	default:
		_E("There is wrong type");
		break;
	}
}



int _bundle_add_to_bundle(bundle *origin, bundle *replace)
{
	retv_if(!origin, ATTACH_PANEL_ERROR_INVALID_PARAMETER);
	retv_if(!replace, ATTACH_PANEL_ERROR_INVALID_PARAMETER);

	bundle_foreach(replace, (void *) __foreach_bundle_to_change_cb, origin);
	retv_if(get_last_result() != BUNDLE_ERROR_NONE, ATTACH_PANEL_ERROR_OUT_OF_MEMORY);

	return ATTACH_PANEL_ERROR_NONE;
}
