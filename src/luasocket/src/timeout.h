#ifndef TIMEOUT_H
#define TIMEOUT_H
/*=========================================================================*\
* Timeout management functions
* LuaSocket toolkit
\*=========================================================================*/
#include "luasocket.h"

/* timeout control structure */
typedef struct t_timeout_
{
    double block; /* maximum time for blocking calls */
    double total; /* total number of miliseconds for operation */
    double start; /* time of start of operation */
} t_timeout;
typedef t_timeout *p_timeout;

#ifndef _WIN32
#pragma GCC visibility push( hidden )
#endif

void timeout_init( p_timeout tm, double block, double total );
double timeout_get( p_timeout tm );
double timeout_getstart( p_timeout tm );
double timeout_getretry( p_timeout tm );
p_timeout timeout_markstart( p_timeout tm );

double timeout_gettime( void );

int timeout_open( lua_State *L );

int timeout_meth_settimeout( lua_State *L, p_timeout tm );
int timeout_meth_gettimeout( lua_State *L, p_timeout tm );

#ifndef _WIN32
#pragma GCC visibility pop
#endif

#define timeout_iszero( tm ) ( ( tm )->block == 0.0 )

#endif /* TIMEOUT_H */
