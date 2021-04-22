# to build:
# $ cmake . (one time only)
# $ cmake --build .

cmake_minimum_required(VERSION 3.10)

project(ufo VERSION 0.1.0 LANGUAGES C)

configure_file(src/config.h.in src/config.h)


add_compile_options(-Wall -Wextra -pedantic -std=c99)

#-----------------------------------------------------------------------
# Source files
#

set(EXEC_SRC_FILES
  src/main.c
)

set(LIB_SRC_FILES
  src/blockfile.c
  src/colon_command.c
  src/d_array.c
  src/d_binding.c
  src/d_bool.c
  src/d_closure.c
  src/d_exn.c
  src/d_hash.c
  src/d_int.c
  src/d_list.c
  src/d_nothing.c
  src/d_queue.c
  src/d_real.c
  src/d_set.c
  src/d_streamin.c
  src/d_streamout.c
  src/d_string.c
  src/d_symbol.c
  src/d_tuple.c
  src/delegate.c
  src/e_abstr.c
  src/e_app.c
  src/e_ident.c
  src/e_if.c
  src/e_let.c
  src/e_letin.c
  src/e_seq.c
  src/e_throw.c
  src/eval.c
  src/gc.c
  src/globals.c
  src/hash.c
  src/lexer.c
  src/lexer_obj.c
  src/main.c
  src/mem.c
  src/object.c
  src/parser.c
  src/repl.c
  src/stacktrace.c
  src/syntax.c
  src/thread.c
  src/vmem.c
)

set(TEST_SRC_FILES
  test/main.c
  test/test.c
  test/test_abstr.c
  test/test_app.c
  test/test_array.c
  test/test_binding.c
  test/test_blockfile.c
  test/test_closure.c
  test/test_delegate.c
  test/test_eval.c
  test/test_gc.c
  test/test_hash.c
  test/test_ident.c
  test/test_if.c
  test/test_let.c
  test/test_letin.c
  test/test_lexer.c
  test/test_list.c
  test/test_mem.c
  test/test_object.c
  test/test_parser.c
  test/test_queue.c
  test/test_seq.c
  test/test_set.c
  test/test_string.c
  test/test_thread.c
  test/test_throw.c
  test/test_tuple.c
  test/test_vmem.c
)

#-----------------------------------------------------------------------
# Library
#

add_library(${PROJECT_NAME}_lib ${LIB_SRC_FILES})
target_include_directories(${PROJECT_NAME}_lib PRIVATE src)

target_link_libraries(${PROJECT_NAME}_lib PUBLIC ${LIB_DEPENDENCIES})

#-----------------------------------------------------------------------
# Main executable
#

add_executable(${PROJECT_NAME} ${EXEC_SRC_FILES})
target_link_libraries(${PROJECT_NAME} PUBLIC ${PROJECT_NAME}_lib)

#-----------------------------------------------------------------------
# Tests
#

add_executable(test-${PROJECT_NAME} ${TEST_SRC_FILES})
target_link_libraries(test-${PROJECT_NAME} ${PROJECT_NAME}_lib)

# Invoke test.
#add_test(test-${PROJECT_NAME} ${CMAKE_CURRENT_BINARY_DIR}/test-${PROJECT_NAME})
