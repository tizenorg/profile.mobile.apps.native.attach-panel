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

#ifndef __TIZEN_ATTACH_PANEL_DOC_H__
#define __TIZEN_ATTACH_PANEL_DOC_H__

/**
 * @defgroup CAPI_PANEL_MODULE Panel
 * @brief The Panel module provides APIs to choose data or apps easily.
 * @ingroup CAPI_APPLICATION_FRAMEWORK
 *
 * @section CAPI_PANEL_MODULE_OVERVIEW Overview
 * <table>
 * <tr><th>API</th><th>Description</th></tr>
 * <tr><td>@ref CAPI_PANEL_ATTACH_MODULE</td><td>Provide functions to attach images, voices, contacts, events and files.</td></tr>
 * <tr><td>@ref CAPI_PANEL_STICKER_MODULE</td><td>Provide functions to attach stickers easily.</td></tr>
 * <tr><td>@ref CAPI_PANEL_SHARE_MODULE</td><td>Provide functions to share data with apps.</td></tr>
 * </table>
 *
 * @defgroup CAPI_PANEL_ATTACH_MODULE Attach panel
 * @brief The Attach Panel provides functions to attach data easily. Users can attach images, take pictures, record voice and select files to attach into the caller app
 * @ingroup CAPI_PANEL_MODULE
 *
 * @section CAPI_PANEL_ATTACH_MODULE_HEADER Required Header
 *   \#include <attach_panel.h>
 * @section CAPI_PANEL_ATTACH_MODULE_OVERVIEW Overview
 * It provides functions for adding an Attach Panel (attach_panel_create()) and deleting it (attach_panel_destroy()) in an app.\n
 * It is shown with (attach_panel_show()) and hiden with (attach_panel_hide()).\n
 * The results can be returned into the result callback that is set (attach_panel_set_result_cb()) or unset (attach_panel_unset_result_cb()).\n
 * Content categories can be added (attach_panel_add_content_category()) or removed (attach_panel_remove_content_category()).\n
 *
 * @section CAPI_PANEL_ATTACH_MODULE_FEATURE Related Features
 * This API is related with the following features:\n
 *  - http://tizen.org/feature/camera\n
 *  - http://tizen.org/feature/microphone\n
 * It is recommended to design feature related codes in your application for reliability.\n
 * You can check if a device supports the related features for this API by using @ref CAPI_SYSTEM_SYSTEM_INFO_MODULE, thereby controlling the procedure of your application.\n
 * To ensure your application is only running on the device with specific features, please define the features in your manifest file using the manifest editor in the SDK.\n
 * More details on featuring your application can be found from <a href="../org.tizen.mobile.native.appprogramming/html/ide_sdk_tools/feature_element.htm"><b>Feature Element</b>.</a>
 */

#endif
