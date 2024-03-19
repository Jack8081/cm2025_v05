﻿#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <os_common_api.h>
#include <ui_mem.h>
#include <memory/mem_cache.h>
#include "res_manager_api.h"
#include "res_mempool.h"
#ifndef CONFIG_SIMULATOR
#include "lz4.h"

#else
#include <fs/fs.h>

#define FS_O_READ		0x01
#define FS_SEEK_SET     SEEK_SET
#define FS_SEEK_END     SEEK_END
#endif

#ifdef CONFIG_SIMULATOR
#define MAX_PARTITION_ID	0
#else
#define MAX_PARTITION_ID	1
#endif

#define RES_MEM_DEBUG			0

#define SCENE_ITEM_SIZE 3

#define MAX_REGULAR_NUM			512

#define STY_PATH_MAX_LEN		24
#ifndef CONFIG_RES_COMPACT_BUFFER_BLOCK_SIZE
#define COMPACT_BUFFER_BLOCK_SIZE		360*360*2+64
#else
#define COMPACT_BUFFER_BLOCK_SIZE		CONFIG_RES_COMPACT_BUFFER_BLOCK_SIZE
#endif
#define COMPACT_BUFFER_MAX_PAD_SIZE		4*1024
#define COMPACT_BUFFER_MARGIN_SIZE		64

#define PACK __attribute__ ((packed))

//以下宏定义资源图片的类型
typedef enum{
    RES_TYPE_INVALID                 = 0,
    RES_TYPE_PIC                     = 1,  // 16 bit picture (RGB_565, R[15:11], G[10:5], B[4:0])
    RES_TYPE_STRING                  = 2,  // string
    RES_TYPE_MSTRING,                      // multi-Language string
    RES_TYPE_LOGO                    = 4,  // BIN format picture
    RES_TYPE_PNG,                          // 24 bit picture with alpha (ARGB_8565, A[23:16], R[15:11], G[10:5], B[4:0])
    RES_TYPE_PIC_4,
    RES_TYPE_PIC_COMPRESSED          = 7,  // compressed 16 bit picture (compressed RGB_565)
    RES_TYPE_PNG_COMPRESSED,               // compressed 24 bit compressed picture with alpha (compressed ARGB_8565)
    RES_TYPE_LOGO_COMPRESSED,              // compressed BIN format picture

    RES_TYPE_PIC_RGB888              = 10, // 24 bit picture without alpha (RGB_888, R[23:16], G[16:8], B[7:0])
    RES_TYPE_PIC_ARGB8888,                 // 32 bit picture with alpha (ARGB_8888, , A[31:24], R[23:16], G[16:8], B[7:0])
    RES_TYPE_PIC_RGB888_COMPRESSED,        // compressed 24 bit picture without alpha (RGB_888)
    RES_TYPE_PIC_ARGB8888_COMPRESSED = 13, // compressed 32 bit picture with alpha (ARGB_8888)

    RES_TYPE_PIC_LZ4 = 14,		// 16 bit picture (RGB_565, R[15:11], G[10:5], B[4:0]) with LZ4 compressed
    RES_TYPE_PIC_ARGB8888_LZ4,	// 32 bit picture with alpha (ARGB_8888, , A[31:24], R[23:16], G[16:8], B[7:0]) with LZ4 compressed

    NUM_RES_TYPE,
}res_type_e;

//style header
typedef struct
{
    uint32_t    scene_sum;
    uint32_t	reserve;
}style_head_t;



//资源文件数据结构
typedef struct
{
    uint8_t  magic[4];        //'R', 'E', 'S', 0x19
    uint16_t counts;
    uint16_t w_format;
    uint8_t brgb;
    uint8_t version;
	uint16_t id_start;
    uint8_t reserved[3];
    uint8_t ch_extend;
}res_head_t;

typedef struct
{
    uint32_t   offset;
    uint16_t   length;
    uint8_t    type;
    uint8_t    name[8];
    uint8_t    len_ext;
}res_entry_t;

typedef struct _buf_block_s
{
	uint32_t source;
	uint32_t id;
	uint32_t size;
	uint32_t ref;
	uint32_t width;
	uint32_t height;
	uint32_t bytes_per_pixel;
	uint32_t format;
	uint8_t* addr;
	uint32_t regular_info;
	struct _buf_block_s* next;
}buf_block_t;

typedef struct
{
	compact_buffer_t* compact_buffer_list;
	buf_block_t* head;
}resource_buffer_t;

typedef struct _regular_info_s
{
	uint32_t scene_id;
	uint32_t magic;
	uint32_t num;
	uint32_t* id;
	struct _regular_info_s* next;	
}regular_info_t;



static resource_buffer_t bitmap_buffer;
static resource_buffer_t text_buffer;
static resource_buffer_t scene_buffer;
static regular_info_t* regular_info_list;
static int screen_bitmap_size;
static int ui_mem_total = 0;

os_mutex bitmap_cache_mutex;
os_mutex bitmap_read_mutex;

//#define MAX_RES_VERSIONS 1
//static const char* part_path[6] = {"/NAND:A/","/NAND:B","/NAND:C/","/NAND:D", "/NAND:E/","/NAND:F"};
//static char latest_partition;
static const char storage_part_name[10] = {'A','B','C','D','E','F','G','H','I','J'};


	
void _resource_buffer_deinit(uint32_t force_clear)
{
	buf_block_t* item;
	buf_block_t* listp;

	if(force_clear)
	{
		listp = text_buffer.head;
		while(listp != NULL)
		{
			item = listp;
			listp = listp->next;
			res_mem_free(RES_MEM_POOL_TXT, item);
		}
		memset(&text_buffer, 0, sizeof(text_buffer));

		listp = scene_buffer.head;
		while(listp != NULL)
		{
			item = listp;
			listp = listp->next;
			res_mem_free(RES_MEM_POOL_SCENE, item);
		}
		memset(&scene_buffer, 0, sizeof(scene_buffer));

		listp = bitmap_buffer.head;
		while(listp != NULL)
		{		
			item = listp;
			listp = listp->next;
			if(item->size > sizeof(buf_block_t))
			{
				res_mem_free(RES_MEM_POOL_BMP, item);
			}
			else
			{
				SYS_LOG_DBG("%d ui mem free %d\n", __LINE__, item->id);
				ui_memory_free(item->addr);
				ui_mem_total--;
				res_mem_free(RES_MEM_POOL_BMP, item);
			}
		}
		memset(&bitmap_buffer, 0, sizeof(bitmap_buffer));		
	}

}

void _resource_buffer_init(void)
{
	memset(&bitmap_buffer, 0, sizeof(bitmap_buffer));
	memset(&text_buffer, 0, sizeof(text_buffer));
	memset(&scene_buffer, 0, sizeof(scene_buffer));
	regular_info_list = NULL;
}

compact_buffer_t* _init_compact_buffer(uint32_t scene_id, uint32_t request_size)
{
	compact_buffer_t* buffer;
	buf_block_t* item;

	buffer = (compact_buffer_t*)res_array_alloc(RES_MEM_SIMPLE_COMPACT, sizeof(compact_buffer_t));
	if(buffer == NULL)
	{
		SYS_LOG_ERR("malloc compact buffer struct failed\n");
		return NULL;
	}

	buffer->addr = (uint8_t*)res_mem_alloc(RES_MEM_POOL_BMP, request_size);
	if(buffer->addr == NULL)
	{
		SYS_LOG_ERR("no enough sapce for compact buffer, size %d\n", request_size);	
		compact_buffer_t* citem = bitmap_buffer.compact_buffer_list;
		while(citem != NULL)
		{
			SYS_LOG_INF("compact buffer addr %p, size %d, freesize %d, sceneid 0x%x\n", citem->addr, citem->offset, citem->free_size, citem->scene_id);
			citem = citem->next;
		}
		
		res_array_free(buffer);
		return NULL;
	}

	//init first item block to prevent false check
	item = (buf_block_t*)buffer->addr;
	item->regular_info = 0;
	
	buffer->scene_id = scene_id;
	buffer->free_size = request_size;
	buffer->offset = 0;

#if RES_MEM_DEBUG
	{
		compact_buffer_t* check = bitmap_buffer.compact_buffer_list;
		uint32_t check_count = 0;
		while(check != NULL)
		{
			if(check == buffer)
			{
				SYS_LOG_ERR("invalid compact buffer list for buffer 0x%x:\n", buffer);
				break;
			}
			check = check->next;
			check_count++;
			if(check_count > 500)
			{
				SYS_LOG_ERR("loophole compact buffer list for buffer 0x%x:\n", buffer);
				break;
			}
		}
		
		if(check != NULL)
		{
			check = bitmap_buffer.compact_buffer_list;
			check_count = 0;
			while(check != NULL)
			{
				SYS_LOG_ERR("compact_buffer: 0x%x, addr 0x%x\n", check, check->addr);
				check = check->next;
				check_count++;
				if(check_count > 500)
				{
					break;
				}				
			}

			res_mem_free(RES_MEM_POOL_BMP, buffer->addr);
			res_array_free(buffer);
			return NULL;
			
		}
	}
#endif

	buffer->next = bitmap_buffer.compact_buffer_list;
	bitmap_buffer.compact_buffer_list = buffer;
//	SYS_LOG_INF("\n testalloc compact buffer inited for 0x%x, size %d, buffer %p, addr %p\n", scene_id, request_size, buffer, buffer->addr);
#if RES_MEM_DEBUG
	if(res_mem_check()<0)
	{
		SYS_LOG_INF("%s %d\n", __FILE__, __LINE__);	
		res_mem_dump();
	}
#endif

	return buffer;
}



uint8_t* _get_resource_scene_buffer(uint32_t source, uint32_t id, uint32_t size)
{
	buf_block_t* item = NULL;

	item = scene_buffer.head;
	while(item != NULL)
	{
		if((item->source == source)&&(item->id == id))
		{
			break;
		}
		item = item->next;
	}

	if(item != NULL)
	{
		//found loaded scene, which is not supposed to happen but fine
		item->ref ++;
		return item->addr;
	}
	else
	{
		item = (buf_block_t*)res_mem_alloc(RES_MEM_POOL_SCENE, sizeof(buf_block_t) + size);
		if(item == NULL)
		{
			SYS_LOG_ERR("error: scene buffer not enough for size %d", size);
			return NULL;
		}
		SYS_LOG_INF("item %p", item);
		item->source = source;
		item->id = id;
		item->size = sizeof(buf_block_t) + size;
		item->ref = 1;
		item->addr = (uint8_t*)item+sizeof(buf_block_t);
		item->next = scene_buffer.head;
		scene_buffer.head = item;

#if RES_MEM_DEBUG
		if(res_mem_check()<0)
		{
			SYS_LOG_INF("%s %d\n", __FILE__, __LINE__);
			res_mem_dump();
		}
#endif		
		return item->addr;
	}
}

uint8_t* _try_resource_scene_buffer(uint32_t source, uint32_t id)
{
	buf_block_t* item;

	item = scene_buffer.head;
	while(item != NULL)
	{
		if((item->source == source)&&(item->id == id))
		{
			break;
		}
		item = item->next;
	}

	if(item != NULL)
	{
		//found loaded text
		item->ref ++;
		return item->addr;
	}
	else
	{
		//not found, need to load from file
		return NULL;
	}
}

