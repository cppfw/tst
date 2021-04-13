include prorab.mk

$(eval $(call prorab-config, ../../config))

this_name := tests

this_srcs := $(call prorab-src-dir, .)
this_srcs += ../harness/testees.cpp

ifeq ($(os), windows)
else
    this_ldflags += -rdynamic
endif

this_ldflags += -L $(d)../../src/out/$(c)
this_ldlibs += -ltst -lutki

$(eval $(prorab-build-app))

$(prorab_this_name): $(abspath $(d)../../src/out/$(c)/libtst$(dot_so))

include prorab-test.mk

this_test_deps := $(prorab_this_name)
this_test_ld_path := ../../src/out/$(c)

this_test_cmd := $(prorab_this_name) --jobs=$(prorab_nproc) --junit-out=junit.xml || true && myci-warning.sh "NOT A REAL FAILURE! Just testing how test cases fail."
$(eval $(prorab-test))

$(eval $(call prorab-include, ../../src/makefile))
