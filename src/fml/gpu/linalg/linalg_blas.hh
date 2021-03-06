// This file is part of fml which is released under the Boost Software
// License, Version 1.0. See accompanying file LICENSE or copy at
// https://www.boost.org/LICENSE_1_0.txt

#ifndef FML_GPU_LINALG_LINALG_BLAS_H
#define FML_GPU_LINALG_LINALG_BLAS_H
#pragma once


#include <stdexcept>

#include "../../_internals/linalgutils.hh"

#include "../arch/arch.hh"

#include "../gpumat.hh"

#include "linalg_err.hh"


namespace fml
{
namespace linalg
{
  /**
    @brief Computes the dot product of two vectors, i.e. the sum of the product
    of the elements.
    
    @details NOTE: if the vectors are of different length, the dot product will
    use only the indices of the smaller-sized vector.
    
    @param[in] x,y Vectors.
    
    @return The dot product.
    
    @tparam REAL should be 'float' or 'double'.
   */
  template <typename REAL>
  REAL dot(const gpuvec<REAL> &x, const gpuvec<REAL> &y)
  {
    err::check_card(x, y);
    const len_t len = std::min(x.size(), y.size());
    
    len_t m, n, k;
    fml::linalgutils::matmult_params(true, false, len, 1, len, 1, &m, &n, &k);
    
    REAL d;
    gpuscalar<REAL> d_device(x.get_card());
    gpublas_status_t check = gpublas::gemm(x.get_card()->blas_handle(),
      GPUBLAS_OP_T, GPUBLAS_OP_N, m, n, k, (REAL)1, x.data_ptr(), len,
      y.data_ptr(), len, (REAL)0, d_device.data_ptr(), 1);
    gpublas::err::check_ret(check, "gemm");
    
    d_device.get_val(&d);
    
    return d;
  }
  
  template <typename REAL>
  REAL dot(const gpuvec<REAL> &x)
  {
    return dot(x, x);
  }
  
  
  
  /**
    @brief Returns alpha*op(x) + beta*op(y) where op(A) is A or A^T
    
    @param[in] transx Should x^T be used?
    @param[in] transy Should y^T be used?
    @param[in] alpha,beta Scalars.
    @param[in] x,y The inputs to the sum.
    @param[out] ret The sum.
    
    @except If x and y are inappropriately sized for the sum, the method will
    throw a 'runtime_error' exception.
    
    @impl Uses the cuBLAS function `cublasXgeam()`.
    
    @tparam REAL should be '__half', 'float', or 'double'.
   */
  template <typename REAL>
  void add(const bool transx, const bool transy, const REAL alpha,
    const REAL beta, const gpumat<REAL> &x, const gpumat<REAL> &y,
    gpumat<REAL> &ret)
  {
    err::check_card(x, y, ret);
    
    len_t m, n;
    fml::linalgutils::matadd_params(transx, transy, x.nrows(), x.ncols(),
      y.nrows(), y.ncols(), &m, &n);
    
    if (ret.nrows() != m || ret.ncols() != n)
      ret.resize(m, n);
    
    auto c = x.get_card();
    gpublas_operation_t cbtransx = transx ? GPUBLAS_OP_T : GPUBLAS_OP_N;
    gpublas_operation_t cbtransy = transy ? GPUBLAS_OP_T : GPUBLAS_OP_N;
    
    gpublas_status_t check = gpublas::geam(c->blas_handle(), cbtransx, cbtransy,
      m, n, alpha, x.data_ptr(), x.nrows(), beta, y.data_ptr(), y.nrows(),
      ret.data_ptr(), m);
    gpublas::err::check_ret(check, "geam");
  }
  
  /// \overload
  template <typename REAL>
  gpumat<REAL> add(const bool transx, const bool transy, const REAL alpha,
    const REAL beta, const gpumat<REAL> &x, const gpumat<REAL> &y)
  {
    err::check_card(x, y);
    
    len_t m, n;
    fml::linalgutils::matadd_params(transx, transy, x.nrows(), x.ncols(),
      y.nrows(), y.ncols(), &m, &n);
    
    auto c = x.get_card();
    gpumat<REAL> ret(c, m, n);
    add(transx, transy, alpha, beta, x, y, ret);
    return ret;
  }
  
  
  
