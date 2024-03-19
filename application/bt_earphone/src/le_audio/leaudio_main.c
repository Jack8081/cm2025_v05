/*
 * Copyright (c) 2019 Actions Semiconductor Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file
 * @brief le audio app main.
 */

#include "leaudio.h"
#include "app_config.h"

static struct leaudio_app_t *p_leaudio_app;

#ifdef CONFIG_BT_TRANSCEIVER
#if (defined CONFIG_SAMPLE_CASE_1) || (defined CONFIG_SAMPLE_CASE_XNT)
static struct gpio_callback leaudio_gpio_cb;

static void _leaudio_aux_plug_in_handle(struct k_work *work)
{
    struct app_msg new_msg = { 0 };

    if (system_check_aux_plug_in())
    {   
		new_msg.type = MSG_INPUT_EVENT;
		new_msg.cmd  = KEY_FUNC_POWER_OFF;
		send_async_msg(APP_ID_MAIN, &new_msg);
    }
}

static void _leaudio_aux_plug_in_cb(const struct device *port, struct gpio_callback *cb, gpio_port_pins_t pins)
{
    SYS_LOG_INF("");
    os_delayed_work_submit(&p_leaudio_app->aux_plug_in_delay_work, 10);
}
#endif
#endif

static int _leaudio_init(void)
{
	CFG_Struct_BTMusic_Multi_Dae_Settings multidae_setting;

#ifdef CONFIG_LE_AUDIO_BACKGROUD_IN_BTCALL
	if(le_audio_is_in_background()) {
		le_audio_enter_foreground();
		leaudio_view_init(BTMUSIC_VIEW, false);
		audio_system_set_stream_volume(AUDIO_STREAM_LE_AUDIO_MUSIC, audio_system_get_stream_volume(AUDIO_STREAM_LE_AUDIO_MUSIC));	
		return 0;
	}

	bt_manager_allow_sco_connect(true);
#endif

	p_leaudio_app = app_mem_malloc(sizeof(struct leaudio_app_t));
	if (!p_leaudio_app) {
		SYS_LOG_ERR("malloc fail!\n");
		return -ENOMEM;
	}

	memset(p_leaudio_app, 0, sizeof(struct leaudio_app_t));

	app_config_read
    (
        CFG_ID_BTMUSIC_MULTI_DAE_SETTINGS,
        &multidae_setting, 0, sizeof(CFG_Struct_BTMusic_Multi_Dae_Settings)
    );

	p_leaudio_app->multidae_enable = multidae_setting.Enable;
	p_leaudio_app->dae_cfg_nums = multidae_setting.Cur_Dae_Num + 1;
	p_leaudio_app->current_dae = 0;

    /* 从vram读回当前音效 */
    if (p_leaudio_app->multidae_enable) {
        if (property_get(BTMUSIC_MULTI_DAE, &p_leaudio_app->current_dae, sizeof(uint8_t)) < 0) {
            p_leaudio_app->current_dae = 0;
			property_set(BTMUSIC_MULTI_DAE, &p_leaudio_app->current_dae, sizeof(uint8_t));
		}

        if (p_leaudio_app->current_dae > p_leaudio_app->dae_cfg_nums) {
            p_leaudio_app->current_dae = 0;
			property_set(BTMUSIC_MULTI_DAE, &p_leaudio_app->current_dae, sizeof(uint8_t));
		}

		leaudio_multi_dae_adjust(p_leaudio_app->current_dae);
    } else {
		app_config_read
		(
			CFG_ID_BT_MUSIC_DAE,
			&p_leaudio_app->dae_enalbe, offsetof(CFG_Struct_BT_Music_DAE, Enable_DAE), sizeof(cfg_uint8)
		);

		app_config_read
		(
			CFG_ID_BT_MUSIC_DAE_AL,
			p_leaudio_app->cfg_dae_data, 0, CFG_SIZE_BT_MUSIC_DAE_AL
		);
	}

	/* FIXME */
	leaudio_view_init(BTMUSIC_VIEW, false);

#if (defined CONFIG_SAMPLE_CASE_1) || (defined CONFIG_SAMPLE_CASE_XNT)
    bt_ui_monitor_submit_work(2000);
#endif

	audio_system_set_stream_volume(AUDIO_STREAM_LE_AUDIO_MUSIC, audio_system_get_stream_volume(AUDIO_STREAM_LE_AUDIO_MUSIC));	

#if (defined CONFIG_SAMPLE_CASE_1) || (defined CONFIG_SAMPLE_CASE_XNT)
	leaudio_event_notify(SYS_EVENT_CUSTOMED_1);
	//leaudio_event_notify(SYS_EVENT_MIC_MUTE_ON);
    p_leaudio_app->gpio_dev = device_get_binding("GPIOA");
    gpio_pin_configure(p_leaudio_app->gpio_dev, 7, GPIO_OUTPUT_INACTIVE);
    
	gpio_pin_interrupt_configure(p_leaudio_app->gpio_dev, AUX_GPIO_PIN_IDX, GPIO_INT_EDGE_RISING);
	gpio_init_callback(&leaudio_gpio_cb, _leaudio_aux_plug_in_cb, BIT(AUX_GPIO_PIN_IDX));
	gpio_add_callback(p_leaudio_app->gpio_dev, &leaudio_gpio_cb);
	os_delayed_work_init(&p_leaudio_app->aux_plug_in_delay_work, _leaudio_aux_plug_in_handle);

    if (system_check_aux_plug_in())
    {
        gpio_pin_set(p_leaudio_app->gpio_dev, 7, 0);
    }
    else
    {
        gpio_pin_set(p_leaudio_app->gpio_dev, 7, 1);
    }
#endif

	SYS_LOG_INF("dae cfg: %d %d %d %d", p_leaudio_app->dae_enalbe, 
		p_leaudio_app->multidae_enable, p_leaudio_app->dae_cfg_nums, p_leaudio_app->current_dae);

	return 0;
}

