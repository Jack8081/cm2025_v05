/*
 * Copyright (c) 2019 Actions Semiconductor Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file ui manager interface
 */

#define SYS_LOG_NO_NEWLINE
#ifdef SYS_LOG_DOMAIN
#undef SYS_LOG_DOMAIN
#endif
#define SYS_LOG_DOMAIN "ui_manager"
#include <os_common_api.h>
#include <msg_manager.h>
#include <mem_manager.h>
#include <srv_manager.h>
#include <app_manager.h>
#include <input_manager.h>
#include <ui_mem.h>
#include <string.h>
#ifdef CONFIG_UI_MANAGER
#include <ui_manager.h>
#endif

#ifdef CONFIG_SEG_LED_MANAGER
#include <seg_led_manager.h>
#endif

#ifdef CONFIG_ESD_MANAGER
#include <esd_manager.h>
#endif

#ifdef CONFIG_LED_MANAGER
#include <led_manager.h>
#endif

#ifdef CONFIG_PLAYTTS
#include <tts_manager.h>
#endif

typedef struct {
	sys_slist_t  view_list;
} ui_manager_context_t;

static ui_manager_context_t global_ui_manager;

static ui_manager_context_t *_ui_manager_get_context(void)
{
	return &global_ui_manager;
}

#ifndef CONFIG_UI_SERVICE
static void _ui_manager_add_view(ui_view_context_t *view)
{
	ui_view_context_t *cur_view;
	ui_view_context_t *pre_view = NULL;
	ui_manager_context_t *ui_manager = _ui_manager_get_context();

	SYS_SLIST_FOR_EACH_CONTAINER(&ui_manager->view_list, cur_view, node) {
		if (view->info.order > cur_view->info.order) {
			sys_slist_insert(&ui_manager->view_list, &pre_view->node, &view->node);
			goto end;
		}
		pre_view = cur_view;
	}

	sys_slist_append(&ui_manager->view_list, (sys_snode_t *)view);

end:
	return;
}

static ui_view_context_t *_ui_manager_get_view_context(uint8_t view_id)
{
	ui_view_context_t *view;

	ui_manager_context_t *ui_manager = _ui_manager_get_context();

	SYS_SLIST_FOR_EACH_CONTAINER(&ui_manager->view_list, view, node) {
		if (view->view_id == view_id)
			return view;
	}

	return NULL;
}

static int _ui_manager_get_view_index(uint8_t view_id)
{
	ui_manager_context_t *ui_manager = _ui_manager_get_context();
	ui_view_context_t *view;
	int  index = 0;

	SYS_SLIST_FOR_EACH_CONTAINER(&ui_manager->view_list, view, node) {
		if (view->view_id == view_id) {
			break;
		}
		index += 1;
	}

	return index;
}

int ui_message_dispatch(uint8_t view_id, uint8_t msg_id, uint32_t msg_data)
{
	ui_view_context_t *view = _ui_manager_get_view_context(view_id);

	if (!view) {
		return -ESRCH;
	}

	return view->info.view_proc(view->view_id, msg_id, (void *)msg_data);
}

static bool _ui_manager_key_event_match(uint32_t current_state, uint32_t match_state)
{
	return ((current_state & match_state) != 0);
}

int ui_manager_dispatch_key_event(uint32_t key_event)
{
	ui_manager_context_t *ui_manager = _ui_manager_get_context();
	ui_view_context_t *view = NULL;
	uint8_t index = 0;
	int result = 0;
	uint32_t current_state = 0xffffffff;
	uint32_t key_type = key_event & 0xffff0000;
	uint16_t key_val = key_event & 0xffff;
	struct app_msg new_msg = { 0 };

	SYS_SLIST_FOR_EACH_CONTAINER(&ui_manager->view_list, view, node) {
		const ui_key_map_t *view_key_map = view->info.view_key_map;

		index = 0;
		current_state = 0xffffffff;
		if (view->info.view_get_state)
			current_state = view->info.view_get_state();

		while (view_key_map[index].key_val != KEY_RESERVED) {
            /* support customed sequence key for earphone, key_type could be (MULTI_CLICK + LONG_PRESS) */
            if (key_val  == view_key_map[index].key_val &&
                key_type == view_key_map[index].key_type)
		    {
				ui_state_match_t state_match = view->info.view_state_match;

				if (!state_match) {
					state_match = _ui_manager_key_event_match;
				}

				if (view_key_map[index].app_state == TRANSMIT_ALL_KEY_EVENT) {
					new_msg.type = MSG_INPUT_EVENT;
					new_msg.value = key_event;
					result = send_async_msg(view->app_id, &new_msg);
					return result;
				}

				if (state_match(current_state, view_key_map[index].app_state)) {
					new_msg.type = MSG_INPUT_EVENT;
					new_msg.cmd = view_key_map[index].app_msg;
					result = send_async_msg(view->app_id, &new_msg);
					return result;
				}
			}
			index++;
		}
	}
	return result;

}
#else
int ui_manager_dispatch_key_event(uint32_t key_event)
{
	struct app_msg msg = {
		.type = MSG_KEY_INPUT,
		.value = key_event,
	};

#ifdef CONFIG_UISRV_KEY_EVENT_BYPASS
	char * app_id = app_manager_get_current_app(); // view->entry->app_id
	if (app_id != NULL) {
		return !send_async_msg(app_id, &msg);
	}

	return -ESRCH;
#else
	return !send_async_msg("ui_service", &msg);
#endif
}

