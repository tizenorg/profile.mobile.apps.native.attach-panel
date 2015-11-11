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

#ifndef __TIZEN_ATTACH_PANEL_H__
#define __TIZEN_ATTACH_PANEL_H__

#include <Elementary.h>
#include <tizen_error.h>
#include <app_control.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/** This definition will be removed */
#ifndef TIZEN_ERROR_PANEL
#define TIZEN_ERROR_PANEL		-0x02F20000
#endif

/**
 * @file attach_panel.h
 * @brief Declares the API of the libattach-panel library.
 */

/**
 * @addtogroup CAPI_PANEL_ATTACH_MODULE
 * @{
 */

/**
 * @brief Enumeration for content categories
 * @since_tizen 2.4
 * @see attach_panel_add_content_category()
 * @see attach_panel_remove_content_category()
 */
typedef enum attach_panel_content_category {
	ATTACH_PANEL_CONTENT_CATEGORY_IMAGE = 1, /**< Attaching images from the gallery */
	ATTACH_PANEL_CONTENT_CATEGORY_CAMERA, /**< Taking pictures or videos with the camera to attach */
	ATTACH_PANEL_CONTENT_CATEGORY_VOICE, /**< Taking voices with the voice-recoder to attach */
	ATTACH_PANEL_CONTENT_CATEGORY_VIDEO, /**< Attaching video from the gallery */
	ATTACH_PANEL_CONTENT_CATEGORY_AUDIO, /**< Attaching audio from my files */
	ATTACH_PANEL_CONTENT_CATEGORY_CALENDAR, /**< Attaching calendar data from the calendar */
	ATTACH_PANEL_CONTENT_CATEGORY_CONTACT, /**< Attaching contact data from the contacts */
	ATTACH_PANEL_CONTENT_CATEGORY_MYFILES, /**< Attaching files data from my files */
} attach_panel_content_category_e;

/**
 * @brief Attach panel handle.
 * @since_tizen 2.4
 */
typedef struct _attach_panel *attach_panel_h;

/**
 * @brief Called when an user selects and confirms something to attach on the caller app.
 *
 * @since_tizen 2.4
 * @param[in] attach_panel Attach panel handler
 * @param[in] content_category results are from the content category
 * @param[in] result app_control handler.\n
 *                   The caller app has to use app_control_get_extra_data_array() to get received data.\n
 *                   http://tizen.org/appcontrol/data/selected\n
 * @param[in] user_data user data
 * @pre The callback must be registered using attach_panel_set_result_cb()\n
 * attach_panel_add_content_category() and attach_panel_show() must be called to invoke this callback.
 *
 * @see @ref CAPI_APP_CONTROL_MODULE API app_control_get_extra_data_array()
 * @see	attach_panel_set_result_cb()
 * @see	attach_panel_unset_result_cb()
 */
typedef void (*attach_panel_result_cb)(attach_panel_h attach_panel, attach_panel_content_category_e content_category, app_control_h result, void *user_data);

/**
 * @brief Enumeration for values of attach-panel response types.
 * @since_tizen 2.4
 */
typedef enum attach_panel_error {
	ATTACH_PANEL_ERROR_NONE = TIZEN_ERROR_NONE, /**< Successfully handled */
	ATTACH_PANEL_ERROR_INVALID_PARAMETER = TIZEN_ERROR_INVALID_PARAMETER, /**< Request is not valid, invalid parameter or invalid argument value */
	ATTACH_PANEL_ERROR_OUT_OF_MEMORY = TIZEN_ERROR_OUT_OF_MEMORY, /**< Memory is not enough to handle a new request */
	ATTACH_PANEL_ERROR_PERMISSION_DENIED = TIZEN_ERROR_PERMISSION_DENIED, /**< Has no permission to attach contents */
	ATTACH_PANEL_ERROR_ALREADY_EXISTS = TIZEN_ERROR_PANEL | 0x01, /**< There is already a panel in the conformant */
	ATTACH_PANEL_ERROR_NOT_INITIALIZED = TIZEN_ERROR_PANEL | 0x02, /**< The panel is not initialized yet */
	ATTACH_PANEL_ERROR_UNSUPPORTED_CONTENT_CATEGORY = TIZEN_ERROR_PANEL | 0x03, /**< Not supported content category */
	ATTACH_PANEL_ERROR_ALREADY_DESTROYED = TIZEN_ERROR_PANEL | 0x05, /**< The panel is already removed */
} attach_panel_error_e;

