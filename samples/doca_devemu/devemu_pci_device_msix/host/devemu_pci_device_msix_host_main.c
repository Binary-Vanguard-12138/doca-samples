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

#include <stdlib.h>
#include <string.h>

#include <doca_argp.h>
#include <doca_error.h>
#include <doca_dev.h>
#include <doca_log.h>

#include <devemu_pci_host_common.h>

DOCA_LOG_REGISTER(DEVEMU_PCI_DEVICE_MSIX_HOST::MAIN);

/* Sample's Logic */
doca_error_t devemu_pci_device_msix_host(const char *pci_address, int vfio_group);

/* Configuration struct */
struct devemu_pci_cfg {
	char pci_address[DOCA_DEVINFO_PCI_ADDR_SIZE]; /* Emulated device PCI address */
	int vfio_group;				      /* The VFIO group ID of the emulated device */
};

#ifndef DOCA_ARCH_DPU

/*
 * ARGP Callback - Handle PCI device address parameter
 *
 * @param [in]: Input parameter
 * @config [in/out]: Program configuration context
 * @return: DOCA_SUCCESS on success and DOCA_ERROR otherwise
 */
static doca_error_t pci_callback(void *param, void *config)
{
	struct devemu_pci_cfg *conf = (struct devemu_pci_cfg *)config;
	const char *addr = (char *)param;

	return parse_emulated_pci_address(addr, conf->pci_address);
}

/*
 * ARGP Callback - Handle VFIO Group ID parameter
 *
 * @param [in]: Input parameter
 * @config [in/out]: Program configuration context
 * @return: DOCA_SUCCESS on success and DOCA_ERROR otherwise
 */
static doca_error_t vfio_group_callback(void *param, void *config)
{
	struct devemu_pci_cfg *conf = (struct devemu_pci_cfg *)config;
	int group_id = *(int *)param;

	if (group_id < 0) {
		DOCA_LOG_ERR("Entered VFIO group ID, is invalid. Must be positive integer. Received %d", group_id);
		return DOCA_ERROR_INVALID_VALUE;
	}

	conf->vfio_group = group_id;

	return DOCA_SUCCESS;
}

/*
 * Register the command line parameters for the sample
 *
 * @return: DOCA_SUCCESS on success and DOCA_ERROR otherwise
 */
static doca_error_t register_devemu_pci_params(void)
{
	doca_error_t result;

	result = register_emulated_pci_address_param(pci_callback);
	if (result != DOCA_SUCCESS)
		return result;

	result = register_vfio_group_param(vfio_group_callback);
	if (result != DOCA_SUCCESS)
		return result;

	return DOCA_SUCCESS;
}

#endif // DOCA_ARCH_DPU

/*
 * Sample main function
 *
 * @argc [in]: command line arguments size
 * @argv [in]: array of command line arguments
 * @return: EXIT_SUCCESS on success and EXIT_FAILURE otherwise
 */
int main(int argc, char **argv)
{
	struct devemu_pci_cfg devemu_pci_cfg;
	doca_error_t result;
	struct doca_log_backend *sdk_log;
	int exit_status = EXIT_FAILURE;

	/* Register a logger backend */
	result = doca_log_backend_create_standard();
	if (result != DOCA_SUCCESS)
		goto sample_exit;

	/* Register a logger backend for internal SDK errors and warnings */
	result = doca_log_backend_create_with_file_sdk(stderr, &sdk_log);
	if (result != DOCA_SUCCESS)
		goto sample_exit;
	result = doca_log_backend_set_sdk_level(sdk_log, DOCA_LOG_LEVEL_INFO);
	if (result != DOCA_SUCCESS)
		goto sample_exit;

	DOCA_LOG_INFO("Starting the sample");

#ifndef DOCA_ARCH_DPU

	result = doca_argp_init(NULL, &devemu_pci_cfg);
	if (result != DOCA_SUCCESS) {
		DOCA_LOG_ERR("Failed to init ARGP resources: %s", doca_error_get_descr(result));
		goto sample_exit;
	}
	result = register_devemu_pci_params();
	if (result != DOCA_SUCCESS) {
		DOCA_LOG_ERR("Failed to register sample command line parameters: %s", doca_error_get_descr(result));
		goto argp_cleanup;
	}
	result = doca_argp_start(argc, argv);
	if (result != DOCA_SUCCESS) {
		DOCA_LOG_ERR("Failed to parse sample input: %s", doca_error_get_descr(result));
		goto argp_cleanup;
	}

	result = devemu_pci_device_msix_host(devemu_pci_cfg.pci_address, devemu_pci_cfg.vfio_group);
	if (result != DOCA_SUCCESS) {
		DOCA_LOG_ERR("devemu_pci_device_msix_host() encountered an error: %s", doca_error_get_descr(result));
		goto argp_cleanup;
	}

	exit_status = EXIT_SUCCESS;

argp_cleanup:
	doca_argp_destroy();

#else // DOCA_ARCH_DPU
	(void)argc;
	(void)argv;
	(void)devemu_pci_cfg;

	DOCA_LOG_ERR("PCI Emulated Device MSI-X Host can run only on the Host");
	exit_status = EXIT_FAILURE;

#endif // DOCA_ARCH_DPU

sample_exit:
	if (exit_status == EXIT_SUCCESS)
		DOCA_LOG_INFO("Sample finished successfully");
	else
		DOCA_LOG_INFO("Sample finished with errors");
	return exit_status;
}
