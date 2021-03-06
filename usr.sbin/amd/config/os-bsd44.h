/*
 * Copyright (c) 1990 Jan-Simon Pendry
 * Copyright (c) 1990 Imperial College of Science, Technology & Medicine
 * Copyright (c) 1990, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Jan-Simon Pendry at Imperial College, London.
 *
 * %sccs.include.redist.c%
 *
 *	@(#)os-bsd44.h	8.2 (Berkeley) 05/10/95
 *
 * $Id: os-bsd44.h,v 5.2.2.1 1992/02/09 15:10:11 jsp beta $
 *
 * 4.4 BSD definitions for Amd (automounter)
 */

/*
 * Does the compiler grok void *
 */
#define	VOIDP

/*
 * Which version of the Sun RPC library we are using
 * This is the implementation release number, not
 * the protocol revision number.
 */
#define	RPC_4

#include <sys/param.h>
#if BSD >= 199506
#define NFS_HDR "misc-bsd44l.h"
#define UFS_HDR "misc-bsd44l.h"
#endif

/*
 * Which version of the NFS interface are we using.
 * This is the implementation release number, not
 * the protocol revision number.
 */
#define	NFS_44
#define HAS_TCP_NFS

/*
 * Does this OS have NDBM support?
 */
#define OS_HAS_NDBM

/*
 * 4.4 doesn't provide NIS.
 */
#undef HAS_NIS_MAPS

/*
 * OS provides strerror()
 */
#define HAS_STRERROR

/*
 * The mount table is obtained from the kernel
 */
#undef	UPDATE_MTAB

/*
 * No mntent info on 4.4 BSD
 */
#undef	MNTENT_HDR

/*
 * Name of filesystem types
 */
#define	MOUNT_TYPE_NFS	MOUNT_NFS
#define	MOUNT_TYPE_UFS	MOUNT_UFS
#undef MTAB_TYPE_UFS
#define	MTAB_TYPE_UFS	"ufs"
#define	MTAB_TYPE_MFS	"mfs"

/*
 * How to unmount filesystems
 */
#undef UNMOUNT_TRAP
#undef	NEED_UMOUNT_FS
#define	NEED_UMOUNT_BSD

/*
 * How to copy an address into an NFS filehandle
 */
#undef NFS_SA_DREF
#if BSD >= 199506
#define	NFS_SA_DREF(dst, src) { \
		(dst).addr = (struct sockaddr *) (src); \
		(dst).addrlen = sizeof(*src); \
		(dst).sotype = SOCK_DGRAM; \
		(dst).proto = 0; \
		(dst).fhsize = FHSIZE; \
		(dst).wsize = NFS_WSIZE; \
		(dst).rsize =  NFS_RSIZE; \
		(dst).readdirsize =  NFS_READDIRSIZE; \
		(dst).timeo = 10; \
		(dst).retrans = NFS_RETRANS; \
		(dst).maxgrouplist = NFS_MAXGRPS; \
		(dst).readahead = NFS_DEFRAHEAD; \
		(dst).leaseterm = 0; \
		(dst).deadthresh = 0; \
	}
#else
#define	NFS_SA_DREF(dst, src) { \
		(dst).addr = (struct sockaddr *) (src); \
		(dst).addrlen = sizeof(*src); \
		(dst).sotype = SOCK_DGRAM; \
		(dst).proto = 0; \
	}
#endif

/*
 * Byte ordering
 */
#ifndef BYTE_ORDER
#include <machine/endian.h>
#endif /* BYTE_ORDER */

#undef ARCH_ENDIAN
#if BYTE_ORDER == LITTLE_ENDIAN
#define ARCH_ENDIAN "little"
#else
#if BYTE_ORDER == BIG_ENDIAN
#define ARCH_ENDIAN "big"
#else
XXX - Probably no hope of running Amd on this machine!
#endif /* BIG */
#endif /* LITTLE */

/*
 * Miscellaneous 4.4 BSD bits
 */
#define	NEED_MNTOPT_PARSER
#define	SHORT_MOUNT_NAME

#define	MNTMAXSTR       128

#define	MNTTYPE_UFS	"ufs"		/* Un*x file system */
#define	MNTTYPE_NFS	"nfs"		/* network file system */
#define	MNTTYPE_MFS	"mfs"		/* memory file system */
#define	MNTTYPE_IGNORE	"ignore"	/* No type specified, ignore this entry */

#define	M_RDONLY	MNT_RDONLY
#define	M_SYNC		MNT_SYNCHRONOUS
#define	M_NOEXEC	MNT_NOEXEC
#define	M_NOSUID	MNT_NOSUID
#define	M_NODEV		MNT_NODEV

#define	MNTOPT_SOFT	"soft"		/* soft mount */
#define	MNTOPT_INTR	"intr"		/* interrupts allowed */

#define NFSMNT_HOSTNAME	0		/* hostname on 4.4 is not optional */

struct mntent {
	char	*mnt_fsname;	/* name of mounted file system */
	char	*mnt_dir;	/* file system path prefix */
	char	*mnt_type;	/* MNTTYPE_* */
	char	*mnt_opts;	/* MNTOPT* */
	int	mnt_freq;	/* dump frequency, in days */
	int	mnt_passno;	/* pass number on parallel fsck */
};

/*
 * Type of a file handle
 */
#undef NFS_FH_TYPE
#if BSD >= 199506
#define NFS_FH_TYPE	void *
#else
#define	NFS_FH_TYPE	nfsv2fh_t *
#endif

/*
 * How to get a mount list
 */
#undef	READ_MTAB_FROM_FILE
#define	READ_MTAB_BSD_STYLE

/*
 * The data for the mount syscall needs the path in addition to the
 * host name since that is the only source of information about the
 * mounted filesystem.
 */
#define	NFS_ARGS_NEEDS_PATH

/*
 * 4.4 has RE support built in
 */
#undef RE_HDR
#define RE_HDR <regexp.h>

#if BSD >= 199506
#undef MTYPE_TYPE
#define MTYPE_TYPE	char *
#define MOUNT_NFS "nfs"
#define MOUNT_UFS "ffs"
#define MOUNT_MFS "mfs"
#endif