/**
 * @brief Creates an attach panel.
 * @since_tizen 2.4
 * @remarks The caller app has to check the return value of this function.\n
 *          A conformant object can have only one @a attach_panel_h.\n
 *          If a caller app try to add more than one attach-panel, it fails to add it.
 *
 * @param[in] conformant The caller's conformant
 * @param[out] attach_panel Attach panel handler
 * @return #ATTACH_PANEL_ERROR_NONE on success,
 *         otherwise a negative error value
 * @retval #ATTACH_PANEL_ERROR_NONE Successful
 * @retval #ATTACH_PANEL_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #ATTACH_PANEL_ERROR_OUT_OF_MEMORY Out of memory
 * @retval #ATTACH_PANEL_ERROR_ALREADY_EXISTS Already exists
 *
 * @see attach_panel_destroy()
 * @see attach_panel_add_content_category()
 * @see attach_panel_remove_content_category()
 * @see attach_panel_set_extra_data()
 * @see attach_panel_set_result_cb()
 * @see attach_panel_unset_result_cb()
 * @see attach_panel_show()
 * @see attach_panel_hide()
 * @see attach_panel_get_visibility()
 *
 * @par Example
 * @code
 * #include <attach_panel.h>
 *
 * struct appdata {
 *   Evas_Object *attach_panel;
 *   Evas_Object *conformant;
 * };
 *
 * static void _result_cb(attach_panel_h attach_panel, attach_panel_content_category_e content_category, app_control_h result, void *data)
 * {
 *   char **select = NULL;
 *   int i = 0;
 *   int length = 0;
 *   int ret = APP_CONTROL_ERROR_NONE;
 *
 *   if (!result) {
 *     // Error handling
 *   }
 *
 *   ret = app_control_get_extra_data_array(result, "http://tizen.org/appcontrol/data/selected", &select, &length);
 *   if (APP_CONTROL_ERROR_NONE != ret || !select) {
 *     // Error handling
 *   }
 *
 *   for (; i < length; i++) {
 *      printf("path is %s, %d\n", select[i], length);
 *      free(select[i]);
 *   }
 *
 *   free(select);
 * }
 *
 * static int app_control(void *data)
 * {
 *   struct appdata *ad = data;
 *   bundle *extra_data = NULL;
 *   int ret = ATTACH_PANEL_ERROR_NONE;
 *
 *   if (!ad) {
 *     // Error handling
 *   }
 *
 *   if (!ad->conformant) {
 *     // Error handling
 *   }
 *
 *   ret = attach_panel_create(ad->conformant, &ad->attach_panel);
 *   if (ATTACH_PANEL_ERROR_NONE != ret) {
 *      // Error handling
 *   }
 *
 *   extra_data = bundle_create();
 *   if (!extra_data) {
 *      // Error handling
 *   }
 *
 *   bundle_add_str(extra_data, "http://tizen.org/appcontrol/data/total_count", "3");
 *   bundle_add_str(extra_data, "http://tizen.org/appcontrol/data/total_size", "10240000");
 *
 *   ret = attach_panel_add_content_category(ad->attach_panel, ATTACH_PANEL_CONTENT_CATEGORY_IMAGE, extra_data);
 *   if (ATTACH_PANEL_ERROR_NONE != ret) {
 *      // Error handling
 *   }
 *
 *   ret = attach_panel_add_content_category(ad->attach_panel, ATTACH_PANEL_CONTENT_CATEGORY_CAMERA, extra_data);
 *   if (ATTACH_PANEL_ERROR_NONE != ret) {
 *      // Error handling
 *   }
 *
 *   ret = attach_panel_add_content_category(ad->attach_panel, ATTACH_PANEL_CONTENT_CATEGORY_VOICE, extra_data);
 *   if (ATTACH_PANEL_ERROR_NONE != ret) {
 *      // Error handling
 *   }
 *
 *   ret = attach_panel_set_result_cb(ad->attach_panel, _result_cb, NULL);
 *   if (ATTACH_PANEL_ERROR_NONE != ret) {
 *      // Error handling
 *   }
 *
 *   ret = attach_panel_show(ad->attach_panel);
 *   if (ATTACH_PANEL_ERROR_NONE != ret) {
 *      // Error handling
 *   }
 *
 *   bundle_free(extra_data);
 *
 * 	 return 0;
 * }
 *
 * @endcode
 */
extern int attach_panel_create(Evas_Object *conformant, attach_panel_h *attach_panel);

/**
 * @brief Destroys the attach panel.
 * @since_tizen 2.4
 * @remarks The caller app has to check the return value of this function.\n
 *
 * @param[in] attach_panel Attach panel handler
 * @return #ATTACH_PANEL_ERROR_NONE on success,
 *         otherwise a negative error value
 * @retval #ATTACH_PANEL_ERROR_NONE Successful
 * @retval #ATTACH_PANEL_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #ATTACH_PANEL_ERROR_ALREADY_DESTROYED already removed
 *
 * @see attach_panel_create()
 * @see attach_panel_add_content_category()
 * @see attach_panel_remove_content_category()
 * @see attach_panel_set_extra_data()
 * @see attach_panel_set_result_cb()
 * @see attach_panel_unset_result_cb()
 * @see attach_panel_show()
 * @see attach_panel_hide()
 * @see attach_panel_get_visibility()
 *
 * @par Example
 * @code
 * #include <attach_panel.h>
 *
 * struct appdata {
 *   Evas_Object *attach_panel;
 *   Evas_Object *conformant;
 * };
 *
 * static int app_terminate(void *data)
 * {
 *   struct appdata *ad = data;
 *   int ret = 0;
 *
 *   if (!ad) {
 *     // Error handling
 *   }
 *
 *   if (!ad->attach_panel) {
 *     // Error handling
 *   }
 *
 *   ret = attach_panel_hide(ad->attach_panel);
 *   if (ATTACH_PANEL_ERROR_NONE != ret) {
 *      // Error handling
 *   }
 *
 *   ret = attach_panel_remove_content_category(ad->attach_panel, ATTACH_PANEL_CONTENT_CATEGORY_IMAGE);
 *   if (ATTACH_PANEL_ERROR_NONE != ret) {
 *      // Error handling
 *   }
 *
 *   ret = attach_panel_remove_content_category(ad->attach_panel, ATTACH_PANEL_CONTENT_CATEGORY_CAMERA);
 *   if (ATTACH_PANEL_ERROR_NONE != ret) {
 *      // Error handling
 *   }
 *
 *   ret = attach_panel_remove_content_category(ad->attach_panel, ATTACH_PANEL_CONTENT_CATEGORY_VOICE);
 *   if (ATTACH_PANEL_ERROR_NONE != ret) {
 *      // Error handling
 *   }
 *
 *   ret = attach_panel_unset_result_cb(ad->attach_panel);
 *   if (ATTACH_PANEL_ERROR_NONE != ret) {
 *      // Error handling
 *   }
 *
 *   ret = attach_panel_destroy(ad->attach_panel);
 *   if (ATTACH_PANEL_ERROR_NONE != ret) {
 *      // Error handling
 *   }
 *   ad->attach_panel = NULL;
 *
 * 	 return 0;
 * }
 *
 * @endcode
 */
