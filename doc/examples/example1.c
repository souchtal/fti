
int main(int argc, char *argv[])
{
    int rank, nbProcs, nbLines, i, M, arg;
    double wtime, *h, *g, memSize, localerror, globalerror = 1;

    MPI_Init(&argc, &argv);
    FTI_Init(argv[2], MPI_COMM_WORLD);

    MPI_Comm_size(FTI_COMM_WORLD, &nbProcs);
    MPI_Comm_rank(FTI_COMM_WORLD, &rank);

    arg = atoi(argv[1]);
    M = (int)sqrt((double)(arg * 1024.0 * 512.0 * nbProcs)/sizeof(double));
    nbLines = (M / nbProcs)+3;
    h = (double *) malloc(sizeof(double *) * M * nbLines);
    g = (double *) malloc(sizeof(double *) * M * nbLines);
    initData(nbLines, M, rank, g);
    memSize = M * nbLines * 2 * sizeof(double) / (1024 * 1024);

    if (rank == 0) {
        printf("Local data size is %d x %d = %f MB (%d).\n", M, nbLines, memSize, arg);
        printf("Target precision : %f \n", PRECISION);
        printf("Maximum number of iterations : %d \n", ITER_TIMES);
    }

    FTI_Protect(0, &i, 1, FTI_INTG);
    FTI_Protect(1, h, M*nbLines, FTI_DBLE);
    FTI_Protect(2, g, M*nbLines, FTI_DBLE);

    wtime = MPI_Wtime();
    for (i = 0; i < ITER_TIMES; i++) {
        int checkpointed = FTI_Snapshot();
        localerror = doWork(nbProcs, rank, M, nbLines, g, h);
        if (((i%ITER_OUT) == 0) && (rank == 0)) {
            printf("Step : %d, error = %f\n", i, globalerror);
        }
        if ((i%REDUCE) == 0) {
            MPI_Allreduce(&localerror, &globalerror, 1, MPI_DOUBLE, MPI_MAX, FTI_COMM_WORLD);
        }
        if(globalerror < PRECISION) {
            break;
        }
    }
    if (rank == 0) {
        printf("Execution finished in %lf seconds.\n", MPI_Wtime() - wtime);
    }

    free(h);
    free(g);

    FTI_Finalize();
    MPI_Finalize();
    return 0;
}