static int _ui_service_send_async(uint8_t view_id, uint8_t msg_id, uint8_t msg_flag, uint32_t msg_data)
{
	struct app_msg msg = {
		.type = MSG_UI_EVENT,
		.sender = view_id,
		.cmd = msg_id,
		.reserve = msg_flag,
		.value = msg_data,
	};

	return !send_async_msg("ui_service", &msg);
}
#endif /* CONFIG_UI_SERVICE */

int ui_manager_send_async(uint8_t view_id, uint8_t msg_id, uint32_t msg_data)
{
#ifdef CONFIG_UI_SERVICE
	SYS_LOG_INF("view_id %d, msg_id %d, msg_data %d\n", view_id, msg_id, msg_data);

	return _ui_service_send_async(view_id, msg_id, 0, msg_data);
#else
	struct app_msg msg = {0};
	ui_view_context_t *view = _ui_manager_get_view_context(view_id);

	if (!view) {
		return -ESRCH;
	}

	msg.type = MSG_UI_EVENT;
	msg.sender = view_id;
	msg.cmd = msg_id;
	msg.value = msg_data;
	SYS_LOG_INF("view_id %d  msg_id %d  msg_data %d\n", view_id, msg_id,  msg_data);
	return send_async_msg("main"/*view->app_id*/, &msg);

#endif
}

int ui_manager_handle_appmsg_async(struct app_msg *msg)
{
	if (msg->type < MSG_APP_MESSAGE_START || msg->callback == NULL) {
		return -EINVAL;
	}

#ifdef CONFIG_UI_SERVICE
	return !send_async_msg("ui_service", msg);
#else
	return -ENOSYS;
#endif
}

#ifdef CONFIG_UI_SERVICE
int ui_view_create(uint8_t view_id, const void *presenter, uint8_t flags)
#else
int ui_view_create(uint8_t view_id, ui_view_info_t *info)
#endif
{
#ifdef CONFIG_UI_SERVICE
	return _ui_service_send_async(view_id, MSG_VIEW_CREATE, flags, (uintptr_t)presenter);
#else
	ui_view_context_t *view;

	view = _ui_manager_get_view_context(view_id);

	if (view != NULL)
		return false;

	view = mem_malloc(sizeof(ui_view_context_t));

	if (!view)
		return false;

	memset(view, 0, sizeof(ui_view_context_t));

	view->view_id = view_id;

	view->app_id = info->app_id;

	memcpy(&view->info, info, sizeof(ui_view_info_t));

	_ui_manager_add_view(view);

#ifdef CONFIG_SEG_LED_MANAGER
	/* unlock seg led */
	seg_led_manager_set_timeout_event(0, NULL);
#endif

	ui_manager_send_async(view_id, MSG_VIEW_CREATE, 0);

	return 0;
#endif
}

int ui_view_layout(uint8_t view_id)
{
#ifdef CONFIG_UI_SERVICE
	return _ui_service_send_async(view_id, MSG_VIEW_LAYOUT, 0, 0);
#else
	return 0;
#endif
}

int ui_view_update(uint8_t view_id)
{
#ifdef CONFIG_UI_SERVICE
	return _ui_service_send_async(view_id, MSG_VIEW_UPDATE, 0, 0);
#else
	return 0;
#endif
}

int ui_view_delete(uint8_t view_id)
{
#ifdef CONFIG_UI_SERVICE
	return _ui_service_send_async(view_id, MSG_VIEW_DELETE, 0, 0);
#else
	ui_manager_context_t *ui_manager = _ui_manager_get_context();
	ui_view_context_t *view = _ui_manager_get_view_context(view_id);

	if (!view)
		return false;

	SYS_LOG_INF("view_id %d\n", view_id);

	view->info.view_proc(view->view_id, MSG_VIEW_DELETE, 0);

	sys_slist_find_and_remove(&ui_manager->view_list, &view->node);

	mem_free(view);

	return true;
#endif
}

