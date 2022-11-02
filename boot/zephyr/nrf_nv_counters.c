/*
 * Copyright (c) 2022 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-BSD-5-Clause-Nordic
 */
#include <stdint.h>
#include <bl_otp_counters.h>
#include "bootutil/fault_injection_hardening.h"

fih_int boot_nv_security_counter_init(void){
    return FIH_SUCCESS;
}

/**
 * Reads the stored value of a given image's security counter.
 *
 * @param image_id          Index of the image (from 0).
 * @param security_cnt      Pointer to store the security counter value.
 *
 * @return                  FIH_SUCCESS on success
 */
fih_int boot_nv_security_counter_get(uint32_t image_id, fih_int *security_cnt){
    int err;
    uint16_t cur_sec_cnt;

    if(security_cnt == NULL){
        return FIH_FAILURE;
    }

    switch (image_id)
    {
    case 0:
        err = get_monotonic_counter(COUNTER_DESC_MCUBOOT_HW_COUNTER_ID0, &cur_sec_cnt);
        break;
    case 1:
        err = get_monotonic_counter(COUNTER_DESC_MCUBOOT_HW_COUNTER_ID1, &cur_sec_cnt);
        break;
    case 2:
        err = get_monotonic_counter(COUNTER_DESC_MCUBOOT_HW_COUNTER_ID2, &cur_sec_cnt);
        break;
    default:
        return FIH_FAILURE;
    }

    if(err != 0){
        return FIH_FAILURE;
    }

    *security_cnt = cur_sec_cnt;

    return FIH_SUCCESS;
}

/**
 * Updates the stored value of a given image's security counter with a new
 * security counter value if the new one is greater.
 *
 * @param image_id          Index of the image (from 0).
 * @param img_security_cnt  New security counter value. The new value must be
 *                          between 0 and UINT32_MAX and it must be greater than
 *                          or equal to the current security counter value.
 *
 * @return                  0 on success; nonzero on failure.
 */
int32_t boot_nv_security_counter_update(uint32_t image_id,
                                        uint32_t img_security_cnt)
{
    /* We only support 16 bits image counters */
    if( (img_security_cnt & 0xFFFF0000) > 0){
        return -1;
    }

    switch (image_id)
    {
    case 0:
        return set_monotonic_counter(COUNTER_DESC_MCUBOOT_HW_COUNTER_ID0, (uint16_t) img_security_cnt);
    case 1:
        return set_monotonic_counter(COUNTER_DESC_MCUBOOT_HW_COUNTER_ID1, (uint16_t) img_security_cnt);
    case 2:
        return set_monotonic_counter(COUNTER_DESC_MCUBOOT_HW_COUNTER_ID2, (uint16_t) img_security_cnt);
    default:
        return FIH_FAILURE;
    }

}

