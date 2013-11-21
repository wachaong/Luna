#ifndef DATATYPEDEFINE_H
#define DATATYPEDEFINE_H

#ifndef NULL
#define NULL 0
#endif
/*
#define  unsigned int //特征索引类型
#define weightidx_t unsigned int //权重索引类型
#define infilefloat_t double //数据文件中特征类型
#define computfloat_t double //计算时使用的特征类型
*/
//typedef unsigned int featureidx_t; //特征索引类型, 系数特征数据大小依赖此值

typedef long long weightidx_t; //权重索引类型
typedef double computfloat_t; //计算时使用的特征类型
//typedef double infilefloat_t; //数据文件中特征类型
//typedef unsigned int infileclk_t;//数据文件中clk,nonclk类型
//typedef unsigned int insparsefileidx_t; //稀疏特征文件中特征索引类型
//typedef unsigned int inselectidx_t; //select_index用的索引类型

//数据文件格式定义
typedef long long size_file_t;//数据大小索引类型
typedef double infile_clk_t;//clk, nonclk类型
typedef unsigned int infile_binfeanum_t;//bin feature number类型
typedef unsigned int infile_binidx_t;//bin feature index 类型
typedef unsigned int infile_multifeanum_t;// multiple feature number类型
typedef unsigned int infile_multiidx_t;// multiple feature index类型
typedef double infile_multivaluefloat_t;// multiple feature value 类型
//2012-01-18添加，非稀疏数据
typedef unsigned int infile_densefeanum_t;//非稀疏特征个数类型
typedef double infile_densefeavalue_t;//非稀疏特征类型
//注意,infile_densefeanum_t需要和endlinesign_t是相同的实际类型，用来判断是否有densefeanum字段
typedef unsigned int endlinesign_t;//每行结束符类型
#define ENDLINESIGN ((unsigned int)(-1))
//2013-03-27添加,稠密矩阵数据
typedef double densematrix_comput_t;//稠密矩阵运算类型

//读取文件相关
//读取文本文件一行的最大长度
#define MAX_READ_FILE_LINE 1024000
//模型文件格式
#define WRITE_MODEL_DELIMITER "\t"
#define READ_INITIALMODEL_DELIMITER "\t \1"
//读取enable_file的参数
//update_enable_file 第几行开始是数据，应该为1
#define UPDATE_ENABLE_FILE_STARTLINEID 1
//小于ENABLE_BINARY_BOUNDRY的值表示不选中，大于等于的表示选中
#define ENABLE_BINARY_BOUNDRY 0.01

//字符串相关
#define MAXSTRLEN 256
//存储文件名的字符串最大长度
#define MAX_FILENAME_LEN 256
//临时字符串长度，存储线程名
#define TEMP_STR_LEN 256

//默认参数
//输入文件格式相关
#define DEFAULT_XINPUT_NONCLKID 0
#define DEFAULT_XINPUT_CLKID sizeof(infile_clk_t)
#define DEFAULT_XINPUT_FEATURESTARTID 2*sizeof(infile_clk_t)
//2013-04-26 数据划分参数
#define DEFAULT_DATADIVIDE_INSGROUPLEN 10
#define DEFAULT_DATADIVIDE_BATCHGROUPNUM 100000
//默认输出模型文件名
#define DEFAULT_MODEL_FILENAME "model"
//优化相关
#define DEFAULT_ITERNUMPERSAVE	15
#define DEFAULT_LAMBDA 1
#define DEFAULT_BETA 1
#define DEFAULT_M	100
#define DEFAULT_MINIDECREASERATE 0.0001
#define DEFAULT_MAXITERNUM 1000
#define DEFAULT_FIRSTSTEPSIZE 1
#define DEFAULT_FIRSTSTEPMAXNORM 10
#define DEFAULT_STEP_MAXNORMMULTIPLIER 0
#define DEFAULT_STEP_DECREASERATE 0.1
#define DEFAULT_STEP_MAXTRYTIME 10
#define DEFAULT_STEP_TOLERANCERATE 0.01
#define DEFAULT_LEN_LBFGS_QUEUE 10
//文件格式
#define DEFAULT_INITMODEL_STARTLINEID 10 //初始模型文件第几行开始是数据
#define DEFAULT_GUIDE_ITERNUM 0

//L-BFGS相关
#define MAX_RHO 1e100
#define MIN_INVERSE_RHO 1e-100
//最小除数
#define MIN_DENOMINATOR 1e-100
//最小log数
#define MIN_LOG_NUM 1e-100
//最大exp数
#define MAX_EXP_NUM 300

//线程每次等待时间
#define THREAD_WAIT_TIME_SEC 1 //单位为秒,s
#define THREAD_WAIT_TIME_NSEC 1000000 //单位为纳秒
#define THREAD_TASK_INITIAL_COUNT 1 //线程任务计数初始值
#define DEFAULT_THREADNUM 1 //默认线程数

//MPI相关
#define MPI_MASTER_NODE 0 //MPI主节点序号
#define MPI_TASKTYPE_EVAL 1 //计算梯度和负斯然
#define MPI_TASKTYPE_ABORT 0 //退出
typedef double mpi_vec_float_t; //定义mpi向量发送接收缓存类型
#define MPI_VEC_FLOAT MPI_DOUBLE //和上面对应

computfloat_t inline trauncate_exp_num(computfloat_t exp_num){
	if (exp_num > MAX_EXP_NUM){
		return MAX_EXP_NUM;
	}
	if (exp_num < - MAX_EXP_NUM){
		return - MAX_EXP_NUM;
	}
	return exp_num;
}

#endif
