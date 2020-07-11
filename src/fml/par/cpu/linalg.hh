// This file is part of fml which is released under the Boost Software
// License, Version 1.0. See accompanying file LICENSE or copy at
// https://www.boost.org/LICENSE_1_0.txt

#ifndef FML_PAR_CPU_LINALG_H
#define FML_PAR_CPU_LINALG_H
#pragma once


#include "parmat.hh"
#include "../../cpu/linalg.hh"


namespace fml
{
namespace linalg
{
  /// @brief Computes lower triangle of alpha*x^T*x
  template <typename REAL>
  void crossprod(const REAL alpha, const parmat_cpu<REAL> &x, cpumat<REAL> &ret)
  {
    const len_t n = x.ncols();
    if (n != ret.nrows() || n != ret.ncols())
      ret.resize(n, n);
    
    linalg::crossprod(alpha, x.data_obj(), ret);
    
    comm r = x.get_comm();
    r.allreduce(n*n, ret.data_ptr());
  }
  
  /// \overload
  template <typename REAL>
  cpumat<REAL> crossprod(const REAL alpha, const parmat_cpu<REAL> &x)
  {
    const len_t n = x.ncols();
    cpumat<REAL> ret(n, n);
    
    crossprod(alpha, x, ret);
    return ret;
  }
  
  
  
  template <typename REAL>
  void cpsvd(const parmat_cpu<REAL> &x, cpuvec<REAL> &s, parmat_cpu<REAL> &u,
    cpumat<REAL> &vt)
  {
    auto cp = crossprod((REAL)1, x);
    x.get_comm().allreduce(cp.nrows()*cp.ncols(), cp.data_ptr());
    eigen_sym(cp, s, vt);
    
    matmult(false, false, (REAL)1, x.data_obj(), vt, u.data_obj());
    
    const REAL s_d = s.data_ptr();
    const REAL u_d = u.data_obj().data_ptr();
    const len_t m = u.data_obj().nrows();
    const len_t n = u.ncols();
    for (len_t j=0; j<n; j++)
    {
      for (len_t i=0; i<m; i++)
        u_d *= s_d[j];
    }
  }
  
  /// \overload
  template <typename REAL>
  void cpsvd(const parmat_cpu<REAL> &x, cpuvec<REAL> &s)
  {
    auto cp = crossprod((REAL)1, x);
    x.get_comm().allreduce(cp.nrows()*cp.ncols(), cp.data_ptr());
    eigen_sym(cp, s); 
  }
}
}


#endif
