#include <stdio.h>
#include <stdlib.h>
#include<vector>
#include <mpi.h>
using namespace std;

typedef std::vector<double> DblVec;

int handler(int size, int id, DblVec& b, int command){
	DblVec a(size);
	for(int i = 0; i < size; i++) a[i] = id;
	MPI_Bcast(&command, 1, MPI_INT, 0, MPI_COMM_WORLD);	
	if(command == 0) return 0;
	if(id != 0){

//		MPI_Status status;
//		MPI_Recv(&value,1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
		if(command == 1){
			printf("%d:HAHA, I've received command 1\n", id);
		}
		if(command == 0){
			return 0;
		}
	}
	MPI_Reduce(&a[0], &b[0], size, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
	return 1;
}

void solve(int size, DblVec& b){
	for(int i = 0; i < 4; i++){
		printf("CALL Eval %d\n", i);
		handler(size, 0, b, 1);
	}
	handler(size, 0, b, 0);
}
int main(int argc, char *argv[]) {
  int numprocs, myid, namelen, localrank;
  char processor_name[MPI_MAX_PROCESSOR_NAME];
  MPI_Status status;

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
  MPI_Comm_rank(MPI_COMM_WORLD, &myid);
  MPI_Comm_rank(MPI_COMM_SELF, &localrank);
  MPI_Get_processor_name(processor_name, &namelen);
  
  int size = 4;
  DblVec b(size);
  
  if(myid ==  0){
  	solve(size, b);
  }
  else{
  	while(1){
  		int ret  = handler(size, myid, b, 0); 
  		if (ret == 0) break;
  	}
  }
  
  
  
  if(myid == 0){
	  for(int i = 0; i < size; i++){
	   		printf("%f\n", b[i]);
	  }
  }
  MPI_Finalize();
  return 0;
}
