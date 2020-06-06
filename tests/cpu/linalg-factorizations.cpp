#include "../catch.hpp"

#include <fml/_internals/arraytools/src/arraytools.hpp>
#include <fml/cpu/cpumat.hh>
#include <fml/cpu/cpuvec.hh>
#include <fml/cpu/linalg.hh>

using namespace arraytools;


TEMPLATE_TEST_CASE("lu", "[linalg]", float, double)
{
  len_t n = 2;
  
  fml::cpumat<TestType> x(n, n);
  x.fill_linspace(1, n*n);
  
  fml::linalg::lu(x);
  
  REQUIRE( fltcmp::eq(x.get(0, 0), 2) );
  REQUIRE( fltcmp::eq(x.get(0, 1), 4) );
  REQUIRE( fltcmp::eq(x.get(1, 0), 0.5) );
  REQUIRE( fltcmp::eq(x.get(1, 1), 1) );
}



TEMPLATE_TEST_CASE("svd", "[linalg]", float, double)
{
  len_t m = 3;
  len_t n = 2;
  
  fml::cpuvec<TestType> v(n);
  v.set(0, 2);
  v.set(1, 5);
  
  fml::cpuvec<TestType> s1, s2, s3;
  
  fml::cpumat<TestType> x(m, n);
  
  x.fill_diag(v);
  fml::linalg::svd(x, s1);
  
  x.fill_diag(v);
  fml::linalg::tssvd(x, s2);
  
  x.fill_diag(v);
  fml::linalg::cpsvd(x, s3);
  
  v.rev();
  REQUIRE( v == s1 );
  REQUIRE( v == s2 );
  REQUIRE( v == s3 );
}



TEMPLATE_TEST_CASE("eigen", "[linalg]", float, double)
{
  len_t n = 2;
  
  fml::cpuvec<TestType> v(n);
  v.set(0, 2);
  v.set(1, 5);
  
  fml::cpumat<TestType> x(n, n);
  x.fill_diag(v);
  
  fml::cpuvec<TestType> values;
  fml::linalg::eigen_sym(x, values);
  
  REQUIRE( v == values );
}



TEMPLATE_TEST_CASE("invert", "[linalg]", float, double)
{
  len_t n = 2;
  
  fml::cpumat<TestType> x(n, n);
  x.fill_linspace(1, n*n);
  
  fml::linalg::invert(x);
  
  REQUIRE( fltcmp::eq(x.get(0, 0), -2) );
  REQUIRE( fltcmp::eq(x.get(1, 0), 1) );
  REQUIRE( fltcmp::eq(x.get(0, 1), 1.5) );
  REQUIRE( fltcmp::eq(x.get(1, 1), -0.5) );
}



TEMPLATE_TEST_CASE("solve", "[linalg]", float, double)
{
  len_t n = 2;
  
  fml::cpuvec<TestType> y(n);
  y.set(0, 1);
  y.set(1, 1);
  
  fml::cpumat<TestType> x(n, n);
  x.fill_zero();
  x.set(0, 0, 2);
  x.set(1, 1, 3);
  
  fml::linalg::solve(x, y);
  
  REQUIRE( fltcmp::eq(y.get(0), 0.5) );
  REQUIRE( fltcmp::eq(y.get(1), (TestType)1/3) );
}



