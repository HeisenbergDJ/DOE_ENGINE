# Test CLI programs used by the runnable fixtures. They are plain C++17
# executables. OUTPUT_NAME keeps the emitted binary matching what the fixture
# Application configurations reference; the top-level copy_test_fixtures target
# places them into the fixture resource directories on every build.
add_executable(doe_test_add_solver add_solver/main.cc)
target_compile_features(doe_test_add_solver PRIVATE cxx_std_17)
set_target_properties(doe_test_add_solver PROPERTIES OUTPUT_NAME add_solver)

add_executable(doe_test_add_dif add_dif/main.cc)
target_compile_features(doe_test_add_dif PRIVATE cxx_std_17)
set_target_properties(doe_test_add_dif PROPERTIES OUTPUT_NAME add_dif)