int ui_view_show(uint8_t view_id)
{
#ifdef CONFIG_UI_SERVICE
	return _ui_service_send_async(view_id, MSG_VIEW_SET_HIDDEN, 0, false);
#else
	return 0;
#endif
}

int ui_view_hide(uint8_t view_id)
{
#ifdef CONFIG_UI_SERVICE
	return _ui_service_send_async(view_id, MSG_VIEW_SET_HIDDEN, 0, true);
#else
	return 0;
#endif
}

int ui_view_set_order(uint8_t view_id, uint16_t order)
{
#ifdef CONFIG_UI_SERVICE
	return _ui_service_send_async(view_id, MSG_VIEW_SET_ORDER, 0, order);
#else
	ui_manager_context_t *ui_manager = _ui_manager_get_context();
	ui_view_context_t *view = _ui_manager_get_view_context(view_id);
	int	old_index;

	if (!view)
		return false;

	old_index = _ui_manager_get_view_index(view_id);

	sys_slist_find_and_remove(&ui_manager->view_list, &view->node);

	view->info.order = order;

	_ui_manager_add_view(view);

	return 0;
#endif
}

int ui_view_set_pos(uint8_t view_id, int16_t x, int16_t y)
{
#ifdef CONFIG_UI_SERVICE
	union {
		uint32_t val_32;
		int16_t val_16[2];
	} value;

	value.val_16[0] = x;
	value.val_16[1] = y;

	return _ui_service_send_async(view_id, MSG_VIEW_SET_POS, 0, value.val_32);
#else
	return 0;
#endif
}

int ui_view_set_drag_attribute(uint8_t view_id, int16_t drag_attribute)
{
#ifdef CONFIG_UI_SERVICE
	return _ui_service_send_async(view_id, MSG_VIEW_SET_DRAG_ATTRIBUTE, 0, drag_attribute);
#else
	return 0;
#endif
}

int ui_view_paint(uint8_t view_id)
{
#ifdef CONFIG_UI_SERVICE
	return _ui_service_send_async(view_id, MSG_VIEW_PAINT, 0, 0);
#else
	return 0;
#endif
}

int ui_view_refresh(uint8_t view_id)
{
#ifdef CONFIG_UI_SERVICE
	return _ui_service_send_async(view_id, MSG_VIEW_REFRESH, 0, 0);
#else
	return 0;
#endif
}

int ui_view_pause(uint8_t view_id)
{
#ifdef CONFIG_UI_SERVICE
	return _ui_service_send_async(view_id, MSG_VIEW_PAUSE, 0, 0);
#else
	return 0;
#endif
}

int ui_view_resume(uint8_t view_id)
{
#ifdef CONFIG_UI_SERVICE
	return _ui_service_send_async(view_id, MSG_VIEW_RESUME, 0, 0);
#else
	return 0;
#endif
}

int ui_view_send_user_msg(uint8_t view_id, uint8_t msg_id, void *msg_data)
{
#ifdef CONFIG_UI_SERVICE
	if (msg_id >= MSG_VIEW_USER_OFFSET) {
		return _ui_service_send_async(view_id, msg_id, 0, (uint32_t)msg_data);
	}

	return -EINVAL;
#else
	return 0;
#endif
}

int ui_msgbox_popup(uint8_t msgbox_id)
{
#ifdef CONFIG_UI_SERVICE
	return _ui_service_send_async(msgbox_id, MSG_MSGBOX_POPUP, 0, 0);
#else
	return 0;
#endif
}

int ui_msgbox_close(uint8_t msgbox_id)
{
#ifdef CONFIG_UI_SERVICE
	return _ui_service_send_async(msgbox_id, MSG_MSGBOX_CLOSE, 0, 0);
#else
	return 0;
#endif
}

#ifdef CONFIG_UI_SERVICE

#ifndef CONFIG_SIMULATOR
__aligned(ARCH_STACK_PTR_ALIGN) __in_section_unique(uisrv.noinit.stack)
static char uisrv_stack_area[CONFIG_UISRV_STACKSIZE];

extern void _ui_service_main_loop(void *parama1, void *parama2, void *parama3);

SERVICE_DEFINE(ui_service, uisrv_stack_area, sizeof(uisrv_stack_area), \
		CONFIG_UISRV_PRIORITY, BACKGROUND_APP, NULL, NULL, NULL, \
		_ui_service_main_loop);
#endif /* CONFIG_SIMULATOR */