  /**
    @brief Computes ret = alpha*op(x)*op(y) where op(A) is A or A^T
    
    @param[in] transx Should x^T be used?
    @param[in] transy Should y^T be used?
    @param[in] alpha Scalar.
    @param[in] x Left multiplicand.
    @param[in] y Right multiplicand.
    @param[out] ret The product.
    
    @except If x and y are inappropriately sized for a matrix product, the
     method will throw a 'runtime_error' exception.
    
    @impl Uses the cuBLAS function `cublasXgemm()`.
    
    @tparam REAL should be '__half', 'float', or 'double'.
   */
  template <typename REAL>
  void matmult(const bool transx, const bool transy, const REAL alpha,
    const gpumat<REAL> &x, const gpumat<REAL> &y, gpumat<REAL> &ret)
  {
    err::check_card(x, y, ret);
    
    const len_t mx = x.nrows();
    const len_t my = y.nrows();
    
    int m, n, k;
    fml::linalgutils::matmult_params(transx, transy, mx, x.ncols(),
      my, y.ncols(), &m, &n, &k);
    
    if (m != ret.nrows() || n != ret.ncols())
      ret.resize(m, n);
    
    gpublas_operation_t cbtransx = transx ? GPUBLAS_OP_T : GPUBLAS_OP_N;
    gpublas_operation_t cbtransy = transy ? GPUBLAS_OP_T : GPUBLAS_OP_N;
    
    gpublas_status_t check = gpublas::gemm(x.get_card()->blas_handle(),
      cbtransx, cbtransy, m, n, k, alpha, x.data_ptr(), mx, y.data_ptr(),
      my, (REAL)0, ret.data_ptr(), m);
    gpublas::err::check_ret(check, "gemm");
  }
  
  /// \overload
  template <typename REAL>
  gpumat<REAL> matmult(const bool transx, const bool transy, const REAL alpha,
    const gpumat<REAL> &x, const gpumat<REAL> &y)
  {
    gpumat<REAL> ret(x.get_card());
    matmult(transx, transy, alpha, x, y, ret);
    
    return ret;
  }
  
  /// \overload
  template <typename REAL>
  void matmult(const bool transx, const bool transy, const REAL alpha,
    const gpumat<REAL> &x, const gpuvec<REAL> &y, gpuvec<REAL> &ret)
  {
    err::check_card(x, y, ret);
    
    const len_t mx = x.nrows();
    const len_t my = y.size();
    
    int m, n, k;
    fml::linalgutils::matmult_params(transx, transy, mx, x.ncols(),
      my, 1, &m, &n, &k);
    auto c = x.get_card();
    int len = std::max(m, n);
    if (len != ret.size())
      ret.resize(len);
    
    gpublas_operation_t cbtransx = transx ? GPUBLAS_OP_T : GPUBLAS_OP_N;
    gpublas_operation_t cbtransy = transy ? GPUBLAS_OP_T : GPUBLAS_OP_N;
    
    gpublas_status_t check = gpublas::gemm(c->blas_handle(), cbtransx, cbtransy,
      m, n, k, alpha, x.data_ptr(), mx, y.data_ptr(), my, (REAL)0,
      ret.data_ptr(), m);
    gpublas::err::check_ret(check, "gemm");
  }
  
  /// \overload
  template <typename REAL>
  gpuvec<REAL> matmult(const bool transx, const bool transy, const REAL alpha,
    const gpumat<REAL> &x, const gpuvec<REAL> &y)
  {
    gpuvec<REAL> ret(x.get_card());
    matmult(transx, transy, alpha, x, y, ret);
    
    return ret;
  }
  
  /// \overload
  template <typename REAL>
  void matmult(const bool transx, const bool transy, const REAL alpha,
    const gpuvec<REAL> &x, const gpumat<REAL> &y, gpuvec<REAL> &ret)
  {
    err::check_card(x, y, ret);
    
    const len_t mx = x.size();
    const len_t my = y.nrows();
    
    int m, n, k;
    fml::linalgutils::matmult_params(transx, transy, mx, 1,
      my, y.ncols(), &m, &n, &k);
    auto c = x.get_card();
    int len = std::max(m, n);
    if (len != ret.size())
      ret.resize(len);
    
    gpublas_operation_t cbtransx = transx ? GPUBLAS_OP_T : GPUBLAS_OP_N;
    gpublas_operation_t cbtransy = transy ? GPUBLAS_OP_T : GPUBLAS_OP_N;
    
    gpublas_status_t check = gpublas::gemm(c->blas_handle(), cbtransx, cbtransy,
      m, n, k, alpha, x.data_ptr(), mx, y.data_ptr(), my, (REAL)0,
      ret.data_ptr(), m);
    gpublas::err::check_ret(check, "gemm");
  }
  
