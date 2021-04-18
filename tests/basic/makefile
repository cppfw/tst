include prorab.mk
include prorab-test.mk
include myci.mk

$(eval $(call myci-try-include, $(CONANBUILDINFO_DIR)conanbuildinfo.mak))

$(eval $(call prorab-config, ../../config))

this_name := tests

this_srcs := $(call prorab-src-dir, .)
this_srcs += ../harness/testees.cpp

ifeq ($(os), windows)
else
    this_ldflags += -rdynamic
endif

this_cxxflags += $(addprefix -I,$(CONAN_INCLUDE_DIRS))
this_ldflags += $(addprefix -L,$(CONAN_LIB_DIRS))

this_ldflags += -L $(d)../../src/out/$(c)
this_ldlibs += -ltst -lutki

$(eval $(prorab-build-app))

$(prorab_this_name): $(abspath $(d)../../src/out/$(c)/libtst$(dot_so))

this_test_deps := $(prorab_this_name)
this_test_ld_path := ../../src/out/$(c) $(CONAN_LIB_DIRS)

# when running the test from msys2 it cannot detect that stdin is not piped, so we need to pipe empty string
# to it to avoid it hanging waiting for run list from stdin
this_test_cmd := echo "" | $(prorab_this_name) --jobs=$(prorab_nproc) --junit-out=junit.xml
$(eval $(prorab-test))

this_test_cmd := cat run_list.txt | $(prorab_this_name) --junit-out=junit_run_list.xml --skipped --passed --outcome
$(eval $(prorab-test))

$(eval $(call prorab-include, ../../src/makefile))
