CC := gcc
CXX := g++
GCCPLUGINS_DIR := $(shell $(CC) -print-file-name=plugin)
PLUGIN_FLAGS := -I$(GCCPLUGINS_DIR)/include -I$(GCCPLUGINS_DIR)/include/c-family #-Wno-unused-parameter -Wno-unused-variable #-fdump-passes
DESTDIR :=
LDFLAGS :=
PROG := cyc_complexity_plugin.so
RM := rm

CONFIG_SHELL := $(shell if [ -x "$$BASH" ]; then echo $$BASH; \
	else if [ -x /bin/bash ]; then echo /bin/bash; \
	else echo sh; fi ; fi)

PLUGINCC := $(shell $(CONFIG_SHELL) gcc-plugin.sh "$(CC)" "$(CXX)" "$(CC)")

ifeq ($(PLUGINCC),$(CC))
PLUGIN_FLAGS += -std=gnu99 -O0
else
PLUGIN_FLAGS += -std=gnu++98 -fno-rtti -Wno-narrowing -Og
endif

PLUGIN_FLAGS += -fPIC -shared -ggdb -Wall -W

all: $(PROG)

$(PROG): cyc_complexity_plugin.c
	$(PLUGINCC) $(PLUGIN_FLAGS) -o $@ $^

run: $(PROG)
	$(CC) -fplugin=$(CURDIR)/$(PROG) test.c -o test -O2 -fdump-tree-all -fdump-ipa-all

clean:
	$(RM) -f $(PROG) test test.c.* test.ltrans0.* test.wpa.* test_*.c.* test_*