extern int attach_panel_destroy(attach_panel_h attach_panel);

/**
 * @brief Adds a content category in the attach panel.
 * @since_tizen 2.4
 * @privlevel public
 * @remarks The caller app has to check the return value of this function.\n
 *          Content categories will be shown as the sequence of using @a attach_panel_add_content_category.\n
 *          Some contents need time to load it all.\n
 *          So, it is needed to use this before the mainloop of attach_panel_show().\n
 *          Privileges,\n
 *          %http://tizen.org/privilege/camera, for using ATTACH_PANEL_CONTENT_CATEGORY_CAMERA\n
 *          %http://tizen.org/privilege/recorder, for using ATTACH_PANEL_CONTENT_CATEGORY_VOICE\n
 *          %http://tizen.org/privilege/appmanager.launch, for adding content categories on the More tab\n
 *          Deliver more information to the callee with a bundle if you need.\n
 *          %http://tizen.org/appcontrol/data/total_count\n
 *          %http://tizen.org/appcontrol/data/total_size
 *
 * @param[in] attach_panel Attach panel handler
 * @param[in] content_category The content_category to be added in the @a attach_panel.
 * @param[in] The attach panel send some information using @a bundle.
 * @return #ATTACH_PANEL_ERROR_NONE on success,
 *         otherwise a negative error value
 * @retval #ATTACH_PANEL_ERROR_NONE Successful
 * @retval #ATTACH_PANEL_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #ATTACH_PANEL_ERROR_PERMISSION_DENIED permission denied
 * @retval #ATTACH_PANEL_ERROR_NOT_INITIALIZED not initialized
 * @retval #ATTACH_PANEL_ERROR_UNSUPPORTED_CONTENT_CATEGORY not supported content category
 * @retval #ATTACH_PANEL_ERROR_ALREADY_DESTROYED already removed
 *
 * @pre Call attach_panel_create() before calling this function.
 * @see attach_panel_create()
 * @see attach_panel_destroy()
 * @see attach_panel_remove_content_category()
 * @see attach_panel_set_result_cb()
 * @see attach_panel_unset_result_cb()
 * @see attach_panel_show()
 * @see attach_panel_hide()
 * @see attach_panel_get_visibility()
 * @see ATTACH_PANEL_CONTENT_CATEGORY_IMAGE
 * @see ATTACH_PANEL_CONTENT_CATEGORY_CAMERA
 * @see ATTACH_PANEL_CONTENT_CATEGORY_VOICE
 * @see ATTACH_PANEL_CONTENT_CATEGORY_VIDEO
 * @see ATTACH_PANEL_CONTENT_CATEGORY_AUDIO
 * @see ATTACH_PANEL_CONTENT_CATEGORY_CALENDAR
 * @see ATTACH_PANEL_CONTENT_CATEGORY_CONTACT
 * @see ATTACH_PANEL_CONTENT_CATEGORY_MYFILES
 *
 * @par Example
 * @code
 * #include <attach_panel.h>
 *
 * struct appdata {
 *   Evas_Object *attach_panel;
 *   Evas_Object *conformant;
 * };
 *
 * static void _result_cb(attach_panel_h attach_panel, attach_panel_content_category_e content_category, app_control_h result, void *data)
 * {
 *   char **select = NULL;
 *   int i = 0;
 *   int length = 0;
 *   int ret = APP_CONTROL_ERROR_NONE;
 *
 *   if (!result) {
 *     // Error handling
 *   }
 *
 *   ret = app_control_get_extra_data_array(result, "http://tizen.org/appcontrol/data/selected", &select, &length);
 *   if (APP_CONTROL_ERROR_NONE != ret || !select) {
 *     // Error handling
 *   }
 *
 *   for (; i < length; i++) {
 *      printf("path is %s, %d\n", select[i], length);
 *      free(select[i]);
 *   }
 *
 *   free(select);
 * }
 *
 * static int app_control(void *data)
 * {
 *   struct appdata *ad = data;
 *   bundle *extra_data = NULL;
 *   int ret = ATTACH_PANEL_ERROR_NONE;
 *
 *   if (!ad) {
 *     // Error handling
 *   }
 *
 *   if (!ad->conformant) {
 *     // Error handling
 *   }
 *
 *   ret = attach_panel_create(ad->conformant, &ad->attach_panel);
 *   if (ATTACH_PANEL_ERROR_NONE != ret) {
 *      // Error handling
 *   }
 *
 *   extra_data = bundle_create();
 *   if (!extra_data) {
 *      // Error handling
 *   }
 *
 *   bundle_add_str(extra_data, "http://tizen.org/appcontrol/data/total_count", "3");
 *   bundle_add_str(extra_data, "http://tizen.org/appcontrol/data/total_size", "10240000");
 *
 *   ret = attach_panel_add_content_category(ad->attach_panel, ATTACH_PANEL_CONTENT_CATEGORY_IMAGE, extra_data);
 *   if (ATTACH_PANEL_ERROR_NONE != ret) {
 *      // Error handling
 *   }
 *
 *   ret = attach_panel_add_content_category(ad->attach_panel, ATTACH_PANEL_CONTENT_CATEGORY_CAMERA, extra_data);
 *   if (ATTACH_PANEL_ERROR_NONE != ret) {
 *      // Error handling
 *   }
 *
 *   ret = attach_panel_add_content_category(ad->attach_panel, ATTACH_PANEL_CONTENT_CATEGORY_VOICE, extra_data);
 *   if (ATTACH_PANEL_ERROR_NONE != ret) {
 *      // Error handling
 *   }
 *
 *   ret = attach_panel_set_result_cb(ad->attach_panel, _result_cb, NULL);
 *   if (ATTACH_PANEL_ERROR_NONE != ret) {
 *      // Error handling
 *   }
 *
 *   ret = attach_panel_show(ad->attach_panel);
 *   if (ATTACH_PANEL_ERROR_NONE != ret) {
 *      // Error handling
 *   }
 *
 *   bundle_free(extra_data);
 *
 * 	 return 0;
 * }
 *
 * @endcode
 */
