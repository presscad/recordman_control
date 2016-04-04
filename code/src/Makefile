#名称：总控Makefile
#作者：lyh,lp	
#时间：2015/5/28 
#版本：1.0.0
#简介：用于批量编译程序，自动判断操作系统调整编译参数，各子Makefile只需按要求编写一个与项目相关的Makefile即可。本文件一般放在src目录执行
#输入：debug,格式为debug=xxx,xxx可为任意值，只要定义了debug参数即认为是要编译debug版本，未指定时认为编译release版本，debug版本与release版本的区别为FILEFLAGS将设置为D，
#							如果子Makefile在目标文件名中使用了FILEFLAGS参数，则可在生成的目标文件名中有大写D标记，如XJWebAcProxy$(FILEFLAGS)将在debug模式生成XJWebAcProxyD，release模式生成XJWebAcProxy
#			 bits,格式为bits=64,目前只支持64，未指定时编译32位程序(或是与机器位数相同)
#			 basedir,格式为basedir=xxx,xxx为搜索子Makefile的起始目录，未指定时从本Makefile的当前目录开始搜索。比如只想编译通讯服务器则可设置basedir=./XJCommuServer
#输出：CFLAGS：编译标志，列出了常用的编译标志，已区分操作系统差异
#			 LINKFLAG：链接标志，列出了常用的链接标志
#			 FILEFLAGS；文件名标志，debug模式可以在文件名后面加上D
#			 EXPORTFLAGS：动态库导出标志，非动态库程序无须使用此项
#行为：目前只支持两种行为，编译和清除，执行make为编译，执行make clean为清除，清除时调用子Makefile的clean行为
#说明：输入参数可同时设置多项，如 make debug=1 basedir=./XJCommuServer 表示编译通信服务器的debug版本
#			 子Makefile的编写方式请参阅Makefile-RecommendatoryPractice，Makefile-exe和Makefile-dll是去除了注释的清爽版，可用做模板

#编译标志
CFLAGS = -w -fpic -DOS_LINUX
#链接标志
LINKFLAGS = -lpthread -ldl -lrt
#动态库导出标志，非动态库程序无须使用此项
EXPORTFLAGS = 
#文件名标志，debug模式可以在文件名后面加上D
FILEFLAGS =

#编译Debug还是Release
ifeq	($(origin debug), undefined)
	CFLAGS += -O2
else
	CFLAGS += -g -ggdb
	FILEFLAGS = D
endif

#编译命令
MAKE = @-make -s

XJ_SUPPORT_OS = Linux AIX SunOS
#获取操作系统类型
XJ_OSNAME = $(shell uname)
$(warning customer's os is $(XJ_OSNAME))
ifeq	($(XJ_OSNAME), $(filter $(XJ_OSNAME), $(XJ_SUPPORT_OS)))
	ifeq	($(XJ_OSNAME), Linux)
		LIBS += 
		CFLAGS += 
		LINKFLAGS += -rdynamic
		EXPORTFLAGS += -shared
	endif
	ifeq	($(XJ_OSNAME), AIX)
		LIBS += 
		CFLAGS += -fno-strict-aliasing -DOS_AIX
		LINKFLAGS += -Wl,-brtl,-G -lnsl
		EXPORTFLAGS += -shared
		ifeq	($(bits), 64)
			CFLAGS += -maix64
			export AR="ar -X64"
			export OBJECT_MODE=64
		endif
		MAKE = @-gmake -s
	endif
	ifeq	($(XJ_OSNAME), SunOS)
		LIBS += 
		CFLAGS += -DOS_SUNUNIX
		LINKFLAGS += -B direct -z lazyload -z ignore -lsocket -lnsl
		EXPORTFLAGS += -G
		ifeq	($(bits), 64)
			CFLAGS += -m64
		endif
		MAKE = @-gmake -s
	endif
else
$(error os_type is $(XJ_OSNAME) not support)
endif

#将参数导出到环境变量中以供其他makefile使用
export CFLAGS
export LINKFLAGS
export EXPORTFLAGS
export FILEFLAGS
OSNAME := $(XJ_OSNAME)
export OSNAME
OSBITS := 32
ifeq	($(bits), 64)
	OSBITS := 64
endif
export OSBITS

#要编译的工程
rootdir := .
ifeq	($(origin basedir), undefined)
	# 未指定根目录，使用当前目录
else
	rootdir := $(basedir)
endif

#查找根目录下所有makefile
dirs := $(shell find $(rootdir)  -type d)
files1 := $(foreach dir,$(dirs),$(wildcard $(dir)/Makefile))
files2 := $(foreach dir,$(dirs),$(wildcard $(dir)/makefile))
files1 := $(patsubst %/Makefile,%,$(files1))
files2 := $(patsubst %/makefile,%,$(files2))
dirs := $(files1) $(files2)
#	去除当前路径
dirs := $(patsubst .,,$(dirs))
	
$(warning subdirs $(dirs))

# 避免clean子目录操作同名，加上_clean_前缀
SUBDIRS := $(dirs)
clean_dirs := $(addprefix _clean_,$(SUBDIRS) )


all: $(SUBDIRS)
	$(warning build all projects done)

.PHONY : $(SUBDIRS) clean $(clean_dirs)

$(SUBDIRS) : 
	$(warning start build $@)
	$(MAKE) -C $@

$(clean_dirs):    
	$(MAKE) -C $(patsubst _clean_%,%,$@) clean

clean: $(clean_dirs)
	@echo "clean all projects done"