include prorab.mk
include prorab-test.mk

$(eval $(call prorab-config, ../../config))

this_no_install := true

this_name := tests

this_srcs := $(call prorab-src-dir, .)
this_srcs += ../harness/testees.cpp

ifeq ($(os), windows)
    this_ldlibs += -l mingw32
else
    this_ldflags += -rdynamic
endif

ifeq ($(os),linux)
    # in case of static linking -pthread option is needed
    this_ldflags += -pthread
endif

this_ldlibs += -l utki$(this_dbg)
this_ldlibs += -l clargs$(this_dbg)
this_ldlibs += -l nitki$(this_dbg)
this_ldlibs += -l opros$(this_dbg)

this_ldlibs += ../../src/out/$(c)/libtst$(this_dbg)$(dot_so)

$(eval $(prorab-build-app))

this_test_deps := $(prorab_this_name)
this_test_ld_path := ../../src/out/$(c)

# when running the test from msys2 it cannot detect that stdin is not piped, so we need to pipe empty string
# to it to avoid it hanging waiting for run list from stdin
this_test_cmd := echo "" | $(prorab_this_name) --jobs=$(prorab_nproc) --junit-out=out/$(c)/junit.xml || true && myci-warning.sh "NOT A REAL FAILURES! Just testing how test cases fail."
$(eval $(prorab-test))

$(eval $(call prorab-include, ../../src/makefile))
