include config.mk
include rules.mk

BINARIES =  \
	status_bar
all: $(patsubst %, bin/%, ${BINARIES})

install: all
	cp bin/status_bar ${INSTALL_PATH}
	chmod +x ${INSTALL_PATH}/status_bar
	cp status_bar.cfg /etc/status_bar.cfg
	chmod +r /etc/status_bar.cfg

STATUS_BAR_DEPS =  \
	buffer  \
	config  \
	cpu  \
	executor  \
	set_status  \
	uptime  \
	volume  \
	wall_time  \
	status_bar
bin/status_bar: $(patsubst %, obj/%.o, ${STATUS_BAR_DEPS})
