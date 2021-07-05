include prorab.mk
include myci.mk

$(eval $(call myci-try-include, $(CONANBUILDINFO_DIR)conanbuildinfo.mak))

$(eval $(call prorab-config, ../config))

this_name := tst

this_soname := $(shell cat $(d)soname.txt)

this_srcs := $(call prorab-src-dir, .)

this_cxxflags += $(addprefix -I,$(CONAN_INCLUDE_DIRS))
this_cxxflags_test += $(addprefix -I,$(CONAN_INCLUDE_DIRS))

this_ldflags += $(addprefix -L,$(CONAN_LIB_DIRS))

this_ldlibs += -lclargs -lutki

ifeq ($(this_tst_no_par),true)
    this_cxxflags += -D TST_NO_PAR
    this_srcs := $(filter-out tst/runner.cpp tst/runners_pool.cpp,$(this_srcs))
else
    this_ldlibs += -lnitki -lopros
endif

$(eval $(prorab-build-lib))
