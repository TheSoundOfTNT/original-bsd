PUSHDIVERT(-1)
#
# Copyright (c) 1983 Eric P. Allman
# Copyright (c) 1988 The Regents of the University of California.
# All rights reserved.
#
# %sccs.include.redist.sh%
#

ifdef(`USENET_MAILER_PATH',, `define(`USENET_MAILER_PATH', /usr/lib/news/inews)')
ifdef(`USENET_MAILER_FLAGS',, `define(`USENET_MAILER_FLAGS', `rlsDFMmn')')
ifdef(`USENET_MAILER_ARGS',, `define(`USENET_MAILER_ARGS', `-m -h -n')')
POPDIVERT
####################################
###  USENET Mailer specification ###
####################################

VERSIONID(`@(#)usenet.m4	6.2 (Berkeley) 02/20/93')

Musenet,	P=USENET_MAILER_PATH, F=USENET_MAILER_FLAGS, S=10, R=20,
		A=inews USENET_MAILER_ARGS $u
