/*
 * Copyright (c) 2019 Actions Semi Co., Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file
 * @brief bt manager avrcp profile.
 */
#define SYS_LOG_NO_NEWLINE
#define SYS_LOG_DOMAIN "bt manager"

#include <os_common_api.h>

#include <zephyr.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <bt_manager.h>
#include "bt_manager_inner.h"
#include "btservice_api.h"
#include <audio_policy.h>
#include <audio_system.h>
#include <volume_manager.h>
#include <msg_manager.h>

static const bt_mgr_event_strmap_t bt_manager_avrcp_event_map[] =
{
    {BTSRV_AVRCP_NONE_EV,                   STRINGIFY(BTSRV_AVRCP_NONE_EV)},
    {BTSRV_AVRCP_CONNECTED,    			    STRINGIFY(BTSRV_AVRCP_CONNECTED)},
    {BTSRV_AVRCP_DISCONNECTED,              STRINGIFY(BTSRV_AVRCP_DISCONNECTED)},
    {BTSRV_AVRCP_STOPED,                    STRINGIFY(BTSRV_AVRCP_STOPED)},
    {BTSRV_AVRCP_PAUSED,                    STRINGIFY(BTSRV_AVRCP_PAUSED)},
    {BTSRV_AVRCP_PLAYING,                   STRINGIFY(BTSRV_AVRCP_PLAYING)},
	{BTSRV_AVRCP_UPDATE_ID3_INFO,           STRINGIFY(BTSRV_AVRCP_UPDATE_ID3_INFO)},
    {BTSRV_AVRCP_TRACK_CHANGE,              STRINGIFY(BTSRV_AVRCP_TRACK_CHANGE)},
};

#define BT_MANAGER_AVRCP_EVENTNUM_STRS (sizeof(bt_manager_avrcp_event_map) / sizeof(bt_mgr_event_strmap_t))
const char *bt_manager_avrcp_evt2str(int num)
{
	return bt_manager_evt2str(num, BT_MANAGER_AVRCP_EVENTNUM_STRS, bt_manager_avrcp_event_map);
}

static void _bt_manager_avrcp_on_paused(bt_mgr_dev_info_t* dev_info)
{
	struct bt_media_play_status playstatus;	
	
    playstatus.handle = dev_info->hdl;
    playstatus.status = BT_STATUS_PAUSED;
    
    bt_manager_media_event(BT_MEDIA_PLAYBACK_STATUS_CHANGED_EVENT, (void*)&playstatus, 
        sizeof(struct bt_media_play_status)
    );

    if (btif_tws_get_dev_role() != BTSRV_TWS_SLAVE) {
        btif_a2dp_user_pause(dev_info->hdl);
    }
}

