

CPPFLAGS = -O2
LDFLAGS = -Xlinker "-(" -lpthread -lcrypto -lz -lm -lalgo_common -llog4cpp -lrt -Xlinker "-)" -L/home/a/lib64  
INCLUDES = -I/home/a/share/algo1/openmpi-current/include -I/home/a/include -I/home/a/include/log4cpp/ -I/home/a/include/algo_common/ -I./include

.PHONY:all
all: mpi-pqlr-score mpi-peval-quadLR
	cp mpi-peval-quadLR ../../../../mpi-quadLR/bin/
	cp mpi-pqlr-score ../../../../mpi-quadLR/bin/

%.o : %.cpp
	 mpic++ $(CPPFLAGS) -c $< -o $@ $(INCLUDES)

mpi-pqlr-score: score.o Log_r.o
	mpic++ $^ $(LDFLAGS) -o $@  $(INCLUDES)

mpi-peval-quadLR: mpi_peval_main.o mpi_peval.o prediction.o Log_r.o
	mpic++ $^ $(LDFLAGS) -o $@  $(INCLUDES)

.PHONY:clean
clean:
	rm -fr *.o
	rm -f mpi-pqlr-score ../../../../mpi-featsel/bin/mpi-pqlr-score
	rm -f mpi-peval-quadLR ../../../../mpi-featsel/bin/mpi-peval-quadLR
