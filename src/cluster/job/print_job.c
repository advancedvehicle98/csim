#include <cluster/defs.h>
#include <cluster/job.h>
#include <cluster/node.h>

#include <stdio.h>


void
cluster_print_job( __IN__ const job_t *j )
{
	const char *blank = " ";
	
	const char *cpu_req     = _requires_cpu( j )     ? " CPU"     : blank;
	const char *cpu_vec_req = _requires_cpu_vec( j ) ? " CPU_VEC" : blank;
	const char *gpu_req     = _requires_gpu( j )     ? " GPU"     : blank;
	const char *npu_req     = _requires_npu( j )     ? " NPU"     : blank;
	const char *tpu_req     = _requires_tpu( j )     ? " TPU"     : blank;
	const char *fpga_req    = _requires_fpga( j )    ? " FPGA"    : blank;
	const char *asic_req    = _requires_asic( j )    ? " ASIC"    : blank;

	const char *assigned_node = j->assigned_node ? j->assigned_node->name : "---";
	
	_DEBUG_PRINTF( "-=-=-=-=-=-=-=-=-=-( %s на узле %s )-=-=-=-=-=-=-=-=-=-=-=-=-=-\n"
				   "                Время исполнения:             %u " _TQ "\n"
				   "                Ожидаемое затраченное время:  %u " _TQ "\n"
				   "                Макс. допустимое время:       %u " _TQ "\n"
				   "                Время ожидания:               %u " _TQ "\n"
				   "                Макс. время ожидания:         %u " _TQ "\n"
				   "                Временная погрешность:        %u " _TQ "\n"
				   "-------------------------------------------------------------\n"
				   "                Память:                       %u " _MQ "\n"
				   "                Кол-во ядер:                  %u\n"
				   "                Кол-во потоков:               %u\n"
				   "                Приоритет:                    %u\n"
				   "                Требуемые фукнции: " _FEATURES_FMT "\n",
				   j->name, assigned_node,
				   j->exec_time, j->estimated_time, j->max_exec_time,
				   j->wait_time, j->max_wait_time, j->max_wait_time,
				   j->mem_size, j->cpu_count, j->thread_count, j->priority,
				   cpu_req, cpu_vec_req, gpu_req, npu_req, tpu_req, fpga_req, asic_req
				   );
}
