# Project: mpiLR
# Makefile created by Dev-C++ 5.5.2

CPPFLAGS = -O2
LDFLAGS = -Xlinker "-(" -lpthread -lcrypto -lz -lm -lalgo_common -llog4cpp -lrt -Xlinker "-)" -L/home/a/lib64  
INCLUDES = -I/home/a/share/algo1/openmpi-current/include -I/home/a/include -I/home/a/include/log4cpp/ -I/home/a/include/algo_common/ -I./include


.PHONY:all
all: mpiLR
	cp mpiLR ../../../../mpi-lr/bin/

%.o : %.cpp
	 mpic++ $(CPPFLAGS) -c $< -o $@ $(INCLUDES)

mpiLR: main.o inslookup.o logreg.o OWLQN.o TerminationCriterion.o
	mpic++ $^ $(LDFLAGS) -o $@  $(INCLUDES)
	
.PHONY:clean
clean:
	rm -fr *.o
	rm -f mpiLR../../../../mpi-lr/bin/mpiLR