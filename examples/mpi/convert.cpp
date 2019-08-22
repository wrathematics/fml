#include <mpi/grid.hh>
#include <mpi/mpimat.hh>
#include <mpi/mpihelpers.hh>


int main()
{
  grid g = grid(PROC_GRID_SQUARE);
  g.print();
  
  mpimat<float> x = mpimat<float>(g, 5, 5, 2, 2);
  
  x.fill_one();
  
  cpumat<float> x_gbl = mpihelpers::mpi2cpu(x);
  if (g.rank0())
  {
    x_gbl.info();
    x_gbl.print();
  }
  
  g.exit();
  g.finalize();
  
  return 0;
}