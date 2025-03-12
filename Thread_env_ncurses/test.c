#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <gsl/gsl_rng.h>
#include <pthread.h>
#include <sys/sysinfo.h>

int main() {
    int nprocs = get_nprocs();
    printf("Nombre de processeur disponibles : %d\n",nprocs);
    return EXIT_SUCCESS;
}