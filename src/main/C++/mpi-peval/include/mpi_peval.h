/** 
 * @copyright (c) 2012 Taobao.com, Inc. All Rights Reserved
 * @author : xiaowen.zl
 * @fax    : +86.10.815.72428
 * @e-mail : xiaowen.zl@taobao.com
 * @date   : 2012-11-26 - 20:36
 * @version: 1.0.0.1
 * 
 * @file   : mpi_peval.h
 * @brief  :
 */

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
