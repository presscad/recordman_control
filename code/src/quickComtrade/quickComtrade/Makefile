#°æ±¾ºÅ
VERSION = 2.7.32.0

VPATH = -I../../common

common_obj = Lock.o LogFile.o globalfun.o XJFile.o XJString.o xjlib.o

cur_obj = XJComtrade.o Channel.o COMTRADEParser.o FileEx.o

AllOBJ =  $(common_obj) $(cur_obj)

TARGETFILENAME = libXJComtrade.so
TARGETPATH = ../../bin/lib/

LINKLIBS = 

$(TARGETFILENAME)-$(VERSION) : $(AllOBJ) mkxjtargetdir
	g++ -o $(TARGETPATH)$(TARGETFILENAME)$(FILEFLAGS)-${VERSION} $(LINKFLAGS) $(AllOBJ) $(LINKLIBS) $(CFLAGS) $(VPATH) $(EXPORTFLAGS)

$(common_obj):%.o: ../../common/%.cpp
	g++  $(CFLAGS) $(VPATH) -c -o $@ $<

$(cur_obj):%.o: %.cpp
	g++  $(CFLAGS) $(VPATH) -c -o $@ $<

.PHONY : mkxjtargetdir clean

mkxjtargetdir :
	if [ ! -d $(TARGETPATH) ]; then mkdir -p $(TARGETPATH); fi

clean :
	-rm *.o $(TARGETPATH)$(TARGETFILENAME)$(FILEFLAGS)-$(VERSION)