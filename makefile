CXX=g++ --std=c++14

SRC_DIR=./src
HEADERS_DIR=./headers
TESTS_DIR=./tests

TEST_BUILD_DIR=./test_build
OBJS_BUILD_DIR=./objs

OBJS=$(patsubst $(SRC_DIR)/%.cpp, $(OBJS_BUILD_DIR)/%.o, $(wildcard $(SRC_DIR)/*.cpp))
TESTS=$(patsubst $(TESTS_DIR)/%.cpp, $(TEST_BUILD_DIR)/%, $(wildcard $(TESTS_DIR)/*.cpp))

CXXFLAGS=-I$(HEADERS_DIR) -lm

.SECONDARY: $(OBJS)

.PHONY: clean tests build_obj_build_path

nothing:

clean:
	rm -f $(OBJS_BUILD_DIR)/*
	rm -f $(TEST_BUILD_DIR)/*


$(OBJS_BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp $(HEADER_DEPS) | $(OBJS_BUILD_DIR)
	$(CXX) -c $< -o $(OBJS_BUILD_DIR)/$*.o $(CXXFLAGS)

$(TEST_BUILD_DIR)/%: $(TESTS_DIR)/%.cpp $(OBJS) | $(TEST_BUILD_DIR)
	$(CXX) $^ -o $(TEST_BUILD_DIR)/$* $(CXXFLAGS)

tests: build_paths $(TESTS)

$(TEST_BUILD_DIR):
	mkdir -p $@

$(OBJS_BUILD_DIR):
	mkdir -p $@

