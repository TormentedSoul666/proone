#include <errno.h>

#include "util_rt.h"
#include "pth.h"


void prne_init_worker (prne_worker_t *w) {
	prne_memzero(w, sizeof(prne_worker_t));
}

void prne_free_worker (prne_worker_t *w) {
	if (w == NULL) {
		return;
	}

	if (w->ctx != NULL) {
		prne_assert(w->free_ctx != NULL);
		w->free_ctx(w->ctx);
	}
	if (w->pth != NULL) {
		pth_abort(w->pth);
	}
	pth_attr_destroy(w->attr);

	prne_memzero(w, sizeof(prne_worker_t));
}

void prne_fin_worker (prne_worker_t *w) {
	if (w->fin != NULL) {
		w->fin(w->ctx);
	}
}

int prne_pth_poll (
	struct pollfd *pfd,
	const nfds_t nfs,
	const int timeout,
	pth_event_t ev)
{
	struct pollfd my_pfd[nfs];
	nfds_t p;
	int ret;

	p = 0;
	for (nfds_t i = 0; i < nfs; i += 1) {
		if (pfd[i].fd >= FD_SETSIZE) {
			errno = EINVAL;
			return -1;
		}
		if (0 <= pfd[i].fd) {
			my_pfd[p].fd = pfd[i].fd;
			my_pfd[p].events = pfd[i].events;
			p += 1;
		}
	}
	if (p == 0) {
		return 0;
	}

	ret = pth_poll_ev(my_pfd, p, timeout, ev);
	if (ret >= 0) {
		p = 0;
		for (nfds_t i = 0; i < nfs; i += 1) {
			if (0 <= pfd[i].fd) {
				pfd[i].revents = my_pfd[p].revents;
				p += 1;
			}
			else {
				pfd[i].revents = 0;
			}
		}
	}

	return ret;
}

void prne_pth_cv_notify (pth_mutex_t *lock, pth_cond_t *cond, bool broadcast) {
	prne_dbgtrap(pth_mutex_acquire(lock, FALSE, NULL));
	prne_dbgtrap(pth_cond_notify(cond, broadcast));
	pth_mutex_release(lock);
}

pth_time_t prne_pth_tstimeout (const struct timespec ts) {
	return pth_timeout(ts.tv_sec, ts.tv_nsec / 1000);
}

void prne_pth_reset_timer (pth_event_t *ev, const struct timespec *ts) {
	pth_event_free(*ev, FALSE);
	if (ts != NULL) {
		*ev = pth_event(
			PTH_EVENT_TIME,
			prne_pth_tstimeout(*ts));
		prne_assert(*ev != NULL);
	}
}
