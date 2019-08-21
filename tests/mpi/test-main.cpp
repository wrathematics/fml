#define CATCH_CONFIG_RUNNER
#include "../catch.hpp"

#include <mpi/grid.hh>
#include <cstdio>

grid g;


int main(int argc, char *argv[])
{
  int num_failed_tests;
  g = grid(PROC_GRID_SQUARE);
  
  if (!g.rank0())
    fclose(stdout);
  
  num_failed_tests = Catch::Session().run(argc, argv);
  
  g.exit();
  g.finalize();
  
  return num_failed_tests;
}
