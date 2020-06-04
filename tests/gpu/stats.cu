#include "../catch.hpp"

#include <_internals/arraytools/src/arraytools.hpp>
#include <gpu/stats.hh>

using namespace arraytools;
using namespace fml;

extern std::shared_ptr<card> c;


TEMPLATE_TEST_CASE("stats - pca", "[stats]", float, double)
{
  len_t m = 3;
  len_t n = 2;
  
  gpumat<TestType> x(c, m, n);
  x.fill_linspace(1, m*n);
  
  gpuvec<TestType> sdev(c);
  gpumat<TestType> rot(c);
  stats::pca(true, true, x, sdev, rot);
  
  TestType sq2 = sqrt(2.0);
  
  
  REQUIRE( sdev.size() == 2 );
  REQUIRE( fltcmp::eq(sdev.get(0), sq2) );
  // CUDA calculates this wrong
  // REQUIRE( fltcmp::eq(sdev.get(1), 0) );
  
  REQUIRE( rot.nrows() == 2 );
  REQUIRE( rot.ncols() == 2 );
  for (len_t i=0; i<n*n; i++)
  {
    auto test = fabs(rot.get(i)); // sign choice is up to LAPACK library
    REQUIRE( fltcmp::eq(test, 1/sq2) );
  }
}
