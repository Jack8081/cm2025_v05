/*
 * Copyright (c) 2019 Actions Semiconductor Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file volume manager interface
 */

#include <os_common_api.h>
#include <app_manager.h>
#include <msg_manager.h>
#include <sys_event.h>
#ifdef CONFIG_UI_MANAGER
#include <ui_manager.h>
#endif
#include "audio_system.h"
#include "audio_policy.h"
#ifdef CONFIG_MEDIA_PLAYER
#include <media_player.h>
#endif
#ifdef CONFIG_ESD_MANAGER
#include <esd_manager.h>
#endif
#ifdef CONFIG_BLUETOOTH
#include <bt_manager.h>
#endif
int system_volume_get(int stream_type)
{
	return audio_system_get_stream_volume(stream_type);
}

static void _system_volume_set_notify(uint32_t volume, bool is_limited)
{
	struct app_msg new_msg = { 0 };

	new_msg.type = MSG_VOLUME_CHANGED_EVENT;
	new_msg.cmd = is_limited;
	new_msg.value = volume;

	send_async_msg("main", &new_msg);
}

#ifdef CONFIG_BT_TRANSCEIVER
int tr_system_volume_set(int stream_type, int volume, bool display, bool remote_sync)
{
	int ret = 0;
	bool is_limited = false;
	static uint32_t volume_timestampe = 0;
	int old_volume = audio_system_get_stream_volume(stream_type);

#ifdef CONFIG_MEDIA_PLAYER
	media_player_t *player = media_player_get_current_main_player();

	/* set DA volume */
	if (player) {
		int da_volume_db = audio_policy_get_da_volume(stream_type, volume);
		media_player_set_volume_pre(player, da_volume_db, da_volume_db);
	}
#endif

	ret = audio_system_set_stream_volume(stream_type, volume);
	if (display) {
		if (ret == MAX_VOLUME_VALUE) {
			if ((uint32_t)(k_cycle_get_32() - volume_timestampe) / (sys_clock_hw_cycles_per_sec() / 1000000)> 500000) {
				sys_event_notify(SYS_EVENT_MAX_VOLUME);
				volume_timestampe = k_cycle_get_32();
			}
			is_limited = true;
		} else if (ret == MIN_VOLUME_VALUE) {
			if ((uint32_t)(k_cycle_get_32() - volume_timestampe) / (sys_clock_hw_cycles_per_sec() / 1000000) > 500000) {
				sys_event_notify(SYS_EVENT_MIN_VOLUME);
				volume_timestampe = k_cycle_get_32();
			}
			is_limited = true;
		}
	}
	if (old_volume == audio_system_get_stream_volume(stream_type)) {
		is_limited = true;
		ret = old_volume;
		goto exit;
	}

	ret = volume;
    
    if(remote_sync)
    {
#ifdef CONFIG_BT_MANAGER
#ifdef CONFIG_BT_AUDIO
        if (stream_type == AUDIO_STREAM_VOICE ||
                stream_type == AUDIO_STREAM_MUSIC ||
                stream_type == AUDIO_STREAM_LE_AUDIO ||
                stream_type == AUDIO_STREAM_LE_AUDIO_MUSIC ||
                stream_type == AUDIO_STREAM_LE_AUDIO_GAME ||
                stream_type == AUDIO_STREAM_USOUND ||
                stream_type == AUDIO_STREAM_TR_USOUND){
            bt_manager_volume_set(volume);
        }
#else
        if (stream_type == AUDIO_STREAM_VOICE) {
            bt_manager_hfp_sync_vol_to_remote(volume);
        } else if (stream_type == AUDIO_STREAM_MUSIC) {
#ifdef CONFIG_BT_AVRCP
            bt_manager_avrcp_sync_vol_to_remote(volume);
#endif
        }
#endif
#endif

#ifdef CONFIG_ESD_MANAGER
        {
            uint16_t volume_info = ((stream_type & 0xff) << 8) | (volume & 0xff);
            esd_manager_save_scene(TAG_VOLUME, (uint8_t *)&volume_info, 2);
        }
#endif

#ifdef CONFIG_TWS
        bt_manager_tws_sync_volume_to_slave(NULL, stream_type, volume);
#endif
    }
	SYS_LOG_INF("old_volume %d new_volume %d\n", old_volume, volume);
exit:

	if (display) {
		_system_volume_set_notify(ret, is_limited);
	}
	return ret;
}
#endif

