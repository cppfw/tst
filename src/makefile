include prorab.mk
include prorab-license.mk
include prorab-clang-format.mk
include prorab-install-dbgsrc.mk

$(eval $(call prorab-config, ../config))

this_src_dir := tst

this_name := $(this_src_dir)$(this_dbg)

this_soname := $(shell cat $(d)soname.txt)

this_srcs := $(call prorab-src-dir, $(this_src_dir))

this_ldlibs += -l clargs$(this_dbg)
this_ldlibs += -l utki$(this_dbg)

ifeq ($(this__tst_no_par),true)
    this_cxxflags += -D TST_NO_PAR
    this_srcs := $(filter-out tst/runner.cpp tst/runners_pool.cpp,$(this_srcs))
else
    this_ldlibs += -l nitki$(this_dbg)
    this_ldlibs += -l opros$(this_dbg)
endif

$(eval $(prorab-build-lib))

$(eval $(prorab-clang-format))

this_license_file := ../LICENSE
$(eval $(prorab-license))

$(eval $(prorab-install-dbgsrc))
