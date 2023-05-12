include prorab.mk
include prorab-test.mk

$(eval $(call prorab-config, ../../config))

this_no_install := true

this_name := tests

this_srcs := $(call prorab-src-dir, .)
this_srcs += ../harness/testees.cpp

ifeq ($(os), windows)
else
    this_ldflags += -rdynamic
endif

this_ldflags += -L $(d)../../src/out/$(c)

ifeq ($(os),linux)
    this_ldlibs += -lpthread
endif

this_ldlibs += -ltst -lutki

$(eval $(prorab-build-app))

$(prorab_this_name): $(abspath $(d)../../src/out/$(c)/libtst$(dot_so))

this_test_deps := $(prorab_this_name)
this_test_ld_path := ../../src/out/$(c)

# when running the test from msys2 it cannot detect that stdin is not piped, so we need to pipe empty string
# to it to avoid it hanging waiting for run list from stdin
this_test_cmd := $(prorab_this_name) --jobs=auto --about-to-run --junit-out=out/$(c)/junit.xml
$(eval $(prorab-test))

this_test_cmd := cat run_list.txt | $(prorab_this_name) --junit-out=out/$(c)/junit_run_list.xml --skipped --passed --outcome --run-list-stdin
$(eval $(prorab-test))

# run one suite
this_test_cmd := cat run_list.txt | $(prorab_this_name) --skipped --passed --outcome --run-list-stdin --suite=check_pointers
$(eval $(prorab-test))

# run one test
this_test_cmd := cat run_list.txt | $(prorab_this_name) --skipped --passed --outcome --run-list-stdin --suite=check_pointers --test=check_is_possible_for_simple_pointer
$(eval $(prorab-test))

$(eval $(call prorab-include, ../../src/makefile))
