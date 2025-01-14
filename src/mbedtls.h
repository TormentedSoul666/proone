#pragma once
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <time.h>

#include <poll.h>

#include <mbedtls/ssl.h>
#include <mbedtls/x509_crt.h>
#include <mbedtls/entropy.h>
#include <pthsem.h>

#define prne_mbedtls_is_nberr(expr) \
	((expr) == MBEDTLS_ERR_SSL_WANT_READ || \
		(expr) == MBEDTLS_ERR_SSL_WANT_WRITE)


// Callback that masks `MBEDTLS_X509_BADCERT_EXPIRED`
int prne_mbedtls_x509_crt_verify_cb (
	void *param,
	mbedtls_x509_crt *crt,
	int crt_depth,
	uint32_t *flags);
int prne_mbedtls_ssl_send_cb (void *ctx, const unsigned char *buf, size_t len);
int prne_mbedtls_ssl_recv_cb (void *ctx, unsigned char *buf, size_t len);
/*
* Workaround for a bug - getrandom() blocks
*/
void prne_mbedtls_entropy_init (mbedtls_entropy_context *ctx);

/* Convenience Functions
*/

// Handles mbedtls_ssl_handshake(), mbedtls_ssl_close_notify()
bool prne_mbedtls_pth_handle (
	mbedtls_ssl_context *ssl,
	int(*mbedtls_f)(mbedtls_ssl_context*),
	const int fd,
	pth_event_t ev);

bool prne_mbedtls_verify_alp (
	const mbedtls_ssl_config *conf,
	const mbedtls_ssl_context *ctx,
	const char *alp);

void prne_mbedtls_perror (const int err, const char *s);