void _free_resource_scene_buffer(void* p)
{
	buf_block_t* item = scene_buffer.head;
	buf_block_t* prev = NULL;

	if(p == NULL)
	{
		return;
	}

	while(item != NULL)
	{
		if(item->addr == p)
		{
			break;
		}
		prev = item;
		item = item->next;
	}

	if(item == NULL)
	{
		SYS_LOG_ERR("scene not found");
		return;
	}
	else if((item != NULL)&&(item->ref > 1))
	{
		item->ref --;
		//FIXME
	}
	else
	{
		if(prev == NULL)
		{
			scene_buffer.head = item->next;
		}
		else
		{
			prev->next = item->next;
		}
		res_mem_free(RES_MEM_POOL_SCENE, (void*)item);

#if RES_MEM_DEBUG
		if(res_mem_check()<0)
		{
			SYS_LOG_INF("%s %d\n", __FILE__, __LINE__);
			res_mem_dump();
		}
#endif		
	}
}

uint8_t* _get_resource_text_buffer(uint32_t source, uint32_t id, uint32_t size)
{
	buf_block_t* item;

	item = text_buffer.head;
	while(item != NULL)
	{
		if((item->source == source)&&(item->id == id))
		{
			break;
		}
		item = item->next;
	}

	if(item != NULL)
	{
		//found loaded text, which is not supposed to happen but fine
		item->ref ++;
		return item->addr;
	}
	else
	{
		item = (buf_block_t*)res_mem_alloc(RES_MEM_POOL_TXT, sizeof(buf_block_t) + size);
		if(item == NULL)
		{
			SYS_LOG_ERR("error: resource buffer not enough for size 0x%x", size);
			return NULL;
		}
		item->source = source;
		item->id = id;
		item->size = sizeof(buf_block_t) + size;
		item->ref = 1;
		item->addr = (uint8_t*)item+sizeof(buf_block_t);
		item->next = text_buffer.head;
		text_buffer.head = item;

		return item->addr;
	}

}


uint8_t* _try_resource_text_buffer(uint32_t source, uint32_t id)
{
	buf_block_t* item;

	item = text_buffer.head;
	while(item != NULL)
	{
		if((item->source == source)&&(item->id == id))
		{
			break;
		}
		item = item->next;
	}

	if(item != NULL)
	{
		//found loaded text
		item->ref ++;
		return item->addr;
	}
	else
	{
		//not found, need to load from file
		return NULL;
	}
}

void _free_resource_text_buffer(void* p)
{
	buf_block_t* item = text_buffer.head;
	buf_block_t* prev = NULL;

	while(item != NULL)
	{
		if(item->addr == p)
		{
			break;
		}
		prev = item;
		item = item->next;
	}

	if(item == NULL)
	{
		SYS_LOG_ERR("resource text %p not found", p);
		return;
	}
	else if((item != NULL)&&(item->ref > 1))
	{
		item->ref --;
		//FIXME
		return;
	}
	else
	{
		if(prev == NULL)
		{
			text_buffer.head = item->next;
		}
		else
		{
			prev->next = item->next;
		}
		res_mem_free(RES_MEM_POOL_TXT, (void*)item);
	}
}


uint8_t* _get_resource_bitmap_buffer_compact(uint32_t source, resource_bitmap_t* bitmap, uint32_t size, uint32_t scene_id)
{
	compact_buffer_t* buffer;
	buf_block_t* item = NULL;
	uint8_t* ui_mem_buf = NULL;

	buffer = bitmap_buffer.compact_buffer_list;
	size = ((size+3)/4)*4;  //aligned next offset to 4 bytes

	if(size == screen_bitmap_size)
	{
		ui_mem_buf = ui_memory_alloc(size);
		if(ui_mem_buf == NULL)
		{
			SYS_LOG_ERR("cant malloc screen size bitmap buffer from ui mem \n");
			res_manager_dump_info();
			return NULL;
		}
		SYS_LOG_DBG("1 ui mem alloc %d, addr %p, scene 0x%x\n", bitmap->sty_data->id, ui_mem_buf, scene_id);		
		ui_mem_total++;
	}

	if(bitmap->regular_info != 0)
	{
		if(size == screen_bitmap_size)
		{
			buffer = _init_compact_buffer(scene_id, sizeof(buf_block_t)+COMPACT_BUFFER_MARGIN_SIZE);
			if(buffer == NULL)
			{
				SYS_LOG_DBG("%d ui mem free %d\n", __LINE__, bitmap->sty_data->id);
				ui_memory_free(ui_mem_buf);
				ui_mem_total--;
				return NULL;
			}
		}
		else
		{
			buffer = _init_compact_buffer(scene_id, size+sizeof(buf_block_t)+COMPACT_BUFFER_MARGIN_SIZE);
			if(buffer == NULL)
			{
				return NULL;
			}
		}
		item = (buf_block_t*)(buffer->addr + buffer->offset);
	}
	else
	{
		while(buffer != NULL)
		{
	//		SYS_LOG_INF("buffer->scene_id 0x%x\n", buffer->scene_id);
			if(buffer->scene_id == scene_id)
			{
				//check current buffer free size
				if(size != screen_bitmap_size && buffer->free_size >= size+sizeof(buf_block_t))
				{
					break;
				}

				if(size == screen_bitmap_size && buffer->free_size >= sizeof(buf_block_t))
				{
					break;
				}
			}
			buffer = buffer->next;
		}

		if(buffer == NULL)
		{
			//malloc a new compact buffer for scene id
			buffer = _init_compact_buffer(scene_id, size+sizeof(buf_block_t)+COMPACT_BUFFER_MARGIN_SIZE);
			if(buffer == NULL)
			{
				return NULL;
			}
		}

		item = (buf_block_t*)(buffer->addr+buffer->offset);		
	}
	item->source = source;
	item->id = bitmap->sty_data->id;
	item->ref = 1;
	item->width = (uint32_t)bitmap->sty_data->width;
	item->height = (uint32_t)bitmap->sty_data->height;
	item->bytes_per_pixel = (uint32_t)bitmap->sty_data->bytes_per_pixel;
	item->format = (uint32_t)bitmap->sty_data->format;
	if(size == screen_bitmap_size)
	{
		item->size = sizeof(buf_block_t);
		item->addr = ui_mem_buf;
	}
	else
	{		
		item->size = sizeof(buf_block_t) + size;
		item->addr = (uint8_t *)item + sizeof(buf_block_t);
	}
	item->next = bitmap_buffer.head;
	item->regular_info = bitmap->regular_info;
	bitmap_buffer.head = item;

//	SYS_LOG_INF("\n\n cache bitmap %d return 0x%x\n", id, item->addr);
	buffer->offset += item->size;
	buffer->free_size -= item->size;

#if RES_MEM_DEBUG
	if(res_mem_check()<0)
	{
		SYS_LOG_INF("%s %d\n", __FILE__, __LINE__);
		res_mem_dump();
	}
#endif	
	return item->addr;

}



uint8_t* _get_resource_bitmap_buffer(uint32_t source, resource_bitmap_t* bitmap, uint32_t size, uint32_t force_ref)
{
	buf_block_t* item;
	uint8_t* ui_mem_buf;

	if(force_ref > 1 )
	{
		return _get_resource_bitmap_buffer_compact(source, bitmap, size, force_ref);
	}

	item = bitmap_buffer.head;
	while(item != NULL)
	{
		if((item->source == source)&&(item->id == bitmap->sty_data->id))
		{
			break;
		}
		item = item->next;
	}

	if(item != NULL)
	{
		//found loaded bitmap, which is not supposed to happen but fine
		SYS_LOG_ERR("should not happen\n");
		if(force_ref >= 1)
		{
			item->ref ++;
		}
	
		return item->addr;
	}
	else
	{
		if(size == screen_bitmap_size)
		{
			ui_mem_buf = ui_memory_alloc(size);
			if(ui_mem_buf == NULL)
			{
				SYS_LOG_ERR("cant malloc screen size bitmap buffer from ui mem \n");
				res_manager_dump_info();
				return NULL;
			}
			ui_mem_total++;
			SYS_LOG_DBG("2 ui mem alloc %d, addr %p, styid 0x%x\n", bitmap->sty_data->id, ui_mem_buf, bitmap->sty_data->sty_id);

			item = (buf_block_t*)res_mem_alloc(RES_MEM_POOL_BMP, sizeof(buf_block_t));	
			if(item == NULL)
			{
				SYS_LOG_DBG("%d ui mem free %d\n", __LINE__, bitmap->sty_data->id);
				ui_memory_free(ui_mem_buf);
				ui_mem_total--;
				SYS_LOG_ERR("error: resource buffer not enough for size %d\n", size);
				return NULL;
			}
			item->size = sizeof(buf_block_t);
			item->addr = ui_mem_buf;
			
		}
		else
		{
			item = (buf_block_t*)res_mem_alloc(RES_MEM_POOL_BMP, sizeof(buf_block_t) + size);
			if(item == NULL)
			{
				SYS_LOG_ERR("error: resource buffer not enough for size %d\n", size);
				return NULL;
			}
			
			item->size = sizeof(buf_block_t) + size;
			item->addr = (uint8_t *)item + sizeof(buf_block_t);
		}
		item->source = source;
		item->id = bitmap->sty_data->id;

		if(force_ref >= 1)
		{
			item->ref = 1;
		}
		else
		{
			item->ref = 0;
		}
		item->width = (uint32_t)bitmap->sty_data->width;
		item->height = (uint32_t)bitmap->sty_data->height;
		item->bytes_per_pixel = (uint32_t)bitmap->sty_data->bytes_per_pixel;
		item->format = (uint32_t)bitmap->sty_data->format;
		item->next = bitmap_buffer.head;
		bitmap_buffer.head = item;
		item->regular_info = bitmap->regular_info;
#if RES_MEM_DEBUG
		if(res_mem_check()<0)
		{
			SYS_LOG_INF("%s %d\n", __FILE__, __LINE__);
			res_mem_dump();
		}
#endif

		return item->addr;
	}

}

//uint8_t* _try_resource_bitmap_buffer(uint32_t source, uint32_t id, uint32_t* width, uint32_t* height,
//										uint32_t* bytes_per_pixel, uint32_t* format, uint32_t force_ref)
uint8_t* _try_resource_bitmap_buffer(uint32_t source, resource_bitmap_t* bitmap, uint32_t force_ref)
{
	buf_block_t* item;

	item = bitmap_buffer.head;
	while(item != NULL)
	{
		if((item->source == source)&&(item->id == bitmap->sty_data->id))
		{
			break;
		}
		item = item->next;
	}

	if(item != NULL)
	{
		//found loaded bitmap
		bitmap->regular_info = item->regular_info;
		if(force_ref == 1)
		{
			item->ref ++;
		}

//		SYS_LOG_INF("\n ### got cached bitmap id %d\n", id);
		return item->addr;
	}
	else
	{
		//not found, need to load from file
		return NULL;
	}
}

