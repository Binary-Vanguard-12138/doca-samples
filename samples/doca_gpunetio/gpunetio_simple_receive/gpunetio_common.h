/*
 * Copyright (c) 2023 NVIDIA CORPORATION AND AFFILIATES.  All rights reserved.
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

#ifndef GPUNETIO_SEND_WAIT_TIME_COMMON_H_
#define GPUNETIO_SEND_WAIT_TIME_COMMON_H_

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#include <cuda.h>
#include <cuda_runtime.h>

#include <doca_error.h>
#include <doca_dev.h>
#include <doca_mmap.h>
#include <doca_gpunetio.h>
#include <doca_eth_rxq.h>
#include <doca_buf_array.h>

#include "common.h"

/* Set alignment to 64kB to work on all platforms */
#define GPU_PAGE_SIZE (1UL << 16)
#define MAX_PCI_ADDRESS_LEN 32U
#define CUDA_BLOCK_THREADS 32
#define PACKET_SIZE 1024
#define ETHER_ADDR_LEN 6
#define MAX_RQ_DESCR_NUM 8192
#define MAX_PKT_NUM 16384
#define MAX_PKT_SIZE 2048
#define MAX_RX_TIMEOUT_NS 500000 // 500us
#define MAX_RX_NUM_PKTS 2048

/* Application configuration structure */
struct sample_send_wait_cfg {
	char gpu_pcie_addr[MAX_PCI_ADDRESS_LEN]; /* GPU PCIe address */
	char nic_pcie_addr[MAX_PCI_ADDRESS_LEN]; /* Network card PCIe address */
};

/* Send queues objects */
struct rxq_queue {
	struct doca_gpu *gpu_dev; /* GPUNetio handler associated to queues*/
	struct doca_dev *ddev;	  /* DOCA device handler associated to queues */

	struct doca_ctx *eth_rxq_ctx;		     /* DOCA Ethernet send queue context */
	struct doca_eth_rxq *eth_rxq_cpu;	     /* DOCA Ethernet send queue CPU handler */
	struct doca_gpu_eth_rxq *eth_rxq_gpu;	     /* DOCA Ethernet send queue GPU handler */
	struct doca_mmap *pkt_buff_mmap;	     /* DOCA mmap to receive packet with DOCA Ethernet queue */
	void *gpu_pkt_addr;			     /* DOCA mmap GPU memory address */
	int dmabuf_fd;				     /* GPU memory dmabuf descriptor */
	struct doca_flow_port *port;		     /* DOCA Flow port */
	struct doca_flow_pipe *rxq_pipe;	     /* DOCA Flow receive pipe */
	struct doca_flow_pipe *root_pipe;	     /* DOCA Flow root pipe */
	struct doca_flow_pipe_entry *root_udp_entry; /* DOCA Flow root entry */
};

/*
 * Launch GPUNetIO simple receive sample
 *
 * @sample_cfg [in]: Sample config parameters
 * @return: DOCA_SUCCESS on success and DOCA_ERROR otherwise
 */
doca_error_t gpunetio_simple_receive(struct sample_send_wait_cfg *sample_cfg);

#if __cplusplus
extern "C" {
#endif

/*
 * Launch a CUDA kernel to send packets with wait on time feature.
 *
 * @stream [in]: CUDA stream to launch the kernel
 * @rxq [in]: DOCA Eth Tx queue to use to send packets
 * @gpu_exit_condition [in]: exit from CUDA kernel
 * @return: DOCA_SUCCESS on success and DOCA_ERROR otherwise
 */
doca_error_t kernel_receive_packets(cudaStream_t stream, struct rxq_queue *rxq, uint32_t *gpu_exit_condition);

#if __cplusplus
}
#endif
#endif