static void _bt_manager_avrcp_callback(uint16_t hdl, btsrv_avrcp_event_e event, void *param)
{
	struct bt_media_play_status playstatus;	
	bt_mgr_dev_info_t* dev_info = bt_mgr_find_dev_info_by_hdl(hdl);
	struct bt_audio_report rep;	
	if (dev_info == NULL)
		return;

	SYS_LOG_INF("hdl: 0x%x event: %s",dev_info->hdl, bt_manager_avrcp_evt2str(event));

	switch (event) {
	/** notifying that avrcp connected */
	case BTSRV_AVRCP_CONNECTED:
		{
			dev_info->avrcp_remote_vol = BT_MANAGER_INVALID_VOLUME;
			bt_manager_media_event(BT_MEDIA_CONNECTED, (void*)&hdl, sizeof(uint16_t));
			bt_manager_avrcp_sync_origin_vol(hdl);
		}
		break;
	/** notifying that avrcp disconnected*/
	case BTSRV_AVRCP_DISCONNECTED:
		{
		    dev_info->avrcp_ext_status = BT_MANAGER_AVRCP_EXT_STATUS_NONE;
			bt_manager_media_event(BT_MEDIA_DISCONNECTED, (void*)&hdl, sizeof(uint16_t));
		}
		break;
	/** notifying that avrcp stoped */
	case BTSRV_AVRCP_STOPED:
	/** notifying that avrcp paused */
	case BTSRV_AVRCP_PAUSED:
		{
			rep.id = BT_AUDIO_ENDPOINT_MUSIC;
			rep.audio_contexts = BT_AUDIO_CONTEXT_MEDIA;
			rep.handle = hdl;		
			bt_manager_media_event(BT_MEDIA_SERVER_PAUSE, (void*)&rep, sizeof(struct bt_audio_report));
		
            if (dev_info->avrcp_ext_status & BT_MANAGER_AVRCP_EXT_STATUS_SUSPEND) {
                dev_info->avrcp_ext_status = BT_MANAGER_AVRCP_EXT_STATUS_NONE;
            } else {
                dev_info->avrcp_ext_status = BT_MANAGER_AVRCP_EXT_STATUS_PAUSED;
            }
            if (!dev_info->a2dp_status_playing) {
                break;
            }
            dev_info->a2dp_status_playing = 0;
            
            _bt_manager_avrcp_on_paused(dev_info);
		}
		break;
	/** notifying that avrcp playing */
	case BTSRV_AVRCP_PLAYING:
		{
            rep.id = BT_AUDIO_ENDPOINT_MUSIC;
            rep.audio_contexts = BT_AUDIO_CONTEXT_MEDIA;
			rep.handle = hdl;		
			bt_manager_media_event(BT_MEDIA_SERVER_PLAY, (void*)&rep, sizeof(struct bt_audio_report));

            dev_info->avrcp_ext_status = BT_MANAGER_AVRCP_EXT_STATUS_PLAYING;
			if (dev_info->a2dp_status_playing)
			{
				break;
			}

			if (dev_info->a2dp_stream_started)
			{
				dev_info->a2dp_status_playing = 1;
			}

			if (dev_info->a2dp_status_playing)
			{
				bt_manager_avrcp_sync_playing_vol(hdl);
				playstatus.handle = hdl;
				playstatus.status = BT_STATUS_PLAYING;

				struct bt_media_report rep;
				rep.handle = hdl;
				bt_manager_media_event(BT_MEDIA_PLAY, &rep, sizeof(struct bt_media_report));
				bt_manager_media_event(BT_MEDIA_PLAYBACK_STATUS_CHANGED_EVENT, (void *)&playstatus, sizeof(struct bt_media_play_status));
			}

			if (!dev_info->a2dp_stream_started && btif_a2dp_stream_is_open(hdl))
			{
				uint16_t active_hdl = btif_a2dp_get_active_hdl();
				if (active_hdl == hdl)
				{
					/* FIX BUG02905791*/
					SYS_LOG_INF("hdl: 0x%x stream is open.", hdl);
					bt_manager_a2dp_check_state();
				}
			}			
		}
		break;
	/** notifying that avrcp cur track change */
	case BTSRV_AVRCP_TRACK_CHANGE:
		{
			bt_manager_media_event(BT_MEDIA_TRACK_CHANGED_EVENT,(void*)&hdl, sizeof(uint16_t));
		}
		break;
	/** notifying that id3 info */
	case BTSRV_AVRCP_UPDATE_ID3_INFO:
		{
#ifdef CONFIG_BT_AVRCP_GET_ID3
			int len = 0;
			hdl =  btif_a2dp_get_active_hdl();
			struct bt_attribute_info *pinfo = (struct bt_attribute_info *)param;
			for(int i =0 ; i< BT_TOTAL_ATTRIBUTE_ITEM_NUM; i++){
				len += sizeof(struct bt_attribute_info) + pinfo[i].len + 1;
			}
			SYS_LOG_INF("avrcp id3 info:%x\n",hdl);

			struct bt_media_id3_info * p_id3_info = bt_mem_malloc(len + sizeof(struct bt_media_id3_info));
			p_id3_info->handle = hdl;
			p_id3_info->num_of_item = BT_TOTAL_ATTRIBUTE_ITEM_NUM;
			struct bt_attribute_info *p_dinfo = p_id3_info->item;
			
			for(int i =0 ; i< BT_TOTAL_ATTRIBUTE_ITEM_NUM; i++){
				if(pinfo[i].len && pinfo[i].data){
					p_dinfo->id = pinfo[i].id;
					p_dinfo->character_id = pinfo[i].character_id;
					p_dinfo->len = pinfo[i].len + 1;
					p_dinfo->data = (uint8_t*)&p_dinfo[1];
					memcpy(p_dinfo->data, pinfo[i].data, pinfo[i].len);
					p_dinfo->data[p_dinfo->len - 1] = 0;
					SYS_LOG_INF("id3 %d %s\n",pinfo[i].id,p_dinfo->data);
					p_dinfo = (struct bt_attribute_info *)((uint8_t*)&p_dinfo[1] + p_dinfo->len);
				}	
			}
			bt_manager_media_event(BT_MEDIA_UPDATE_ID3_INFO_EVENT, (void *)p_id3_info,len + sizeof(struct bt_media_id3_info));
			if(p_id3_info)
				bt_mem_free(p_id3_info);

#endif
		}
		break;
	default:
		break;
	}
}

