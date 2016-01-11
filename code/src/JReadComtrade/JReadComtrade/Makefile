#°æ±¾ºÅ
VERSION = 2.7.3.0

VPATH = -I ./include \
				-I ./include/linux

common_obj = common_lib.o GECodeConvert.o

cur_obj = ReadWave.o Comtrade.o

AllOBJ =  $(common_obj) $(cur_obj)

TARGETFILENAME = libJReadComtrade.so
TARGETPATH = ../../../bin/lib/

LINKLIBS = 

$(TARGETFILENAME)-$(VERSION) : $(AllOBJ) mkxjtargetdir
	g++ -o $(TARGETPATH)$(TARGETFILENAME)$(FILEFLAGS)-${VERSION} $(LINKFLAGS) $(AllOBJ) $(LINKLIBS) $(CFLAGS) $(VPATH) $(EXPORTFLAGS)

$(common_obj):%.o: ../../../common/%.cpp
	g++  $(CFLAGS) $(VPATH) -c -o $@ $<

$(cur_obj):%.o: %.cpp
	g++ $(CFLAGS) $(VPATH) -c -o $@ $<

.PHONY : mkxjlibdir clean

mkxjtargetdir :
	if [ ! -d $(TARGETPATH) ]; then mkdir -p $(TARGETPATH); fi

clean :
	-rm *.o $(TARGETPATH)$(TARGETFILENAME)$(FILEFLAGS)-$(VERSION)