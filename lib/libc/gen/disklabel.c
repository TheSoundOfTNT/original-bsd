/*
 * Copyright (c) 1983, 1987, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * %sccs.include.redist.c%
 */

#ifndef lint
static char sccsid[] = "@(#)disklabel.c	8.2 (Berkeley) 05/03/95";
#endif /* not lint */

#include <sys/param.h>
#define DKTYPENAMES
#include <sys/disklabel.h>
#include <ufs/ufs/dinode.h>
#include <ufs/ffs/fs.h>

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static int	error __P((int));
static int	gettype __P((char *, char **));

struct disklabel *
getdiskbyname(name)
	const char *name;
{
	static struct	disklabel disk;
	register struct	disklabel *dp = &disk;
	register struct partition *pp;
	char	*buf;
	char  	*db_array[2] = { _PATH_DISKTAB, 0 };
	char	*cp, *cq;	/* can't be register */
	char	p, max, psize[3], pbsize[3],
		pfsize[3], poffset[3], ptype[3];
	u_int32_t *dx;

	if (cgetent(&buf, db_array, (char *) name) < 0)
		return NULL;

	bzero((char *)&disk, sizeof(disk));
	/*
	 * typename
	 */
	cq = dp->d_typename;
	cp = buf;
	while (cq < dp->d_typename + sizeof(dp->d_typename) - 1 &&
	    (*cq = *cp) && *cq != '|' && *cq != ':')
		cq++, cp++;
	*cq = '\0';
	/*
	 * boot name (optional)  xxboot, bootxx
	 */
	cgetstr(buf, "b0", &dp->d_boot0);
	cgetstr(buf, "b1", &dp->d_boot1);

	if (cgetstr(buf, "ty", &cq) > 0 && strcmp(cq, "removable") == 0)
		dp->d_flags |= D_REMOVABLE;
	else  if (cq && strcmp(cq, "simulated") == 0)
		dp->d_flags |= D_RAMDISK;
	if (cgetcap(buf, "sf", ':') != NULL)
		dp->d_flags |= D_BADSECT;

#define getnumdflt(field, dname, dflt) \
        { long f; (field) = (cgetnum(buf, dname, &f) == -1) ? (dflt) : f; }

	getnumdflt(dp->d_secsize, "se", DEV_BSIZE);
	cgetnum(buf, "nt",(long *) &dp->d_ntracks);
	cgetnum(buf, "ns",(long *) &dp->d_nsectors);
	cgetnum(buf, "nc",(long *) &dp->d_ncylinders);

	if (cgetstr(buf, "dt", &cq) > 0)
		dp->d_type = gettype(cq, dktypenames);
	else
		getnumdflt(dp->d_type, "dt", 0);
	getnumdflt(dp->d_secpercyl, "sc", dp->d_nsectors * dp->d_ntracks);
	getnumdflt(dp->d_secperunit, "su", dp->d_secpercyl * dp->d_ncylinders);
	getnumdflt(dp->d_rpm, "rm", 3600);
	getnumdflt(dp->d_interleave, "il", 1);
	getnumdflt(dp->d_trackskew, "sk", 0);
	getnumdflt(dp->d_cylskew, "cs", 0);
	getnumdflt(dp->d_headswitch, "hs", 0);
	getnumdflt(dp->d_trkseek, "ts", 0);
	getnumdflt(dp->d_bbsize, "bs", BBSIZE);
	getnumdflt(dp->d_sbsize, "sb", SBSIZE);
	strcpy(psize, "px");
	strcpy(pbsize, "bx");
	strcpy(pfsize, "fx");
	strcpy(poffset, "ox");
	strcpy(ptype, "tx");
	max = 'a' - 1;
	pp = &dp->d_partitions[0];
	for (p = 'a'; p < 'a' + MAXPARTITIONS; p++, pp++) {
		psize[1] = pbsize[1] = pfsize[1] = poffset[1] = ptype[1] = p;
		if (cgetnum(buf, psize,(long *) &pp->p_size) == -1)
			pp->p_size = 0;
		else {
			cgetnum(buf, poffset, (long *) &pp->p_offset);
			getnumdflt(pp->p_fsize, pfsize, 0);
			if (pp->p_fsize) {
				long bsize;

				if (cgetnum(buf, pbsize, &bsize) == 0)
					pp->p_frag = bsize / pp->p_fsize;
				else
					pp->p_frag = 8;
			}
			getnumdflt(pp->p_fstype, ptype, 0);
			if (pp->p_fstype == 0 && cgetstr(buf, ptype, &cq) > 0)
				pp->p_fstype = gettype(cq, fstypenames);
			max = p;
		}
	}
	dp->d_npartitions = max + 1 - 'a';
	(void)strcpy(psize, "dx");
	dx = dp->d_drivedata;
	for (p = '0'; p < '0' + NDDATA; p++, dx++) {
		psize[1] = p;
		getnumdflt(*dx, psize, 0);
	}
	dp->d_magic = DISKMAGIC;
	dp->d_magic2 = DISKMAGIC;
	free(buf);
	return (dp);
}

static int
gettype(t, names)
	char *t;
	char **names;
{
	register char **nm;

	for (nm = names; *nm; nm++)
		if (strcasecmp(t, *nm) == 0)
			return (nm - names);
	if (isdigit(*t))
		return (atoi(t));
	return (0);
}

static int
error(err)
	int err;
{
	char *p;

	(void)write(STDERR_FILENO, "disktab: ", 9);
	(void)write(STDERR_FILENO, _PATH_DISKTAB, sizeof(_PATH_DISKTAB) - 1);
	(void)write(STDERR_FILENO, ": ", 2);
	p = strerror(err);
	(void)write(STDERR_FILENO, p, strlen(p));
	(void)write(STDERR_FILENO, "\n", 1);
}
