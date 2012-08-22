# add custom variables to this file

# OF_ROOT allows to move projects outside apps/* just set this variable to the
# absoulte path to the OF root folder

OF_ROOT = ../../..


# USER_CFLAGS allows to pass custom flags to the compiler
# for example search paths like:
# USER_CFLAGS = -I src/objects

#USER_CFLAGS = -I/usr/include/festival -I/usr/include/speech_tools
USER_CFLAGS = -I/usr/include/flite
USER_CFLAGS += $(addprefix -I, $(shell find ../common -type d)) $(shell pkg-config jsoncpp --cflags)


# USER_LDFLAGS allows to pass custom flags to the linker
# for example libraries like:
# USER_LDFLAGS = libs/libawesomelib.a

#USER_LDFLAGS = -lFestival -lestools -lestbase -leststring
USER_LDFLAGS = -lflite_cmu_us_kal -lflite_cmu_time_awb -lflite_cmu_us_kal16 -lflite_cmu_us_awb -lflite_cmu_us_rms -lflite_cmu_us_slt -lflite_usenglish -lflite_cmulex -lflite
 
USER_LDFLAGS += $(shell pkg-config jsoncpp --libs)

USER_LIBS = -lgcrypt -laubio 


EXCLUDE_FROM_SOURCE="bin,.xcodeproj,obj"

# change this to add different compiler optimizations to your project

USER_COMPILER_OPTIMIZATION = -march=native -mtune=native -Os
USER_SOURCE_DIR = ../common

# android specific, in case you want to use different optimizations
USER_LIBS_ARM = 
USER_LIBS_ARM7 = 
USER_LIBS_NEON = 

# android optimizations

ANDROID_COMPILER_OPTIMIZATION = -Os

NDK_PLATFORM = android-8

# uncomment this for custom application name (if the folder name is different than the application name)
#APPNAME = folderName

# uncomment this for custom package name, must be the same as the java package that contains OFActivity
#PKGNAME = cc.openframeworks.$(APPNAME)





# linux arm flags

LINUX_ARM7_COMPILER_OPTIMIZATIONS = -march=armv7-a -mtune=cortex-a8 -finline-functions -funroll-all-loops  -O3 -funsafe-math-optimizations -mfpu=neon -ftree-vectorize -mfloat-abi=hard -mfpu=vfp



