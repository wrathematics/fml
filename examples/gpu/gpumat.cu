#include <fml/gpu.hh>



int main()
{
  auto c = fml::new_card(0);
  c->info();
  
  len_t n = 5;
  fml::gpumat<float> x(c, n, n);
  x.info();
  
  x.fill_eye();
  x.scale(3.f);
  x.print();
  
  x.fill_linspace(1.f, (float) n*n);
  
  fml::cpumat<float> x_cpu;
  fml::copy::gpu2cpu(x, x_cpu);
  x_cpu.print();
  
  return 0;
}
