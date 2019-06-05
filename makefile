CXX=g++ --std=c++14

SRC_DIR=./src
HEADERS_DIR=./headers
TESTS_DIR=./tests

HEADER_DEPS=$(HEADERS_DIR)/assert.h

BUILD_DIR=./build
TEST_BUILD_DIR=./test_build
OBJS_BUILD_DIR=./objs

OBJS=$(patsubst $(SRC_DIR)/%.cpp, $(OBJS_BUILD_DIR)/%.o, $(wildcard $(SRC_DIR)/*.cpp))
TESTS=$(patsubst $(TESTS_DIR)/%.cpp, $(TEST_BUILD_DIR)/%, $(wildcard $(TESTS_DIR)/*.cpp))

CXXFLAGS=-I$(HEADERS_DIR)

nothing:

clean:
	rm -f $(BUILD_DIR)/*
	rm -f $(OBJS_BUILD_DIR)/*
	rm -f $(TEST_BUILD_DIR)/*

.SECONDARY: $(OBJS)

$(HEADER_DEPS):

$(OBJS_BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) -c $^ -o $(OBJS_BUILD_DIR)/$*.o $(CXXFLAGS)

$(TEST_BUILD_DIR)/%: $(TESTS_DIR)/%.cpp $(OBJS)
	$(CXX) $^ -o $(TEST_BUILD_DIR)/$* $(CXXFLAGS)

tests: build_paths $(TESTS)

build_paths:
	mkdir -p $(BUILD_DIR)
	mkdir -p $(TEST_BUILD_DIR)
	mkdir -p $(OBJS_BUILD_DIR)
