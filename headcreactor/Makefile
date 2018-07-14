CXX = g++
ROOT_PATH = $(shell pwd) 
#Set target output path and the path of intermidiate object 
#The path macros should include in $(ROOT_PATH)/path 
OUT_PATH = out
TARGET = HeadCreactor
#include
CFLAGS =-ggdb \
	-I. 
#Dependent libraries 
LDFLAGS = \
	-L. \
	-lpthread
#Set CPP source directory 
CPP_SRCDIR = . 
#Traverse every directory in $(CPP_SRCDIR), and find every cpp file 
CPP_SOURCES = $(foreach d,$(CPP_SRCDIR),$(wildcard $(d)/*.cpp) ) 
#Traverse every cpp file in $(CPP_SOURCES) and get corresponding object file(.o) 
CPP_OBJFILES = $(patsubst %.cpp,%.o,$(CPP_SOURCES)) 
#Set C source directory 
C_SRCDIR = .
#C Source files 
C_SOURCES = $(foreach d,$(C_SRCDIR),$(wildcard $(d)/*.c) ) 
C_OBJFILES = $(patsubst %.c,%.o,$(C_SOURCES)) 
#Set vpath where to find these types of files 
vpath %.cpp $(dir $(CPP_SOURCES)) 
vpath %.c $(dir $(C_SOURCES)) 
vpath %.o $(OBJ_PATH) 
#The first target to be executed 
all: test target
test:
	@echo $(CPP_SOURCES)
	@echo $(CPP_OBJFILES)
	@echo "--------------"
target: $(TARGET) 
#Static dependecy pattern 
$(C_OBJFILES):%.o:%.c 
	$(CXX) -c $(CFLAGS) -o $@ $< 
$(CPP_OBJFILES):%.o:%.cpp
	$(CXX) -c $(CFLAGS) -o $@ $< 
$(TARGET): $(CPP_OBJFILES) $(C_OBJFILES) 
	$(CXX) -o $@ $^ $(LDFLAGS) 
clean: 
	-rm -rf $(CPP_OBJFILES)
	-rm -rf $(C_OBJFILES) 
	-rm -f $(TARGET)
