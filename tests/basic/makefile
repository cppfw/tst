include prorab.mk

$(eval $(call prorab-config, ../../config))

this_name := tests

this_srcs := $(call prorab-src-dir, .)

this_ldflags += -L $(d)../../src/out/$(c)
this_ldlibs += -ltestlord

$(eval $(prorab-build-app))

$(prorab_this_name): $(abspath $(d)../../src/out/$(c)/libtestlord$(dot_so))

define this_rules
    test:: $(prorab_this_name)
$(.RECIPEPREFIX)@myci-running-test.sh $(notdir $(abspath $(d)))
$(.RECIPEPREFIX)$(a)cp $(d)../../src/out/$(c)/*.dll $(d)out/$(c) || true
$(.RECIPEPREFIX)$(a)LD_LIBRARY_PATH=$(d)../../src/out/$(c) DYLD_LIBRARY_PATH=$$$$LD_LIBRARY_PATH $(prorab_this_name)
$(.RECIPEPREFIX)@myci-passed.sh
endef
$(eval $(this_rules))

$(eval $(call prorab-include, ../../src/makefile))
