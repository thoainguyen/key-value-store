LIB_META_INC = -I$(META_DIR)/include
LIB_META_SRC = $(wildcard $(META_DIR)/src/*.c)

LIB_META_OBJ = $(patsubst %.c,%.o, $(LIB_META_SRC))


define log =
$(info ======================================)
$(info $1)
$(info ======================================)
endef

$(START):
	$(call log, Compiling meta...)

$(STOP):
	$(call log, meta is successfully compiled...)


%.o: %.c
	$(CXX) $(CXXFLAGS)  -c -o $@ $<  $(LIB_META_INC)


all: $(START) $(LIB_META) $(STOP)
	$(call log, meta is successfully compiled...)


$(LIB_META): $(LIB_META_OBJ)
	$(AR) $(CR) $(LIB_META) $(LIB_META_OBJ)

clean:
	rm -rf $(LIB_META) $(LIB_META_OBJ)