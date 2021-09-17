all: liblvgl.a liblvglfonts.a

liblvgl.a:
	$(MAKE) --file=lvgl.mk

liblvglfonts.a:
	$(MAKE) --file=fonts.mk

clean:
	$(MAKE) --file=lvgl.mk clean
	$(MAKE) --file=fonts.mk clean

.PHONY: clean all
