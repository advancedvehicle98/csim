#include <cluster/node.h>

#include <stdio.h>


void
cluster_get_feature_string( __OUT__       char           str[ FEATURE_STR_SIZE ],
							__IN__  const feature_mask_t mask )
{
	const char *blank = " ";
	
	const char *cpu_req     = _requires_cpu( mask )     ? " CPU"     : blank;
	const char *cpu_vec_req = _requires_cpu_vec( mask ) ? " CPU_VEC" : blank;
	const char *gpu_req     = _requires_gpu( mask )     ? " GPU"     : blank;
	const char *npu_req     = _requires_npu( mask )     ? " NPU"     : blank;
	const char *tpu_req     = _requires_tpu( mask )     ? " TPU"     : blank;
	const char *fpga_req    = _requires_fpga( mask )    ? " FPGA"    : blank;
	const char *asic_req    = _requires_asic( mask )    ? " ASIC"    : blank;

	snprintf( str, FEATURE_STR_SIZE, _FEATURES_FMT,
			  cpu_req, cpu_vec_req, gpu_req, npu_req,
			  tpu_req, fpga_req, asic_req );
}
