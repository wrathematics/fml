#include <fml/cpu.hh>


static inline void print_det(int sign, float modulus)
{
  printf("sgn = %d\n", sign);
  printf("mod = %f\n", modulus);
  printf("sgn*exp(mod) = %f\n", sign*exp(modulus));
  printf("\n");
}



int main()
{
  len_t n = 2;
  
  fml::cpumat<float> x(n, n);
  x.fill_linspace(1, n*n);
  
  x.info();
  x.print(0);
  
  int sign;
  float modulus;
  fml::linalg::det(x, sign, modulus);
  print_det(sign, modulus);
  
  n = 4;
  x.resize(n, n);
  x.fill_linspace(1, n*n);
  
  x.info();
  x.print(0);
  
  fml::linalg::det(x, sign, modulus);
  print_det(sign, modulus);
  
  return 0;
}