extern int attach_panel_add_content_category(attach_panel_h attach_panel, attach_panel_content_category_e content_category, bundle * extra_data);

/**
 * @brief Removes the content category from the attach panel.
 * @since_tizen 2.4
 * @remarks The caller app has to check the return value of this function.\n
 *
 * @param[in] attach_panel Attach panel handler
 * @param[in] content_category The content_category adding in the @a attach_panel.
 * @return #ATTACH_PANEL_ERROR_NONE on success,
 *         otherwise a negative error value
 * @retval #ATTACH_PANEL_ERROR_NONE Successful
 * @retval #ATTACH_PANEL_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #ATTACH_PANEL_ERROR_NOT_INITIALIZED not initialized
 * @retval #ATTACH_PANEL_ERROR_ALREADY_DESTROYED already removed
 *
 * @pre Call attach_panel_create() before calling this function.
 * @see attach_panel_create()
 * @see attach_panel_destroy()
 * @see attach_panel_add_content_category()
 * @see attach_panel_set_extra_data()
 * @see attach_panel_set_result_cb()
 * @see attach_panel_unset_result_cb()
 * @see attach_panel_show()
 * @see attach_panel_hide()
 * @see attach_panel_get_visibility()
 * @see ATTACH_PANEL_CONTENT_CATEGORY_IMAGE
 * @see ATTACH_PANEL_CONTENT_CATEGORY_CAMERA
 * @see ATTACH_PANEL_CONTENT_CATEGORY_VOICE
 * @see ATTACH_PANEL_CONTENT_CATEGORY_VIDEO
 * @see ATTACH_PANEL_CONTENT_CATEGORY_AUDIO
 * @see ATTACH_PANEL_CONTENT_CATEGORY_CALENDAR
 * @see ATTACH_PANEL_CONTENT_CATEGORY_CONTACT
 * @see ATTACH_PANEL_CONTENT_CATEGORY_MYFILES
 *
 * @par Example
 * @code
 * #include <attach_panel.h>
 *
 * struct appdata {
 *   Evas_Object *attach_panel;
 *   Evas_Object *conformant;
 * };
 *
 * static int app_terminate(void *data)
 * {
 *   struct appdata *ad = data;
 *   int ret = 0;
 *
 *   if (!ad) {
 *     // Error handling
 *   }
 *
 *   if (!ad->attach_panel) {
 *     // Error handling
 *   }
 *
 *   ret = attach_panel_hide(ad->attach_panel);
 *   if (ATTACH_PANEL_ERROR_NONE != ret) {
 *      // Error handling
 *   }
 *
 *   ret = attach_panel_remove_content_category(ad->attach_panel, ATTACH_PANEL_CONTENT_CATEGORY_IMAGE);
 *   if (ATTACH_PANEL_ERROR_NONE != ret) {
 *      // Error handling
 *   }
 *
 *   ret = attach_panel_remove_content_category(ad->attach_panel, ATTACH_PANEL_CONTENT_CATEGORY_CAMERA);
 *   if (ATTACH_PANEL_ERROR_NONE != ret) {
 *      // Error handling
 *   }
 *
 *   ret = attach_panel_remove_content_category(ad->attach_panel, ATTACH_PANEL_CONTENT_CATEGORY_VOICE);
 *   if (ATTACH_PANEL_ERROR_NONE != ret) {
 *      // Error handling
 *   }
 *
 *   ret = attach_panel_unset_result_cb(ad->attach_panel);
 *   if (ATTACH_PANEL_ERROR_NONE != ret) {
 *      // Error handling
 *   }
 *
 *   ret = attach_panel_destroy(ad->attach_panel);
 *   if (ATTACH_PANEL_ERROR_NONE != ret) {
 *      // Error handling
 *   }
 *   ad->attach_panel = NULL;
 *
 *   return 0;
 * }
 *
 * @endcode
 */
extern int attach_panel_remove_content_category(attach_panel_h attach_panel, attach_panel_content_category_e content_category);