uint32_t _check_bitmap_in_compact_buffer(void* p)
{
	compact_buffer_t* buffer;
	uint32_t ret = 0;

	buffer = bitmap_buffer.compact_buffer_list;
	while(buffer != NULL)
	{
		if(p >= (void*)buffer->addr && p < (void*)(buffer->addr + buffer->offset))
		{
			ret = 1;
			break;
		}
		buffer = buffer->next;
	}

	return ret;
}

void _free_resource_bitmap_buffer(void *p)
{
	buf_block_t* item = bitmap_buffer.head;
	buf_block_t* prev = NULL;
	uint32_t is_compact = 0;

#if RES_MEM_DEBUG
	if(res_mem_check()<0)
	{
		SYS_LOG_INF("%s %d item 0x%x \n", __FILE__, __LINE__, item);
		res_mem_dump();
	}
#endif		
	while(item != NULL)
	{
		if(item->addr == p)
		{
			break;
		}
		prev = item;
		item = item->next;
	}

	if(item == NULL)
	{
//		SYS_LOG_ERR("resource bitmap not found %p", p);
/*		
		item = bitmap_buffer.head;
		while(item!=NULL)
		{
			SYS_LOG_INF("item in list 0x%x \n", item->addr);
			item = item->next;
		}
*/
		return;
	}

	//check if p belongs to scene buffer, use item in check in case p is in ui mem
	if(bitmap_buffer.compact_buffer_list != NULL)
	{
		is_compact = _check_bitmap_in_compact_buffer((void*)item);
	}
	
	if(item->ref > 1)
	{
		item->ref --;
		//FIXME
		return;
	}
	else if(item->regular_info != 0)
	{
		return;
	}
	else
	{
		if(prev == NULL)
		{
			bitmap_buffer.head = item->next;
		}
		else
		{
			prev->next = item->next;
		}

		if(is_compact == 0)
		{
			//if item size equals buf_block_t meaning it's a screen size
			if(item->size > sizeof(buf_block_t))
			{
				res_mem_free(RES_MEM_POOL_BMP, (void*)item);
			}			
			else
			{
				SYS_LOG_DBG("%d ui mem free %d\n", __LINE__, item->id);
				ui_memory_free(item->addr);
				ui_mem_total--;
				res_mem_free(RES_MEM_POOL_BMP, (void*)item);
			}
		}
		else
		{
			//in case item was deleted in bitmap list first, 
			//then when scene unloads, no check upon it, so ui mem leaked.
			if(item->size == sizeof(buf_block_t))
			{
				SYS_LOG_DBG("%d ui mem free %d\n", __LINE__, item->id);
				ui_memory_free(item->addr);	
				ui_mem_total--;
				item->addr = NULL;
			}
		}

#if RES_MEM_DEBUG
		if(res_mem_check()<0)
		{
			SYS_LOG_INF("%s %d\n", __FILE__, __LINE__);
			res_mem_dump();
		}
#endif		
	}
}

static int _init_pic_search_param(resource_info_t* info, const char* picres_path)
{
	int i,k;
	int ret;
	res_head_t res_head;
	res_entry_t* entry;
	char* partition;
	char* res_path = NULL;
	int part_count = 0;
	
	info->pic_search_param = (pic_search_param_t*)res_mem_alloc(RES_MEM_POOL_BMP, (MAX_PARTITION_ID+1)*sizeof(pic_search_param_t));
	if(info->pic_search_param == NULL)
	{
		SYS_LOG_INF("no enough memory to store pic res param\n");
		return -1;
	}
	memset(info->pic_search_param, 0, (MAX_PARTITION_ID+1)*sizeof(pic_search_param_t));

	res_path = (char*)mem_malloc(strlen(picres_path)+1);
	if(res_path == NULL)
	{
		SYS_LOG_INF("no enough memory to store pic res path\n");
		return -1;
	}
	memset(res_path, 0, strlen(picres_path)+1);
	strcpy(res_path, picres_path);
#ifdef CONFIG_SIMULATOR
	partition = NULL;
	part_count = 0;
#else
	if(res_is_auto_search_files())
	{
		partition = strrchr(res_path, ':') + 1;
		part_count = MAX_PARTITION_ID;
	}
	else
	{
		partition = NULL;
		part_count = 0;
	}
#endif
	for(i=0;i<=part_count;i++)
	{
		if (partition)
		{
			*partition = storage_part_name[i];
			SYS_LOG_DBG("partition %c\n", *partition);
		}

		SYS_LOG_DBG("res_path %s\n", res_path);

		if(i == 0)
		{
			ret = res_fs_open(&info->pic_fp, res_path);
			if ( ret < 0 )
			{
				SYS_LOG_ERR(" open picture resource file %s failed!\n", res_path);
				mem_free(res_path);
				return -1;
			}
			
			res_fs_seek(&info->pic_fp, 0, FS_SEEK_SET);
			ret = res_fs_read(&info->pic_fp, &res_head, sizeof(res_head_t));
			if(ret < sizeof(res_head_t))
			{
				SYS_LOG_ERR("read picture resource header failed\n");
				res_fs_close(&info->pic_fp);
				mem_free(res_path);
				return -1;
			}


			if(res_head.magic[0]!='R' || res_head.magic[1]!='E' || res_head.magic[2]!='S')
			{
				SYS_LOG_ERR("read picture resource header invalid\n");
				res_fs_close(&info->pic_fp);
				mem_free(res_path);
				return -1;
			}
			
			entry = res_mem_alloc(RES_MEM_POOL_BMP, res_head.counts*sizeof(res_entry_t));
			if(entry == NULL)
			{
				SYS_LOG_ERR("malloc res entries failed\n");
				res_fs_close(&info->pic_fp);
				mem_free(res_path);
				return -1;
			}
			ret = res_fs_read(&info->pic_fp, entry, res_head.counts*sizeof(res_entry_t));
			if(ret < res_head.counts*sizeof(res_entry_t))
			{
				SYS_LOG_ERR("read picture resource entries failed\n");
				res_mem_free(RES_MEM_POOL_BMP, entry);
				res_fs_close(&info->pic_fp);
				mem_free(res_path);
				return -1;
			}
			info->pic_search_param[i].id_start = 1;
			info->pic_search_param[i].id_end = info->pic_search_param[i].id_start + res_head.counts - 1;
			info->pic_search_param[i].pic_offsets = res_mem_alloc(RES_MEM_POOL_BMP, res_head.counts*sizeof(uint32_t));
			SYS_LOG_INF("id start %d, id_end %d\n", info->pic_search_param[i].id_start, info->pic_search_param[i].id_end);
			for(k=0;k<res_head.counts;k++)
			{
				//skip 4 bytes before bitmap data, which are metric params
				info->pic_search_param[i].pic_offsets[k]= entry[k].offset+4;
			}
			res_mem_free(RES_MEM_POOL_BMP, entry);
		}
		else
		{

			ret = res_fs_open(&info->pic_search_param[i].res_fp, res_path);
			if ( ret < 0 )
			{
				continue;
			}
			res_fs_seek(&info->pic_search_param[i].res_fp, 0, FS_SEEK_SET);
			ret = res_fs_read(&info->pic_search_param[i].res_fp, &res_head, sizeof(res_head_t));
			if(ret < sizeof(res_head_t))
			{
				SYS_LOG_ERR("read picture resource header failed\n");
				mem_free(res_path);
				return -1;
			}			
			entry = res_mem_alloc(RES_MEM_POOL_BMP, res_head.counts*sizeof(res_entry_t));
			if(entry == NULL)
			{
				SYS_LOG_ERR("malloc res entries failed\n");
				res_fs_close(&info->pic_search_param[i].res_fp);
				mem_free(res_path);
				return -1;
			}			
			ret = res_fs_read(&info->pic_search_param[i].res_fp, entry, res_head.counts*sizeof(res_entry_t));
			if(ret < res_head.counts*sizeof(res_entry_t))
			{
				SYS_LOG_ERR("read picture resource entries failed\n");
				res_mem_free(RES_MEM_POOL_BMP, entry);
				res_fs_close(&info->pic_search_param[i].res_fp);
				mem_free(res_path);
				return -1;
			}			
			info->pic_search_param[i].id_start = res_head.id_start;
			info->pic_search_param[i].id_end = res_head.id_start + res_head.counts - 1;
			info->pic_search_param[i].pic_offsets = res_mem_alloc(RES_MEM_POOL_BMP, res_head.counts*sizeof(uint32_t));
			SYS_LOG_INF("id start %d, id_end %d\n", info->pic_search_param[i].id_start, info->pic_search_param[i].id_end);

			for(k=0;k<res_head.counts;k++)
			{
				//skip 4 bytes before bitmap data, which are metric params			
				info->pic_search_param[i].pic_offsets[k]= entry[k].offset+4;
			}
			res_mem_free(RES_MEM_POOL_BMP, entry);
		}
		info->pic_search_max_volume++;
	}
	mem_free(res_path);
	return 0;
}

//读取style 文件的头部信息, 返回值是该配置文件中scene 的总数
//缓存图片资源文件索引信息
static int _read_head( resource_info_t* info )
{
    style_head_t* head;
    int ret;
	uint32_t sty_size;

	os_strace_u32(SYS_TRACE_ID_RES_SCENE_PRELOAD_0, (uint32_t)0);	

	res_fs_seek(&info->style_fp, 0, FS_SEEK_END);
	sty_size = res_fs_tell(&info->style_fp);
	info->sty_mem = (uint8_t*)res_mem_alloc(RES_MEM_POOL_SCENE, sty_size);
	if(info->sty_mem == NULL)
	{
		SYS_LOG_ERR("no enough mem for style data\n");
		return -1;
	}

	res_fs_seek(&info->style_fp, 0, FS_SEEK_SET);
	
	os_strace_end_call_u32(SYS_TRACE_ID_RES_SCENE_PRELOAD_0, (uint32_t)0);
	ret = res_fs_read(&info->style_fp, info->sty_mem, sty_size);
	if(ret < sty_size)
	{
		SYS_LOG_ERR("read style file error:%d\n", ret);
		res_mem_free(RES_MEM_POOL_SCENE, info->sty_mem);
		info->sty_mem = NULL;
		return -1;
	}

	head = (style_head_t*)info->sty_mem;
	if(head->scene_sum == 0xffffffff)
	{
		SYS_LOG_ERR("read style file invalid 0x%x\n", head->scene_sum);
		free(info->sty_mem);
		info->sty_mem = NULL;
		return -1;
	}
	info->sum = head->scene_sum;
	info->scenes = (resource_scene_t*)(info->sty_mem + sizeof(style_head_t));
	info->sty_data = info->sty_mem + sizeof(style_head_t) + sizeof(resource_scene_t)*info->sum;

#if RES_MEM_DEBUG
	if(res_mem_check()<0)
	{
		SYS_LOG_INF("%s %d\n", __FILE__, __LINE__);
		res_mem_dump();
	}
#endif		

    return 0;
}

