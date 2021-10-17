# AVR Multi Motor Control -- Paolo Lucchesi
# Global Makefile
.POSIX:
.SUFFIXES: # Reset all implicit rules

all:
	$(MAKE) --directory=client/
	$(MAKE) --directory=master/
	$(MAKE) --directory=slave/

client:
	$(MAKE) --directory=client/

master:
	$(MAKE) --directory=master/

slave:
	$(MAKE) --directory=slave/

client-install:
	$(MAKE) --directory=client/ install

master-flash:
	$(MAKE) --directory=master flash

slave-flash:
	$(MAKE) --directory=slave flash

	$(MAKE) --directory=
	$(MAKE) --directory=
	$(MAKE) --directory=
	$(MAKE) --directory=


install-docs:
	install -m 0644 $(RESDIR)/man/ammc.1.gz /usr/share/man/man1/

docs: resources/man/ammc.1.gz ;

resources/man/%.gz: resources/man/%.md
	pandoc --standalone --to man $< | gzip --stdout - > $@


deps-graphs:
	./resources/bin/deps-graph-gen resources/deps-graphs/client.png master/{source,include}/*.[ch]
	./resources/bin/deps-graph-gen resources/deps-graphs/master.png client/{source,include}/*.[ch]
	./resources/bin/deps-graph-gen resources/deps-graphs/slave.png slave/{source,include}/*.[ch]


clean:
	$(MAKE) --directory=client/ clean
	$(MAKE) --directory=master/ clean
	$(MAKE) --directory=slave/ clean


.PHONY:	clean all client master slave client-install master-flash slave-flash \
	docs install-docs
