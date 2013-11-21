

CPPFLAGS = 
LDFLAGS = -Xlinker "-(" -lpthread -lcrypto -lz -lm -lalgo_common -llog4cpp -lrt -Xlinker "-)" -L/home/a/lib64  
INCLUDES = -I/home/a/share/algo1/openmpi-current/include -I/home/a/include -I/home/a/include/log4cpp/ -I/home/a/include/algo_common/ -I./include

.PHONY:all
all: mpi-pfs-score mpi-peval-featsel
	cp mpi-peval-featsel ../../../../mpi-featsel/bin/
	cp mpi-pfs-score ../../../../mpi-featsel/bin/

%.o : %.cpp
	 mpic++ $(CPPFLAGS) -c $< -o $@ $(INCLUDES)

mpi-pfs-score: score.o Log_r.o
	mpic++ $^ $(LDFLAGS) -o $@  $(INCLUDES)

mpi-peval-featsel: mpi_peval_main.o mpi_peval.o prediction.o Log_r.o
	mpic++ $^ $(LDFLAGS) -o $@  $(INCLUDES)

.PHONY:clean
clean:
	rm -fr *.o
	rm -f mpi-pfs-score ../../../../mpi-lr/bin/mpi-pfs-score
	rm -f mpi-peval-featsel ../../../../mpi-lr/bin/mpi-peval-featsel