static int res_manager_inited = 0;
void res_manager_init(void)
{
	if(res_manager_inited == 0)
	{
		res_mem_init();
		_resource_buffer_init();
		

		os_mutex_init(&bitmap_cache_mutex);
		os_mutex_init(&bitmap_read_mutex);
		res_manager_inited = 1;
	}
}

void res_manager_set_screen_size(uint32_t screen_w, uint32_t screen_h)
{
	screen_bitmap_size = screen_w*screen_h*2;
}

void res_manager_clear_cache(uint32_t force_clear)
{
	_resource_buffer_deinit(force_clear);
}

int32_t res_manager_set_str_file(resource_info_t* info, const char* text_path)
{
	char* file_path = NULL;
	char* partition;	
	int i = 0;
	int ret;
	buf_block_t* listp;
	buf_block_t* item;
	buf_block_t* prev;

	if( text_path != NULL)
	{
		//close old file
		res_fs_close(&info->text_fp);
		if(info->str_path)
		{
			mem_free(info->str_path);
			info->str_path = NULL;
		}
		
#ifdef CONFIG_SIMULATOR
		ret = res_fs_open(&info->text_fp, text_path);
		if ( ret < 0 )
		{
			SYS_LOG_ERR(" open text resource file %s failed!", text_path);
			goto ERR_EXIT;
		}

#else
		if(res_is_auto_search_files())
		{
			file_path = (char*)mem_malloc(strlen(text_path)+1);
			if(file_path == NULL)
			{
				SYS_LOG_ERR("malloc file path failed\n");
				goto ERR_EXIT;
			}

			memset(file_path, 0, strlen(text_path)+1);				
			strcpy(file_path, text_path);
			partition = strrchr(file_path, ':') + 1;
			if(partition == NULL)
			{
				SYS_LOG_ERR("invalid str file path\n");
				goto ERR_EXIT;
			}

			for(i=MAX_PARTITION_ID;i>=0;i--)
			{
				*partition = storage_part_name[i];
				ret = res_fs_open(&info->text_fp, file_path);
				if ( ret < 0 )
				{
					SYS_LOG_INF("open str file path %s failed\n", file_path);
					continue;
				}
				else
				{
					SYS_LOG_INF("open str file path %s ok\n", file_path);
					break;
				}
			}

			if(i<0)
			{
				SYS_LOG_ERR(" open str file %s failed!", text_path);
				goto ERR_EXIT;
			}
		}
		else
		{
			ret = res_fs_open(&info->text_fp, text_path);
			if ( ret < 0 )
			{
				SYS_LOG_ERR(" open text resource file %s failed!", text_path);
				goto ERR_EXIT;
			}
		}
#endif

		info->str_path = (uint8_t*)mem_malloc(strlen(text_path)+1);
		if(info->str_path == NULL)
		{
			SYS_LOG_ERR("malloc info str path failed\n");
			goto ERR_EXIT;
		}
		memset(info->str_path, 0, strlen(text_path)+1);
		strcpy(info->str_path, text_path);	

		//clear cache
		listp = text_buffer.head;
		prev = NULL;
		while(listp != NULL)
		{			
			if(listp->source == (uint32_t)info)
			{
				item = listp;
				listp = listp->next;
				if(prev)
				{
					prev->next = listp;
				}
				if(text_buffer.head == item)
				{
					text_buffer.head = listp;
				}
				res_mem_free(RES_MEM_POOL_TXT, item);
			}
			else
			{
				prev = listp;
				listp = listp->next;
			}
		}
//		memset(&text_buffer, 0, sizeof(text_buffer));	
	}
	else
	{
		SYS_LOG_INF("text resource file path null");
	}

	
	if(file_path)
	{
		mem_free(file_path);
	}
	return 0;
ERR_EXIT:
	if(file_path)
		mem_free(file_path);
	res_fs_close(&info->text_fp);
	if(info->str_path)
	{
		mem_free(info->str_path);
		info->str_path = NULL;
	}
	return -1;	
}

resource_info_t* res_manager_open_resources( const char* style_path, const char* picture_path, const char* text_path )
{
    int ret;
    resource_info_t* info;
	char* partition;
	char* file_path = NULL;

	os_strace_u32(SYS_TRACE_ID_RES_SCENE_PRELOAD_2, (uint32_t)0);	
    info = (resource_info_t*)mem_malloc( sizeof( resource_info_t ) );
    if ( info == NULL )
    {
        SYS_LOG_ERR( "error: malloc resource info failed !" );
        return NULL;
    }
    memset(info, 0, sizeof(resource_info_t));

	
    if ( style_path != NULL )
    {
    	int i;
#ifdef CONFIG_SIMULATOR
		ret = res_fs_open(&info->style_fp, style_path);
		if ( ret < 0 )
	    {
	        SYS_LOG_ERR(" open style file %s failed!", style_path);
	        goto ERR_EXIT;
	    }
#else
		if(res_is_auto_search_files())
		{
			file_path = (char*)mem_malloc(strlen(style_path)+1);
			if(file_path == NULL)
			{
		        SYS_LOG_INF("malloc style file path failed!");
		        goto ERR_EXIT;
			}
			memset(file_path, 0, strlen(style_path)+1);
	    	strcpy(file_path, style_path);
			partition = strrchr(file_path, ':') + 1;
			if(partition == NULL)
				goto ERR_EXIT;
		
			SYS_LOG_INF("MAX_PARTITION_ID %d\n", MAX_PARTITION_ID);
			for(i=MAX_PARTITION_ID;i>=0;i--)
			{
				SYS_LOG_INF("partition %d: %c\n", i, *partition);
				*partition = storage_part_name[i];
				ret = res_fs_open(&info->style_fp, file_path);
				if ( ret < 0 )
				{
					continue;
				}
				else
				{
					SYS_LOG_INF("style file_path %d: %s\n", i, file_path);			
					break;
				}
			}

			if(i<0)
			{
				SYS_LOG_ERR(" open style file %s failed!", style_path);
				goto ERR_EXIT;
			}
		}
		else
		{
			ret = res_fs_open(&info->style_fp, style_path);
			if ( ret < 0 )
			{
				SYS_LOG_ERR(" open style file %s failed!", style_path);
				goto ERR_EXIT;
			}
		}
#endif
		info->sty_path = (uint8_t*)mem_malloc(strlen(style_path)+1);
		memset(info->sty_path, 0, strlen(style_path)+1);
		strcpy(info->sty_path, style_path);
    }
    else
    {
        SYS_LOG_INF("style file path null");
    }

    if( picture_path != NULL)
    {
		_init_pic_search_param(info, picture_path);
    }
    else
    {
    	SYS_LOG_INF("picture resource file path null");
    }

    if( text_path != NULL)
    {
#ifdef CONFIG_SIMULATOR
		ret = res_fs_open(&info->text_fp, text_path);
	    if ( ret < 0 )
	    {
	        SYS_LOG_ERR(" open text resource file %s failed!", text_path);
	        goto ERR_EXIT;
	    }

#else
		if(res_is_auto_search_files())
		{
	    	int i = 0;
			if(file_path)
				mem_free(file_path);
			
			file_path = (char*)mem_malloc(strlen(text_path)+1);
			memset(file_path, 0, strlen(text_path)+1);		    	
	    	strcpy(file_path, text_path);
			partition = strrchr(file_path, ':') + 1;
			if(partition == NULL)
				goto ERR_EXIT;

			for(i=MAX_PARTITION_ID;i>=0;i--)
			{
				*partition = storage_part_name[i];
				ret = res_fs_open(&info->text_fp, file_path);
				if ( ret < 0 )
				{
					continue;
				}
				else
				{
					break;
				}
			}

			if(i<0)
			{
				SYS_LOG_ERR(" open text file %s failed!", text_path);
				goto ERR_EXIT;
			}
		}
		else
		{
			ret = res_fs_open(&info->text_fp, text_path);
			if ( ret < 0 )
			{
				SYS_LOG_ERR(" open text resource file %s failed!", text_path);
				goto ERR_EXIT;
			}
		}
#endif

		info->str_path = (uint8_t*)mem_malloc(strlen(text_path)+1);
		memset(info->str_path, 0, strlen(text_path)+1);
		strcpy(info->str_path, text_path);		
    }
    else
    {
    	SYS_LOG_INF("text resource file path null");
    }

	os_strace_end_call_u32(SYS_TRACE_ID_RES_SCENE_PRELOAD_2, (uint32_t)0);
    _read_head( info );    //读取style头部信息，不做返回值判断，可能不存在样式文件。

	if(file_path)
		mem_free(file_path);
    return info;

ERR_EXIT:
	if(file_path)
		mem_free(file_path);
	res_fs_close(&info->style_fp);
	res_fs_close(&info->pic_fp);
	res_fs_close(&info->text_fp);
	if(info->sty_path)
	{
		mem_free(info->sty_path);
		info->sty_path = NULL;
	}
	if(info->str_path)
	{
		mem_free(info->str_path);
		info->str_path = NULL;
	}
	mem_free(info);
	return NULL;
}

void res_manager_close_resources( resource_info_t* info )
{
	if(info == NULL)
	{
		SYS_LOG_ERR("null resource info pointer");
		return;
	}

	res_fs_close(&info->style_fp);
	res_fs_close(&info->pic_fp);
	res_fs_close(&info->text_fp);

	if(info->sty_mem != NULL)
	{
		res_mem_free(RES_MEM_POOL_SCENE, info->sty_mem);
		info->sty_mem = NULL;
		info->scenes = NULL;
	}


	if(info->sty_path != NULL)
	{
		mem_free(info->sty_path);
		info->sty_path = NULL;
	}

	if(info->str_path != NULL)
	{
		mem_free(info->str_path);
		info->str_path = NULL;
	}

	if(info->pic_search_param != NULL)
	{
		int i;
		for(i=0;i<(MAX_PARTITION_ID+1);i++)
		{
			if(info->pic_search_param[i].pic_offsets != NULL)
			{
				res_mem_free(RES_MEM_POOL_BMP, info->pic_search_param[i].pic_offsets);
			}
		}	
		res_mem_free(RES_MEM_POOL_BMP, info->pic_search_param);
		info->pic_search_param = NULL;
	}
	mem_free(info);
}

