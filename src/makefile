include prorab.mk
include myci.mk

$(eval $(call myci-try-include, $(CONANBUILDINFO_DIR)conanbuildinfo.mak))

$(eval $(call prorab-config, ../config))

this_name := tst

this_soname := $(shell cat $(d)soname.txt)

this_srcs := $(call prorab-src-dir, .)

# this_dot_hxx := .hxx
# this_hdrs := $(call prorab-hdr-dir, .)

this_cxxflags += $(addprefix -I,$(CONAN_INCLUDE_DIRS))
this_cxxflags_test += $(addprefix -I,$(CONAN_INCLUDE_DIRS))

this_ldflags += $(addprefix -L,$(CONAN_LIB_DIRS))

this_ldlibs += -lclargs -lutki -lnitki -lopros

ifeq ($(os), windows)
else
    this_ldlibs += -ldl
endif

$(eval $(prorab-build-lib))
