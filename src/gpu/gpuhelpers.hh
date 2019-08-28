#ifndef FML_GPU_GPUHELPERS_H
#define FML_GPU_GPUHELPERS_H


#include <stdexcept>

#include "../cpu/cpumat.hh"
#include "../cpu/cpuvec.hh"

#include "gpumat.hh"
#include "gpuvec.hh"


namespace gpuhelpers
{
  // gpu2cpu
  template <typename REAL>
  void gpu2cpu_noalloc(gpuvec<REAL> &gpu, cpuvec<REAL> &cpu)
  {
    if (gpu.size() != cpu.size())
      throw std::runtime_error("non-conformable arguments");
    
    size_t len = gpu.size() * sizeof(REAL);
    gpu.get_card()->mem_gpu2cpu(cpu.data_ptr(), gpu.data_ptr(), len);
  }
  
  template <typename REAL>
  cpuvec<REAL> gpu2cpu(gpuvec<REAL> &gpu)
  {
    cpuvec<REAL> cpu(gpu.size());
    gpu2cpu_noalloc(gpu, cpu);
    return cpu;
  }
  
  
  
  template <typename REAL>
  void gpu2cpu_noalloc(gpumat<REAL> &gpu, cpumat<REAL> &cpu)
  {
    if (gpu.nrows() != cpu.nrows() || gpu.ncols() != cpu.ncols())
      throw std::runtime_error("non-conformable arguments");
    
    size_t len = (size_t) gpu.nrows() * gpu.ncols() * sizeof(REAL);
    gpu.get_card()->mem_gpu2cpu(cpu.data_ptr(), gpu.data_ptr(), len);
  }
  
  template <typename REAL>
  cpumat<REAL> gpu2cpu(gpumat<REAL> &gpu)
  {
    cpumat<REAL> cpu(gpu.nrows(), gpu.ncols());
    gpu2cpu_noalloc(gpu, cpu);
    return cpu;
  }
  
  
  
  // cpu2gpu
  template <typename REAL>
  void cpu2gpu_noalloc(cpuvec<REAL> &cpu, gpuvec<REAL> &gpu)
  {
    if (gpu.size() != cpu.size())
      throw std::runtime_error("non-conformable arguments");
    
    size_t len = cpu.size() * sizeof(REAL);
    gpu.get_card()->mem_cpu2gpu(gpu.data_ptr(), cpu.data_ptr(), len);
  }
  
  template <typename REAL>
  gpuvec<REAL> cpu2gpu(std::shared_ptr<card> c, cpuvec<REAL> &cpu)
  {
    gpuvec<REAL> gpu(c, cpu.nrows(), cpu.ncols());
    cpu2gpu_noalloc(cpu, gpu);
    return gpu;
  }
  
  
  
  template <typename REAL>
  void cpu2gpu_noalloc(cpumat<REAL> &cpu, gpumat<REAL> &gpu)
  {
    if (gpu.nrows() != cpu.nrows() || gpu.ncols() != cpu.ncols())
      throw std::runtime_error("non-conformable arguments");
    
    size_t len = (size_t) gpu.nrows() * gpu.ncols() * sizeof(REAL);
    gpu.get_card()->mem_cpu2gpu(gpu.data_ptr(), cpu.data_ptr(), len);
  }
  
  template <typename REAL>
  gpumat<REAL> cpu2gpu(cpumat<REAL> &cpu, std::shared_ptr<card> c)
  {
    gpumat<REAL> gpu(c, cpu.nrows(), cpu.ncols());
    cpu2gpu_noalloc(cpu, gpu);
    return gpu;
  }
}


#endif
