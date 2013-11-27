

#ifndef LR4CTR_MPI_AUC_H
#define LR4CTR_MPI_AUC_H

#include <mpi.h>
#include "prediction.h"

/**
 * @brief 
 *
 * @param 
 * @param 
 *
 * @return 
 */
int mpi_peval (int num_procs, int rank_id, char *auc_file);

#endif
