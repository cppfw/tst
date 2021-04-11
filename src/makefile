include prorab.mk

$(eval $(call prorab-config, ../config))

this_name := tst

this_soname := $(shell cat $(d)soname.txt)

this_srcs := $(call prorab-src-dir, .)

this_ldlibs += -lclargs -lutki -lnitki -lopros

ifeq ($(os), windows)
else
    this_ldlibs += -ldl
endif

$(eval $(prorab-build-lib))
