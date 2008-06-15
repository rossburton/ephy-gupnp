CPPFLAGS = `pkg-config --cflags epiphany-2.22 gupnp-1.0`
CFLAGS = -g -Wall -O1
LDFLAGS = -fPIC -shared `pkg-config --libs epiphany-2.22 gupnp-1.0`

EXTLIB=libupnpextension.so
EXTDIR=$(HOME)/.gnome2/epiphany/extensions
EXTFILE=upnp.ephy-extension

all: $(EXTLIB)

clean:
	rm -f $(EXTLIB)

$(EXTLIB): ephy-upnp-extension.c upnp.c
	$(LINK.c) -o $@ $^

install: $(EXTLIB)
	sed -e 's|%EXTENSION_DIR%|$(EXTDIR)|g' -e 's|%LIBRARY%|$(EXTLIB)|g' $(EXTFILE).in > $(EXTDIR)/$(EXTFILE)
	cp $(EXTLIB) $(EXTDIR)/

uninstall:
	rm -f $(EXTDIR)/$(EXTLIB) $(EXTDIR)/$(EXTFILE)

.PHONY: all clean install uninstall