#if (defined CONFIG_VOLUME_MANAGER) || (defined CONFIG_BT_AVRCP_VOL_SYNC)
static uint32_t _bt_manager_music_to_avrcp_volume(uint32_t music_vol)
{
	uint32_t  avrcp_vol = 0;
#ifdef CONFIG_AUDIO
	uint32_t  max_volume = audio_policy_get_max_volume(AUDIO_STREAM_MUSIC);

	if (music_vol == 0) {
		avrcp_vol = 0;
	} else if (music_vol >= max_volume) {
		avrcp_vol = 0x7F;
	} else {
		avrcp_vol = (music_vol * 0x80 / max_volume);
	}
#endif
	return avrcp_vol;
}
#endif

#ifdef CONFIG_VOLUME_MANAGER
static uint32_t _bt_manager_avrcp_to_music_volume(uint32_t avrcp_vol)
{
	uint32_t  music_vol = 0;
	uint32_t  max_volume = audio_policy_get_max_volume(AUDIO_STREAM_MUSIC);

	if (avrcp_vol == 0) {
		music_vol = 0;
	} else if (avrcp_vol >= 0x7F) {
		music_vol = max_volume;
	} else {
		music_vol = (avrcp_vol + 1) * max_volume / 0x80;
		if (music_vol == 0) {
			music_vol = 1;
		}
	}

	return music_vol;
}
#endif

static void _bt_manager_avrcp_get_volume_callback(uint16_t hdl, uint8_t *volume, bool remote_reg)
{
#ifdef CONFIG_VOLUME_MANAGER
  #ifdef CONFIG_BT_EARPHONE_SPEC
	bt_mgr_dev_info_t* dev_info = bt_mgr_find_dev_info_by_hdl(hdl);
	btmgr_sync_ctrl_cfg_t *sync_ctrl_config = bt_manager_get_sync_ctrl_config();
	uint32_t  music_vol;
	uint8_t  need_update = 0;
	bool init_reg_flag = false;

	if (dev_info == NULL)
	{
		SYS_LOG_ERR("error \n");
		return;
	}

	if (remote_reg) 
	{
	    if (!dev_info->avrcp_remote_support_vol_sync) {
	        init_reg_flag = true;
	    }
		dev_info->avrcp_remote_support_vol_sync = 1;
		if(dev_info->new_dev)
		{
			SYS_LOG_INF("support_vol_sync, new_dev");		
			dev_info->bt_music_vol = sync_ctrl_config->bt_music_default_volume;
			dev_info->new_dev = 0;
			need_update =1;
		}
	}

	if (dev_info && dev_info->bt_music_vol <= audio_policy_get_max_volume(AUDIO_STREAM_MUSIC))
	{
		music_vol = dev_info->bt_music_vol;
	}
	else
	{
		music_vol = audio_system_get_stream_volume(AUDIO_STREAM_MUSIC);
	}

	if (need_update)
	{
		bt_mgr_dev_info_t * a2dp_active_dev = bt_mgr_get_a2dp_active_dev();
		bool notify_app = true;
		/* no current A2DP active device
		 */
		if (a2dp_active_dev && dev_info->hdl != a2dp_active_dev->hdl)
		{
			SYS_LOG_INF("no active ad2p dev\n");
			notify_app = false;
		}
		bt_manager_save_dev_volume();	
		bt_manager_sync_volume_from_phone(&dev_info->addr, true, dev_info->bt_music_vol, false, notify_app);	
	}
	
	*volume = (uint8_t)_bt_manager_music_to_avrcp_volume(music_vol);
	

    /* init register avrcp volume 0x7f that not pop volume bar on phone device? */
	if (init_reg_flag) {
	    *volume = 0x7f;
	}
	SYS_LOG_INF("0x%x, %d, [%d, %d]", dev_info->hdl, remote_reg, music_vol, *volume);

  #else
	uint32_t  music_vol = audio_system_get_stream_volume(AUDIO_STREAM_MUSIC);
	*volume = (uint8_t)_bt_manager_music_to_avrcp_volume(music_vol);
  #endif
#endif

}

