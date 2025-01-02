BUILD := debug
BUILD_DIR := build/${BUILD}/
.DEFAULT_GOAL := ${BUILD_DIR}is-online

CC := g++
cxxflags.common :=  -I. `pkg-config --cflags gtkmm-3.0 libcurl` -std=c++17
cxxflags.debug := -g -O0
cxxflags.release := -O2
cxxflags.sanitize_address:= -g -O0 -fsanitize=address
CXXFLAGS := ${cxxflags.${BUILD}} ${cxxflags.common}

ldflags.common :=
ldflags.debug :=
ldflags.release :=
ldflags.sanitize_address := -fsanitize=address
LDFLAGS := ${ldflags.${BUILD}} ${ldflags.common}


_OBJ := main_window.o data_manager.o url_item_box.o worker_queue.o thread_safe_queue.o utils.o resources.o url_data.o
_TEST_OBJ := data_manager_unittest.o utils_unittest.o
OBJ := ${patsubst %,${BUILD_DIR}%,${_OBJ}}
TEST_OBJ := ${patsubst %,${BUILD_DIR}%,${_TEST_OBJ}}
$(shell mkdir -p $(BUILD_DIR))

${BUILD_DIR}%.o: %.cc
	$(CC) -c -o $@ $< $(CXXFLAGS)

${BUILD_DIR}is-online: $(OBJ)
	$(CC) ${LDFLAGS} -o ${BUILD_DIR}is-online is_online.cc $^ `pkg-config --cflags --libs gtkmm-3.0 libcurl sqlite3`

${BUILD_DIR}run_tests: $(OBJ) $(TEST_OBJ)
	$(CC) ${LDFLAGS} -o ${BUILD_DIR}run_tests run_tests.cc /usr/lib/x86_64-linux-gnu/libgtest.a $^ `pkg-config --cflags --libs gtkmm-3.0 libcurl sqlite3 gtest`

.PHONY: tests
tests: ${BUILD_DIR}run_tests

.PHONY: clean
clean:
	@echo "Removing build files"
	rm ${BUILD_DIR}*.o ${BUILD_DIR}is-online 
	rm ${BUILD_DIR}run_tests
