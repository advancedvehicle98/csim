#include <cluster/internal.h>

#include <assert.h>
#include <stdlib.h>


uint32_t
_cluster_parse_file_wrapper( __STATE__       cluster_t   *c,
							 __IN__    const char        *f,
							 __IN__          uint32_t ( *parse_lines )( cluster_t *, FILE * ) )
{
	assert( f );
	
	FILE *fp = fopen( f, "r+" );

	if ( ! fp ) {
		_ERROR_PRINTF( "Не удалось открыть файл %s", f );
		return EXIT_FAILURE;
	}

	if ( parse_lines( c, fp ) == EXIT_FAILURE ) goto _failure;
	
	fclose( fp );

	_DEBUG_PRINTF( "Завершён парсинг %s", f );
	
	return EXIT_SUCCESS;

_failure:
	fclose( fp );

	return EXIT_FAILURE;
}
