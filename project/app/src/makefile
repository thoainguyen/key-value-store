APP = $(BUILD_DIR)/app
APP_INC = -I$(APP_DIR)/include -I$(META_DIR)/include

APP_SRC = $(wildcard $(APP_DIR)/src/*.c)
APP_OBJ = $(patsubst %.c,%.o, $(APP_SRC))

%.o: %.c
	$(CXX) $(CXXFLAGS) -c -o $@ $< $(APP_INC)

BIN = 

define log =
$(info ======================================)
$(info $1)
$(info ======================================)
endef

$(START):
	$(call log, Compiling app...)

$(STOP):
	$(call log, app is successfully compiled...)


all: $(START) $(APP) $(STOP)
	$(call log, app is compiled successfully)

$(APP): $(APP_OBJ)
	$(CXX) $(INCLUDES) $(CXXFLAGS)  -o $(APP) $(APP_OBJ) $(LIB_META) 

clean:
	rm -rf $(APP) $(APP_OBJ)