resource_scene_t* _get_scene( resource_info_t* info, unsigned int* scene_item, uint32_t id )
{
	resource_scene_t* scene;
    unsigned int offset;
    unsigned int size;

	//FIXME: scene buffer should differ from bitmap buffer as in space range, 
	//so the mutex wont affect each other.
	os_strace_u32(SYS_TRACE_ID_RES_SCENE_PRELOAD_1, (uint32_t)id);
#if RES_MEM_DEBUG
	if(res_mem_check()<0)
	{
		SYS_LOG_INF("%s %d\n", __FILE__, __LINE__);
		res_mem_dump();
	}
#endif		

	
    offset = *( scene_item + 1 );
    size = *( scene_item + 2 );
	SYS_LOG_INF("sty data %p, scene size %d, offset 0x%x", info->sty_data, size, offset);	
    scene = ( resource_scene_t* )_try_resource_scene_buffer((uint32_t)info, id);
	if(scene != NULL)
	{
//		SYS_LOG_INF("scene cached buffer 0x%x, size 0x%x", scene, size);
//		SYS_LOG_INF("scene %d, %d, %d %d", scene->x, scene->y, scene->width, scene->height);
		os_strace_end_call_u32(SYS_TRACE_ID_RES_SCENE_PRELOAD_1, (uint32_t)id);
		return scene;
	}
	else
	{
		scene = ( resource_scene_t* )_get_resource_scene_buffer((uint32_t)info, id, size);
		if(scene == NULL)
		{
			SYS_LOG_ERR("error: cant get scene buffer");
			return NULL;
		}
	}	
	os_strace_end_call_u32(SYS_TRACE_ID_RES_SCENE_PRELOAD_1, (uint32_t)id);
	os_strace_u32(SYS_TRACE_ID_RES_SCENE_PRELOAD_2, (uint32_t)id);

	memcpy(scene, info->sty_data+offset, size);
	scene->scene_id = id;
	os_strace_end_call_u32(SYS_TRACE_ID_RES_SCENE_PRELOAD_2, (uint32_t)id);
//	SYS_LOG_INF("scene id 0x%x, scene_id 0x%x\n", id, scene->scene_id);
//	SYS_LOG_INF("scene %d, %d, %d %d", scene->x, scene->y, scene->width, scene->height);
	
#if RES_MEM_DEBUG
	if(res_mem_check()<0)
	{
		SYS_LOG_INF("%s %d\n", __FILE__, __LINE__);
		res_mem_dump();
	}
#endif		
	
    return scene;
}

resource_scene_t* res_manager_load_scene(resource_info_t* info, uint32_t scene_id)
{
    unsigned int i;
    resource_scene_t* scenes;

	
    if ( info == NULL )
    {
        SYS_LOG_ERR("resource info null");
        return NULL;
    }
	
    scenes = info->scenes;
	SYS_LOG_INF("scene id 0x%x, info sum %d", scene_id, info->sum);
    for ( i = 0; i < info->sum; i++ )
    {   	
//    	SYS_LOG_INF("scene_item 0x%x", scenes[i].scene_id);
        if ( scenes[i].scene_id == scene_id )
        {
        	
			return &scenes[i];
        }
    }

    SYS_LOG_ERR("cannot find scene id: 0x%x", scene_id);
    return NULL;
}


int32_t res_manager_set_pictures_regular(resource_info_t* info, uint32_t scene_id, uint32_t group_id, uint32_t subgroup_id, uint32_t* id, uint32_t num)
{
	regular_info_t* item;
	uint8_t* offset;
	uint32_t magic;

	magic = scene_id + group_id + subgroup_id;

	item = regular_info_list;
	while(item != NULL)
	{
		if(item->magic == magic && item->num == num)
		{
			//FIXME:we ignore sty_path(aka.source) here for simplicity
			if(memcmp(item->id, id, num*sizeof(uint32_t))==0)
			{
				//got a match
				return 0;
			}
		}

		item = item->next;
	}

	item = (regular_info_t*)res_mem_alloc(RES_MEM_POOL_SCENE, sizeof(regular_info_t)+num*sizeof(uint32_t));
	if(item == NULL)
	{
		SYS_LOG_ERR("error: resource buffer not enough for regular info\n");
		return -1;
	}
	memset(item, 0, sizeof(regular_info_t)+num*sizeof(uint32_t));
	
	item->magic = magic;
	item->scene_id = scene_id;
	offset = (uint8_t*)item;
	offset += sizeof(regular_info_t);
	item->id = (uint32_t*)offset;
	memcpy(item->id, id, num*sizeof(uint32_t));
	item->num = num;
	item->next = regular_info_list;
	regular_info_list = item;	
	return 0;
}

int32_t res_manager_clear_regular_pictures(const uint8_t* sty_path, uint32_t scene_id)
{
	regular_info_t* regular = regular_info_list;
	regular_info_t* prev = NULL;
	regular_info_t* found = NULL;
	buf_block_t* item;

	while(regular != NULL)
	{
		if(regular->scene_id == scene_id)
		{		
			item = bitmap_buffer.head;
			while(item != NULL)
			{
				//clear regular flag
				if(item->regular_info == (uint32_t)regular)
				{
					item->regular_info = 0;
				}
				item = item->next;
			}

			found = regular;
			regular = found->next;
			if(found == regular_info_list)
			{
				regular_info_list = regular;
			}
			res_mem_free(RES_MEM_POOL_SCENE, found);
		}
		else
		{
			prev = regular;
			regular = regular->next;
		}
	}

	return 0;
}

void _unload_scene_bitmaps(uint32_t scene_id)
{
	compact_buffer_t* buffer = NULL;
	compact_buffer_t* prev_buf= NULL;
	compact_buffer_t* found;
	buf_block_t* item;
	buf_block_t* prev;
	uint8_t* buffer_end;

	SYS_LOG_INF("\n\n ###### _unload_scene_bitmaps 0x%x \n", scene_id);
	if(scene_id == 0)
	{
		return;
	}

	os_mutex_lock(&bitmap_cache_mutex, OS_FOREVER);
	buffer = bitmap_buffer.compact_buffer_list;
	while(buffer != NULL)
	{
		if(buffer->scene_id == scene_id)
		{
			//check regular
			item = (buf_block_t*)buffer->addr;
			if(buffer->offset != 0 && item->regular_info != 0)
			{
				prev_buf = buffer;
				buffer = buffer->next;	
				continue;
			}

//			SYS_LOG_INF("testalloc compact unload 0x%x, buffer %p, addr %p\n", scene_id, buffer, buffer->addr);
			//clear buffer nodes from bitmap list
			buffer_end = buffer->addr + buffer->offset -1;
			item = bitmap_buffer.head;
			prev = bitmap_buffer.head;
			while(item != NULL)
			{
				if(((void*)item >= (void*)buffer->addr) && ((void*)item <= (void*)buffer_end))
				{
					if(item->size == sizeof(buf_block_t) && item->addr != NULL)
					{
						SYS_LOG_DBG("%d ui mem free %d\n", __LINE__, item->id);
						ui_memory_free(item->addr);
						ui_mem_total--;
					}
				
					if(item == bitmap_buffer.head)
					{
						bitmap_buffer.head = item->next;
					}
					else
					{
						prev->next = item->next;
					}
				}
				else
				{
					prev = item;
				}
				item = item->next;
			}

			//release scene data
			if(buffer == bitmap_buffer.compact_buffer_list)
			{
				bitmap_buffer.compact_buffer_list = buffer->next;
			}
			else
			{
				prev_buf->next = buffer->next;
			}

			found = buffer;
			buffer = found->next;
			res_mem_free(RES_MEM_POOL_BMP, found->addr);

#if RES_MEM_DEBUG
			if(res_mem_check()<0)
			{
				SYS_LOG_INF("%s %d found addr 0x%x\n", __FILE__, __LINE__, found->addr);
				res_mem_dump();
			}
#endif		

			res_array_free(found);

		}
		else
		{
			prev_buf = buffer;
			buffer = buffer->next;
		}
	}

	os_mutex_unlock(&bitmap_cache_mutex);
}


void _unload_scene(uint32_t id, resource_scene_t* scene)
{
	_unload_scene_bitmaps(id);
	
	_free_resource_scene_buffer(scene);
}

void res_manager_unload_scene(uint32_t id, resource_scene_t* scene)
{

	if(scene != NULL)
	{
		SYS_LOG_INF("unload scene 0x%x\n", scene->scene_id);
		_unload_scene(id, scene);
	}
	else if(id > 0)
	{
		_unload_scene_bitmaps(id);
	}
}

void _unload_bitmap(resource_bitmap_t* bitmap)
{
	if(bitmap == NULL)
	{
		SYS_LOG_ERR("error: unload null resource");
		return;
	}
	os_mutex_lock(&bitmap_cache_mutex, OS_FOREVER);
	if(bitmap->regular_info == 0 && bitmap->buffer != NULL)
	{
		_free_resource_bitmap_buffer(bitmap->buffer);
	}
	os_mutex_unlock(&bitmap_cache_mutex);
}

void _unload_string(resource_text_t* text)
{
	if(text == NULL)
	{
		SYS_LOG_ERR("error: unload null resource");
		return;
	}
	_free_resource_text_buffer(text->buffer);
}

int _search_res_id_in_files(resource_info_t* info, resource_bitmap_t* bitmap, uint32_t* bmp_pos)
{
	uint32_t id = bitmap->sty_data->id;
	int low, high, mid;
	
	if(info->pic_search_param == NULL)
	{
		return -1;
	}

	low = 0;
	high = info->pic_search_max_volume-1;
	mid = (info->pic_search_max_volume-1)/2;
	while(id < info->pic_search_param[mid].id_start || id > info->pic_search_param[mid].id_end)
	{
		if(id < info->pic_search_param[mid].id_start)
		{
			high = mid - 1;
		}
		else
		{
			low = mid + 1;			
		}

		if(low > high)
		{
			//not found
			return -1;
		}
		mid = (low+high)/2;
	}

	id -= info->pic_search_param[mid].id_start;
	*bmp_pos = info->pic_search_param[mid].pic_offsets[id];
	return mid;
}

