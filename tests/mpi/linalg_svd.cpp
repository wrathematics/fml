#include "../catch.hpp"

#include <fml/_internals/arraytools/src/arraytools.hpp>
#include <fml/cpu/cpuvec.hh>
#include <fml/mpi/mpimat.hh>
#include <fml/mpi/linalg.hh>

using namespace arraytools;

extern fml::grid g;


TEMPLATE_TEST_CASE("svd - values - ts", "[linalg]", float, double)
{
  len_t m = 3;
  len_t n = 2;
  
  fml::cpuvec<TestType> v(n);
  v.set(0, 2);
  v.set(1, 5);
  
  fml::mpimat<TestType> x(g, m, n, 1, 1);
  
  SECTION("qrsvd")
  {
    x.fill_diag(v);
    fml::cpuvec<TestType> s_ts;
    fml::linalg::qrsvd(x, s_ts);
    s_ts.rev();
    REQUIRE( v == s_ts );
  }
  
  SECTION("cpsvd")
  {
    x.fill_diag(v);
    fml::cpuvec<TestType> s_cp;
    fml::linalg::cpsvd(x, s_cp);
    
    s_cp.rev();
    REQUIRE( v == s_cp );
  }
}



TEMPLATE_TEST_CASE("svd - ts", "[linalg]", float, double)
{
  // matrix from https://en.wikipedia.org/wiki/Singular_value_decomposition#Example
  fml::mpimat<TestType> x(g, 5, 4, 1, 1);
  x.fill_zero();
  x.set(0, 0, 1);
  x.set(4, 0, 2);
  x.set(1, 3, 2);
  x.set(2, 1, 3);
  
  fml::cpuvec<TestType> s;
  fml::mpimat<TestType> u(g, 1, 1), vt(g, 1, 1);
  
  SECTION("qrsvd")
  {
    fml::linalg::qrsvd(x, s, u, vt);
    
    REQUIRE( fltcmp::eq(s.get(0), 3) );
    REQUIRE( fltcmp::eq(s.get(1), sqrt(5)) );
    REQUIRE( fltcmp::eq(s.get(2), 2) );
    
    REQUIRE( fltcmp::eq(fabs(u.get(0, 1)), sqrtf(0.2f)) );
    REQUIRE( fltcmp::eq(fabs(u.get(4, 1)), sqrtf(0.8f)) );
    
    REQUIRE( fltcmp::eq(fabs(vt.get(1, 0)), 1) );
    REQUIRE( fltcmp::eq(fabs(vt.get(0, 1)), 1) );
  }
  
  SECTION("cpsvd")
  {
    fml::linalg::cpsvd(x, s, u, vt);
    
    REQUIRE( fltcmp::eq(s.get(0), 3) );
    REQUIRE( fltcmp::eq(s.get(1), sqrt(5)) );
    REQUIRE( fltcmp::eq(s.get(2), 2) );
    
    REQUIRE( fltcmp::eq(fabs(u.get(0, 1)), sqrtf(0.2f)) );
    REQUIRE( fltcmp::eq(fabs(u.get(4, 1)), sqrtf(0.8f)) );
    
    REQUIRE( fltcmp::eq(fabs(vt.get(1, 0)), 1) );
    REQUIRE( fltcmp::eq(fabs(vt.get(0, 1)), 1) );
  }
}



TEMPLATE_TEST_CASE("svd - values - sf", "[linalg]", float, double)
{
  len_t m = 2;
  len_t n = 3;
  
  fml::cpuvec<TestType> v(m);
  v.set(0, 2);
  v.set(1, 5);
  
  fml::mpimat<TestType> x(g, m, n, 1, 1);
  
  SECTION("qrsvd")
  {
    x.fill_diag(v);
    fml::cpuvec<TestType> s_ts;
    fml::linalg::qrsvd(x, s_ts);
    s_ts.rev();
    REQUIRE( v == s_ts );
  }
  
  SECTION("cpsvd")
  {
    x.fill_diag(v);
    fml::cpuvec<TestType> s_cp;
    fml::linalg::cpsvd(x, s_cp);
    
    s_cp.rev();
    REQUIRE( v == s_cp );
  }
}



TEMPLATE_TEST_CASE("svd - sf", "[linalg]", float, double)
{
  // matrix from https://en.wikipedia.org/wiki/Singular_value_decomposition#Example
  fml::mpimat<TestType> x(g, 4, 5, 1, 1);
  x.fill_zero();
  x.set(0, 0, 1);
  x.set(3, 1, 2);
  x.set(1, 2, 3);
  x.set(0, 4, 2);
  
  fml::cpuvec<TestType> s;
  fml::mpimat<TestType> u(g, 1, 1), vt(g, 1, 1);
  
  SECTION("qrsvd")
  {
    fml::linalg::qrsvd(x, s, u, vt);
    
    REQUIRE( fltcmp::eq(s.get(0), 3) );
    REQUIRE( fltcmp::eq(s.get(1), sqrt(5)) );
    REQUIRE( fltcmp::eq(s.get(2), 2) );
    
    REQUIRE( fltcmp::eq(fabs(u.get(1, 0)), 1) );
    REQUIRE( fltcmp::eq(fabs(u.get(0, 1)), 1) );
    
    REQUIRE( fltcmp::eq(fabs(vt.get(1, 0)), sqrtf(0.2f)) );
    REQUIRE( fltcmp::eq(fabs(vt.get(1, 4)), sqrtf(0.8f)) );
  }
  
  SECTION("cpsvd")
  {
    fml::linalg::cpsvd(x, s, u, vt);
    
    REQUIRE( fltcmp::eq(s.get(0), 3) );
    REQUIRE( fltcmp::eq(s.get(1), sqrt(5)) );
    REQUIRE( fltcmp::eq(s.get(2), 2) );
    
    REQUIRE( fltcmp::eq(fabs(u.get(1, 0)), 1) );
    REQUIRE( fltcmp::eq(fabs(u.get(0, 1)), 1) );
    
    REQUIRE( fltcmp::eq(fabs(vt.get(1, 0)), sqrtf(0.2f)) );
    REQUIRE( fltcmp::eq(fabs(vt.get(1, 4)), sqrtf(0.8f)) );
  }
}
