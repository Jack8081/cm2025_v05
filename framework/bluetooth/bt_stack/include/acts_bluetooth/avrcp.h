/** @file
 * @brief Audio/Video Remote control Protocol header.
 */

/*
 * Copyright (c) 2015-2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef __BT_AVRCP_H
#define __BT_AVRCP_H

#ifdef __cplusplus
extern "C" {
#endif

enum {
	BT_AVRCP_PASS_THROUGH_PUSHED = 0,
	BT_AVRCP_PASS_THROUGH_RELEASED = 1,
};

enum {
	BT_AVRCP_CMD_STATE_PASS_THROUGH_PUSHED = 0,
	BT_AVRCP_CMD_STATE_PASS_THROUGH_RELEASED = 1,
	BT_AVRCP_RSP_STATE_PASS_THROUGH_PUSHED = 0x80,
	BT_AVRCP_RSP_STATE_PASS_THROUGH_RELEASED = 0x81,
};

typedef enum {
	AVRCP_OPERATION_ID_SKIP = 0x3C,
	AVRCP_OPERATION_ID_VOLUME_UP = 0x41,
	AVRCP_OPERATION_ID_VOLUME_DOWN = 0x42,
	AVRCP_OPERATION_ID_MUTE = 0x43,

	AVRCP_OPERATION_ID_PLAY = 0x44,
	AVRCP_OPERATION_ID_STOP = 0x45,
	AVRCP_OPERATION_ID_PAUSE = 0x46,
	AVRCP_OPERATION_ID_REWIND = 0x48,
	AVRCP_OPERATION_ID_FAST_FORWARD = 0x49,
	AVRCP_OPERATION_ID_FORWARD = 0x4B,
	AVRCP_OPERATION_ID_BACKWARD = 0x4C,
	AVRCP_OPERATION_ID_UNDEFINED = 0xFF
} avrcp_op_id;

enum {
	BT_AVRCP_EVENT_PLAYBACK_STATUS_CHANGED = 0x01,
	BT_AVRCP_EVENT_TRACK_CHANGED = 0x02,
	BT_AVRCP_EVENT_TRACK_REACHED_END = 0x03,
	BT_AVRCP_EVENT_TRACK_REACHED_START = 0x04,
	BT_AVRCP_EVENT_PLAYBACK_POS_CHANGED = 0x05,
	BT_AVRCP_EVENT_BATT_STATUS_CHANGED = 0x06,
	BT_AVRCP_EVENT_SYSTEM_STATUS_CHANGED = 0x07,
	BT_AVRCP_EVENT_PLAYER_APPLICATION_SETTING_CHANGED = 0x08,
	BT_AVRCP_EVENT_NOW_PLAYING_CONTENT_CHANGED = 0x09,
	BT_AVRCP_EVENT_AVAILABLE_PLAYERS_CHANGED = 0x0a,
	BT_AVRCP_EVENT_ADDRESSED_PLAYER_CHANGED = 0x0b,
	BT_AVRCP_EVENT_UIDS_CHANGED = 0x0c,
	BT_AVRCP_EVENT_VOLUME_CHANGED = 0x0d,
};

enum {
	BT_AVRCP_PLAYBACK_STATUS_STOPPED = 0x00,
	BT_AVRCP_PLAYBACK_STATUS_PLAYING = 0x01,
	BT_AVRCP_PLAYBACK_STATUS_PAUSED = 0x02,
	BT_AVRCP_PLAYBACK_STATUS_FWD_SEEK = 0x03,
	BT_AVRCP_PLAYBACK_STATUS_REV_SEEK = 0x04,
	BT_AVRCP_PLAYBACK_STATUS_ERROR = 0xFF,
};

enum {
	BT_AVRCP_ATTRIBUTE_ID_TITLE  = 0x01,
	BT_AVRCP_ATTRIBUTE_ID_ARTIST = 0x02,
	BT_AVRCP_ATTRIBUTE_ID_ALBUM  = 0x03,
	BT_AVRCP_ATTRIBUTE_ID_GENRE  = 0x06,
	BT_AVRCP_ATTRIBUTE_ID_TIME   = 0x07,
};

struct attribute_info{
	uint32_t id;
	uint16_t character_id;
	uint16_t len;
	uint8_t* data;
}__packed;

#define TOTAL_ATTRIBUTE_ITEM_NUM   5

struct id3_info{
	struct attribute_info item[TOTAL_ATTRIBUTE_ITEM_NUM];
}__packed;

#ifdef __cplusplus
}
#endif

#endif /* __BT_AVRCP_H */
