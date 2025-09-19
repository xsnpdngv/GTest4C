set(CTEST_PROJECT_NAME "gtest4c")
set(CTEST_NIGHTLY_START_TIME "00:00:00 UTC")

set(CTEST_DROP_METHOD "https")
set(CTEST_DROP_SITE "my.cdash.org")
set(CTEST_DROP_LOCATION "/submit.php?project=${CTEST_PROJECT_NAME}")
set(CTEST_DROP_SITE_CDASH TRUE)

# Memory checking
find_program(CTEST_MEMORYCHECK_COMMAND valgrind)
# set(CTEST_MEMORYCHECK_TYPE "Valgrind")
set(MEMORYCHECK_COMMAND_OPTIONS "--leak-check=full --error-exitcode=1")
