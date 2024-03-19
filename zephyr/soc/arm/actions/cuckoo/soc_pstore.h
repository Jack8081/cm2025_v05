/*
 * Copyright (c) 2019 Actions Semiconductor Co., Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file peripheral pstore configuration macros for Actions SoC
 */

#ifndef	_ACTIONS_SOC_PSTORE_H_
#define	_ACTIONS_SOC_PSTORE_H_

#define SOC_PSTORE_TAG_CAPACITY   0
#define SOC_PSTORE_TAG_FLAG_CAP   1


#ifndef _ASMLANGUAGE

int soc_pstore_set(u32_t tag, u32_t value);
int soc_pstore_get(u32_t tag, u32_t *p_value);
int soc_pstore_reset_all(void);

#endif /* _ASMLANGUAGE */

#endif /* _ACTIONS_SOC_PSTORE_H_	*/
