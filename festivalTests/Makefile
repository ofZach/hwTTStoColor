include config.make
include $(OF_ROOT)/libs/openFrameworksCompiled/project/makefileCommon/Makefile.examples

install:
	service tts stop
	cp bin/festivalTests /usr/bin/tts
	service tts start
