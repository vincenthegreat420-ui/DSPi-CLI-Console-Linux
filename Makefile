CC ?= cc
PKG_CONFIG ?= pkg-config

PREFIX ?= /usr/local
BINDIR ?= $(PREFIX)/bin
UDEVDIR ?= /etc/udev/rules.d

CFLAGS ?= -O2 -g
CPPFLAGS += -D_DEFAULT_SOURCE
CFLAGS += -std=c11 -Wall -Wextra -Wpedantic
LDLIBS += $(shell $(PKG_CONFIG) --libs libusb-1.0)
CFLAGS += $(shell $(PKG_CONFIG) --cflags libusb-1.0)

SRC := src/dspi-cli.c
BIN := dspi-cli

.PHONY: all clean install install-udev uninstall

all: $(BIN)

$(BIN): $(SRC) check-libusb
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $< $(LDLIBS)

check-libusb:
	@$(PKG_CONFIG) --exists libusb-1.0 || { \
		echo "libusb-1.0 development files were not found."; \
		echo "Install libusb headers first, for example:"; \
		echo "  Debian/Ubuntu: sudo apt install build-essential pkg-config libusb-1.0-0-dev"; \
		echo "  Fedora:        sudo dnf install gcc make pkgconf-pkg-config libusb1-devel"; \
		echo "  Arch:          sudo pacman -S base-devel pkgconf libusb"; \
		exit 1; \
	}

install: $(BIN)
	install -d "$(DESTDIR)$(BINDIR)"
	install -m 0755 "$(BIN)" "$(DESTDIR)$(BINDIR)/$(BIN)"

install-udev:
	install -d "$(DESTDIR)$(UDEVDIR)"
	install -m 0644 udev/60-dspi-cli.rules "$(DESTDIR)$(UDEVDIR)/60-dspi-cli.rules"
	@echo "Reload udev rules with: sudo udevadm control --reload-rules && sudo udevadm trigger"

uninstall:
	rm -f "$(DESTDIR)$(BINDIR)/$(BIN)"
	rm -f "$(DESTDIR)$(UDEVDIR)/60-dspi-cli.rules"

clean:
	rm -f "$(BIN)"