extern bool bt_manager_startup_reconnect(void);

static void _leaudio_exit(void)
{
	if (!p_leaudio_app) {
		goto exit;
	}

#ifdef CONFIG_LE_AUDIO_BACKGROUD_IN_BTCALL
	le_audio_check_set_background();
#endif

	if (thread_timer_is_running(&p_leaudio_app->monitor_timer))
		thread_timer_stop(&p_leaudio_app->monitor_timer);

	if (thread_timer_is_running(&p_leaudio_app->key_timer))
		thread_timer_stop(&p_leaudio_app->key_timer);

	if (thread_timer_is_running(&p_leaudio_app->delay_play_timer))
		thread_timer_stop(&p_leaudio_app->delay_play_timer);

	if(p_leaudio_app->need_resume_key){
		ui_key_filter(false);
	}

	leaudio_stop_playback();
	leaudio_stop_capture();

	if ((!p_leaudio_app->playback_player || !p_leaudio_app->playback_player_run)
		&& (!p_leaudio_app->capture_player || !p_leaudio_app->capture_player_run)) {
		leaudio_set_share_info(false, false);
		leaudio_exit_playback();
		leaudio_exit_capture();
	}

#ifdef CONFIG_PLAYTTS
    tts_manager_wait_finished(true);
#endif

	leaudio_view_deinit();

#ifdef CONFIG_PROPERTY
	property_flush_req(NULL);
#endif

#ifdef CONFIG_LE_AUDIO_BACKGROUD_IN_BTCALL
	if(p_leaudio_app->leaudio_need_backgroud) {
		le_audio_enter_background();
		app_manager_thread_exit(APP_ID_LE_AUDIO);
		return;
	}
#endif

#ifdef CONFIG_BT_TRANSCEIVER
#if (defined CONFIG_SAMPLE_CASE_1) || (defined CONFIG_SAMPLE_CASE_XNT)
    gpio_remove_callback(p_leaudio_app->gpio_dev, &leaudio_gpio_cb);
    os_delayed_work_cancel(&p_leaudio_app->aux_plug_in_delay_work);
#endif
#endif

	app_mem_free(p_leaudio_app);
	p_leaudio_app = NULL;

exit:
	app_manager_thread_exit(APP_ID_LE_AUDIO);

	SYS_LOG_INF("exit finished\n");
}

