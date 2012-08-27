include config.make
include $(OF_ROOT)/libs/openFrameworksCompiled/project/makefileCommon/Makefile.examples

install:
	service ttswatchdog stop
	cp bin/watchdog /usr/bin/ttswatchdog
	service ttswatchdog start