static void _bt_manager_avrcp_set_volume_callback(uint16_t hdl, uint8_t volume, bool sync_to_remote_pending)
{
#ifdef CONFIG_VOLUME_MANAGER
	uint32_t music_vol = (uint8_t)_bt_manager_avrcp_to_music_volume(volume);
	SYS_LOG_INF("volume %d\n", volume);
	bt_manager_avrcp_sync_vol_to_local(hdl, music_vol, sync_to_remote_pending);
#endif
}


void bt_manager_avrcp_sync_vol_to_local(uint16_t hdl, uint8_t music_vol, bool sync_to_remote_pending)
{
#ifdef CONFIG_VOLUME_MANAGER
  #ifndef CONFIG_BT_EARPHONE_SPEC
    system_volume_sync_remote_to_device(AUDIO_STREAM_MUSIC, music_vol);
  #else
    bt_mgr_dev_info_t* dev_info = bt_mgr_find_dev_info_by_hdl(hdl);
    btmgr_sync_ctrl_cfg_t * cfg =  bt_manager_get_sync_ctrl_config();
    uint8_t  avrcp_vol;
    bt_mgr_dev_info_t * a2dp_active_dev = bt_mgr_get_a2dp_active_dev();
    bool notify_app = true;

    if (dev_info == NULL || 
		dev_info->is_tws || 
		btif_tws_get_dev_role() == BTSRV_TWS_SLAVE)
    {
        return;
    }
    avrcp_vol = (uint8_t)_bt_manager_music_to_avrcp_volume(music_vol);
    dev_info->avrcp_remote_vol = avrcp_vol;

    if (sync_to_remote_pending) {
        SYS_LOG_INF("sync_to_remote_pending");
        return;
    }
    
    if (cfg->a2dp_volume_sync_when_playing)
    {
        if (!dev_info->a2dp_status_playing)
        {
			SYS_LOG_INF("dev no playing\n");        
            return;
        }
    }

    dev_info->bt_music_vol = music_vol;
    bt_manager_save_dev_volume();	

    /* no current A2DP active device
     */
    if (a2dp_active_dev && dev_info->hdl != a2dp_active_dev->hdl)
    {
		SYS_LOG_INF("no active ad2p dev\n");
        notify_app = false;
    }
	SYS_LOG_INF("hdl 0x%x bt_music_vol: %d,avrcp_remote_vol: %d\n", dev_info->hdl, dev_info->bt_music_vol,dev_info->avrcp_remote_vol);

    bt_manager_sync_volume_from_phone(&dev_info->addr, true, music_vol, false, notify_app);
  #endif
#endif
}



void bt_manager_avrcp_sync_origin_vol(uint16_t hdl)
{
#ifdef CONFIG_VOLUME_MANAGER
  #ifdef CONFIG_BT_EARPHONE_SPEC
    btmgr_sync_ctrl_cfg_t * cfg =  bt_manager_get_sync_ctrl_config();
    bt_mgr_dev_info_t* dev_info = bt_mgr_find_dev_info_by_hdl(hdl);

    if (dev_info == NULL ||
        dev_info->is_tws ||
        btif_tws_get_dev_role() == BTSRV_TWS_SLAVE)
    {
        return;
    }

    if (cfg->a2dp_origin_volume_sync_to_remote  == 0 ||
        cfg->a2dp_volume_sync_when_playing == 1)
    {
        return;
    }

    SYS_LOG_INF("volume %d\n", dev_info->bt_music_vol);

    bt_manager_avrcp_sync_vol_to_remote_by_addr(hdl, dev_info->bt_music_vol,
        cfg->a2dp_origin_volume_sync_delay_ms
    );
  #endif	
#endif
}