/**
 * @brief Sets extra data to send to the content category using a bundle.
 * @since_tizen 2.4
 * @privlevel public
 * @remarks The caller app has to check the return value of this function.\n
 *          extra data will be changed using @a attach_panel_set_extra_data.\n
 *          %http://tizen.org/appcontrol/data/total_count\n
 *          %http://tizen.org/appcontrol/data/total_size
 *
 * @param[in] attach_panel Attach panel handler
 * @param[in] content_category The content_category to be set the some information in the @a attach_panel.
 * @param[in] The attach panel set some information using @a bundle.
 * @return #ATTACH_PANEL_ERROR_NONE on success,
 *         otherwise a negative error value
 * @retval #ATTACH_PANEL_ERROR_NONE Successful
 * @retval #ATTACH_PANEL_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #ATTACH_PANEL_ERROR_ALREADY_DESTROYED already removed
 * @retval #ATTACH_PANEL_ERROR_OUT_OF_MEMORY Fail to set the extra data
 *
 * @pre Call attach_panel_create() before calling this function.
 * @see attach_panel_create()
 * @see attach_panel_destroy()
 * @see attach_panel_add_content_category()
 * @see attach_panel_remove_content_category()
 * @see attach_panel_set_result_cb()
 * @see attach_panel_unset_result_cb()
 * @see attach_panel_show()
 * @see attach_panel_hide()
 * @see attach_panel_get_visibility()
 * @see ATTACH_PANEL_CONTENT_CATEGORY_IMAGE
 * @see ATTACH_PANEL_CONTENT_CATEGORY_CAMERA
 * @see ATTACH_PANEL_CONTENT_CATEGORY_VOICE
 * @see ATTACH_PANEL_CONTENT_CATEGORY_VIDEO
 * @see ATTACH_PANEL_CONTENT_CATEGORY_AUDIO
 * @see ATTACH_PANEL_CONTENT_CATEGORY_CALENDAR
 * @see ATTACH_PANEL_CONTENT_CATEGORY_CONTACT
 * @see ATTACH_PANEL_CONTENT_CATEGORY_MYFILES
 *
 * @par Example
 * @code
 * #include <attach_panel.h>
 *
 * struct appdata {
 *   Evas_Object *attach_panel;
 *   Evas_Object *conformant;
 * };
 *
 * static void _result_cb(attach_panel_h attach_panel, attach_panel_content_category_e content_category, app_control_h result, void *data)
 * {
 *   char **select = NULL;
 *   int i = 0;
 *   int length = 0;
 *   int ret = APP_CONTROL_ERROR_NONE;
 *
 *   if (!result) {
 *     // Error handling
 *   }
 *
 *   ret = app_control_get_extra_data_array(result, "http://tizen.org/appcontrol/data/selected", &select, &length);
 *   if (APP_CONTROL_ERROR_NONE != ret || !select) {
 *     // Error handling
 *   }
 *
 *   for (; i < length; i++) {
 *      printf("path is %s, %d\n", select[i], length);
 *      free(select[i]);
 *   }
 *
 *   free(select);
 * }
 *
 * static void _event_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
 * {
 *   struct appdata *ad = data;
 *   bundle *extra_data = NULL;
 *   int ret = APP_CONTROL_ERROR_NONE;
 *
 *   if (!ad) {
 *     // Error handling
 *   }
 *
 *   extra_data = bundle_create();
 *   if (!extra_data) {
 *      // Error handling
 *   }
 *
 *   bundle_add_str(extra_data, "http://tizen.org/appcontrol/data/total_count", "5");
 *   bundle_add_str(extra_data, "http://tizen.org/appcontrol/data/total_size", "20480000");
 *
 *   ret = attach_panel_set_extra_data(ad->attach_panel, ATTACH_PANEL_CONTENT_CATEGORY_IMAGE, extra_data);
 *   if (ATTACH_PANEL_ERROR_NONE != ret) {
 *      // Error handling
 *   }
 *
 *   ret = attach_panel_set_extra_data(ad->attach_panel, ATTACH_PANEL_CONTENT_CATEGORY_CAMERA, extra_data);
 *   if (ATTACH_PANEL_ERROR_NONE != ret) {
 *      // Error handling
 *   }
 *
 *   ret = attach_panel_set_extra_data(ad->attach_panel, ATTACH_PANEL_CONTENT_CATEGORY_VOICE, extra_data);
 *   if (ATTACH_PANEL_ERROR_NONE != ret) {
 *      // Error handling
 *   }
 *
 *   bundle_free(extra_data);
 *
 * }
 *
 * static int app_control(void *data)
 * {
 *   struct appdata *ad = data;
 *   bundle *extra_data = NULL;
 *   int ret = ATTACH_PANEL_ERROR_NONE;
 *
 *   if (!ad) {
 *     // Error handling
 *   }
 *
 *   if (!ad->conformant) {
 *     // Error handling
 *   }
 *
 *   ret = attach_panel_create(ad->conformant, &ad->attach_panel);
 *   if (ATTACH_PANEL_ERROR_NONE != ret) {
 *      // Error handling
 *   }
 *
 *   extra_data = bundle_create();
 *   if (!extra_data) {
 *      // Error handling
 *   }
 *
 *   bundle_add_str(extra_data, "http://tizen.org/appcontrol/data/total_count", "3");
 *   bundle_add_str(extra_data, "http://tizen.org/appcontrol/data/total_size", "10240000");
 *
 *   ret = attach_panel_add_content_category(ad->attach_panel, ATTACH_PANEL_CONTENT_CATEGORY_IMAGE, extra_data);
 *   if (ATTACH_PANEL_ERROR_NONE != ret) {
 *      // Error handling
 *   }
 *
 *   ret = attach_panel_add_content_category(ad->attach_panel, ATTACH_PANEL_CONTENT_CATEGORY_CAMERA, extra_data);
 *   if (ATTACH_PANEL_ERROR_NONE != ret) {
 *      // Error handling
 *   }
 *
 *   ret = attach_panel_add_content_category(ad->attach_panel, ATTACH_PANEL_CONTENT_CATEGORY_VOICE, extra_data);
 *   if (ATTACH_PANEL_ERROR_NONE != ret) {
 *      // Error handling
 *   }
 *
 *   ret = attach_panel_set_result_cb(ad->attach_panel, _result_cb, NULL);
 *   if (ATTACH_PANEL_ERROR_NONE != ret) {
 *      // Error handling
 *   }
 *
 *   ret = attach_panel_show(ad->attach_panel);
 *   if (ATTACH_PANEL_ERROR_NONE != ret) {
 *      // Error handling
 *   }
 *
 *   bundle_free(extra_data);
 *
 * 	 return 0;
 * }
 *
 * @endcode
 */
extern int attach_panel_set_extra_data(attach_panel_h attach_panel, attach_panel_content_category_e content_category, bundle *extra_data);

