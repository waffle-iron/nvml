/*
 * Copyright 2015-2016, Intel Corporation
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in
 *       the documentation and/or other materials provided with the
 *       distribution.
 *
 *     * Neither the name of the copyright holder nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * btree_map.h -- TreeMap sorted collection implementation
 */

#ifndef	BTREE_MAP_H
#define BTREE_MAP_H

#include <libpmemobj.h>

#ifndef	BTREE_MAP_TYPE_OFFSET
#define BTREE_MAP_TYPE_OFFSET 1012
#endif

struct btree_map;
TOID_DECLARE(struct btree_map, BTREE_MAP_TYPE_OFFSET + 0);

int btree_map_check(PMEMobjpool *pop, TOID(struct btree_map) map);
int btree_map_new(PMEMobjpool *pop, TOID(struct btree_map) *map, void *arg);
int btree_map_delete(PMEMobjpool *pop, TOID(struct btree_map) *map);
int btree_map_insert(PMEMobjpool *pop, TOID(struct btree_map) map,
	uint64_t key, PMEMoid value);
int btree_map_insert_new(PMEMobjpool *pop, TOID(struct btree_map) map,
		uint64_t key, size_t size, unsigned int type_num,
		void (*constructor)(PMEMobjpool *pop, void *ptr, void *arg),
		void *arg);
PMEMoid btree_map_remove(PMEMobjpool *pop, TOID(struct btree_map) map,
		uint64_t key);
int btree_map_remove_free(PMEMobjpool *pop, TOID(struct btree_map) map,
		uint64_t key);
int btree_map_clear(PMEMobjpool *pop, TOID(struct btree_map) map);
PMEMoid btree_map_get(PMEMobjpool *pop, TOID(struct btree_map) map,
		uint64_t key);
int btree_map_lookup(PMEMobjpool *pop, TOID(struct btree_map) map,
		uint64_t key);
int btree_map_foreach(PMEMobjpool *pop, TOID(struct btree_map) map,
	int (*cb)(uint64_t key, PMEMoid value, void *arg), void *arg);
int btree_map_is_empty(PMEMobjpool *pop, TOID(struct btree_map) map);

#endif /* BTREE_MAP_H */