int32_t _load_bitmap(resource_info_t* info, resource_bitmap_t* bitmap, uint32_t force_ref)
{
	int32_t ret;
	int32_t bmp_size;
	int32_t compress_size = 0;
	uint8_t *compress_buf = NULL;
#ifdef CONFIG_RES_MANAGER_USE_SDFS
	struct sd_file** pic_fp;
#else
	struct fs_file_t* pic_fp;
#endif
	uint32_t bmp_pos = 0;

	if((info == NULL) || (bitmap == NULL))
	{
		SYS_LOG_ERR("error: invalid param to load bitmap");
		return -1;
	}

	os_strace_u32(SYS_TRACE_ID_RES_BMP_LOAD_0, (uint32_t)bitmap->sty_data->id);
	os_mutex_lock(&bitmap_cache_mutex, OS_FOREVER);

#if RES_MEM_DEBUG
	if(res_mem_check()<0)
	{
		SYS_LOG_INF("%s %d\n", __FILE__, __LINE__);
		res_mem_dump();
	}
#endif		
	//bitmap->buffer = _try_resource_bitmap_buffer((uint32_t)info, bitmap->id, &bitmap->width, &bitmap->height, &bitmap->bytes_per_pixel, &bitmap->format, force_ref);
	bitmap->buffer = _try_resource_bitmap_buffer((uint32_t)info, bitmap, force_ref);
	if(bitmap->buffer != NULL)
	{
		//got bitmap from bitmap cache
		os_mutex_unlock(&bitmap_cache_mutex);
		os_strace_end_call_u32(SYS_TRACE_ID_RES_BMP_LOAD_0, (uint32_t)bitmap->sty_data->id);
		return 0;
	}
	
	bmp_size = bitmap->sty_data->width * bitmap->sty_data->height * bitmap->sty_data->bytes_per_pixel;
//	SYS_LOG_ERR("styid 0x%x, id %d, wh %d, %d ,bmp_size %d\n", bitmap->sty_data->sty_id, bitmap->sty_data->id, bitmap->sty_data->width, bitmap->sty_data->height, bmp_size);
	bitmap->buffer = _get_resource_bitmap_buffer((uint32_t)info, bitmap, bmp_size, force_ref);
	if(bitmap->buffer == NULL)
	{
		SYS_LOG_ERR("error: no buffer to load bitmap");
		os_mutex_unlock(&bitmap_cache_mutex);
		os_strace_end_call_u32(SYS_TRACE_ID_RES_BMP_LOAD_0, (uint32_t)bitmap->sty_data->id);
		return -1;
	}
	os_strace_end_call_u32(SYS_TRACE_ID_RES_BMP_LOAD_0, (uint32_t)bitmap->sty_data->id);	
	os_strace_u32(SYS_TRACE_ID_RES_BMP_LOAD_1, (uint32_t)bitmap->sty_data->id);

	ret = _search_res_id_in_files(info, bitmap, &bmp_pos);
	if(ret <= 0)
	{
		pic_fp = &info->pic_fp;
	}
	else
	{
		pic_fp = &info->pic_search_param[ret].res_fp;
	}
	
//	SYS_LOG_INF("\n pres_entry length %d, type %d, offset 0x%x \n", pres_entry->length, pres_entry->type, pres_entry->offset);
	res_fs_seek(pic_fp, bmp_pos, FS_SEEK_SET);

	compress_size = bitmap->sty_data->compress_size;
	if (compress_size > 0)
	{
		compress_buf = (uint8_t*)res_mem_alloc(RES_MEM_POOL_BMP, compress_size);
		if(compress_buf == NULL)
		{
			SYS_LOG_ERR("error: no buffer to load compressed bitmap");
			os_mutex_unlock(&bitmap_cache_mutex);
			os_strace_end_call_u32(SYS_TRACE_ID_RES_BMP_LOAD_1, (uint32_t)bitmap->sty_data->id);
			return -1;
		}
		
		ret = res_fs_read(pic_fp, compress_buf, compress_size);
		if(ret < compress_size)
		{
			SYS_LOG_ERR("bitmap read error %d\n", ret);
		}

		os_strace_end_call_u32(SYS_TRACE_ID_RES_BMP_LOAD_1, (uint32_t)bitmap->sty_data->id);	
		os_strace_u32(SYS_TRACE_ID_RES_BMP_LOAD_2, (uint32_t)bitmap->sty_data->id);
		if(ret == compress_size)
		{
			ret = LZ4_decompress_safe(compress_buf, bitmap->buffer, compress_size, bmp_size);
		}
		res_mem_free(RES_MEM_POOL_BMP, compress_buf);
		os_strace_end_call_u32(SYS_TRACE_ID_RES_BMP_LOAD_2, (uint32_t)bitmap->sty_data->id);
	}
	else
	{
		ret = res_fs_read(pic_fp, bitmap->buffer, bmp_size);
		os_strace_end_call_u32(SYS_TRACE_ID_RES_BMP_LOAD_1, (uint32_t)bitmap->sty_data->id);	
	}
	
	if(ret < bmp_size)
	{
		SYS_LOG_ERR("bitmap load error %d, compress_buf %p, compress_size %d, styid 0x%x, id %d, w %d, h %d, bmp_size %d, buffer %p, bmp_pos 0x%x\n", 
			ret, compress_buf, compress_size, bitmap->sty_data->sty_id, bitmap->sty_data->id, bitmap->sty_data->width, bitmap->sty_data->height, 
			bmp_size, bitmap->buffer, bmp_pos);
	}

	
	
	if(force_ref <= 1 || bmp_size == screen_bitmap_size)
	{
		mem_dcache_clean(bitmap->buffer, bmp_size);
//		SYS_LOG_INF("load normal size %d, buffer %p\n", bmp_size, bitmap->buffer);
	}
	
	
#if RES_MEM_DEBUG
	if(res_mem_check()<0)
	{
		SYS_LOG_INF("%s %d\n", __FILE__, __LINE__);
		res_mem_dump();
	}
#endif		

	os_mutex_unlock(&bitmap_cache_mutex);
	return 0;
}

int _load_string(resource_info_t* info, resource_text_t* text)
{
	int ret;
    res_head_t res_head;
    res_entry_t res_entry;

	if((info == NULL) || (text == NULL))
	{
		SYS_LOG_INF("error: invalid param to load text");
		return -1;
	}
	text->buffer = _try_resource_text_buffer((uint32_t)info, text->sty_data->id);
	if(text->buffer != NULL)
	{
		//got string buffer from cache
		return 0;
	}

	res_fs_seek(&info->text_fp, 0, FS_SEEK_SET);
	memset(&res_head, 0, sizeof(res_head));
	ret = res_fs_read(&info->text_fp, &res_head, sizeof(res_head));
    if (ret < sizeof(res_head) || res_head.version != 2)
    {
        //support utf8 only for now
        SYS_LOG_ERR("error: unsupported encoding %d\n", res_head.version);
        return -1;
    }

    memset(&res_entry, 0, sizeof(res_entry_t));
	res_fs_seek(&info->text_fp,(int)(text->sty_data->id * (int)sizeof(res_entry_t)) , FS_SEEK_SET);
	ret = res_fs_read(&info->text_fp, &res_entry, sizeof(res_entry_t));
    if(ret < sizeof(res_entry_t))
    {
    	SYS_LOG_ERR("error: cannot find string resource");
    	return -1;
    }

//	SYS_LOG_INF("offset %d, length %d", res_entry.offset, res_entry.length);
	res_fs_seek(&info->text_fp, res_entry.offset, FS_SEEK_SET);

    text->buffer = _get_resource_text_buffer((uint32_t)info, text->sty_data->id, res_entry.length+1);
	if(text->buffer == NULL)
	{
		SYS_LOG_ERR("error: cannot get text buffer");
		return -1;
	}
	
//	SYS_LOG_INF("text buffer 0x%x\n", text->buffer);
    memset(text->buffer, 0, res_entry.length+1);
	ret = res_fs_read(&info->text_fp, text->buffer, res_entry.length);
	if(ret < res_entry.length)
	{
		SYS_LOG_ERR("error: cannot read string resource");
		return -1;
	}

	return 0;
}

uint32_t _check_resource_is_regular(uint32_t res_magic, uint32_t res_sty_id)
{
	regular_info_t* regular = regular_info_list;
	uint32_t i;

	while(regular != NULL)
	{
		if(regular->magic+res_sty_id == res_magic)
		{
			uint32_t* id = regular->id;
			for(i=0;i<regular->num;i++)
			{
				if(res_sty_id == id[i])
				{
					return (uint32_t)regular;
				}
			}
		}

		regular = regular->next;
	}

	return 0;
}

void* res_manager_get_scene_child(resource_info_t* res_info, resource_scene_t* scene, uint32_t id)
{
    uint32_t i;
    resource_t* resource;
	char* buf = NULL;   //地址不对齐，不能用数据类型指针。
	int ret;

    if ( scene == NULL )
    {
    	SYS_LOG_ERR("null scene");
        return NULL;
    }

    buf = (char*)res_info->sty_data + scene->child_offset;
	resource = (resource_t*)buf;

//	SYS_LOG_INF("resource sum %d, child offset %d, LF 0x%x\n", scene->resource_sum, scene->child_offset, id);
    for ( i = 0; i < scene->resource_sum; i++ )
    {
//	   	SYS_LOG_INF("resource id 0x%x, offset 0x%x",resource->id, resource->offset);
        if ( resource->id == id )
        {
            break;
        }

        buf = res_info->sty_data + resource->offset;
		resource = (resource_t*)buf;
    }

    if(i >= scene->resource_sum)
    {
    	SYS_LOG_DBG("error: cannot find resource 0x%x", id);
    	return NULL;
    }

//	SYS_LOG_INF("resource->type 0x%x", resource.type);
    if(resource->type == RESOURCE_TYPE_GROUP)
    {
		//buf is now on group resource start
    	resource_group_t* res = (resource_group_t*)res_array_alloc(RES_MEM_SIMPLE_INNER, sizeof(resource_group_t));
    	if(res == NULL)
    	{
    		SYS_LOG_ERR("error: no space for resource");
    		return NULL;
    	}
		sty_group_t* group = (sty_group_t*)resource;
		res->sty_data = group;		
		return res;
    }

    if(resource->type == RESOURCE_TYPE_PICTURE)
    {
    	//buf is now on resource start
    	resource_bitmap_t* res = (resource_bitmap_t*)res_array_alloc(RES_MEM_SIMPLE_INNER, sizeof(resource_bitmap_t));
    	if(res == NULL)
    	{
    		SYS_LOG_ERR("error: no space for resource");
    		return NULL;
    	}
		memset(res, 0, sizeof(resource_bitmap_t));		
		sty_picture_t* pic = (sty_picture_t*)resource;
		res->sty_data = pic;
		res->regular_info = 0;
//		SYS_LOG_INF("pic w %d, h %d, id 0x%x", res->width, res->height, res->id);
    	ret = _load_bitmap(res_info, res, 1);
		if(ret < 0)
		{
			SYS_LOG_ERR("get scene child bitmap faild\n");
			res_manager_release_resource(res);
			res = NULL;
		}
		return res;
    }

    if(resource->type == RESOURCE_TYPE_TEXT)
    {
    	//buf is now on resource start
    	resource_text_t* res = (resource_text_t*)res_array_alloc(RES_MEM_SIMPLE_INNER, sizeof(resource_text_t));
    	if(res == NULL)
    	{
    		SYS_LOG_ERR("error: no space for resource");
    		return NULL;
    	}
		memset(res, 0, sizeof(resource_text_t));
		sty_string_t* str = (sty_string_t*)resource;
		res->sty_data = str;
//		SYS_LOG_INF("str size %d, id 0x%x", res->font_size, res->id);
    	ret = _load_string(res_info, res);
		if(ret < 0)
		{
			res_manager_release_resource(res);
			res = NULL;
		}
		return res;
    }

	if(resource->type == RESOURCE_TYPE_PICREGION)
	{
		resource_picregion_t* res = (resource_picregion_t*)res_array_alloc(RES_MEM_SIMPLE_INNER, sizeof(resource_picregion_t));
		if(res == NULL)
		{
    		SYS_LOG_ERR("error: no space for resource");
    		return NULL;
		}
		sty_picregion_t* picreg = (sty_picregion_t*)resource;
		res->sty_data = picreg;
		res->regular_info = 0;
		return res;
	}
	return NULL;
}

