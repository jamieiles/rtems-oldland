ACLOCAL_AMFLAGS = -I ../../../aclocal

include $(top_srcdir)/../../../automake/compile.am

CLEANFILES =
DISTCLEANFILES =
noinst_PROGRAMS =

include_libcpudir = $(includedir)/libcpu

include_libcpu_HEADERS = shared/interrupts/isr_entries.h

include $(srcdir)/preinstall.am

include $(top_srcdir)/../../../automake/local.am
