/*
 * Copyright (c) 2024 NVIDIA CORPORATION AND AFFILIATES.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification, are permitted
 * provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright notice, this list of
 *       conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright notice, this list of
 *       conditions and the following disclaimer in the documentation and/or other materials
 *       provided with the distribution.
 *     * Neither the name of the NVIDIA CORPORATION nor the names of its contributors may be used
 *       to endorse or promote products derived from this software without specific prior written
 *       permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL NVIDIA CORPORATION BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TOR (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#ifndef DPA_INITIATOR_TARGET_COMMON_DEFS_H_
#define DPA_INITIATOR_TARGET_COMMON_DEFS_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Number of expected receive completions
 */
#define EXPECTED_NUM_RECEIVES (4)

/**
 * @brief DPA thread #1 device argument struct
 */
struct dpa_thread1_arg {
	doca_dpa_dev_t dpa_ctx_handle;
	uint64_t notification_comp_handle;
	uint64_t dpa_comp_handle;
	uint64_t target_rdma_handle;
	uint64_t local_buf_addr;
	uint32_t dpa_mmap_handle;
	size_t length;
} __attribute__((__packed__, aligned(8)));

/**
 * @brief DPA thread #2 device argument struct
 */
struct dpa_thread2_arg {
	uint64_t sync_event_handle;
	uint64_t completion_count;
} __attribute__((__packed__, aligned(8)));

#ifdef __cplusplus
}
#endif

#endif /* DPA_INITIATOR_TARGET_COMMON_DEFS_H_ */
