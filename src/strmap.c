#include "strmap.h"
#include "util_ct.h"
#include "util_rt.h"

#include <string.h>
#include <errno.h>


static int strmap_cmp_f (const void *a, const void *b) {
	return strcmp(
		((const prne_strmap_tuple_t *)a)->key,
		((const prne_strmap_tuple_t *)b)->key);
}

void prne_init_strmap (prne_strmap_t *map) {
	prne_memzero(map, sizeof(prne_strmap_t));
}

void prne_free_strmap (prne_strmap_t *map) {
	if (map == NULL) {
		return;
	}
	prne_strmap_clear(map);
}

void prne_strmap_clear (prne_strmap_t *map) {
	for (size_t i = 0; i < map->size; i += 1) {
		prne_free((void*)map->tbl[i].key);
	}
	prne_free(map->tbl);

	prne_memzero(map, sizeof(prne_strmap_t));
}

const prne_strmap_tuple_t *prne_strmap_insert (
	prne_strmap_t *map,
	const char* key,
	const prne_strmap_val_t val)
{
	prne_strmap_tuple_t *ret;
	prne_strmap_tuple_t t;

	t.key = key;
	t.val = val;
	ret = (prne_strmap_tuple_t*)bsearch(
		&t,
		map->tbl,
		map->size,
		sizeof(prne_strmap_tuple_t),
		strmap_cmp_f);
	if (ret == NULL) {
		const size_t sl = strlen(key);
		void *ny_tbl;

		t.key = prne_alloc_str(sl);
		if (t.key == NULL) {
			return NULL;
		}
		memcpy((void*)t.key, key, sl + 1);
		ny_tbl = (prne_strmap_tuple_t*)prne_realloc(
			map->tbl,
			sizeof(prne_strmap_tuple_t),
			map->size + 1);
		if (ny_tbl == NULL) {
			prne_free((void*)t.key);
			return NULL;
		}
		map->tbl = (prne_strmap_tuple_t*)ny_tbl;
		map->tbl[map->size] = t;
		map->size += 1;

		qsort(map->tbl, map->size, sizeof(prne_strmap_tuple_t), strmap_cmp_f);
		ret = (prne_strmap_tuple_t*)prne_strmap_lookup(map, key);
		prne_dbgast(ret != NULL);
	}
	else {
		ret->val = t.val;
	}

	return ret;
}

void prne_strmap_erase (prne_strmap_t *map, const char* key) {
	prne_strmap_tuple_t t, *e;

	t.key = key;
	t.val = 0;
	e = (prne_strmap_tuple_t*)bsearch(
		&t,
		map->tbl,
		map->size,
		sizeof(prne_strmap_tuple_t),
		strmap_cmp_f);
	if (e == NULL) {
		return;
	}
	prne_free((void*)e->key);
	e->key = NULL;
	if (map->size == 1) {
		prne_strmap_clear(map);
	}
	else {
		void *ny;

		memmove(
			e,
			e + 1,
			sizeof(prne_strmap_tuple_t) * (map->size - 1 - (e - map->tbl)));
		map->size -= 1;
		ny = prne_realloc(map->tbl, sizeof(prne_strmap_tuple_t), map->size);
		if (ny != NULL) {
			map->tbl = (prne_strmap_tuple_t*)ny;
		}
	}
}

const prne_strmap_tuple_t *prne_strmap_lookup (
	prne_strmap_t *map,
	const char* key)
{
	prne_strmap_tuple_t t;

	t.key = key;
	t.val = 0;

	return (const prne_strmap_tuple_t*)bsearch(
		&t,
		map->tbl,
		map->size,
		sizeof(prne_strmap_tuple_t),
		strmap_cmp_f);
}