void bt_manager_avrcp_sync_playing_vol(uint16_t hdl)
{
#ifdef CONFIG_VOLUME_MANAGER
  #ifdef CONFIG_BT_EARPHONE_SPEC
    btmgr_sync_ctrl_cfg_t * cfg =  bt_manager_get_sync_ctrl_config();
    bt_mgr_dev_info_t* dev_info = bt_mgr_find_dev_info_by_hdl(hdl);

    if (dev_info == NULL ||
		dev_info->is_tws ||
        btif_tws_get_dev_role() == BTSRV_TWS_SLAVE)
    {
        return;
    }
	SYS_LOG_INF("volume %d\n", dev_info->bt_music_vol);

    if (cfg->a2dp_volume_sync_when_playing == 0)
    {
        return;
    }

    bt_manager_avrcp_sync_vol_to_remote_by_addr(hdl, dev_info->bt_music_vol, 
        cfg->a2dp_Playing_volume_sync_delay_ms
    );
  #endif
#endif
}

static void _bt_manager_avrcp_pass_ctrl_callback(uint16_t hdl, uint8_t cmd, uint8_t state)
{
	SYS_LOG_INF("cmd %d\n", cmd);

	if(state){//pushed
#ifdef CONFIG_VOLUME_MANAGER
		uint32_t  music_vol = audio_system_get_stream_volume(AUDIO_STREAM_MUSIC);
		uint32_t  max_volume = audio_policy_get_max_volume(AUDIO_STREAM_MUSIC);
		if (cmd == BTSRV_AVRCP_CMD_VOLUMEUP) {
			if (music_vol == max_volume) {
				return;
			}
			bt_manager_avrcp_sync_vol_to_local(hdl, music_vol + 1, false);
		} else if (cmd == BTSRV_AVRCP_CMD_VOLUMEDOWN) {
			if (music_vol == 0) {
				return;
			}
			bt_manager_avrcp_sync_vol_to_local(hdl, music_vol - 1, false);
		}
#endif
	}
}

static const btsrv_avrcp_callback_t btm_avrcp_ctrl_cb = {
	.event_cb = _bt_manager_avrcp_callback,
	.get_volume_cb = _bt_manager_avrcp_get_volume_callback,
	.set_volume_cb = _bt_manager_avrcp_set_volume_callback,
	.pass_ctrl_cb = _bt_manager_avrcp_pass_ctrl_callback,
};

int bt_manager_avrcp_profile_start(void)
{
	return btif_avrcp_start((btsrv_avrcp_callback_t *)&btm_avrcp_ctrl_cb);
}

int bt_manager_avrcp_profile_stop(void)
{
	return btif_avrcp_stop();
}

int bt_manager_avrcp_play(void)
{
	return btif_avrcp_send_command(BTSRV_AVRCP_CMD_PLAY);
}

int bt_manager_avrcp_stop(void)
{
	return btif_avrcp_send_command(BTSRV_AVRCP_CMD_STOP);
}

int bt_manager_avrcp_pause(void)
{
#ifdef CONFIG_BT_EARPHONE_SPEC
	bt_mgr_dev_info_t* a2dp_dev = bt_mgr_get_a2dp_active_dev();
	
	if (a2dp_dev != NULL &&
	    a2dp_dev->a2dp_status_playing &&
	    a2dp_dev->a2dp_stream_started &&
	    a2dp_dev->avrcp_connected &&
	    a2dp_dev->avrcp_ext_status != BT_MANAGER_AVRCP_EXT_STATUS_NONE)
	{
	    SYS_LOG_INF("force_paused");
	    a2dp_dev->a2dp_status_playing = 0;

	    bt_manager_tws_sync_a2dp_status(a2dp_dev->hdl, NULL);
	    _bt_manager_avrcp_on_paused(a2dp_dev);
	}
#endif
	return btif_avrcp_send_command(BTSRV_AVRCP_CMD_PAUSE);
}

