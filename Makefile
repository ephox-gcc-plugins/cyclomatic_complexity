CC := gcc
CXX := g++
GCCPLUGINS_DIR := $(shell $(CC) -print-file-name=plugin)
PLUGIN_FLAGS += -I$(GCCPLUGINS_DIR)/include -I$(GCCPLUGINS_DIR)/include/c-family -fPIC -shared -O2 -ggdb -Wall -W
DESTDIR :=
LDFLAGS :=
PROG := cyc_complexity_plugin.so
RM := rm

CONFIG_SHELL := $(shell if [ -x "$$BASH" ]; then echo $$BASH; \
	else if [ -x /bin/bash ]; then echo /bin/bash; \
	else echo sh; fi ; fi)

PLUGINCC := $(shell $(CONFIG_SHELL) gcc-plugin.sh "$(CC)" "$(CXX)" "$(CC)")

ifeq ($(PLUGINCC),$(CC))
PLUGIN_FLAGS := -I$(GCCPLUGINS_DIR)/include -std=gnu99
else
PLUGIN_FLAGS := -I$(GCCPLUGINS_DIR)/include -std=gnu++98 -fno-rtti -Wno-narrowing -Wno-unused-parameter
endif

PLUGIN_FLAGS += -fPIC -shared -O2 -ggdb -Wall -W

all: $(PROG)

$(PROG): cyc_complexity_plugin.c
	$(PLUGINCC) $(PLUGIN_FLAGS) -o $@ $^

run: $(PROG)
	$(CC) -fplugin=$(CURDIR)/$(PROG) test.c -o test -O2

clean:
	$(RM) -f $(PROG) test
