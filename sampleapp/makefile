SRC = $(shell find . -name '*.cpp') $(shell find . -name '*.c')
EXCLUDE_SRC = 
OBJ_DIR = .obj
FSRC = $(filter-out $(EXCLUDE_SRC), $(SRC))
OBJ = $(FSRC:%=$(OBJ_DIR)/%.o)

DEP_DIR = .deps

FLAGS = -fPIC -Wall -Wno-unknown-pragmas -Werror -I ../include -DNEO4JPP_IMPL_FILE
CXXFLAGS = -std=c++14
CFLAGS = 
LINKFLAGS = -lgtest -lgtest_main -lpthread -lneo4j-client

OUTFILE = sample

.PHONY: clean debug release

all: debug

debug: FLAGS += -g
debug: $(OUTFILE)

release: FLAGS += -O2 -march=native
release: $(OUTFILE)

$(OUTFILE): $(OBJ)
	@echo Generating binary
	@$(CXX) -o $@ $^ $(LINKFLAGS)
	@echo Build done

$(OBJ_DIR)/%.cc.o: %.cc
	@echo Building $<
	@mkdir -p `dirname $@`
	@$(CXX) -c $(FLAGS) $(CXXFLAGS) $< -o $@
	@mkdir -p `dirname $(DEP_DIR)/$@.d`
	@$(CXX) -c $(FLAGS) $(CXXFLAGS) -MT '$@' -MM $< > $(DEP_DIR)/$@.d

$(OBJ_DIR)/%.cpp.o: %.cpp
	@echo Building $<
	@mkdir -p `dirname $@`
	@$(CXX) -c $(FLAGS) $(CXXFLAGS) $< -o $@
	@mkdir -p `dirname $(DEP_DIR)/$@.d`
	@$(CXX) -c $(FLAGS) $(CXXFLAGS) -MT '$@' -MM $< > $(DEP_DIR)/$@.d

$(OBJ_DIR)/%.c.o: %.c
	@echo Building $<
	@mkdir -p `dirname $@`
	@$(CC) -c $(FLAGS) $(CFLAGS) $< -o $@
	@mkdir -p `dirname $(DEP_DIR)/$@.d`
	@$(CC) -c $(FLAGS) $(CFLAGS) -MT '$@' -MM $< > $(DEP_DIR)/$@.d

clean:
	@echo Removing binary
	@rm -f $(OUTFILE)
	@echo Removing objects
	@rm -rf $(OBJ_DIR)
	@echo Removing dependency files
	@rm -rf $(DEP_DIR)

-include $(OBJ:%=$(DEP_DIR)/%.d)