int bt_manager_avrcp_play_next(void)
{
	return btif_avrcp_send_command(BTSRV_AVRCP_CMD_FORWARD);
}

int bt_manager_avrcp_play_previous(void)
{
	return btif_avrcp_send_command(BTSRV_AVRCP_CMD_BACKWARD);
}

int bt_manager_avrcp_fast_forward(bool start)
{
	if (start) {
		btif_avrcp_send_command(BTSRV_AVRCP_CMD_FAST_FORWARD_START);
	} else {
		btif_avrcp_send_command(BTSRV_AVRCP_CMD_FAST_FORWARD_STOP);
	}
	return 0;
}

int bt_manager_avrcp_fast_backward(bool start)
{
	if (start) {
		btif_avrcp_send_command(BTSRV_AVRCP_CMD_FAST_BACKWARD_START);
	} else {
		btif_avrcp_send_command(BTSRV_AVRCP_CMD_FAST_BACKWARD_STOP);
	}
	return 0;
}

int bt_manager_avrcp_sync_vol_to_remote_by_addr(uint16_t hdl, uint32_t music_vol, uint16_t delay_ms)
{
#ifdef CONFIG_BT_AVRCP_VOL_SYNC
    uint32_t avrcp_vol = _bt_manager_music_to_avrcp_volume(music_vol);

#ifdef CONFIG_BT_EARPHONE_SPEC
    btmgr_sync_ctrl_cfg_t * cfg =  bt_manager_get_sync_ctrl_config();
    bt_mgr_dev_info_t* dev_info;

	if (hdl)
		dev_info = bt_mgr_find_dev_info_by_hdl(hdl);
    else
		dev_info = bt_mgr_get_a2dp_active_dev();

    if (dev_info == NULL || 
		dev_info->is_tws || 
		btif_tws_get_dev_role() == BTSRV_TWS_SLAVE)
    {
        return 0;
    }

	if (cfg->a2dp_volume_sync_when_playing)
	{
		if (!dev_info->a2dp_status_playing)
		{
			SYS_LOG_INF("dev no playing\n");		
			return 0;
		}
	}
		
	if (!dev_info->avrcp_connected)
	{
		SYS_LOG_INF("dev no avrcp_connected\n");
		return 0;
	}

    if (music_vol <= audio_policy_get_max_volume(AUDIO_STREAM_MUSIC))
    {
        dev_info->bt_music_vol = music_vol;
    }
    else
    {
        dev_info->bt_music_vol = audio_system_get_stream_volume(AUDIO_STREAM_MUSIC);
    }

	avrcp_vol = _bt_manager_music_to_avrcp_volume(music_vol);

    SYS_LOG_INF("0x%x, %d, %d->%d", dev_info->hdl, music_vol, dev_info->avrcp_remote_vol, avrcp_vol);
    if (dev_info->avrcp_remote_vol == avrcp_vol)
    {
        return 0;
    }

	dev_info->avrcp_remote_vol = avrcp_vol;
	bt_manager_save_dev_volume();

	btmgr_feature_cfg_t *cfg_feature = bt_manager_get_feature_config();
	if(!cfg_feature->sp_avrcp_vol_syn){
		return 0;
	}
#endif

	return btif_avrcp_sync_vol(hdl, delay_ms);
#else
	return 0;
#endif
}


int bt_manager_avrcp_sync_vol_to_remote(uint32_t music_vol)
{
	return bt_manager_avrcp_sync_vol_to_remote_by_addr(0, music_vol, 0);
}


#ifdef CONFIG_BT_AVRCP_GET_ID3
int bt_manager_avrcp_get_id3_info(void)
{
	return btif_avrcp_get_id3_info();
}
#else
int bt_manager_avrcp_get_id3_info(void)
{
	return -EIO;
}
#endif

int bt_manager_avrcp_set_absolute_volume(uint8_t *data, uint8_t len)
{
	return btif_avrcp_set_absolute_volume(data, len);
}

