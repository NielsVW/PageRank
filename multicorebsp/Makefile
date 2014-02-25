include include.mk

library: ${OBJECTS} mcbsp.opp
	mkdir include || true
	mkdir lib || true
	cp mcbsp.h include/
	cp mcbsp.hpp include/
	cp mcbsp-affinity.h include/
	${AR} rcs lib/libmcbsp${VERSION}.a ${OBJECTS} mcbsp.opp

clean:
	rm -f ${OBJECTS} mcbsp.opp
	rm -rf include
	rm -rf lib

