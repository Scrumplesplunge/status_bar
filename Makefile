include config.mk
include rules.mk

BINARIES =  \
	status_bar
all: $(patsubst %, bin/%, ${BINARIES})

INSTALL_DEPS =  \
	bin/status_bar
install: all
	cp ${INSTALL_DEPS} ${INSTALL_PATH}

STATUS_BAR_DEPS =  \
	executor  \
	set_status  \
	uptime  \
	volume  \
	status_bar
bin/status_bar: $(patsubst %, obj/%.o, ${STATUS_BAR_DEPS})
