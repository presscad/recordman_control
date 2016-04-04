#°æ±¾ºÅ
VERSION = 2.7.32.0

VPATH = -I../../../common \
        -I../../../interface

common_obj = Lock.o LogFile.o globalfun.o CCommonFile.o StrongString.o common_lib.o GECodeConvert.o

cur_obj = Comtrade.o Channel.o COMTRADEParser.o FileEx.o

AllOBJ =  $(common_obj) $(cur_obj) tinyxml2.o cJSON.o

TARGETFILENAME = libquickComtrade.so
TARGETPATH = ../../../bin/lib/

LINKLIBS = 

$(TARGETFILENAME)-$(VERSION) : $(AllOBJ) mkxjtargetdir
	g++ -o $(TARGETPATH)$(TARGETFILENAME)$(FILEFLAGS)-${VERSION} $(LINKFLAGS) $(AllOBJ) $(LINKLIBS) $(CFLAGS) $(VPATH) $(EXPORTFLAGS)

$(common_obj):%.o: ../../../common/%.cpp
	g++  $(CFLAGS) $(VPATH) -c -o $@ $<
	
cJSON.o : ../../../common_open_source/cJSON/cJSON.c
	g++ -o cJSON.o  $(CFLAGS) $(VPATH)\
					-c ../../../common_open_source/cJSON/cJSON.c

tinyxml2.o : ../../../common_open_source/tinyxml2/tinyxml2.cpp
	g++ -o tinyxml2.o  $(CFLAGS) $(VPATH)\
					-c ../../../common_open_source/tinyxml2/tinyxml2.cpp
	
$(cur_obj):%.o: %.cpp
	g++  $(CFLAGS) $(VPATH) -c -o $@ $<

.PHONY : mkxjtargetdir clean

mkxjtargetdir :
	if [ ! -d $(TARGETPATH) ]; then mkdir -p $(TARGETPATH); fi

clean :
	-rm *.o $(TARGETPATH)$(TARGETFILENAME)$(FILEFLAGS)-$(VERSION)