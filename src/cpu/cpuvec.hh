#ifndef FML_CPU_CPUVEC_H
#define FML_CPU_CPUVEC_H
#pragma once


#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <stdexcept>

#include "../omputils.hh"
#include "../univec.hh"


/**
 * @brief Vector class for data held on a single CPU. 
 * @tparam T should be 'int', 'float' or 'double'.
 */
template <typename T>
class cpuvec : public univec<T>
{
  public:
    cpuvec();
    cpuvec(len_t size);
    cpuvec(T *data, len_t size, bool free_on_destruct=false);
    cpuvec(const cpuvec &x);
    ~cpuvec();
    
    void resize(len_t size);
    void set(T *data, len_t size, bool free_on_destruct=false);
    cpuvec<T> dupe() const;
    
    void print(uint8_t ndigits=4, bool add_final_blank=true) const;
    void info() const;
    
    void fill_zero();
    void fill_one();
    void fill_val(const T v);
    void fill_linspace(const T start, const T stop);
    
    void scale(const T s);
    void rev();
    
    const T operator()(len_t i) const; // getter
    T& operator()(len_t i); // setter
    
    bool operator==(const cpuvec<T> &x) const;
    bool operator!=(const cpuvec<T> &x) const;
    
    cpuvec<T>& operator=(const cpuvec<T> &x);
  
  private:
    void free();
    void check_params(len_t size);
};



// -----------------------------------------------------------------------------
// public
// -----------------------------------------------------------------------------

// constructors/destructor

template <typename T>
cpuvec<T>::cpuvec()
{
  this->_size = 0;
  this->data = NULL;
  
  this->free_data = true;
}



template <typename T>
cpuvec<T>::cpuvec(len_t size)
{
  check_params(size);
  
  const size_t len = (size_t) size * sizeof(T);
  this->data = (T*) std::malloc(len);
  if (this->data == NULL)
    throw std::bad_alloc();
  
  this->_size = size;
  
  this->free_data = true;
}



template <typename T>
cpuvec<T>::cpuvec(T *data_, len_t size, bool free_on_destruct)
{
  check_params(size);
  
  this->_size = size;
  this->data = data_;
  
  this->free_data = free_on_destruct;
}



template <typename T>
cpuvec<T>::cpuvec(const cpuvec<T> &x)
{
  this->_size = x.size();
  this->data = x.data_ptr();
  
  this->free_data = false;
}



template <typename T>
cpuvec<T>::~cpuvec()
{
  this->free();
}



// memory management

template <typename T>
void cpuvec<T>::resize(len_t size)
{
  check_params(size);
  
  if (this->_size == size)
    return;
  
  const size_t len = (size_t) size * sizeof(T);
  
  void *realloc_ptr = realloc(this->data, len);
  if (realloc_ptr == NULL)
    throw std::bad_alloc();
  
  this->data = (T*) realloc_ptr;
  
  this->_size = size;
}



template <typename T>
void cpuvec<T>::set(T *data, len_t size, bool free_on_destruct)
{
  check_params(size);
  
  this->free();
  
  this->_size = size;
  this->data = data;
  
  this->free_data = free_on_destruct;
}



template <typename T>
cpuvec<T> cpuvec<T>::dupe() const
{
  cpuvec<T> cpy(this->_size);
  
  const size_t len = (size_t) this->_size * sizeof(T);
  memcpy(cpy.data_ptr(), this->data, len);
  
  return cpy;
}



// printers

template <typename T>
void cpuvec<T>::print(uint8_t ndigits, bool add_final_blank) const
{
  for (len_t i=0; i<this->_size; i++)
    this->printval(this->data[i], ndigits);
  
  putchar('\n');
  if (add_final_blank)
    putchar('\n');
}



template <typename T>
void cpuvec<T>::info() const
{
  printf("# cpuvec");
  printf(" %d", this->_size);
  printf(" type=%s", typeid(T).name());
  printf("\n");
}



// fillers

template <typename T>
void cpuvec<T>::fill_zero()
{
  const size_t len = (size_t) this->_size * sizeof(T);
  memset(this->data, 0, len);
}



template <typename T>
void cpuvec<T>::fill_one()
{
  this->fill_val((T) 1);
}



template <typename T>
void cpuvec<T>::fill_val(const T v)
{
  #pragma omp parallel for simd if(this->_size > omputils::OMP_MIN_SIZE)
  for (len_t i=0; i<this->_size; i++)
    this->data[i] = v;
}



template <>
inline void cpuvec<int>::fill_linspace(const int start, const int stop)
{
  if (start == stop)
    this->fill_val(start);
  else
  {
    const float v = (stop-start)/((float) this->_size - 1);
    
    #pragma omp parallel for simd if(this->_size > omputils::OMP_MIN_SIZE)
    for (len_t i=0; i<this->_size; i++)
      this->data[i] = (int) roundf(v*((float) i) + start);
  }
}

template <typename REAL>
void cpuvec<REAL>::fill_linspace(const REAL start, const REAL stop)
{
  if (start == stop)
    this->fill_val(start);
  else
  {
    const REAL v = (stop-start)/((REAL) this->_size - 1);
    
    #pragma omp parallel for simd if(this->_size > omputils::OMP_MIN_SIZE)
    for (len_t i=0; i<this->_size; i++)
      this->data[i] = v*((REAL) i) + start;
  }
}




template <typename T>
void cpuvec<T>::scale(const T s)
{
  #pragma omp parallel for simd if(this->_size > omputils::OMP_MIN_SIZE)
  for (len_t i=0; i<this->_size; i++)
    this->data[i] *= s;
}



template <typename T>
void cpuvec<T>::rev()
{
  len_t j = this->_size - 1;
  
  for (len_t i=0; i<this->_size/2; i++)
  {
    const T tmp = this->data[i];
    this->data[i] = this->data[j];
    this->data[j] = tmp;
    j--;
  }

}



// operators

template <typename T>
const T cpuvec<T>::operator()(len_t i) const
{
  this->check_index(i);
  
  return this->data[i];
}

template <typename T>
T& cpuvec<T>::operator()(len_t i)
{
  this->check_index(i);
  
  return this->data[i];
}



template <typename T>
bool cpuvec<T>::operator==(const cpuvec<T> &x) const
{
  if (this->_size != x.size())
    return false;
  else if (this->data == x.data_ptr())
    return true;
  
  const T *x_d = x.data_ptr();
  for (len_t i=0; i<this->_size; i++)
  {
    if (this->data[i] != x_d[i])
      return false;
  }
  
  return true;
}

template <typename T>
bool cpuvec<T>::operator!=(const cpuvec<T> &x) const
{
  return !(*this == x);
}



template <typename T>
cpuvec<T>& cpuvec<T>::operator=(const cpuvec<T> &x)
{
  this->_size = x.size();
  this->data = x.data_ptr();
  
  this->free_data = false;
  return *this;
}



// -----------------------------------------------------------------------------
// private
// -----------------------------------------------------------------------------

template <typename T>
void cpuvec<T>::free()
{
  if (this->free_data && this->data)
  {
    std::free(this->data);
    this->data = NULL;
  }
}



template <typename REAL>
void cpuvec<REAL>::check_params(len_t size)
{
  if (size < 0)
    throw std::runtime_error("invalid dimensions");
}


#endif
