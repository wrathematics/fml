#ifndef FML_PAR_COMM_H
#define FML_PAR_COMM_H


#define OMPI_SKIP_MPICXX 1
#include <mpi.h>

#include <cstdarg>
#include <cstdio>
#include <stdexcept>
#include <vector>


class comm
{
  public:
    comm();
    comm(const comm &c);
    
    void inherit_comm(MPI_Comm comm);
    
    void finalize();
    
    void printf(int rank, const char *fmt, ...);
    void info();
    
    bool rank0();
    std::vector<int> jid(int n);
    void barrier();
    
    void send(int n, int *data, int dest, int tag=0);
    void send(int n, float *data, int dest, int tag=0);
    void send(int n, double *data, int dest, int tag=0);
    void recv(int n, int *data, int source, int tag=0);
    void recv(int n, float *data, int source, int tag=0);
    void recv(int n, double *data, int source, int tag=0);
    
    void allreduce(int n, int *data);
    void allreduce(int n, float *data);
    void allreduce(int n, double *data);
    
    void reduce(int n, int *data, int root=0);
    void reduce(int n, float *data, int root=0);
    void reduce(int n, double *data, int root=0);
    
    void bcast(int n, int *data, int root);
    void bcast(int n, float *data, int root);
    void bcast(int n, double *data, int root);
    
    MPI_Comm get_comm() const {return _comm;};
    int rank() const {return _rank;};
    int size() const {return _size;};
  
  protected:
    MPI_Comm _comm;
    int _rank;
    int _size;
  
  private:
    void init();
    void set_metadata();
    void check_ret(int ret);
};



// -----------------------------------------------------------------------------
// public
// -----------------------------------------------------------------------------

// constructors/destructor

comm::comm()
{
  init();
  
  _comm = MPI_COMM_WORLD;
  
  set_metadata();
}



comm::comm(const comm &c)
{
  _comm = c.get_comm();
  _rank = c.rank();
  _size = c.size();
}



void comm::inherit_comm(MPI_Comm comm)
{
  _comm = comm;
  set_metadata();
}



void comm::finalize()
{
  int ret = MPI_Finalize();
  check_ret(ret);
}



// printers

void comm::printf(int rank, const char *fmt, ...)
{
  if (_rank == rank)
  {
    va_list args;
    
    va_start(args, fmt);
    vfprintf(stdout, fmt, args);
    va_end(args);
  }
}



void comm::info()
{
  printf(0, "## MPI on %d ranks\n\n", _size);
}



// misc

bool comm::rank0()
{
  return (_rank == 0);
}



std::vector<int> comm::jid(int n)
{
  std::vector<int> ret;
  
  if (n > _size)
  {
    int local = n / _size;
    int rem = n % _size;
    
    if (rem == 0 || (_rank < (_size - rem)))
    {
      ret.resize(local);
      for (int i=0; i<local; i++)
        ret[i] = i + (_rank*local);
    }
    else
    {
      ret.resize(local+1);
      for (int i=0; i<=local; i++)
        ret[i] = i + (_rank*(local+1)) - (_size - rem);
    }
  }
  else
  {
    if (n > _rank)
    {
      ret.resize(1);
      ret[0] = _rank;
    }
    else
      ret.resize(0);
  }
  
  return ret;
}



void comm::barrier()
{
  int ret = MPI_Barrier(_comm);
  check_ret(ret);
}



// send/recv

void comm::send(int n, int *data, int dest, int tag)
{
  int ret = MPI_Send(data, n, MPI_INT, dest, tag, _comm);
  check_ret(ret);
}

void comm::send(int n, float *data, int dest, int tag)
{
  int ret = MPI_Send(data, n, MPI_FLOAT, dest, tag, _comm);
  check_ret(ret);
}

void comm::send(int n, double *data, int dest, int tag)
{
  int ret = MPI_Send(data, n, MPI_DOUBLE, dest, tag, _comm);
  check_ret(ret);
}



void comm::recv(int n, int *data, int source, int tag)
{
  int ret = MPI_Recv(data, n, MPI_INT, source, tag, _comm, MPI_STATUS_IGNORE);
  check_ret(ret);
}

void comm::recv(int n, float *data, int source, int tag)
{
  int ret = MPI_Recv(data, n, MPI_FLOAT, source, tag, _comm, MPI_STATUS_IGNORE);
  check_ret(ret);
}

void comm::recv(int n, double *data, int source, int tag)
{
  int ret = MPI_Recv(data, n, MPI_DOUBLE, source, tag, _comm, MPI_STATUS_IGNORE);
  check_ret(ret);
}



// reductions

void comm::allreduce(int n, int *data)
{
  int ret = MPI_Allreduce(MPI_IN_PLACE, data, n, MPI_INT, MPI_SUM, _comm);
  check_ret(ret);
}

void comm::allreduce(int n, float *data)
{
  int ret = MPI_Allreduce(MPI_IN_PLACE, data, n, MPI_FLOAT, MPI_SUM, _comm);
  check_ret(ret);
}

void comm::allreduce(int n, double *data)
{
  int ret = MPI_Allreduce(MPI_IN_PLACE, data, n, MPI_DOUBLE, MPI_SUM, _comm);
  check_ret(ret);
}



void comm::reduce(int n, int *data, int root)
{
  int ret = MPI_Reduce(MPI_IN_PLACE, data, n, MPI_INT, MPI_SUM, root, _comm);
  check_ret(ret);
}

void comm::reduce(int n, float *data, int root)
{
  int ret = MPI_Reduce(MPI_IN_PLACE, data, n, MPI_FLOAT, MPI_SUM, root, _comm);
  check_ret(ret);
}

void comm::reduce(int n, double *data, int root)
{
  int ret = MPI_Reduce(MPI_IN_PLACE, data, n, MPI_DOUBLE, MPI_SUM, root, _comm);
  check_ret(ret);
}



// broadcasters

void comm::bcast(int n, int *data, int root)
{
  int ret = MPI_Bcast(data, n, MPI_INT, root, _comm);
  check_ret(ret);
}

void comm::bcast(int n, float *data, int root)
{
  int ret = MPI_Bcast(data, n, MPI_FLOAT, root, _comm);
  check_ret(ret);
}

void comm::bcast(int n, double *data, int root)
{
  int ret = MPI_Bcast(data, n, MPI_DOUBLE, root, _comm);
  check_ret(ret);
}



// -----------------------------------------------------------------------------
// private
// -----------------------------------------------------------------------------

void comm::init()
{
  int ret;
  int flag;
  
  ret = MPI_Initialized(&flag);
  check_ret(ret);
  
  if (!flag)
  {
    ret = MPI_Init(NULL, NULL);
    check_ret(ret);
  }
}



void comm::set_metadata()
{
  int ret;
  
  ret = MPI_Comm_rank(_comm, &_rank);
  check_ret(ret);
  
  ret = MPI_Comm_size(_comm, &_size);
  check_ret(ret);
}



void comm::check_ret(int ret)
{
  if (ret != MPI_SUCCESS && _rank == 0)
  {
    int slen;
    char s[MPI_MAX_ERROR_STRING];
    
    MPI_Error_string(ret, s, &slen);
    throw std::runtime_error(s);
  }
}


#endif