static int _ui_service_start(void)
{
	struct app_msg msg = {0};

	if (!srv_manager_check_service_is_actived(UI_SERVICE_NAME)) {
		if (srv_manager_active_service(UI_SERVICE_NAME)) {
			SYS_LOG_DBG("ui service start ok\n");
		} else {
			SYS_LOG_ERR("ui service start failed\n");
			return -ESRCH;
		}
	}

	msg.type = MSG_INIT_APP;

	return !send_async_msg(UI_SERVICE_NAME, &msg);
}

static int _ui_service_stop(void)
{
	int ret = 0;

	if (!srv_manager_check_service_is_actived(UI_SERVICE_NAME)) {
		SYS_LOG_ERR("ui service_stop failed\n");
		ret = -ESRCH;
		goto exit;
	}

	if (!srv_manager_exit_service(UI_SERVICE_NAME)) {
		ret = -ETIMEDOUT;
		goto exit;
	}

	SYS_LOG_DBG("ui service_stop success!\n");
exit:
	return ret;
}
#endif /* CONFIG_UI_SERVICE */

int ui_manager_init(void)
{
	ui_manager_context_t *ui_manager = _ui_manager_get_context();

	memset(ui_manager, 0, sizeof(ui_manager_context_t));

#ifdef CONFIG_UI_MEMORY_MANAGER
	ui_memory_init();
#endif

#ifdef CONFIG_UI_SERVICE
	_ui_service_start();
#else
	sys_slist_init(&ui_manager->view_list);
#endif

#ifdef CONFIG_LED_MANAGER
	led_manager_init();
#endif

#ifdef CONFIG_SEG_LED_MANAGER
	seg_led_manager_init();
#endif

	return 0;
}

int ui_manager_exit(void)
{
	ui_manager_context_t *ui_manager = _ui_manager_get_context();

	if (!ui_manager)
		return -ESRCH;

#ifdef CONFIG_LED_MANAGER
	led_manager_deinit();
#endif


#ifdef CONFIG_SEG_LED_MANAGER
	seg_led_manager_deinit();
#endif

#ifdef CONFIG_UI_SERVICE
	_ui_service_stop();
#endif

	return 0;
}

int ui_manager_post_display(void)
{
#ifdef CONFIG_UI_SERVICE
	return _ui_service_send_async(0, MSG_FORCE_POST_DISPLAY, 0, 0);
#else
	return 0;
#endif
}

int ui_manager_set_draw_period(uint8_t multiple)
{
	if (multiple <= 0)
		return -EINVAL;

#ifdef CONFIG_UI_SERVICE
	return _ui_service_send_async(0, MSG_VIEW_SET_DRAW_PERIOD, 0, multiple);
#else
	return 0;
#endif
}

int ui_manager_set_notify_callback(ui_notify_cb_t callback)
{
#ifdef CONFIG_UI_SERVICE
	return _ui_service_send_async(0, MSG_VIEW_SET_CALLBACK, UI_CB_NOTIFY, (uintptr_t)callback);
#else
	return 0;
#endif
}

int ui_manager_set_msgbox_popup_callback(ui_msgbox_popup_cb_t callback)
{
#ifdef CONFIG_UI_SERVICE
	return _ui_service_send_async(0, MSG_VIEW_SET_CALLBACK, UI_CB_MSGBOX, (uintptr_t)callback);
#else
	return 0;
#endif
}

int ui_manager_set_keyevent_callback(ui_keyevent_cb_t callback)
{
#ifdef CONFIG_UI_SERVICE
	return _ui_service_send_async(0, MSG_VIEW_SET_CALLBACK, UI_CB_KEYEVENT, (uintptr_t)callback);
#else
	return 0;
#endif
}

int ui_manager_gesture_set_enabled(bool en)
{
#ifdef CONFIG_UI_SERVICE
	return _ui_service_send_async(0, MSG_GESTURE_ENABLE, 0, en);
#else
	return -ENOSYS;
#endif
}

int ui_manager_gesture_set_dir(uint8_t filter)
{
#ifdef CONFIG_UI_SERVICE
	return _ui_service_send_async(0, MSG_GESTURE_SET_DIR, 0, filter);
#else
	return -ENOSYS;
#endif
}

int ui_manager_gesture_wait_release(void)
{
#ifdef CONFIG_UI_SERVICE
	return _ui_service_send_async(0, MSG_GESTURE_WAIT_RELEASE, 0, 0);
#else
	return -ENOSYS;
#endif
}

int ui_manager_gesture_forbid_draw(bool en)
{
#ifdef CONFIG_UI_SERVICE
	return _ui_service_send_async(0, MSG_GESTURE_FORBID_DRAW, 0, en);
#else
	return -ENOSYS;
#endif
}

