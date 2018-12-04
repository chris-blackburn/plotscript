#define CATCH_CONFIG_MAIN
#include "catch.hpp"

// intentially empty

// needed for the threaded interperter
#include <atomic>
extern std::atomic<bool> interrupt_flag;
