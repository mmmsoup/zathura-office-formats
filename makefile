PLUGIN_NAME = zathura-office-formats
PLUGIN_VERSION = 1.0.0

PDF_DIR = ~/.cache/$(PLUGIN_NAME)
TMP_DIR = ~/.cache/$(PLUGIN_NAME)/tmp

VERSION_DEFINES = $(shell echo "$(PLUGIN_VERSION)" | awk -F. '{print "-DVERSION_MAJ=" $$1 " -DVERSION_MIN=" $$2 " -DVERSION_REV=" $$3}')

CC = gcc
PKGCONFIG = cairo glib-2.0 gtk+-3.0 poppler-glib
CFLAGS = $(VERSION_DEFINES) -DPLUGIN_NAME=\"$(PLUGIN_NAME)\" -DPDF_DIR=\"$(PDF_DIR)\" -DTMP_DIR=\"$(TMP_DIR)\" $(shell pkg-config --cflags $(PKGCONFIG)) -I./zathura-pdf-poppler/zathura-pdf-poppler
LDFLAGS = $(shell pkg-config --libs $(PKGCONFIG)) -lcrypto

SRC = $(shell ls zathura-pdf-poppler/zathura-pdf-poppler/*.c | grep -v plugin.c)
OBJ = $(patsubst zathura-pdf-poppler/zathura-pdf-poppler/%.c, build/%.o, $(SRC))

build/lib$(PLUGIN_NAME).so: build/$(PLUGIN_NAME).o $(OBJ)
	$(CC) -shared build/*.o -o build/lib$(PLUGIN_NAME).so $(LDFLAGS)

build/$(PLUGIN_NAME).o: $(PLUGIN_NAME).c zathura-pdf-poppler/zathura-pdf-poppler/plugin.h
	mkdir -p build
	$(CC) -c -o build/$(PLUGIN_NAME).o $(CFLAGS) $(PLUGIN_NAME).c

$(OBJ): $(SRC) zathura-pdf-poppler/zathura-pdf-poppler/plugin.h zathura-pdf-poppler/zathura-pdf-poppler/utils.h
	cd build && $(CC) -c -DVERSION_MAJOR=0 -DVERSION_MINOR=0 -DVERSION_REV=0 $(patsubst %, ../%, $(SRC)) $(CFLAGS)

clean:
	rm -r build

install: build/lib$(PLUGIN_NAME).so
	cp build/lib$(PLUGIN_NAME).so /usr/lib/zathura
	cp zathura-office-formats.desktop /usr/share/applications

uninstall:
	rm /usr/lib/zathura/lib$(PLUGIN_NAME).so
	rm /usr/share/applications/zathura-office-formats.desktop

decache:
	rm -r $(PDF_DIR)
