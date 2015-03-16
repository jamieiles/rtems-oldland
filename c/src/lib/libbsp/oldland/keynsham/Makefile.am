ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am

include_bspdir = $(includedir)/bsp

dist_project_lib_DATA = bsp_specs

include_HEADERS = include/bsp.h
include_HEADERS += ../../shared/include/tm27.h

nodist_include_HEADERS = include/bspopts.h
nodist_include_bsp_HEADERS = ../../shared/include/bootcard.h
nodist_include_bsp_HEADERS += ../../shared/include/console-polled.h
include_bsp_HEADERS = ../../shared/include/irq-generic.h \
		      include/irq.h \
		      clock/timer_defines.h
DISTCLEANFILES = include/bspopts.h

nodist_include_HEADERS += ../../shared/include/coverhd.h

noinst_LIBRARIES = libbspstart.a
libbspstart_a_SOURCES = start/start.S
project_lib_DATA = start.$(OBJEXT)

dist_project_lib_DATA += startup/linkcmds

noinst_LIBRARIES += libbsp.a
libbsp_a_SOURCES =

libbsp_a_SOURCES += ../../../libcpu/oldland/shared/include/isr_entries.h

# startup
libbsp_a_SOURCES += ../../shared/bspclean.c ../../shared/bsplibc.c \
    ../../shared/bsppredriverhook.c ../../shared/bsppretaskinghook.c \
    ../../shared/bspgetworkarea.c ../../shared/bsppost.c \
    ../../shared/bootcard.c ../../shared/sbrk.c \
    ../../shared/gnatinstallhandler.c
# clock
libbsp_a_SOURCES += clock/clockdrv.c ../../../shared/clockdrv_shell.h
# console
libbsp_a_SOURCES += ../../shared/console-polled.c console/console-io.c
# timer
libbsp_a_SOURCES += ../../shared/timerstub.c

libbsp_a_SOURCES += irq/irq.c
libbsp_a_SOURCES += startup/bspstart.c

# Cache
libbsp_a_SOURCES += ../../../libcpu/shared/src/cache_manager.c
libbsp_a_SOURCES += ../../shared/include/cache_.h
libbsp_a_CPPFLAGS = -I$(srcdir)/../../shared/include

libbsp_a_SOURCES += ../../shared/src/irq-generic.c

EXTRA_DIST = times

include $(srcdir)/preinstall.am
include $(top_srcdir)/../../../../automake/local.am
