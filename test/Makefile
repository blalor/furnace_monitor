#Set this to @ to keep the makefile quiet
SILENCE = @
ARFLAGS += -c

#---- Outputs ----#
COMPONENT_NAME = furnace

#--- Inputs ----#
CPP_PLATFORM = Gcc
PROJECT_HOME_DIR = ..

MOCK_AVR_HOME = $(PROJECT_HOME_DIR)/test/support/MockAVR
CPPUTEST_HOME = $(PROJECT_HOME_DIR)/test/support/CppUTest

CPPUTEST_USE_MEM_LEAK_DETECTION = Y
CPPUTEST_USE_STD_CPP_LIB = N
CPPUTEST_USE_GCOV = Y

CPPUTEST_OBJS_DIR = $(PROJECT_HOME_DIR)/build/objs
CPPUTEST_LIB_DIR  = $(PROJECT_HOME_DIR)/build/lib
CPPUTEST_GCOV_DIR = $(PROJECT_HOME_DIR)/build/gcov

CLOCK = 8000000
CPPUTEST_ADDITIONAL_CFLAGS = -DF_CPU=$(CLOCK)
CPPUTEST_ADDITIONAL_CXXFLAGS = -DF_CPU=$(CLOCK)

SRC_DIRS = \
	$(PROJECT_HOME_DIR)/main/src

TEST_SRC_DIRS = \
	src 

INCLUDE_DIRS = \
  $(CPPUTEST_HOME)/include \
  $(PROJECT_HOME_DIR)/main/src \
  $(PROJECT_HOME_DIR)/test/src \
  $(MOCK_AVR_HOME)/include

# MOCKS_SRC_DIRS = \
# 	mocks 

CPPUTEST_WARNINGFLAGS = -Wall -Wswitch-default -Werror 
CPPUTEST_CFLAGS = -std=c99 
CPPUTEST_CFLAGS += -Wall -Wstrict-prototypes -pedantic
LD_LIBRARIES = -lpthread -L$(MOCK_AVR_HOME) -lMockAVR
  
include $(CPPUTEST_HOME)/build/MakefileWorker.mk

# bit of a hack; shouldn't really need to declare this dep here…
$(CPPUTEST_LIB):
	make -C $(CPPUTEST_HOME) all extensions

$(MOCK_AVR_HOME)/libMockAVR.a:
	make -C $(MOCK_AVR_HOME) all

$(TEST_TARGET): $(MOCK_AVR_HOME)/libMockAVR.a
