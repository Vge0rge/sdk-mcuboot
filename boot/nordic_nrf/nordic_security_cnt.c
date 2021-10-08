/*
 * Copyright (c) 2021 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include "bootutil/security_cnt.h"
#include "bl_secure_counters.h"
#include <stdint.h>

#define COUNTER_MATCH_FLAG_LOW 0xA5A5
#define COUNTER_MATCH_FLAG_HIGH 0xB5B5

fih_int
boot_nv_security_counter_init(void)
{
    /* Do nothing. */
    return 0;
}

fih_int
boot_nv_security_counter_get(uint32_t image_id, fih_int *security_cnt)
{
    *security_cnt = get_monotonic_counter((uint16_t)image_id & 0xFFFF);

    return 0;
}

int32_t
boot_nv_security_counter_update(uint32_t image_id, uint32_t img_security_cnt)
{
    volatile uint16_t cur_security_cnt;
    volatile uint16_t cur_security_cnt_dublicate;
    volatile uint32_t op_cnt = 0;
    volatile uint32_t counter_match_flag;

    /* We only support 16 bits image counters */
    if( (img_security_cnt & 0xFFFF0000) > 0){
        return -1;
    }

    cur_security_cnt = get_monotonic_counter((uint16_t)image_id & 0xFFFF);
    op_cnt++;
    if (cur_security_cnt == (uint16_t)img_security_cnt){
        op_cnt++;
        counter_match_flag = COUNTER_MATCH_FLAG_LOW;
    }

    /* Check for skipped operations and read the value twice to harden against glitching */
    if (counter_match_flag == COUNTER_MATCH_FLAG_LOW && op_cnt != 2){
        return -1;
    }else if (counter_match_flag == COUNTER_MATCH_FLAG_LOW && op_cnt == 2){
        cur_security_cnt_dublicate = get_monotonic_counter((uint16_t)image_id & 0xFFFF);
        op_cnt++;
        if (cur_security_cnt_dublicate == img_security_cnt){
            op_cnt++;
            counter_match_flag |= (COUNTER_MATCH_FLAG_HIGH << 16);
        }

        /* If the counter_match_flag doesn't have the correct value or any operation is skipped
        * return error. If operations and flag match return success.
        **/
        if(counter_match_flag == ( COUNTER_MATCH_FLAG_LOW | (COUNTER_MATCH_FLAG_HIGH << 16))
           && op_cnt == 4){
            return 0;
        } else {
            return -1;
        }

    }


    return set_monotonic_counter((uint16_t)image_id & 0xFFFF, (uint16_t)img_security_cnt & 0xFFFF);
}