/**
 * @brief Sets the result callback that will be called when an user selects and confirms something to attach in the attach panel.
 * @since_tizen 2.4
 * @remarks The caller app has to check the return value of this function.\n
 *          We can set only one callback function with this API.\n
 *          If you set multiple callbacks with this API,\n
 *          the last one is registered only.
 *
 * @param[in] attach_panel Attach panel handler
 * @param[in] result_cb Attach panel result callback
 * @param[in] user_data User data
 * @return #ATTACH_PANEL_ERROR_NONE on success,
 *         otherwise a negative error value
 * @retval #ATTACH_PANEL_ERROR_NONE Successful
 * @retval #ATTACH_PANEL_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #ATTACH_PANEL_ERROR_ALREADY_DESTROYED already removed
 *
 * @pre Call attach_panel_create() before calling this function.
 * @post The result_cb set with attach_panel_set_result_cb() will be called after an user select something to attach.
 * @see attach_panel_create()
 * @see attach_panel_destroy()
 * @see attach_panel_add_content_category()
 * @see attach_panel_remove_content_category()
 * @see attach_panel_set_extra_data()
 * @see attach_panel_unset_result_cb()
 * @see attach_panel_show()
 * @see attach_panel_hide()
 * @see attach_panel_get_visibility()
 * @see attach_panel_result_cb
 *
 * @par Example
 * @code
 * #include <attach_panel.h>
 *
 * struct appdata {
 *   Evas_Object *attach_panel;
 *   Evas_Object *conformant;
 * };
 *
 * static void _result_cb(attach_panel_h attach_panel, attach_panel_content_category_e content_category, app_control_h result, void *data)
 * {
 *   char **select = NULL;
 *   int i = 0;
 *   int length = 0;
 *   int ret = APP_CONTROL_ERROR_NONE;
 *
 *   if (!result) {
 *     // Error handling
 *   }
 *
 *   ret = app_control_get_extra_data_array(result, "http://tizen.org/appcontrol/data/selected", &select, &length);
 *   if (APP_CONTROL_ERROR_NONE != ret || !select) {
 *     // Error handling
 *   }
 *
 *   for (; i < length; i++) {
 *      printf("path is %s, %d\n", select[i], length);
 *      free(select[i]);
 *   }
 *
 *   free(select);
 * }
 *
 * static int app_control(void *data)
 * {
 *   struct appdata *ad = data;
 *   bundle *extra_data = NULL;
 *   int ret = ATTACH_PANEL_ERROR_NONE;
 *
 *   if (!ad) {
 *     // Error handling
 *   }
 *
 *   if (!ad->conformant) {
 *     // Error handling
 *   }
 *
 *   ret = attach_panel_create(ad->conformant, &ad->attach_panel);
 *   if (ATTACH_PANEL_ERROR_NONE != ret) {
 *      // Error handling
 *   }
 *
 *   extra_data = bundle_create();
 *   if (!extra_data) {
 *      // Error handling
 *   }
 *
 *   bundle_add_str(extra_data, "http://tizen.org/appcontrol/data/total_count", "3");
 *   bundle_add_str(extra_data, "http://tizen.org/appcontrol/data/total_size", "10240000");
 *
 *   ret = attach_panel_add_content_category(ad->attach_panel, ATTACH_PANEL_CONTENT_CATEGORY_IMAGE, extra_data);
 *   if (ATTACH_PANEL_ERROR_NONE != ret) {
 *      // Error handling
 *   }
 *
 *   ret = attach_panel_add_content_category(ad->attach_panel, ATTACH_PANEL_CONTENT_CATEGORY_CAMERA, extra_data);
 *   if (ATTACH_PANEL_ERROR_NONE != ret) {
 *      // Error handling
 *   }
 *
 *   ret = attach_panel_add_content_category(ad->attach_panel, ATTACH_PANEL_CONTENT_CATEGORY_VOICE, extra_data);
 *   if (ATTACH_PANEL_ERROR_NONE != ret) {
 *      // Error handling
 *   }
 *
 *   ret = attach_panel_set_result_cb(ad->attach_panel, _result_cb, NULL);
 *   if (ATTACH_PANEL_ERROR_NONE != ret) {
 *      // Error handling
 *   }
 *
 *   ret = attach_panel_show(ad->attach_panel);
 *   if (ATTACH_PANEL_ERROR_NONE != ret) {
 *      // Error handling
 *   }
 *
 *   bundle_free(extra_data);
 *
 * 	 return 0;
 * }
 *
 * @endcode
 */
extern int attach_panel_set_result_cb(attach_panel_h attach_panel, attach_panel_result_cb result_cb, void *user_data);

/**
 * @brief Unsets the result callback that will be called when an user selects and confirms something to attach in the attach panel.
 * @since_tizen 2.4
 * @remarks The caller app has to check the return value of this function.\n
 *
 * @param[in] attach_panel Attach panel handler
 * @param[in] result_cb Attach panel result callback
 * @return #ATTACH_PANEL_ERROR_NONE on success,
 *         otherwise a negative error value
 * @retval #ATTACH_PANEL_ERROR_NONE Successful
 * @retval #ATTACH_PANEL_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #ATTACH_PANEL_ERROR_ALREADY_DESTROYED already removed
 *
 * @pre Call attach_panel_create() before calling this function.
 * @see attach_panel_create()
 * @see attach_panel_destroy()
 * @see attach_panel_add_content_category()
 * @see attach_panel_remove_content_category()
 * @see attach_panel_set_extra_data()
 * @see attach_panel_set_result_cb()
 * @see attach_panel_show()
 * @see attach_panel_hide()
 * @see attach_panel_get_visibility()
 *
 * @par Example
 * @code
 * #include <attach_panel.h>
 *
 * struct appdata {
 *   Evas_Object *attach_panel;
 *   Evas_Object *conformant;
 * };
 *
 * static int app_terminate(void *data)
 * {
 *   struct appdata *ad = data;
 *   int ret = 0;
 *
 *   if (!ad) {
 *     // Error handling
 *   }
 *
 *   if (!ad->attach_panel) {
 *     // Error handling
 *   }
 *
 *   ret = attach_panel_hide(ad->attach_panel);
 *   if (ATTACH_PANEL_ERROR_NONE != ret) {
 *      // Error handling
 *   }
 *
 *   ret = attach_panel_remove_content_category(ad->attach_panel, ATTACH_PANEL_CONTENT_CATEGORY_IMAGE);
 *   if (ATTACH_PANEL_ERROR_NONE != ret) {
 *      // Error handling
 *   }
 *
 *   ret = attach_panel_remove_content_category(ad->attach_panel, ATTACH_PANEL_CONTENT_CATEGORY_CAMERA);
 *   if (ATTACH_PANEL_ERROR_NONE != ret) {
 *      // Error handling
 *   }
 *
 *   ret = attach_panel_remove_content_category(ad->attach_panel, ATTACH_PANEL_CONTENT_CATEGORY_VOICE);
 *   if (ATTACH_PANEL_ERROR_NONE != ret) {
 *      // Error handling
 *   }
 *
 *   ret = attach_panel_unset_result_cb(ad->attach_panel);
 *   if (ATTACH_PANEL_ERROR_NONE != ret) {
 *      // Error handling
 *   }
 *
 *   ret = attach_panel_destroy(ad->attach_panel);
 *   if (ATTACH_PANEL_ERROR_NONE != ret) {
 *      // Error handling
 *   }
 *   ad->attach_panel = NULL;
 *
 * 	 return 0;
 * }
 *
 * @endcode
 */
