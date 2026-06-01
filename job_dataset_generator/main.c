#include <cluster/node.h>

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


#define STR_EQUAL( L, R ) ( strcmp( L, R ) == 0 )

#define MAX_OUT_FILE_NAME 64


bool        p_rewrite          = false;

size_t      p_job_count        = 20;

const char *p_out_file_name    = "jobs";

uint64_t    p_finish_time      = 40;

uint64_t    p_min_time         = 5;
uint64_t    p_max_time         = 20;

uint64_t    p_min_wait         = 10;
uint64_t    p_max_wait         = 20;

uint64_t    p_time_var         = 5;

uint32_t    p_min_mem          = 32;
uint32_t    p_max_mem          = 1024;

uint32_t    p_min_cpu          = 1;
uint32_t    p_max_cpu          = 64;

uint32_t    p_min_thread       = 1;
uint32_t    p_max_thread       = 64;

uint32_t    p_min_prio         = 9;
uint32_t    p_max_prio         = 0;

uint64_t    p_allowed_features = 1;

bool        p_feature_not_rand = false;

bool        p_to_stdout        = false;


static size_t interpret( const char *opt, const char *tail[], const size_t tail_len );
static uint32_t parse_args( const size_t argc, const char *argv[] );
static void print_help( void );


uint32_t
main( const unsigned  argc,
	  const char     *argv[] )
{
	if ( parse_args( argc, argv ) ) {
		print_help();
		return EXIT_FAILURE;
	}

	FILE *f;

	if ( p_to_stdout )
		f = stdout;
	else if ( ! p_rewrite )
		f = fopen( p_out_file_name, "wx" );
	else
		f = fopen( p_out_file_name, "w" );

	if ( ! f ) {
		printf( "%s уже существует\n", p_out_file_name );
		return EXIT_FAILURE;
	}

	size_t job_count = p_job_count;

	srand( time( NULL ) );
	
	while ( job_count-- ) {
		fprintf( f,
				 "%u job_%u %u 0 %u %u %u %u %u %u %u\n",
				 rand() % p_finish_time,
				 p_job_count - job_count,
				 rand() % ( p_max_time - p_min_time ) + p_min_time,
				 rand() % ( p_max_wait - p_min_wait ) + p_min_wait,
				 rand() % p_time_var,
				 rand() % ( p_max_mem - p_min_mem ) + p_min_mem,
				 rand() % ( p_max_cpu - p_min_cpu ) + p_min_cpu,
				 rand() % ( p_max_thread - p_min_thread ) + p_min_thread,
				 rand() % ( p_min_prio - p_max_prio ) + p_max_prio,
				 ( p_feature_not_rand
				   ? p_allowed_features
				   : rand() & p_allowed_features | NODE_FEATURE_CPU) );
	}

	fclose( f );
	
	return EXIT_SUCCESS;
}


