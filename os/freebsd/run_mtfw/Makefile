# $FreeBSD$

KMOD=	run_mtfw
FIRMWS=	run_mt.fw:run_mtfw:1

CLEANFILES=	run_mt.fw

run_mt.fw: ${.CURDIR}/mt7610.fw.uu
	uudecode -p $? > ${.TARGET}

.include <bsd.kmod.mk>
