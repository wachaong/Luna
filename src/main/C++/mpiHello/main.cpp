#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main(int argc, char *argv[]) {
  int numprocs, myid, namelen, localrank, sourceid, destid, rec_token,tag=1, cast=0;
  int send[4] = {0};
  int recv[4] = {0};
  char processor_name[MPI_MAX_PROCESSOR_NAME];
  MPI_Status status;

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
  MPI_Comm_rank(MPI_COMM_WORLD, &myid);
  MPI_Comm_rank(MPI_COMM_SELF, &localrank);
  MPI_Get_processor_name(processor_name, &namelen);

  destid = (myid + 1)% numprocs;
  sourceid = (myid-1 + numprocs)%numprocs;
  MPI_Send(&myid, 1, MPI_INT, destid, tag, MPI_COMM_WORLD);
  MPI_Recv(&rec_token, 1, MPI_INT, sourceid, tag, MPI_COMM_WORLD, &status);
  printf("Process %d on %s out of %d receive hello from %d\n", myid, processor_name, numprocs, rec_token);

  MPI_Finalize();
  return 0;
}