size_t
interpret( const char   *opt,
		   const char   *tail[],
		   const size_t  tail_len )
{
	if ( STR_EQUAL( opt, "-o" ) ) {
		if ( ! tail[ 0 ] ) return 0;
		
		p_out_file_name = tail[ 0 ];
		return 2;
	}

	if ( STR_EQUAL( opt, "-c" ) ) {
		if ( ! tail[ 0 ] ) return 0;
		
		p_job_count = atol( tail[ 0 ] );
		return 2;
	}

	if ( STR_EQUAL( opt, "-r" ) ) {
		p_rewrite = true;
		return 1;
	}

	if ( STR_EQUAL( opt, "-t" ) ) {
		if ( ! tail[ 0 ] ) return 0;
		
		p_finish_time = atoll( tail[ 0 ] );
		return 2;
	}

	if ( STR_EQUAL( opt, "--time-var" ) ) {
		if ( ! tail[ 0 ] ) return 0;
		
		p_time_var = atoll( tail[ 0 ] );
		
		return 2;
	}

	if ( STR_EQUAL( opt, "--exec-time" ) ) {
		if ( ! tail[ 0 ] ) return 0;

		p_min_time = atoll( tail[ 0 ] );
		p_max_time = atoll( tail[ 1 ] );

		return 3;
	}

	if ( STR_EQUAL( opt, "--wait-time" ) ) {
		if ( ! tail[ 0 ] ) return 0;

		p_min_wait = atoll( tail[ 0 ] );
		p_max_wait = atoll( tail[ 1 ] );

		return 3;
	}

	if ( STR_EQUAL( opt, "--mem" ) ) {
		if ( ! tail[ 0 ] ) return 0;

		p_min_mem = atoll( tail[ 0 ] );
		p_max_mem = atoll( tail[ 1 ] );

		return 3;
	}

	if ( STR_EQUAL( opt, "--cpu" ) ) {
		if ( ! tail[ 0 ] ) return 0;

		p_min_cpu = atoll( tail[ 0 ] );
		p_max_cpu = atoll( tail[ 1 ] );

		return 3;
	}

	if ( STR_EQUAL( opt, "--thread" ) ) {
		if ( ! tail[ 0 ] ) return 0;

		p_min_thread = atoll( tail[ 0 ] );
		p_max_thread = atoll( tail[ 1 ] );

		return 3;
	}

	if ( STR_EQUAL( opt, "--priority" ) ) {
		if ( ! tail[ 0 ] ) return 0;

		p_min_prio = atoll( tail[ 0 ] );
		p_max_prio = atoll( tail[ 1 ] );

		return 3;
	}

	if ( STR_EQUAL( opt, "--fcpu-vec" ) ) {
		p_allowed_features |= NODE_FEATURE_CPU_VEC;
		return 1;
	}

	if ( STR_EQUAL( opt, "--fgpu" ) ) {
		p_allowed_features |= NODE_FEATURE_GPU;
		return 1;
	}

	if ( STR_EQUAL( opt, "--fnpu" ) ) {
		p_allowed_features |= NODE_FEATURE_NPU;
		return 1;
	}

	if ( STR_EQUAL( opt, "--ftpu" ) ) {
		p_allowed_features |= NODE_FEATURE_TPU;
		return 1;
	}

	if ( STR_EQUAL( opt, "--ffpga" ) ) {
		p_allowed_features |= NODE_FEATURE_FPGA;
		return 1;
	}

	if ( STR_EQUAL( opt, "--fasic" ) ) {
		p_allowed_features |= NODE_FEATURE_ASIC;
		return 1;
	}

	if ( STR_EQUAL( opt, "--fnot-rand" ) ) {
		p_feature_not_rand = true;
		return 1;
	}

	if ( STR_EQUAL( opt, "--stdout" ) ) {
		p_to_stdout = true;
		return 1;
	}
	
	return 0;
}


uint32_t
parse_args( const size_t  argc,
			const char   *argv[] )
{
	if ( argc == 1 ) return EXIT_SUCCESS;

	size_t next = 0;
	
	for ( size_t a = 1; a < argc; a += next ) {
		next = interpret( argv[ a ], &( argv[ a+1 ] ), argc - a );

		if ( ! next ) return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}


void
print_help( void )
{
	puts( "job_dataset_generator <ключи>"
		  "\n\nключи:"
		  "\n\t-c <n>                      кол-во задач"
		  "\n\t-o <файл>                   файл для вывода (по умолчанию jobs)"
		  "\n\t-r                          перезапись файла"
		  "\n\t-t <t>                      конечный момент времени для появления задач"
		  "\n\t--exec-time <min-t> <max-t> граничное времени исполнения задач"
		  "\n\t--wait-time <min-t> <max-t> границы времени ожидания"
		  "\n\t--time-var <t>              временная погрешность (не используется)"
		  "\n\t--mem <min-m> <max-m>       границы памяти"
		  "\n\t--cpu <min-c> <max-c>       границы кол-ва процессоров"
		  "\n\t--thread <min-t> <max-t>    границы кол-ва потоков (ядер)"
		  "\n\t--priority <min-p> <max-p>  границы приоритетов"
		  "\n\t--fnot-rand                 функции, указываемые через флаги, появляются во всех задачах"
		  "\n\t--stdout                    вывод в консоль, вместо файла (игнорируется -o)"
		  "\n\n\tФлаги функций: "
		  "\n\t\t--fcpu-vec CPU_VEC (векторные инструкции)"
		  "\n\t\t--fgpu     GPU (видеокарта)"
		  "\n\t\t--fnpu     NPU (нейронный процессор)"
		  "\n\t\t--ftpu     TPU (тензорный процессор)"
		  "\n\t\t--ffpga    FPGA (ПЛИС)"
		  "\n\t\t--fasic    ASIC");
}
