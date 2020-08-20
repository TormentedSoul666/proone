#include "protocol.h"
#include "util_rt.h"
#include "config.h"
#include "dvault.h"

#include <string.h>
#include <assert.h>
#include <errno.h>
#include <sys/random.h>


static uint8_t proto_buf[PRNE_HTBT_PROTO_MIN_BUF];
static size_t proto_buf_cnt_len;

static void test_ser (void);


int main (void) {
	// prne_arch_t string functions
	for (prne_arch_t i = PRNE_ARCH_NONE + 1; i < NB_PRNE_ARCH; i += 1) {
		assert(i == prne_arch_fstr(prne_arch_tostr(i)));
	}
	
	test_ser();

	return 0;
}

static void test_ser (void) {
	static size_t actual;
	static char test_id[256];
	static char test_pw[256];
	static prne_htbt_msg_head_t mh_a, mh_b;
	static prne_htbt_status_t s_a, s_b;
	static prne_host_cred_t hc_a, hc_b;
	static uint8_t salt;
	static uint8_t *cred_data = NULL;
	static size_t cred_data_len = 0;
	static prne_htbt_host_info_t hi_a, hi_b;
	static prne_htbt_cmd_t cmd_a, cmd_b;
	static char *test_args[] = {
		"sudo",
		"systemctl",
		"enable",
		"--now",
		"NetworkManager",
		NULL
	};
	static char test_args_mem[] = "\x00\x2Bsudo\0systemctl\0enable\0--now\0NetworkManager";
	static char *empty_args[] = {
		NULL
	};
	static char *long_args[] = {
		"", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", 
		"", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", NULL
	};
	static char *too_long_args[] = {
		"", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", 
		"", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", NULL
	};
	static char *long_mem_args[] = {
		"123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", 
		"123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", 
		"123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", 
		"123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", 
		"123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", 
		"123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", 
		"123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", 
		"123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", 
		"123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", 
		"123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", 
		"123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", "123", NULL
	};
	static char *too_long_mem_args[] = {
		"1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", 
		"1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", 
		"1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", 
		"1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", 
		"1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", 
		"1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "1234567", "12345678", NULL
	};
	static prne_htbt_bin_meta_t bm_a, bm_b;
	static const uint8_t prog_ver[] = PRNE_PROG_VER;

	// init
	for (size_t i = 0; i < 255; i += 1) {
		test_id[i] = (char)(i + 1);
		test_pw[254 - i] = (char)(i + 1);
	}
	test_pw[255] = test_id[255] = 0;
	assert(strlen(test_id) == 255 && strlen(test_pw) == 255);

	assert(prne_geturandom(&salt, 1) == 1);

	// free functions should accept NULL
	prne_htbt_free_msg_head(NULL);
	prne_htbt_free_status(NULL);
	prne_free_host_cred(NULL);
	prne_htbt_free_host_info(NULL);
	prne_htbt_free_cmd(NULL);
	prne_htbt_free_bin_meta(NULL);

	// msg head
	prne_htbt_init_msg_head(&mh_a);
	prne_htbt_init_msg_head(&mh_b);
	// normal NOOP case
	// req
	assert(prne_htbt_ser_msg_head(proto_buf, PRNE_HTBT_PROTO_MIN_BUF, &proto_buf_cnt_len, &mh_a) == PRNE_HTBT_SER_RC_OK);
	assert(proto_buf_cnt_len == 3 && memcmp("\x80\x00\x00", proto_buf, proto_buf_cnt_len) == 0);
	assert(prne_htbt_dser_msg_head(proto_buf, PRNE_HTBT_PROTO_MIN_BUF, &proto_buf_cnt_len, &mh_b) == PRNE_HTBT_SER_RC_OK);
	assert(prne_htbt_eq_msg_head(&mh_a, &mh_b));
	// rsp
	mh_a.is_rsp = true;
	assert(prne_htbt_ser_msg_head(proto_buf, PRNE_HTBT_PROTO_MIN_BUF, &proto_buf_cnt_len, &mh_a) == PRNE_HTBT_SER_RC_OK);
	assert(proto_buf_cnt_len == 3 && memcmp("\x00\x00\x00", proto_buf, proto_buf_cnt_len) == 0);
	assert(prne_htbt_dser_msg_head(proto_buf, PRNE_HTBT_PROTO_MIN_BUF, &proto_buf_cnt_len, &mh_b) == PRNE_HTBT_SER_RC_OK);
	assert(prne_htbt_eq_msg_head(&mh_a, &mh_b));
	// error cases
	// using id other than 0 for NOOP should be an error
	mh_a.id = 1;
	mh_a.op = PRNE_HTBT_OP_NOOP;
	assert(prne_htbt_ser_msg_head(proto_buf, PRNE_HTBT_PROTO_MIN_BUF, &proto_buf_cnt_len, &mh_a) == PRNE_HTBT_SER_RC_FMT_ERR);
	// using id 0 for OP other than NOOP should be an error
	mh_a.id = 0;
	mh_a.op = PRNE_HTBT_OP_HOST_INFO;
	assert(prne_htbt_ser_msg_head(proto_buf, PRNE_HTBT_PROTO_MIN_BUF, &proto_buf_cnt_len, &mh_a) == PRNE_HTBT_SER_RC_FMT_ERR);
	// using id over 0x8000 should be an error
	mh_a.id = 0x8000;
	mh_a.op = PRNE_HTBT_OP_STATUS;
	assert(prne_htbt_ser_msg_head(proto_buf, PRNE_HTBT_PROTO_MIN_BUF, &proto_buf_cnt_len, &mh_a) == PRNE_HTBT_SER_RC_FMT_ERR);
	// normal cases
	mh_a.is_rsp = false;
	mh_a.id = 0x1234;
	mh_a.op = PRNE_HTBT_OP_STATUS;
	assert(prne_htbt_ser_msg_head(proto_buf, PRNE_HTBT_PROTO_MIN_BUF, &proto_buf_cnt_len, &mh_a) == PRNE_HTBT_SER_RC_OK);
	assert(proto_buf_cnt_len == 3 && memcmp("\x92\x34\x01", proto_buf, proto_buf_cnt_len) == 0);
	assert(prne_htbt_dser_msg_head(proto_buf, PRNE_HTBT_PROTO_MIN_BUF, &proto_buf_cnt_len, &mh_b) == PRNE_HTBT_SER_RC_OK);
	assert(prne_htbt_eq_msg_head(&mh_a, &mh_b));
	mh_a.is_rsp = true;
	mh_a.id = 0x5678;
	mh_a.op = PRNE_HTBT_OP_STATUS;
	assert(prne_htbt_ser_msg_head(proto_buf, PRNE_HTBT_PROTO_MIN_BUF, &proto_buf_cnt_len, &mh_a) == PRNE_HTBT_SER_RC_OK);
	assert(proto_buf_cnt_len == 3 && memcmp("\x56\x78\x01", proto_buf, proto_buf_cnt_len) == 0);
	assert(prne_htbt_dser_msg_head(proto_buf, PRNE_HTBT_PROTO_MIN_BUF, &proto_buf_cnt_len, &mh_b) == PRNE_HTBT_SER_RC_OK);
	assert(prne_htbt_eq_msg_head(&mh_a, &mh_b));
	// just testing (placeholder)
	prne_htbt_free_msg_head(&mh_a);
	prne_htbt_free_msg_head(&mh_b);

	// status
	prne_htbt_init_status(&s_a);
	prne_htbt_init_status(&s_b);
	s_a.code = PRNE_HTBT_STATUS_ERRNO;
	s_a.err = EHOSTUNREACH;
	assert(prne_htbt_ser_status(proto_buf, PRNE_HTBT_PROTO_MIN_BUF, &proto_buf_cnt_len, &s_a) == PRNE_HTBT_SER_RC_OK);
	assert(proto_buf_cnt_len == 5 && memcmp("\x02\x00\x00\x00\x71", proto_buf, 5) == 0);
	assert(prne_htbt_dser_status(proto_buf, PRNE_HTBT_PROTO_MIN_BUF, &proto_buf_cnt_len, &s_b) == PRNE_HTBT_SER_RC_OK);
	assert(prne_htbt_eq_status(&s_a, &s_b));
	prne_htbt_free_status(&s_a);
	prne_htbt_free_status(&s_b);	

	// empty cred
	prne_init_host_cred(&hc_a);
	prne_init_host_cred(&hc_b);
	assert(prne_alloc_host_cred(&hc_a, 0, 0));
	hc_a.id[0] = 0;
	hc_a.pw[0] = 0;
	assert(prne_enc_host_cred(proto_buf, PRNE_HTBT_PROTO_MIN_BUF, &proto_buf_cnt_len, salt, &hc_a) == PRNE_HTBT_SER_RC_OK);
	assert(prne_dec_host_cred(proto_buf, proto_buf_cnt_len, &hc_b) == PRNE_HTBT_SER_RC_OK);
	assert(prne_eq_host_cred(&hc_a, &hc_b));
	prne_free_host_cred(&hc_a);
	prne_free_host_cred(&hc_b);
	// normal case
	assert(prne_alloc_host_cred(&hc_a, 255, 255));
	strcpy(hc_a.id, test_id);
	strcpy(hc_a.pw, test_pw);
	assert(prne_enc_host_cred(proto_buf, PRNE_HTBT_PROTO_MIN_BUF, &proto_buf_cnt_len, salt, &hc_a) == PRNE_HTBT_SER_RC_OK);
	cred_data_len = proto_buf_cnt_len;
	cred_data = (uint8_t*)prne_malloc(1, proto_buf_cnt_len);
	memcpy(cred_data, proto_buf, proto_buf_cnt_len);
	assert(prne_dec_host_cred(proto_buf, proto_buf_cnt_len, &hc_b) == PRNE_HTBT_SER_RC_OK);
	assert(prne_eq_host_cred(&hc_a, &hc_b));
	prne_free_host_cred(&hc_a);
	prne_free_host_cred(&hc_b);

	// host info
	prne_htbt_init_host_info(&hi_a);
	prne_htbt_init_host_info(&hi_b);
	prne_htbt_alloc_host_info(&hi_a, cred_data_len);
	hi_a.parent_uptime = 0xABBABABEFEFFFFFE;
	hi_a.child_uptime = 0xDEADBEEFAABBCCDD;
	hi_a.rerun_cnt = 0x1122334455667788;
	hi_a.bne_cnt = 0x8899AABBCCDDEEFF;
	hi_a.infect_cnt = 0xABBAABBAABBAABBA;
	hi_a.parent_pid = 0xDEADBEEF;
	hi_a.child_pid = 0xBABEBABE;
	memcpy(hi_a.prog_ver, prog_ver, sizeof(prog_ver));
	memcpy(hi_a.boot_id, "\x30\x1d\x25\x39\x90\x85\x42\xfd\x90\xb6\x20\x0b\x4a\x3b\x08\x55", 16);
	memcpy(hi_a.instance_id, "\x25\xdc\x7e\xa2\x4a\xc6\x4a\x29\x9f\xac\xbe\x18\x42\x33\xc4\x85", 16);
	memcpy(hi_a.cred, cred_data, cred_data_len);
	hi_a.arch = prne_host_arch;
	assert(prne_htbt_ser_host_info(proto_buf, PRNE_HTBT_PROTO_MIN_BUF, &proto_buf_cnt_len, &hi_a) == PRNE_HTBT_SER_RC_OK);
	assert(
		proto_buf_cnt_len == 99 + cred_data_len &&
		memcmp(proto_buf, prog_ver, 16) == 0 &&
		memcmp(
			proto_buf + 16,
			"\x30\x1d\x25\x39\x90\x85\x42\xfd\x90\xb6\x20\x0b\x4a\x3b\x08\x55"
			"\x25\xdc\x7e\xa2\x4a\xc6\x4a\x29\x9f\xac\xbe\x18\x42\x33\xc4\x85"
			"\xAB\xBA\xBA\xBE\xFE\xFF\xFF\xFE"
			"\xDE\xAD\xBE\xEF\xAA\xBB\xCC\xDD"
			"\x11\x22\x33\x44\x55\x66\x77\x88"
			"\x88\x99\xAA\xBB\xCC\xDD\xEE\xFF"
			"\xAB\xBA\xAB\xBA\xAB\xBA\xAB\xBA"
			"\xDE\xAD\xBE\xEF"
			"\xBA\xBE\xBA\xBE"
			"\x02\x01",
			82) == 0 &&
		proto_buf[16 + 82] == (uint8_t)prne_host_arch &&
		memcmp(proto_buf + 16 + 82 + 1, cred_data, cred_data_len) == 0);
	assert(prne_htbt_dser_host_info(proto_buf, proto_buf_cnt_len, &actual, &hi_b) == PRNE_HTBT_SER_RC_OK);
	assert(prne_htbt_eq_host_info(&hi_a, &hi_b));
	prne_htbt_free_host_info(&hi_a);
	prne_htbt_free_host_info(&hi_b);

	prne_htbt_init_cmd(&cmd_a);
	assert(prne_htbt_set_cmd(&cmd_a, long_args));
	assert(!prne_htbt_set_cmd(&cmd_a, too_long_args));
	assert(prne_htbt_set_cmd(&cmd_a, long_mem_args));
	assert(!prne_htbt_set_cmd(&cmd_a, too_long_mem_args));
	// empty cmd
	assert(prne_htbt_set_cmd(&cmd_a, NULL) && cmd_a.argc == 0 && cmd_a.args == NULL && cmd_a.mem == NULL && cmd_a.mem_len == 0);
	assert(prne_htbt_set_cmd(&cmd_a, empty_args) && cmd_a.argc == 0 && cmd_a.args == NULL && cmd_a.mem == NULL && cmd_a.mem_len == 0);
	assert(prne_htbt_ser_cmd(proto_buf, PRNE_HTBT_PROTO_MIN_BUF, &proto_buf_cnt_len, &cmd_a) == PRNE_HTBT_SER_RC_OK);
	assert(proto_buf_cnt_len == 2 && memcmp(proto_buf, "\x00\x00", 2) == 0);
	prne_htbt_free_cmd(&cmd_a);
	// cmd
	prne_htbt_init_cmd(&cmd_a);
	prne_htbt_init_cmd(&cmd_b);
	assert(prne_htbt_set_cmd(&cmd_a, test_args));
	assert(prne_htbt_ser_cmd(proto_buf, PRNE_HTBT_PROTO_MIN_BUF, &proto_buf_cnt_len, &cmd_a) == PRNE_HTBT_SER_RC_OK);
	assert(proto_buf_cnt_len == sizeof(test_args_mem) && memcmp(proto_buf, test_args_mem, sizeof(test_args_mem)) == 0);
	assert(prne_htbt_dser_cmd(proto_buf, proto_buf_cnt_len, &actual, &cmd_b) == PRNE_HTBT_SER_RC_OK);
	assert(prne_htbt_eq_cmd(&cmd_a, &cmd_b));
	prne_htbt_free_cmd(&cmd_a);
	prne_htbt_free_cmd(&cmd_b);

	// bin meta
	prne_htbt_init_bin_meta(&bm_a);
	prne_htbt_init_bin_meta(&bm_b);
	assert(prne_htbt_set_cmd(&bm_a.cmd, test_args));
	bm_a.bin_size = 0xBBAAEE;
	assert(prne_htbt_ser_bin_meta(proto_buf, PRNE_HTBT_PROTO_MIN_BUF, &proto_buf_cnt_len, &bm_a) == PRNE_HTBT_SER_RC_OK);
	assert(proto_buf_cnt_len == sizeof(test_args_mem) + 3 && memcmp(proto_buf, "\xBB\xAA\xEE", 3) == 0 && memcmp(proto_buf + 3, test_args_mem, sizeof(test_args_mem)) == 0);
	assert(prne_htbt_dser_bin_meta(proto_buf, proto_buf_cnt_len, &actual, &bm_b) == PRNE_HTBT_SER_RC_OK);
	assert(prne_htbt_eq_bin_meta(&bm_a, &bm_b));
	prne_htbt_free_bin_meta(&bm_a);
	prne_htbt_free_bin_meta(&bm_b);


	prne_free(cred_data);
	cred_data = NULL;
	cred_data_len = 0;
}