void* res_manager_get_group_child(resource_info_t* res_info, resource_group_t* resgroup, uint32_t id )
{
    unsigned int i;
	resource_t* resource;
	char*buf;
	int ret;

    if ( resgroup == NULL )
    {
        SYS_LOG_ERR("invalid group");
        return NULL;
    }

    buf = (char*)res_info->sty_data + resgroup->sty_data->child_offset;
	resource = (resource_t*)buf;

//	SYS_LOG_INF("resource sum %d, search id 0x%x\n", resgroup->sty_data->resource_sum, id);
    for ( i = 0; i < resgroup->sty_data->resource_sum; i++ )
    {
//    	SYS_LOG_INF("\n ### resource id 0x%x\n", resource->id);
        if ( resource->id == id )
        {
            break;
        }

        buf = res_info->sty_data + resource->offset;
		resource = (resource_t*)buf;
    }

    if(i >= resgroup->sty_data->resource_sum)
    {
    	SYS_LOG_ERR("cant find resource 0x%x", id);
    	return NULL;
    }

//	SYS_LOG_INF("resource.type 0x%x", resource.type);
    if(resource->type == RESOURCE_TYPE_GROUP)
    {
		//buf is now on group resource start
    	resource_group_t* res = (resource_group_t*)res_array_alloc(RES_MEM_SIMPLE_INNER, sizeof(resource_group_t));
    	if(res == NULL)
    	{
    		SYS_LOG_ERR("error: no space for resource");
    		return NULL;
    	}
		memset(res, 0, sizeof(resource_group_t));
		sty_group_t* group = (sty_group_t*)resource;
		res->sty_data = group;
		return res;

    }

    if(resource->type == RESOURCE_TYPE_PICTURE)
    {
    	//buf is now on resource start
    	resource_bitmap_t* res = (resource_bitmap_t*)res_array_alloc(RES_MEM_SIMPLE_INNER, sizeof(resource_bitmap_t));
    	if(res == NULL)
    	{
    		SYS_LOG_ERR("error: no space for resource");
    		return NULL;
    	}
		memset(res, 0, sizeof(resource_bitmap_t));		
		sty_picture_t* pic = (sty_picture_t*)resource;
		res->sty_data = pic;
//		SYS_LOG_INF("pic w %d, h %d, id 0x%x", res->width, res->height, res->id);
		res->regular_info = 0;
    	ret = _load_bitmap(res_info, res, 1);
		if(ret < 0)
		{
			SYS_LOG_ERR("get group child bitmap faild\n");
			res_manager_release_resource(res);
			res = NULL;
		}
		return res;
    }

    if(resource->type == RESOURCE_TYPE_TEXT)
    {
    	//buf is now on resource start
    	resource_text_t* res = (resource_text_t*)res_array_alloc(RES_MEM_SIMPLE_INNER, sizeof(resource_text_t));
    	if(res == NULL)
    	{
    		SYS_LOG_ERR("error: no space for resource");
    		return NULL;
    	}
		memset(res, 0, sizeof(resource_text_t));
		sty_string_t* str = (sty_string_t*)resource;
		res->sty_data = str;
//		SYS_LOG_INF("str size %d, id 0x%x", res->font_size, res->id);
    	ret = _load_string(res_info, res);
		if(ret < 0)
		{
			res_manager_release_resource(res);
			res = NULL;
		}

		return res;
    }

	if(resource->type == RESOURCE_TYPE_PICREGION)
	{
		resource_picregion_t* res = (resource_picregion_t*)res_array_alloc(RES_MEM_SIMPLE_INNER, sizeof(resource_picregion_t));
		if(res == NULL)
		{
    		SYS_LOG_ERR("error: no space for resource");
    		return NULL;
		}
		sty_picregion_t* picreg = (sty_picregion_t*)resource;
		res->sty_data = picreg;
		res->regular_info = 0;
		return res;
	}
	return NULL;
}

resource_bitmap_t* res_manager_load_frame_from_picregion(resource_info_t* info, resource_picregion_t* picreg, uint32_t frame)
{
	resource_bitmap_t* bitmap;
	uint8_t* buf;
	int32_t ret;

	if(info == NULL || picreg == NULL)
	{
        SYS_LOG_ERR("invalid picreg");
        return NULL;
	}

	bitmap = (resource_bitmap_t*)res_array_alloc(RES_MEM_SIMPLE_INNER, sizeof(resource_bitmap_t));
	if(bitmap == NULL)
	{
        SYS_LOG_ERR("no space for frame loading");
        return NULL;
	}
	buf = info->sty_data + picreg->sty_data->id_offset + sizeof(sty_picture_t)*frame;
	bitmap->sty_data = (sty_picture_t*)buf;
	bitmap->regular_info = picreg->regular_info;

	ret = _load_bitmap(info, bitmap, 1);
	if(ret < 0)
	{
		SYS_LOG_ERR("picregion load frame faild \n");
		res_manager_release_resource(bitmap);
		bitmap = NULL;
	}

	return bitmap;
}


void res_manager_release_resource(void* resource)
{
	uint32_t type = *((uint32_t*)(*((uint32_t*)(resource))));

	if(type == RESOURCE_TYPE_GROUP)
	{
		res_array_free(resource);
	}
	else if(type == RESOURCE_TYPE_PICTURE)
	{
		resource_bitmap_t* bitmap = (resource_bitmap_t*)resource;
		_unload_bitmap(bitmap);
		res_array_free(resource);
	}
	else if(type == RESOURCE_TYPE_TEXT)
	{
		resource_text_t* text = (resource_text_t*)resource;
		_unload_string(text);
		res_array_free(resource);
	}
	else if(type == RESOURCE_TYPE_PICREGION)
	{
		resource_picregion_t* picreg = (resource_picregion_t*)resource;
		res_array_free(picreg);
	}
}

void res_manager_free_resource_structure(void* resource)
{
	if(resource != NULL)
	{
		res_array_free(resource);
	}
}

void res_manager_free_bitmap_data(void* data)
{
	if(data != NULL)
	{
		os_mutex_lock(&bitmap_cache_mutex, OS_FOREVER);
		_free_resource_bitmap_buffer(data);
		os_mutex_unlock(&bitmap_cache_mutex);
	}
}

void res_manager_free_text_data(void* data)
{
	if(data != NULL)
	{
		_free_resource_text_buffer(data);
	}
}

int32_t res_manager_preload_bitmap(resource_info_t* res_info, resource_bitmap_t* bitmap)
{
	int32_t ret;
	ret = _load_bitmap(res_info, bitmap, 0);
	if(ret < 0)
	{
		SYS_LOG_ERR("preload bitmap error 0x%x, %p\n", bitmap->sty_data->id, bitmap->sty_data);
		return -1;
	}
	else
	{
		//SYS_LOG_INF("preload success 0x%x\n", bitmap->sty_data->id);
		return 0;
	}

}

int32_t res_manager_preload_bitmap_compact(uint32_t scene_id, resource_info_t* res_info, resource_bitmap_t* bitmap)
{
	int32_t ret;

//	SYS_LOG_INF("\n res_manager_preload_bitmap_compact scene_id 0x%x\n", scene_id);
	ret = _load_bitmap(res_info, bitmap, scene_id);
	if(ret < 0)
	{
		SYS_LOG_ERR("preload bitmap compact error scene 0x%x, 0x%x\n", scene_id, bitmap->sty_data->id);
		return -1;
	}
	else
	{
		//SYS_LOG_INF("preload success 0x%x\n", bitmap->sty_data->id);
		return 0;
	}

}


int32_t res_manager_load_bitmap(resource_info_t* res_info, resource_bitmap_t* bitmap)
{
	int32_t ret;

	ret = _load_bitmap(res_info, bitmap, 1);
	if(ret < 0)
	{
		SYS_LOG_ERR("load bitmap error 0x%x\n", bitmap->sty_data->sty_id);
		return -1;
	}
	else
	{
		return 0;
	}

}

uint32_t res_manager_get_bitmap_buf_block_unit_size(void)
{
	return sizeof(buf_block_t);
}

int32_t res_manager_init_compact_buffer(uint32_t scene_id, size_t size)
{
	compact_buffer_t* ret;
	os_mutex_lock(&bitmap_cache_mutex, OS_FOREVER);
	
	ret = _init_compact_buffer(scene_id, size);
	if(ret == NULL)
	{
		SYS_LOG_ERR("alloc compact buffer failed: 0x%x, %d\n", scene_id, size);
		os_mutex_unlock(&bitmap_cache_mutex);
		return -1;
	}

	os_mutex_unlock(&bitmap_cache_mutex);
	return 0;
}

void* res_manager_preload_from_scene(resource_info_t* res_info, resource_scene_t* scene, uint32_t id)
{
	uint32_t i;
	resource_t* resource;
	char* buf = NULL;	//地址不对齐，不能用数据类型指针。

	if ( scene == NULL )
	{
		SYS_LOG_ERR("null scene to preload");
		return NULL;
	}

	buf = (char*)res_info->sty_data + scene->child_offset;
	resource = (resource_t*)buf;

//	SYS_LOG_INF("resource sum %d, child offset %d", scene->resource_sum, scene->child_offset);
	for ( i = 0; i < scene->resource_sum; i++ )
	{
//		SYS_LOG_INF("resource id 0x%x, offset 0x%x",resource.id, resource.offset);
		if ( resource->id == id )
		{
			break;
		}

		buf = res_info->sty_data + resource->offset;
		resource = (resource_t*)buf;
	}

	if(i >= scene->resource_sum)
	{
		SYS_LOG_DBG("error: cannot find resource 0x%x", id);
		return NULL;
	}


    if(resource->type == RESOURCE_TYPE_GROUP)
    {
		//buf is now on group resource start
		resource_group_t* res = (resource_group_t*)res_array_alloc(RES_MEM_SIMPLE_INNER, sizeof(resource_group_t));
		if(res == NULL)
		{
			SYS_LOG_ERR("error: no space for resource");
			return NULL;
		}
		sty_group_t* group = (sty_group_t*)resource;
		res->sty_data = group;		

		return res;
    }

	if(resource->type == RESOURCE_TYPE_PICREGION)
	{
		resource_picregion_t* res = (resource_picregion_t*)res_array_alloc(RES_MEM_SIMPLE_INNER, sizeof(resource_picregion_t));
		if(res == NULL)
		{
    		SYS_LOG_ERR("error: no space for resource");
    		return NULL;
		}
		sty_picregion_t* picreg = (sty_picregion_t*)resource;
		res->sty_data = picreg;
		res->regular_info = _check_resource_is_regular(picreg->magic, picreg->sty_id);
		return res;
	}	

	//	SYS_LOG_INF("resource->type 0x%x", resource.type);
	if(resource->type == RESOURCE_TYPE_PICTURE)
	{
    	//buf is now on resource start
		resource_bitmap_t* res = (resource_bitmap_t*)res_array_alloc(RES_MEM_SIMPLE_INNER, sizeof(resource_bitmap_t));
		if(res == NULL)
		{
			SYS_LOG_ERR("error: no space for resource");
			return NULL;
		}
		memset(res, 0, sizeof(resource_bitmap_t));		
		sty_picture_t* pic = (sty_picture_t*)resource;
		res->sty_data = pic;

		res->regular_info = _check_resource_is_regular(pic->magic, pic->sty_id);
//		SYS_LOG_INF("scene 0x%x, id 0x%x, regular 0x%x", scene->scene_id, id, res->regular_info);

#if RES_MEM_DEBUG
		if(res_mem_check()<0)
		{
			SYS_LOG_INF("%s %d\n", __FILE__, __LINE__);
			res_mem_dump();
		}
#endif		

		return res;
	}

//	SYS_LOG_ERR("wrong type to preload");
	return NULL;
}