extern int attach_panel_unset_result_cb(attach_panel_h attach_panel);

/**
 * @brief Shows the attach panel, asynchronously.
 * @since_tizen 2.4
 * @remarks The caller app has to check the return value of this function.\n
 *
 * @param[in] attach_panel Attach panel handler
 * @return #ATTACH_PANEL_ERROR_NONE on success,
 *         otherwise a negative error value
 * @retval #ATTACH_PANEL_ERROR_NONE Successful
 * @retval #ATTACH_PANEL_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #ATTACH_PANEL_ERROR_ALREADY_DESTROYED already removed
 *
 * @pre Call attach_panel_create() before calling this function.
 * @see attach_panel_create()
 * @see attach_panel_destroy()
 * @see attach_panel_add_content_category()
 * @see attach_panel_remove_content_category()
 * @see attach_panel_set_extra_data()
 * @see attach_panel_set_result_cb()
 * @see attach_panel_unset_result_cb()
 * @see attach_panel_hide()
 * @see attach_panel_get_visibility()
 *
 * @par Example
 * @code
 * #include <attach_panel.h>
 *
 * struct appdata {
 *   Evas_Object *attach_panel;
 *   Evas_Object *conformant;
 * };
 *
 * static void _result_cb(attach_panel_h attach_panel, attach_panel_content_category_e content_category, app_control_h result, void *data)
 * {
 *   char **select = NULL;
 *   int i = 0;
 *   int length = 0;
 *   int ret = APP_CONTROL_ERROR_NONE;
 *
 *   if (!result) {
 *     // Error handling
 *   }
 *
 *   ret = app_control_get_extra_data_array(result, "http://tizen.org/appcontrol/data/selected", &select, &length);
 *   if (APP_CONTROL_ERROR_NONE != ret || !select) {
 *     // Error handling
 *   }
 *
 *   for (; i < length; i++) {
 *      printf("path is %s, %d\n", select[i], length);
 *      free(select[i]);
 *   }
 *
 *   free(select);
 * }
 *
 * static int app_control(void *data)
 * {
 *   struct appdata *ad = data;
 *   bundle *extra_data = NULL;
 *   int ret = ATTACH_PANEL_ERROR_NONE;
 *
 *   if (!ad) {
 *     // Error handling
 *   }
 *
 *   if (!ad->conformant) {
 *     // Error handling
 *   }
 *
 *   ret = attach_panel_create(ad->conformant, &ad->attach_panel);
 *   if (ATTACH_PANEL_ERROR_NONE != ret) {
 *      // Error handling
 *   }
 *
 *   extra_data = bundle_create();
 *   if (!extra_data) {
 *      // Error handling
 *   }
 *
 *   bundle_add_str(extra_data, "http://tizen.org/appcontrol/data/total_count", "3");
 *   bundle_add_str(extra_data, "http://tizen.org/appcontrol/data/total_size", "10240000");
 *
 *   ret = attach_panel_add_content_category(ad->attach_panel, ATTACH_PANEL_CONTENT_CATEGORY_IMAGE, extra_data);
 *   if (ATTACH_PANEL_ERROR_NONE != ret) {
 *      // Error handling
 *   }
 *
 *   ret = attach_panel_add_content_category(ad->attach_panel, ATTACH_PANEL_CONTENT_CATEGORY_CAMERA, extra_data);
 *   if (ATTACH_PANEL_ERROR_NONE != ret) {
 *      // Error handling
 *   }
 *
 *   ret = attach_panel_add_content_category(ad->attach_panel, ATTACH_PANEL_CONTENT_CATEGORY_VOICE, extra_data);
 *   if (ATTACH_PANEL_ERROR_NONE != ret) {
 *      // Error handling
 *   }
 *
 *   ret = attach_panel_set_result_cb(ad->attach_panel, _result_cb, NULL);
 *   if (ATTACH_PANEL_ERROR_NONE != ret) {
 *      // Error handling
 *   }
 *
 *   ret = attach_panel_show(ad->attach_panel);
 *   if (ATTACH_PANEL_ERROR_NONE != ret) {
 *      // Error handling
 *   }
 *
 *   bundle_free(extra_data);
 *
 * 	 return 0;
 * }
 *
 * @endcode
 */
extern int attach_panel_show(attach_panel_h attach_panel);

