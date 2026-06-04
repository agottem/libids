AM_CFLAGS = -std=c99 -I$(top_srcdir)/include

lib_LTLIBRARIES = libids.la

libids_la_SOURCES = source/clist.c \
                    source/hash.c

libids_la_LDFLAGS = -static

libidsdir = $(includedir)/ids

libids_HEADERS = include/ids/error.h     \
                 include/ids/utils.h     \
                 include/ids/clist.h     \
                 include/ids/clist_inl.h \
                 include/ids/hash.h      \
                 include/ids/hash_inl.h

TESTS = tests/test_clist \
        tests/test_hash

check_PROGRAMS = $(TESTS)

tests_test_clist_SOURCES = tests/test_clist.c
tests_test_hash_SOURCES  = tests/test_hash.c

LDADD = libids.la