resource_bitmap_t* res_manager_preload_from_group(resource_info_t* res_info, resource_group_t* group, uint32_t scene_id, uint32_t id)
{
	uint32_t i;
	resource_t* resource;
	char* buf = NULL;	//地址不对齐，不能用数据类型指针。

	if ( group == NULL )
	{
		SYS_LOG_ERR("null scene to preload");
		return NULL;
	}

	buf = (char*)res_info->sty_data + group->sty_data->child_offset;
	resource = (resource_t*)buf;

//	SYS_LOG_INF("resource sum %d, search id 0x%x\n", resgroup->resource_sum, id);
	for ( i = 0; i < group->sty_data->resource_sum; i++ )
	{
//		SYS_LOG_INF("\n ### resource id 0x%x\n", resource->id);
		if ( resource->id == id )
		{
			break;
		}

		buf = res_info->sty_data + resource->offset;
		resource = (resource_t*)buf;
	}

	if(i >= group->sty_data->resource_sum)
	{
		SYS_LOG_ERR("cant find resource 0x%x", id);
		return NULL;
	}


//	SYS_LOG_INF("resource->type 0x%x", resource.type);
	if(resource->type == RESOURCE_TYPE_PICTURE)
	{
		//buf is now on resource start
		resource_bitmap_t* res = (resource_bitmap_t*)res_array_alloc(RES_MEM_SIMPLE_INNER, sizeof(resource_bitmap_t));
		if(res == NULL)
		{
			SYS_LOG_ERR("error: no space for resource to preload");
			return NULL;
		}
		memset(res, 0, sizeof(resource_bitmap_t));
		sty_picture_t* pic = (sty_picture_t*)resource;
		res->sty_data = pic;

		res->regular_info = _check_resource_is_regular(pic->magic, pic->sty_id);
//		SYS_LOG_INF("scene 0x%x, id 0x%x, regular 0x%x", scene_id, id, res->regular_info);
#if RES_MEM_DEBUG
		if(res_mem_check()<0)
		{
			SYS_LOG_INF("%s %d\n", __FILE__, __LINE__);
			res_mem_dump();
		}
#endif		

		return res;
	}

//	SYS_LOG_ERR("wrong type to preload");
	return NULL;

}

resource_bitmap_t* res_manager_preload_from_picregion(resource_info_t* res_info, resource_picregion_t* picreg, uint32_t frame)
{
	resource_bitmap_t* bitmap;
	uint8_t* buf;

	if(res_info == NULL || picreg == NULL)
	{
        SYS_LOG_ERR("invalid picreg for preload");
        return NULL;
	}

	bitmap = (resource_bitmap_t*)res_array_alloc(RES_MEM_SIMPLE_INNER, sizeof(resource_bitmap_t));
	if(bitmap == NULL)
	{
        SYS_LOG_ERR("no space for frame preloading");
        return NULL;
	}
	
	buf = res_info->sty_data + picreg->sty_data->id_offset + sizeof(sty_picture_t)*frame;
	bitmap->sty_data = (sty_picture_t*)buf;
	bitmap->regular_info = picreg->regular_info;

	return bitmap;

}


void* res_manager_preload_next_group_child(resource_info_t * info, resource_group_t* group, int* count, uint32_t* offset, uint32_t scene_id, uint32_t pargroup_id)
{
    resource_t* resource;
	char* buf = NULL;   //地址不对齐，不能用数据类型指针。
	uint32_t cur_count = *count;
	uint32_t next_offset = *offset;

	if(group == NULL)
	{
		SYS_LOG_ERR("invalid param for res_manager_preload_next_group_child\n");
		return NULL;
	}

	if(cur_count == 0)
	{
		buf = (char*)info->sty_data + group->sty_data->child_offset;
		resource = (resource_t*)buf;
		
		*count = 1;
		*offset = resource->offset;
	}
	else if(cur_count >= group->sty_data->resource_sum)
	{
		//reach the last one
		return NULL;
	}
	else
	{
		buf = info->sty_data + next_offset;
		resource = (resource_t*)buf;
		cur_count++;
		next_offset = resource->offset;
		*count = cur_count;
		*offset = next_offset;
	}
	if(resource->type == RESOURCE_TYPE_GROUP)
	{
		//buf is now on group resource start
		resource_group_t* res = (resource_group_t*)res_array_alloc(RES_MEM_SIMPLE_INNER, sizeof(resource_group_t));
		if(res == NULL)
		{
			SYS_LOG_ERR("error: no space for resource");
			return NULL;
		}
	
		sty_group_t* group = (sty_group_t*)resource;
		res->sty_data = group;

		return res;
	}

	if(resource->type == RESOURCE_TYPE_PICREGION)
	{
		resource_picregion_t* res = (resource_picregion_t*)res_array_alloc(RES_MEM_SIMPLE_INNER, sizeof(resource_picregion_t));
		if(res == NULL)
		{
			SYS_LOG_ERR("error: no space for resource");
			return NULL;
		}

		sty_picregion_t* picreg = (sty_picregion_t*)resource;
		res->sty_data = picreg;
		res->regular_info = _check_resource_is_regular(picreg->magic, picreg->sty_id);
		return res;
	}

	if(resource->type == RESOURCE_TYPE_PICTURE)
	{
		//buf is now on resource start
		resource_bitmap_t* res = (resource_bitmap_t*)res_array_alloc(RES_MEM_SIMPLE_INNER, sizeof(resource_bitmap_t));
		if(res == NULL)
		{
			SYS_LOG_ERR("error: no space for resource to preload");
			return NULL;
		}
		memset(res, 0, sizeof(resource_bitmap_t));
		sty_picture_t* pic = (sty_picture_t*)resource;
		res->sty_data = pic;
		res->regular_info = _check_resource_is_regular(pic->magic, pic->sty_id);

//		SYS_LOG_INF("pic w %d, h %d, id 0x%x", res->width, res->height, res->id);
//		SYS_LOG_INF("scene 0x%x, id 0x%x, regular 0x%x", scene_id, resource->id, res->regular_info);

		return res;
	}

	//ignore text
	return NULL;
}

void* res_manager_preload_next_scene_child(resource_info_t* info, resource_scene_t* scene, uint32_t* count, uint32_t* offset)
{
    resource_t* resource;
	uint8_t* buf = NULL;   //地址不对齐，不能用数据类型指针。
	uint32_t cur_count = *count;
	uint32_t next_offset = (uint32_t)*offset;

	if(scene == NULL)
	{
		SYS_LOG_ERR("invalid param for res_manager_preload_next_scene_child\n");
		return NULL;
	}

	if(cur_count == 0)
	{
		buf = (uint8_t*)info->sty_data + scene->child_offset;
		resource = (resource_t*)buf;
		*count = 1;
		*offset = resource->offset;
	}
	else if(cur_count >= scene->resource_sum)
	{
		//reach the last one
		return NULL;
	}
	else
	{
		buf = (uint8_t*)info->sty_data + next_offset;
		resource = (resource_t*)buf;
		cur_count++;
		next_offset = resource->offset;
		*count = cur_count;
		*offset = next_offset;
	}

    if(resource->type == RESOURCE_TYPE_GROUP)
    {
		//buf is now on group resource start
    	resource_group_t* res = (resource_group_t*)res_array_alloc(RES_MEM_SIMPLE_INNER, sizeof(resource_group_t));
    	if(res == NULL)
    	{
    		SYS_LOG_ERR("error: no space for resource");
    		return NULL;
    	}
	
		sty_group_t* group = (sty_group_t*)resource;
		res->sty_data = group;	

		return res;
    }

	if(resource->type == RESOURCE_TYPE_PICREGION)
	{
		resource_picregion_t* res = (resource_picregion_t*)res_array_alloc(RES_MEM_SIMPLE_INNER, sizeof(resource_picregion_t));
		if(res == NULL)
		{
    		SYS_LOG_ERR("error: no space for resource");
    		return NULL;
		}

		sty_picregion_t* picreg = (sty_picregion_t*)resource;
		res->sty_data = picreg;
		res->regular_info = _check_resource_is_regular(picreg->magic, picreg->sty_id);		
		return res;
	}

	if(resource->type == RESOURCE_TYPE_PICTURE)
	{
		//buf is now on resource start
		resource_bitmap_t* res = (resource_bitmap_t*)res_array_alloc(RES_MEM_SIMPLE_INNER, sizeof(resource_bitmap_t));
		if(res == NULL)
		{
			SYS_LOG_ERR("error: no space for resource to preload");
			return NULL;
		}
		memset(res, 0, sizeof(resource_bitmap_t));
		
		sty_picture_t* pic = (sty_picture_t*)resource;
		res->sty_data = pic;
 		res->regular_info = _check_resource_is_regular(pic->magic, pic->sty_id);
//		SYS_LOG_INF("scene 0x%x, id 0x%x, regular 0x%x", cur_scene_id, resource->id, res->regular_info);
		return res;
	}

	//ignore text;
	return NULL;
}

void res_manager_preload_finish_check(uint32_t scene_id)
{
	compact_buffer_t* buffer;

	os_strace_u32(SYS_TRACE_ID_RES_CHECK_PRELOAD, (uint32_t)scene_id);	
	os_mutex_lock(&bitmap_cache_mutex, OS_FOREVER);

	buffer = bitmap_buffer.compact_buffer_list;
	while(buffer != NULL)
	{
		if(buffer->scene_id == scene_id)
		{
			mem_dcache_clean(buffer->addr, buffer->offset);			
		}
		buffer = buffer->next;	
	}

	os_mutex_unlock(&bitmap_cache_mutex);
	os_strace_end_call_u32(SYS_TRACE_ID_RES_CHECK_PRELOAD, (uint32_t)scene_id);	
}

uint32_t res_manager_scene_is_loaded(uint32_t scene_id)
{
	compact_buffer_t* buffer = bitmap_buffer.compact_buffer_list;
	buf_block_t* item;

	while(buffer)
	{
//		SYS_LOG_DBG("buffer->scene_id 0x%x, scene_id 0x%x\n", buffer->scene_id, scene_id);
		if(buffer->scene_id == scene_id)
		{
			item = (buf_block_t*)buffer->addr;
			if(item->regular_info == 0)
			{
				return 1;
			}
		}

		buffer = buffer->next;
	}

	return 0;
}

void res_manager_dump_info(void)
{
	compact_buffer_t* citem;
	buf_block_t* bitem;

	//mem dump
	res_mem_dump();	

	//ui mem info
	SYS_LOG_INF("full screen bitmap total %d\n", ui_mem_total);

	//compact buffer dump();
	citem = bitmap_buffer.compact_buffer_list;
	while(citem != NULL)
	{
		SYS_LOG_INF("compact buffer sceneid 0x%x, addr %p, size %d, freesize %d ", citem->scene_id, citem->addr, citem->offset, citem->free_size);
		citem = citem->next;
	}
	
	//bitmap dump();
	bitem = bitmap_buffer.head;
	while(bitem != NULL)
	{
		SYS_LOG_INF("bitmap buffer id %d, width %d, height %d, addr %p, size %d ", bitem->id, bitem->width, bitem->height, bitem->addr, bitem->size);
		bitem = bitem->next;
	}
}