/**
 * @brief Hides the attach panel, asynchronously.
 * @since_tizen 2.4
 * @remarks The caller app has to check the return value of this function.\n
 *
 * @param[in] attach_panel Attach panel handler
 * @return #ATTACH_PANEL_ERROR_NONE on success,
 *         otherwise a negative error value
 * @retval #ATTACH_PANEL_ERROR_NONE Successful
 * @retval #ATTACH_PANEL_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #ATTACH_PANEL_ERROR_ALREADY_DESTROYED already removed
 *
 * @pre Call attach_panel_create() before calling this function.
 * @see attach_panel_create()
 * @see attach_panel_destroy()
 * @see attach_panel_add_content_category()
 * @see attach_panel_remove_content_category()
 * @see attach_panel_set_extra_data()
 * @see attach_panel_set_result_cb()
 * @see attach_panel_unset_result_cb()
 * @see attach_panel_show()
 * @see attach_panel_get_visibility()
 *
 * @par Example
 * @code
 * #include <attach_panel.h>
 *
 * struct appdata {
 *   Evas_Object *attach_panel;
 *   Evas_Object *conformant;
 * };
 *
 * static int app_terminate(void *data)
 * {
 *   struct appdata *ad = data;
 *   int ret = 0;
 *
 *   if (!ad) {
 *     // Error handling
 *   }
 *
 *   if (!ad->attach_panel) {
 *     // Error handling
 *   }
 *
 *   ret = attach_panel_hide(ad->attach_panel);
 *   if (ATTACH_PANEL_ERROR_NONE != ret) {
 *      // Error handling
 *   }
 *
 *   ret = attach_panel_remove_content_category(ad->attach_panel, ATTACH_PANEL_CONTENT_CATEGORY_IMAGE);
 *   if (ATTACH_PANEL_ERROR_NONE != ret) {
 *      // Error handling
 *   }
 *
 *   ret = attach_panel_remove_content_category(ad->attach_panel, ATTACH_PANEL_CONTENT_CATEGORY_CAMERA);
 *   if (ATTACH_PANEL_ERROR_NONE != ret) {
 *      // Error handling
 *   }
 *
 *   ret = attach_panel_remove_content_category(ad->attach_panel, ATTACH_PANEL_CONTENT_CATEGORY_VOICE);
 *   if (ATTACH_PANEL_ERROR_NONE != ret) {
 *      // Error handling
 *   }
 *
 *   ret = attach_panel_unset_result_cb(ad->attach_panel);
 *   if (ATTACH_PANEL_ERROR_NONE != ret) {
 *      // Error handling
 *   }
 *
 *   ret = attach_panel_destroy(ad->attach_panel);
 *   if (ATTACH_PANEL_ERROR_NONE != ret) {
 *      // Error handling
 *   }
 *   ad->attach_panel = NULL;
 *
 * 	 return 0;
 * }
 *
 * @endcode
 */
extern int attach_panel_hide(attach_panel_h attach_panel);

/**
 * @brief Gets a value that indicates whether the attach_panel is visible.
 * @since_tizen 2.4
 * @remarks The caller app has to check the return value of this function.\n
 *
 * @param[in] attach_panel Attach panel handler
 * @param[out] visible value of attach_panel state
 * @return #ATTACH_PANEL_ERROR_NONE on success,
 *         otherwise a negative error value
 * @retval #ATTACH_PANEL_ERROR_NONE Successful
 * @retval #ATTACH_PANEL_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #ATTACH_PANEL_ERROR_ALREADY_DESTROYED already removed
 *
 * @pre Call attach_panel_create() before calling this function.
 * @see attach_panel_create()
 * @see attach_panel_destroy()
 * @see attach_panel_add_content_category()
 * @see attach_panel_remove_content_category()
 * @see attach_panel_set_extra_data()
 * @see attach_panel_set_result_cb()
 * @see attach_panel_unset_result_cb()
 * @see attach_panel_show()
 * @see attach_panel_hide()
 *
 * @par Example
 * @code
 * #include <attach_panel.h>
 *
 * struct appdata {
 *   Evas_Object *attach_panel;
 *   Evas_Object *conformant;
 * };
 *
 * static int app_terminate(void *data)
 * {
 *   struct appdata *ad = data;
 *   bool vislble = false;
 *   int ret = 0;
 *
 *   if (!ad) {
 *     // Error handling
 *   }
 *
 *   if (!ad->attach_panel) {
 *     // Error handling
 *   }
 *
 *   ret = attach_panel_get_visibility(ad->attach_panel, &visible);
 *   if (ATTACH_PANEL_ERROR_NONE != ret) {
 *      // Error handling
 *   }
 *
 *   if (visible) {
 *      ret = attach_panel_hide(ad->attach_panel);
 *      if (ATTACH_PANEL_ERROR_NONE != ret) {
 *         // Error handling
 *      }
 *   }
 *
 *   ret = attach_panel_remove_content_category(ad->attach_panel, ATTACH_PANEL_CONTENT_CATEGORY_IMAGE);
 *   if (ATTACH_PANEL_ERROR_NONE != ret) {
 *      // Error handling
 *   }
 *
 *   ret = attach_panel_remove_content_category(ad->attach_panel, ATTACH_PANEL_CONTENT_CATEGORY_CAMERA);
 *   if (ATTACH_PANEL_ERROR_NONE != ret) {
 *      // Error handling
 *   }
 *
 *   ret = attach_panel_remove_content_category(ad->attach_panel, ATTACH_PANEL_CONTENT_CATEGORY_VOICE);
 *   if (ATTACH_PANEL_ERROR_NONE != ret) {
 *      // Error handling
 *   }
 *
 *   ret = attach_panel_unset_result_cb(ad->attach_panel);
 *   if (ATTACH_PANEL_ERROR_NONE != ret) {
 *      // Error handling
 *   }
 *
 *   ret = attach_panel_destroy(ad->attach_panel);
 *   if (ATTACH_PANEL_ERROR_NONE != ret) {
 *      // Error handling
 *   }
 *   ad->attach_panel = NULL;
 *
 * 	 return 0;
 * }
 *
 * @endcode
 */
extern int attach_panel_get_visibility(attach_panel_h attach_panel, bool *visible);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif
#endif // __TIZEN_ATTACH_PANEL_H__

