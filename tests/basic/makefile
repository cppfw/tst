include prorab.mk

$(eval $(call prorab-config, ../../config))

this_name := tests

this_srcs := $(call prorab-src-dir, .)
this_srcs += ../harness/testees.cpp

this_ldflags += -L $(d)../../src/out/$(c)
this_ldlibs += -ltst -lutki

$(eval $(prorab-build-app))

$(prorab_this_name): $(abspath $(d)../../src/out/$(c)/libtst$(dot_so))

define this_rules
    test:: $(prorab_this_name)
$(.RECIPEPREFIX)@myci-running-test.sh $(notdir $(abspath $(d)))
$(.RECIPEPREFIX)$(a)cp $(d)../../src/out/$(c)/*.dll $(d)out/$(c) || true
$(.RECIPEPREFIX)$(a)LD_LIBRARY_PATH=$(d)../../src/out/$(c) DYLD_LIBRARY_PATH=$$$$LD_LIBRARY_PATH $(prorab_this_name)
endef
$(eval $(this_rules))

$(eval $(call prorab-include, ../../src/makefile))