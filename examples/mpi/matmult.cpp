#include <mpi/grid.hh>
#include <mpi/linalg.hh>
#include <mpi/mpimat.hh>


int main()
{
  grid g = grid(PROC_GRID_SQUARE);
  g.info();
  
  len_t n = 2;
  
  mpimat<float> x(g, n, n, 1, 1);
  mpimat<float> y(g, n, n, 1, 1);
  x.fill_linspace(1.f, (float) n*n);
  y.fill_linspace((float) n*n, 1.f);
  
  mpimat<float> z = linalg::matmult(false, false, 1.0f, x, y);
  
  z.info();
  cpumat<float> z_gbl = mpihelpers::mpi2cpu(z);
  if (g.rank0())
    z_gbl.print();
  
  linalg::matmult_noalloc(true, false, 1.0f, x, y, z);
  mpihelpers::mpi2cpu_noalloc(z, z_gbl);
  if (g.rank0())
    z_gbl.print();
  
  linalg::matmult_noalloc(false, true, 1.0f, x, y, z);
  mpihelpers::mpi2cpu_noalloc(z, z_gbl);
  if (g.rank0())
    z_gbl.print();
  
  linalg::matmult_noalloc(true, true, 1.0f, x, y, z);
  mpihelpers::mpi2cpu_noalloc(z, z_gbl);
  if (g.rank0())
    z_gbl.print();
  
  g.exit();
  g.finalize();
  
  return 0;
}
