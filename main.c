#include <mpi.h>
#include <stdio.h>
#include <time.h>

int main(){
    clock_t start_l, end_l, start_b, end_b;
    double time_l, time_b, latency, bandwidth;

    int rank, size, tmp;
    int buf[1000001];
    MPI_Init(NULL,NULL);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    MPI_Comm_size(MPI_COMM_WORLD,&size);

    // measure latency
    if(rank == 0) {
        start_l = clock();
        for(int i = 0; i < 1000001; i++) {
            MPI_Send(&tmp,1,MPI_INT,1,0,MPI_COMM_WORLD);
            MPI_Recv(&tmp,1,MPI_INT,1,0, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
        }
        end_l = clock();
        time_l = ((double) (end_l - start_l)) / CLOCKS_PER_SEC / 2;

    }
    else if(rank == 1) {
        for(int i = 0; i < 1000001; i++) {
            MPI_Recv(&tmp,1,MPI_INT,0,0, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
            MPI_Send(&tmp,1,MPI_INT,0,0,MPI_COMM_WORLD);
        }
    }

    // measure bandwidth
    if(rank == 0) {
        start_b = clock();
        MPI_Send(&buf,1000001,MPI_INT,1,0,MPI_COMM_WORLD);
        MPI_Recv(&buf,1000001,MPI_INT,1,0, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
        end_b = clock();
        time_b = ((double) (end_b - start_b)) / CLOCKS_PER_SEC / 2;
        latency = (time_l - time_b);    // in us
        bandwidth = 4 * 1000001 / (time_b - (latency / 1e6)) / (1024 * 1024);     // in MiB/sec
        printf("1000001 * (int+latency): %f sec\n", time_l);
        printf("(1000001 * int) + latency: %f sec\n", time_b);
        printf("Latency: %f us\n", latency);
        printf("Bandwidth: %f MiB/sec\n", bandwidth);
    }
    else if(rank == 1) {
        MPI_Recv(&buf,1000001,MPI_INT,0,0, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
        MPI_Send(&buf,1000001,MPI_INT,0,0,MPI_COMM_WORLD);
    }

    MPI_Finalize();

    return 0;
}