TEMPLATE_TEST_CASE("QR and LQ - square", "[linalg]", float, double)
{
  // test matrix from here https://en.wikipedia.org/wiki/QR_decomposition#Example_2
  fml::cpumat<TestType> x(3, 3);
  x.set(0, 0, 12);
  x.set(1, 0, 6);
  x.set(2, 0, -4);
  x.set(0, 1, -51);
  x.set(1, 1, 167);
  x.set(2, 1, 24);
  x.set(0, 2, 4);
  x.set(1, 2, -68);
  x.set(2, 2, -41);
  
  auto orig = x.dupe();
  
  // QR
  fml::cpuvec<TestType> aux;
  fml::linalg::qr(false, x, aux);
  
  fml::cpumat<TestType> Q;
  fml::cpuvec<TestType> work;
  fml::linalg::qr_Q(x, aux, Q, work);
  
  REQUIRE( fltcmp::eq(fabs(Q.get(0, 0)), (TestType)6/7) );
  REQUIRE( fltcmp::eq(fabs(Q.get(1, 0)), (TestType)3/7) );
  REQUIRE( fltcmp::eq(fabs(Q.get(1, 1)), (TestType)158/175) );
  REQUIRE( fltcmp::eq(fabs(Q.get(1, 2)), (TestType)6/175) );
  
  fml::cpumat<TestType> R;
  fml::linalg::qr_R(x, R);
  
  REQUIRE( fltcmp::eq(fabs(R.get(0, 0)), (TestType)14) );
  REQUIRE( fltcmp::eq(fabs(R.get(1, 0)), 0) );
  REQUIRE( fltcmp::eq(fabs(R.get(0, 1)), (TestType)21) );
  REQUIRE( fltcmp::eq(fabs(R.get(1, 2)), (TestType)70) );
  
  // LQ
  fml::linalg::xpose(orig, x);
  fml::cpumat<TestType> tR;
  fml::linalg::xpose(R, tR);
  
  fml::linalg::lq(x, aux);
  
  fml::cpumat<TestType> L;
  fml::linalg::lq_L(x, L);
  
  fml::linalg::lq_Q(x, aux, Q, work);
  
  REQUIRE( fltcmp::eq(fabs(Q.get(0, 0)), (TestType)6/7) );
  REQUIRE( fltcmp::eq(fabs(Q.get(0, 1)), (TestType)3/7) );
  REQUIRE( fltcmp::eq(fabs(Q.get(1, 1)), (TestType)158/175) );
  REQUIRE( fltcmp::eq(fabs(Q.get(2, 1)), (TestType)6/175) );
  
  REQUIRE( tR == L );
}



TEMPLATE_TEST_CASE("QR", "[linalg]", float, double)
{
  fml::cpuvec<TestType> aux, work;
  fml::cpumat<TestType> Q, R;
  
  fml::cpumat<TestType> x(3, 2);
  x.fill_linspace(1, 6);
  fml::linalg::qr(false, x, aux);
  fml::linalg::qr_Q(x, aux, Q, work);
  fml::linalg::qr_R(x, R);
  auto test = fml::linalg::matmult(false, false, (TestType)1.0, Q, R);
  x.fill_linspace(1, 6);
  REQUIRE( x == test );
  
  fml::cpumat<TestType> y(2, 3);
  y.fill_linspace(1, 6);
  fml::linalg::qr(false, y, aux);
  fml::linalg::qr_Q(y, aux, Q, work);
  fml::linalg::qr_R(y, R);
  fml::linalg::matmult(false, false, (TestType)1.0, Q, R, test);
  y.fill_linspace(1, 6);
  REQUIRE( y == test );
}



TEMPLATE_TEST_CASE("LQ", "[linalg]", float, double)
{
  fml::cpuvec<TestType> aux, work;
  fml::cpumat<TestType> L, Q;
  
  fml::cpumat<TestType> x(3, 2);
  x.fill_linspace(1, 6);
  fml::linalg::lq(x, aux);
  fml::linalg::lq_Q(x, aux, Q, work);
  fml::linalg::lq_L(x, L);
  auto test = fml::linalg::matmult(false, false, (TestType)1.0, L, Q);
  x.fill_linspace(1, 6);
  REQUIRE( x == test );
  
  fml::cpumat<TestType> y(2, 3);
  y.fill_linspace(1, 6);
  fml::linalg::lq(y, aux);
  fml::linalg::lq_Q(y, aux, Q, work);
  fml::linalg::lq_L(y, L);
  fml::linalg::matmult(false, false, (TestType)1.0, L, Q, test);
  y.fill_linspace(1, 6);
  REQUIRE( y == test );
}



TEMPLATE_TEST_CASE("chol", "[linalg]", float, double)
{
  // test matrix from here https://en.wikipedia.org/wiki/Cholesky_decomposition#Example
  fml::cpumat<TestType> x(3, 3);
  x.set(0, 0, 4);
  x.set(1, 0, 12);
  x.set(2, 0, -16);
  x.set(0, 1, 12);
  x.set(1, 1, 37);
  x.set(2, 1, -43);
  x.set(0, 2, -16);
  x.set(1, 2, -43);
  x.set(2, 2, 98);
  
  fml::linalg::chol(x);
  
  REQUIRE( fltcmp::eq(fabs(x.get(0, 0)), (TestType)2) );
  REQUIRE( fltcmp::eq(fabs(x.get(0, 1)), (TestType)0) );
  REQUIRE( fltcmp::eq(fabs(x.get(1, 1)), (TestType)1) );
  REQUIRE( fltcmp::eq(fabs(x.get(2, 1)), (TestType)5) );
}