static void leaudio_main_loop(void *parama1, void *parama2, void *parama3)
{
	struct app_msg msg = {0};
	int result = 0;
	bool terminaltion = false;

	if (_leaudio_init()) {
		SYS_LOG_ERR("init failed");
		_leaudio_exit();
		return;
	}

	bt_manager_audio_stream_restore(BT_TYPE_LE);

	while (!terminaltion) {
		if (receive_msg(&msg, thread_timer_next_timeout())) {
            app_msg_print(&msg, __FUNCTION__);
			switch (msg.type) {
			case MSG_EXIT_APP:
				_leaudio_exit();
				terminaltion = true;
				break;
			case MSG_BT_EVENT:
				leaudio_bt_event_proc(&msg);
				break;
			case MSG_INPUT_EVENT:
				leaudio_input_event_proc(&msg);
				break;
			case MSG_TTS_EVENT:
				leaudio_tts_event_proc(&msg);
				break;
			case MSG_TWS_EVENT:
				leaudio_tws_event_proc(&msg);
				break;
			default:
				SYS_LOG_WRN("error: 0x%x!\n", msg.type);
				break;
			}
			if (msg.callback != NULL)
				msg.callback(&msg, result, NULL);
		}
		thread_timer_handle_expired();
	}
}

struct leaudio_app_t *leaudio_get_app(void)
{
	return p_leaudio_app;
}

#ifdef CONFIG_LE_AUDIO_BACKGROUD_IN_BTCALL

void leaudio_restore(struct thread_timer *ttimer, void *expiry_fn_arg);
void leaudio_bt_event_proc(struct app_msg *msg);

void le_audio_enter_background(void)
{
    p_leaudio_app->leaudio_backgroud = 1;
	p_leaudio_app->leaudio_need_backgroud = 0;
	
	SYS_LOG_INF("");
#ifdef CONFIG_PLAYTTS
	tip_manager_lock();
    tts_manager_lock();
    tts_manager_wait_finished(false);
#endif

	if(p_leaudio_app->tts_playing) {
		p_leaudio_app->tts_playing = 0;
	}
}

void le_audio_enter_foreground(void)
{
	if(p_leaudio_app->leaudio_backgroud) {
		p_leaudio_app->leaudio_backgroud = 0;
		
		SYS_LOG_INF("");
#ifdef CONFIG_PLAYTTS
		tts_manager_unlock();
		tip_manager_unlock();
#endif
	}
}

void le_audio_check_set_background(void)
{
	if(p_leaudio_app && p_leaudio_app->leaudio_backgroud == 0) {
		const u8_t * app_name = app_manager_get_next_app();
		if(app_name && strcmp(app_name, APP_ID_BTCALL) == 0 
			&& (p_leaudio_app->playback_player 
				|| (p_leaudio_app->bt_sink_enabled && p_leaudio_app->tts_playing))) {
			p_leaudio_app->leaudio_need_backgroud = 1;
		}
	}
}

bool le_audio_is_in_background(void)
{
	if(p_leaudio_app && p_leaudio_app->leaudio_backgroud) {
		return true;
	}

	return false;
}

void le_audio_background_stop(void)
{
	if(le_audio_is_in_background() == false) {
		return;
	}

	SYS_LOG_INF("");
	leaudio_stop_playback();
	leaudio_set_share_info(false, false);
	leaudio_exit_playback();
}

void le_audio_background_prepare(void)
{
	if(le_audio_is_in_background() == false) {
		return;
	}
	
	SYS_LOG_INF("");
	leaudio_restore(&p_leaudio_app->monitor_timer, NULL);
}

void le_audio_background_start(void)
{
	if(le_audio_is_in_background() == false) {
		return;
	}
	
	SYS_LOG_INF("");
	leaudio_start_playback();
}


void le_audio_background_exit(void)
{
	if(le_audio_is_in_background()) {
		struct leaudio_app_t *leaudio_app = p_leaudio_app;

		SYS_LOG_INF("");
		le_audio_background_stop();
		bt_manager_event_notify(BT_CALL_REPLAY, NULL, 0);
	
		leaudio_app->leaudio_backgroud = 0;
		p_leaudio_app = NULL;
		
		app_mem_free(leaudio_app);
	}
}

void le_audio_background_bt_event_proc(struct app_msg *msg)
{
	if(le_audio_is_in_background() == false) {
		return;
	}
	
	leaudio_bt_event_proc(msg);
}
#endif

APP_DEFINE(le_audio, share_stack_area, sizeof(share_stack_area), CONFIG_APP_PRIORITY,
	   FOREGROUND_APP, NULL, NULL, NULL,
	   leaudio_main_loop, NULL);
