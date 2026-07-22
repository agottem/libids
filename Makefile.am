AM_CFLAGS = -std=c11 -I$(top_srcdir)/include

lib_LTLIBRARIES = libids.la

libids_la_SOURCES = source/arr.c       \
                    source/clist.c     \
                    source/hash.c      \
                    source/spsc_ring.c

libids_la_LDFLAGS = -static

libidsdir = $(includedir)/ids

libids_HEADERS = include/ids/err.h           \
                 include/ids/sys.h           \
                 include/ids/alg.h           \
                 include/ids/cont_of.h       \
                 include/ids/arr.h           \
                 include/ids/arr_inl.h       \
                 include/ids/clist.h         \
                 include/ids/clist_inl.h     \
                 include/ids/hash.h          \
                 include/ids/hash_inl.h      \
                 include/ids/spsc_ring.h     \
                 include/ids/spsc_ring_inl.h

TESTS = tests/test_arr       \
        tests/test_clist     \
        tests/test_hash      \
        tests/test_spsc_ring

check_PROGRAMS = $(TESTS)

tests_test_arr_SOURCES       = tests/test_arr.c
tests_test_clist_SOURCES     = tests/test_clist.c
tests_test_hash_SOURCES      = tests/test_hash.c
tests_test_spsc_ring_SOURCES = tests/test_spsc_ring.c

LDADD = libids.la
