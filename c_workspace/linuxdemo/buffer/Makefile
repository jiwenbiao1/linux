PROGRAM   := a.out
SHELL := /bin/bash
OS_NAME := $(shell uname -s)
AR := ar            
SED:= sed          
AWK:= awk
MV := mv
RM := rm -f
ECHO := echo

CC      := gcc
CFLAGS := -c -g  -w #-Wall
CPP :=
CPPFLAGS :=
CXX      := g++
CXXFLAGS := -c -g -w# -Wall
CXXPP :=
CXXPPFLAGS :=
INCLUDES := #-I$(ORACLE_HOME)/rdbms/demo -I$(ORACLE_HOME)/rdbms/public  \
  -I$(ORACLE_HOME)/plsql/public -I$(ORACLE_HOME)/network/public  -I./include -I./include/app -I./include/tools  \
  -I./include/tools/file  -I./include/tools/common
LIBDIRS :=#-L$(ORACLE_HOME)/lib -L$(ORACLE_HOME)/rdbms/lib

#-lclntsh -lnsl -lpthread -Wl,-Bdynamic -lgcc_s    ,同时有动态库和静态库时默认使用动态库，   -Wl,-Bdynamic 指定和动态库相连， -Wl,-Bstatic 指定和静态库相连
CLDFLAGS    := -lpthread# -lm  -lclntsh -lnsl -lpthread  -Wl,-Bdynamic  -lgcc_s
CXXLDFLAGS  := # -lm  -lclntsh -lnsl -lpthread  -Wl,-Bdynamic -lgcc_s  -lstdc++ 
DCPPFLAGS := -D${OS_NAME}   
ifeq '${OS_NAME}' 'SunOS'
    CLDFLAGS += -lsocket
    CXXLDFLAGS += -lsocket
    DCPPFLAGS += -D_POSIX_PTHREAD_SEMANTICS -D_REENTRANT
endif
 

SRC_DIR   := . 
SFIX     :=  .c .C .cpp  .cc .CPP  .c++  .cp  .cxx
VPATH := ${SRC_DIR}        
BIN := ./bin
SOURCES := $(foreach x,${SRC_DIR},\
           $(wildcard  \
             $(addprefix  ${x}/*,${SFIX}) ) )

OBJS := $(addsuffix .o ,$(basename $(notdir ${SOURCES}) ) )    

DEPENDS := $(addsuffix .d ,$(basename  ${SOURCES} ) )  

.PHONY : all check  clean  install
 
all :  ${PROGRAM}  install
 
LDCXX := $(strip $(filter-out  %.c , ${SOURCES} ) )
 
ifdef LDCXX   #有C++源码时,所有源码都使用g++编译，包括C源码，将CC、CFLAGS 的值设置为对应的${CXX}、 ${CXXFLAGS}的值
    CC := ${CXX}                    #重置C编译器为C++编译器
    CFLAGS :=  ${CXXFLAGS}          #重置C编译选现为C++编译选现
    CPP :=  ${CXXPP}                #重置C预编译器为C++预编译器
    CPPFLAGS := ${CXXPPFLAGS}       #重置C预编译的选项为C++预编译的选项
endif

${PROGRAM} :  ${DEPENDS}  ${OBJS} 
ifeq ($(strip $(filter-out  %.c  , ${SOURCES} ) ),)    #只有C源码时使用gcc连接
	${CC}   ${OBJS} -o $@    ${LIBDIRS}  ${CLDFLAGS}   
else                                                 #有C++源码时使用g++连接
	$(CXX)   ${OBJS} -o $@      ${LIBDIRS}  ${CXXLDFLAGS}   
endif

%.o : %.c
	$(CC)      ${DCPPFLAGS}    ${CFLAGS}      ${INCLUDES}   $<
 
%.o : %.C
	$(CXX)     ${DCPPFLAGS}    ${CXXFLAGS}    ${INCLUDES}   $<
 
%.o : %.cc
	${CXX}     ${DCPPFLAGS}    ${CXXFLAGS}    ${INCLUDES}   $<
 
%.o : %.cpp
	${CXX}     ${DCPPFLAGS}    ${CXXFLAGS}    ${INCLUDES}   $<
 
%.o : %.CPP
	${CXX}     ${DCPPFLAGS}    ${CXXFLAGS}    ${INCLUDES}   $<
 
%.o : %.c++
	${CXX}     ${DCPPFLAGS}    ${CXXFLAGS}    ${INCLUDES}   $<
 
%.o : %.cp
	${CXX}     ${DCPPFLAGS}    ${CXXFLAGS}    ${INCLUDES}   $<
 
%.o : %.cxx
	${CXX}     ${DCPPFLAGS}    ${CXXFLAGS}    ${INCLUDES}   $<
 
%.d : %.c
	@${CC}     -M   -MD    ${INCLUDES} $<
 
%.d : %.C
	@${CXX}    -MM  -MD    ${INCLUDES} $<
 
%.d : %.cc
	@${CXX}    -MM  -MD    ${INCLUDES} $<
 
%.d : %.cpp
	@${CXX}    -MM  -MD    ${INCLUDES} $<
 
%.d : %.CPP
	@${CXX}    -MM  -MD    ${INCLUDES} $<
 
%.d : %.c++
	@${CXX}    -MM  -MD    ${INCLUDES} $<
 
%.d : %.cp
	@${CXX}    -MM  -MD    ${INCLUDES} $<
 
%.d : %.cxx
	@${CXX}    -MM  -MD    ${INCLUDES} $<
 
check :
	@${ECHO}  MAKEFILES : ${MAKEFILES}
	@${ECHO}  MAKECMDGOALS : ${MAKECMDGOALS}
	@${ECHO}  SHELL  : ${SHELL}
	@${ECHO}  OS_NAME  : ${OS_NAME}
	@${ECHO}  SRC_DIR : ${SRC_DIR}
	@${ECHO}  SFIX : ${SFIX}
	@${ECHO}  VPATH : ${VPATH}
	@${ECHO}  BIN : ${BIN}
	@${ECHO}  SOURCES : ${SOURCES}
	@${ECHO}  OBJS : ${OBJS}
	@${ECHO}  DEPENDS : ${DEPENDS}
	@${ECHO}  PROGRAM : ${PROGRAM}
	@${ECHO}  CC :  ${CC}
	@${ECHO}  CFLAGS : ${CFLAGS}
	@${ECHO}  CPP : ${CPP}
	@${ECHO}  CPPFLAGS : ${CPPFLAGS}
	@${ECHO}  CXX :  ${CXX}
	@${ECHO}  CXXFLAGS : ${CXXFLAGS}
	@${ECHO}  CXXPP : ${CXXPP}
	@${ECHO}  CXXPPFLAGS : ${CXXPPFLAGS}       
	@${ECHO}  INCLUDES : ${INCLUDES}
	@${ECHO}  LIBDIRS : ${LIBDIRS}
	@${ECHO}  CLDFLAGS : ${CLDFLAGS}
	@${ECHO}  CXXLDFLAGS : ${CXXLDFLAGS}
	@${ECHO}  DCPPFLAGS : ${DCPPFLAGS}
	uname    -a
clean :
	-${RM} ${BIN}/${PROGRAM}
	-${RM} ${BIN}/*.o
	-${RM} ${BIN}/*.d
	-${RM} -r ${BIN}
	-${RM} *.o
	-${RM} *.d
install :
	mkdir -p ./bin
	-${MV} ${PROGRAM} ${BIN}
	-${MV}  *.o ${BIN}
	-${MV}  *.d ${BIN}
