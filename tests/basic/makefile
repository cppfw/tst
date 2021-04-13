include prorab.mk
include prorab-test.mk

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

this_test_deps := $(prorab_this_name)
this_test_ld_path := ../../src/out/$(c)

this_test_cmd := $(this_out_dir)$(this_name) --jobs=$(prorab_nproc) --junit-out=junit.xml
$(eval $(prorab-test))

this_test_cmd := cat run_list.txt | $(this_out_dir)$(this_name) --junit-out=junit_run_list.xml --skipped --passed
$(eval $(prorab-test))

$(eval $(call prorab-include, ../../src/makefile))