  /// \overload
  template <typename REAL>
  gpuvec<REAL> matmult(const bool transx, const bool transy, const REAL alpha,
    const gpuvec<REAL> &x, const gpumat<REAL> &y)
  {
    gpuvec<REAL> ret(x.get_card());
    matmult(transx, transy, alpha, x, y, ret);
    
    return ret;
  }
  
  
  
  /**
    @brief Computes lower triangle of alpha*x^T*x
    
    @param[in] alpha Scalar.
    @param[in] x Input data matrix.
    @param[out] ret The product.
    
    @impl Uses the cuBLAS function `cublasXsyrk()`.
    
    @allocs If the output dimension is inappropriately sized, it will
    automatically be re-allocated.
    
    @except If a reallocation is triggered and fails, a `bad_alloc` exception
    will be thrown.
    
    @tparam REAL should be '__half', 'float', or 'double'.
   */
  template <typename REAL>
  void crossprod(const REAL alpha, const gpumat<REAL> &x, gpumat<REAL> &ret)
  {
    err::check_card(x, ret);
    
    const len_t m = x.nrows();
    const len_t n = x.ncols();
    
    if (n != ret.nrows() || n != ret.ncols())
      ret.resize(n, n);
    
    matmult(true, false, alpha, x, x, ret);
  }
  
  /// \overload
  template <typename REAL>
  gpumat<REAL> crossprod(const REAL alpha, const gpumat<REAL> &x)
  {
    const len_t n = x.ncols();
    gpumat<REAL> ret(x.get_card(), n, n);
    
    crossprod(alpha, x, ret);
    
    return ret;
  }
  
  
  
  /**
    @brief Computes lower triangle of alpha*x*x^T
    
    @param[in] alpha Scalar.
    @param[in] x Input data matrix.
    @param[out] ret The product.
    
    @impl Uses the cuBLAS function `cublasXsyrk()`.
    
    @allocs If the output dimension is inappropriately sized, it will
    automatically be re-allocated.
    
    @except If a reallocation is triggered and fails, a `bad_alloc` exception
    will be thrown.
    
    @tparam REAL should be '__half', 'float', or 'double'.
   */
  template <typename REAL>
  void tcrossprod(const REAL alpha, const gpumat<REAL> &x, gpumat<REAL> &ret)
  {
    err::check_card(x, ret);
    
    const len_t m = x.nrows();
    const len_t n = x.ncols();
    
    if (m != ret.nrows() || m != ret.ncols())
      ret.resize(m, m);
    
    matmult(false, true, alpha, x, x, ret);
  }
  
  /// \overload
  template <typename REAL>
  gpumat<REAL> tcrossprod(const REAL alpha, const gpumat<REAL> &x)
  {
    const len_t m = x.nrows();
    gpumat<REAL> ret(x.get_card(), m, m);
    
    tcrossprod(alpha, x, ret);
    
    return ret;
  }
  
  
  
  /**
    @brief Computes the transpose out-of-place (i.e. in a copy).
    
    @param[in] x Input data matrix.
    @param[out] tx The transpose.
    
    @impl Uses the cuBLAS function `cublasXgeam()`.
    
    @allocs If the output dimension is inappropriately sized, it will
    automatically be re-allocated.
    
    @except If a reallocation is triggered and fails, a `bad_alloc` exception
    will be thrown.
    
    @tparam REAL should be '__half', 'float', or 'double'.
   */
  template <typename REAL>
  void xpose(const gpumat<REAL> &x, gpumat<REAL> &tx)
  {
    err::check_card(x, tx);
    
    const len_t m = x.nrows();
    const len_t n = x.ncols();
    
    if (m != tx.ncols() || n != tx.nrows())
      tx.resize(n, m);
    
    auto cbh = x.get_card()->blas_handle();
    
    gpublas_status_t check = gpublas::geam(cbh, GPUBLAS_OP_T, GPUBLAS_OP_N, n, m, (REAL)1.0, x.data_ptr(), m, (REAL) 0.0, tx.data_ptr(), n, tx.data_ptr(), n);
    gpublas::err::check_ret(check, "geam");
  }
  
  /// \overload
  template <typename REAL>
  gpumat<REAL> xpose(const gpumat<REAL> &x)
  {
    gpumat<REAL> tx(x.get_card(), x.ncols(), x.nrows());
    xpose(x, tx);
    return tx;
  }
}
}


#endif
