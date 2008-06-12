CPPFLAGS = `pkg-config --cflags epiphany-2.22 gupnp-1.0`
CFLAGS = -g -Wall -O1
LDFLAGS = -shared `pkg-config --libs epiphany-2.22 gupnp-1.0`

libupnp.so: ephy-upnp-extension.c upnp.c
	$(LINK.c) -o $@ $^