int system_volume_set(int stream_type, int volume, bool display)
{
	int ret = 0;
	bool is_limited = false;
	static uint32_t volume_timestampe = 0;
	int old_volume = audio_system_get_stream_volume(stream_type);

#ifdef CONFIG_MEDIA_PLAYER
	media_player_t *player = media_player_get_current_main_player();

	/* set DA volume */
	if (player) {
		int da_volume_db = audio_policy_get_da_volume(stream_type, volume);
		media_player_set_volume_pre(player, da_volume_db, da_volume_db);
	}
#endif

	ret = audio_system_set_stream_volume(stream_type, volume);
	if (display) {
		if (ret == MAX_VOLUME_VALUE) {
			if ((uint32_t)(k_cycle_get_32() - volume_timestampe) / (sys_clock_hw_cycles_per_sec() / 1000000)> 500000) {
				sys_event_notify(SYS_EVENT_MAX_VOLUME);
				volume_timestampe = k_cycle_get_32();
			}
			is_limited = true;
		} else if (ret == MIN_VOLUME_VALUE) {
			if ((uint32_t)(k_cycle_get_32() - volume_timestampe) / (sys_clock_hw_cycles_per_sec() / 1000000) > 500000) {
				sys_event_notify(SYS_EVENT_MIN_VOLUME);
				volume_timestampe = k_cycle_get_32();
			}
			is_limited = true;
		}
	}
	if (old_volume == audio_system_get_stream_volume(stream_type)) {
		is_limited = true;
		ret = old_volume;
		goto exit;
	}

	ret = volume;

#ifdef CONFIG_BT_MANAGER
  #ifdef CONFIG_BT_AUDIO
	if (stream_type == AUDIO_STREAM_VOICE ||
		stream_type == AUDIO_STREAM_MUSIC ||
		stream_type == AUDIO_STREAM_LE_AUDIO ||
		stream_type == AUDIO_STREAM_LE_AUDIO_MUSIC ||
		stream_type == AUDIO_STREAM_LE_AUDIO_GAME ||
		stream_type == AUDIO_STREAM_USOUND ||
		stream_type == AUDIO_STREAM_TR_USOUND){
		bt_manager_volume_set(volume);
	}
  #else
	if (stream_type == AUDIO_STREAM_VOICE) {
		bt_manager_hfp_sync_vol_to_remote(volume);
	} else if (stream_type == AUDIO_STREAM_MUSIC) {
#ifdef CONFIG_BT_AVRCP
		bt_manager_avrcp_sync_vol_to_remote(volume);
#endif
	}
  #endif
#endif

#ifdef CONFIG_ESD_MANAGER
	{
		uint16_t volume_info = ((stream_type & 0xff) << 8) | (volume & 0xff);
		esd_manager_save_scene(TAG_VOLUME, (uint8_t *)&volume_info, 2);
	}
#endif

#ifdef CONFIG_TWS
	bt_manager_tws_sync_volume_to_slave(NULL, stream_type, volume);
#endif
	SYS_LOG_INF("old_volume %d new_volume %d\n", old_volume, volume);
exit:

	if (display) {
		_system_volume_set_notify(ret, is_limited);
	}
	return ret;
}

int system_volume_down(int stream_type, int decrement)
{
	return system_volume_set(stream_type,
				system_volume_get(stream_type) - decrement, true);
}

int system_volume_up(int stream_type, int increment)
{
	return system_volume_set(stream_type,
				system_volume_get(stream_type) + increment, true);
}

static void _system_volume_change_notify(uint32_t volume)
{
#ifdef CONFIG_BT_MANAGER
	struct bt_volume_report rep;
	rep.handle = 0;//to do
	rep.value = volume;
	bt_manager_volume_event(BT_VOLUME_VALUE, &rep, sizeof(rep));
#endif
	
}

void system_volume_sync_remote_to_device(uint32_t stream_type, uint32_t volume)
{
	int old_volume = audio_system_get_stream_volume(stream_type);

	if (old_volume == volume) {
		return;
	}

	SYS_LOG_INF("old_volume %d new_volume %d\n", old_volume, volume);

#ifdef CONFIG_TWS
	bt_manager_tws_sync_volume_to_slave(NULL, stream_type, volume);
#endif

#ifdef CONFIG_ESD_MANAGER
	{
		uint16_t volume_info = ((stream_type & 0xff) << 8) | (volume & 0xff);
		esd_manager_save_scene(TAG_VOLUME, (uint8_t *)&volume_info, 2);
	}
#endif

	_system_volume_change_notify(volume);

#ifdef CONFIG_TWS
	if (bt_manager_tws_get_dev_role() == BTSRV_TWS_SLAVE) {
		return;
	}
#endif
}


int system_player_volume_set(int stream_type, int volume)
{
	int ret = 0;
	int old_volume = audio_system_get_stream_volume(stream_type);
	media_player_t *player = media_player_get_current_main_player();

	/* set DA volume */
	if (player) {
		int da_volume_db = audio_policy_get_da_volume(stream_type, volume);
		media_player_set_volume_pre(player, da_volume_db, da_volume_db);
	}

	ret = audio_system_set_stream_volume(stream_type, volume);

	if (old_volume == audio_system_get_stream_volume(stream_type)) {
		ret = old_volume;
		goto exit;
	}
	ret = volume;

	SYS_LOG_INF("old_volume %d new_volume %d\n", old_volume, volume);
exit:

	return ret;
}



