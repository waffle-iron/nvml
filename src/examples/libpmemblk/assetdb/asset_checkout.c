/*
 * Copyright 2014-2016, Intel Corporation
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
 * asset_checkout -- mark an asset as checked out to someone
 *
 * Usage:
 *	asset_checkin /path/to/pm-aware/file asset-ID name
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include <assert.h>
#include <libpmemblk.h>

#include "asset.h"

int
main(int argc, char *argv[])
{
	PMEMblkpool *pbp;
	struct asset asset;
	int assetid;

	if (argc < 4) {
		fprintf(stderr, "usage: %s assetdb asset-ID name\n", argv[0]);
		exit(1);
	}

	const char *path = argv[1];
	assetid = atoi(argv[2]);
	assert(assetid > 0);

	/* open an array of atomically writable elements */
	if ((pbp = pmemblk_open(path, sizeof (struct asset))) == NULL) {
		perror("pmemblk_open");
		exit(1);
	}

	/* read a required element in */
	if (pmemblk_read(pbp, &asset, assetid) < 0) {
		perror("pmemblk_read");
		exit(1);
	}

	/* check if it contains any data */
	if ((asset.state != ASSET_FREE) &&
		(asset.state != ASSET_CHECKED_OUT)) {
		fprintf(stderr, "Asset ID %d not found", assetid);
		exit(1);
	}

	if (asset.state == ASSET_CHECKED_OUT) {
		fprintf(stderr, "Asset ID %d already checked out\n", assetid);
		exit(1);
	}

	/* update user name, set checked out state, and take timestamp */
	strncpy(asset.user, argv[3], ASSET_USER_NAME_MAX - 1);
	asset.user[ASSET_USER_NAME_MAX - 1] = '\0';
	asset.state = ASSET_CHECKED_OUT;
	time(&asset.time);

	/* put it back in the block */
	if (pmemblk_write(pbp, &asset, assetid) < 0) {
		perror("pmemblk_write");
		exit(1);
	}

	pmemblk_close(pbp);
}
