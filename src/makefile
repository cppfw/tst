include prorab.mk
include prorab-license.mk
include prorab-clang-format.mk

$(eval $(call prorab-config, ../config))

this_name := tst

this_soname := $(shell cat $(d)soname.txt)

this_srcs := $(call prorab-src-dir, .)

this_ldlibs += -lclargs -lutki

ifeq ($(this_tst_no_par),true)
    this_cxxflags += -D TST_NO_PAR
    this_srcs := $(filter-out tst/runner.cpp tst/runners_pool.cpp,$(this_srcs))
else
    this_ldlibs += -lnitki -lopros
endif

$(eval $(prorab-build-lib))

$(eval $(prorab-clang-format))

this_src_dir := 
this_license_file := ../LICENSE
$(eval $(prorab-license))
