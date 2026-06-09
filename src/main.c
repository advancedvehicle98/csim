#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <cluster/cluster.h>

#include <schedule_algorithms.h>


static uint32_t check_args( const int argc, const char *argv[] );
static uint32_t get_schedule_functions( scheduler_t *s, const char *type );
static void print_available_schedule_types( void );


uint32_t
main( const int   argc,
	  const char *argv[] )
{
	cluster_t c;

	if ( check_args( argc, argv ) ) return EXIT_FAILURE;
	
	const char *jf = argv[ 1 ];
	const char *cf = argv[ 2 ];
	const char *st = argv[ 3 ];

	_DEBUG_PRINTF( "Выбранный режим планирования: %s", st );
	
	scheduler_t s;

	if ( get_schedule_functions( &s, st ) == EXIT_FAILURE ) {
		_ERROR_PUTS( "Некорректный тип планировщика" );
		print_available_schedule_types();
		return EXIT_FAILURE;
	}
	
	if ( cluster_init( &c, jf, cf, &s ) == EXIT_FAILURE ) {
		_ERROR_PUTS( "Не удалось инициализировать кластер" );
		return EXIT_FAILURE;
	}

	if ( cluster_simulate( &c ) == EXIT_FAILURE) {
		_ERROR_PUTS( "Ошибка симуляции кластера" );
		return EXIT_FAILURE;
	}
	
	cluster_destroy( &c );
	
	return EXIT_SUCCESS;
}


uint32_t
check_args( const int   argc,
			const char *argv[] )
{
	if ( argc != 4 ) {
		puts(" [!!!] Использование: ./csim <путь к датасету с задачами> <путь к датасету с узлами> <режим планировки>" );
		print_available_schedule_types();
		return EXIT_FAILURE;
	}
	
	return EXIT_SUCCESS;
}


uint32_t
get_schedule_functions( scheduler_t *s, const char *type )
{
	if ( _STR_EQUAL( type, "fifo" ) ) {
		s->schedule   = schedule_fifo;
		s->init       = init_fifo;
		s->distribute = distribute_fifo;
		s->destroy    = destroy_fifo;
		s->print      = print_fifo;
		
		return EXIT_SUCCESS;
	}
	
	if ( _STR_EQUAL( type, "priority" ) ) {
		s->schedule   = schedule_priority;
		s->init       = init_priority;
		s->distribute = distribute_priority;
		s->destroy    = destroy_priority;
		s->print      = print_priority;
		
		return EXIT_SUCCESS;
	}
	
	if ( _STR_EQUAL( type, "backfill" ) ) {
		s->schedule   = schedule_backfill;
		s->init       = init_backfill;
		s->distribute = distribute_backfill;
		s->destroy    = destroy_backfill;
		s->print      = print_backfill;
		
		return EXIT_SUCCESS;
	}
	
	if ( _STR_EQUAL( type, "round_robin" ) ) {
		s->schedule   = schedule_round_robin;
		s->init       = init_round_robin;
		s->distribute = distribute_round_robin;
		s->destroy    = destroy_round_robin;
		s->print      = print_round_robin;
		
		return EXIT_SUCCESS;
	}
	
	return EXIT_FAILURE;
}


void
print_available_schedule_types( void )
{
	puts( ALGORITHMS_HELP );
}
