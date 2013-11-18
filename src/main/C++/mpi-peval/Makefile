

CPPFLAGS = 
LDFLAGS = -Xlinker "-(" -lpthread -lcrypto -lz -lm -lalgo_common -llog4cpp -lrt -Xlinker "-)" -L/home/a/lib64  
INCLUDES = -I/home/a/share/algo1/openmpi-current/include -I/home/a/include -I/home/a/include/log4cpp/ -I/home/a/include/algo_common/ -I./include

.PHONY:all
all: mpi-plr-score mpi-peval
	cp mpi-peval ../bin/
	cp mpi-plr-score ../bin/

%.o : %.cpp
	 mpic++ $(CPPFLAGS) -c $< -o $@ $(INCLUDES)

mpi-plr-score: score.o Log_r.o
	mpic++ $^ $(LDFLAGS) -o $@  $(INCLUDES)

mpi-peval: mpi_peval_main.o mpi_peval.o prediction.o Log_r.o
	mpic++ $^ $(LDFLAGS) -o $@  $(INCLUDES)

.PHONY:clean
clean:
	rm -fr *.o
	rm -f mpi-plr-score ../bin/mpi-plr-score
	rm -f mpi-peval ../bin/mpi-peval
