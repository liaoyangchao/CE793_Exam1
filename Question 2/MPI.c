# include <math.h>
# include <stdlib.h>
# include <stdio.h>
# include <time.h>
# include "mpi.h"

int main ( int argc, char *argv[] );
double boundary_condition ( double x, double time );
double initial_condition ( double x, double time );
double rhs ( double x, double time );
void timestamp ( );
void update ( int id, int p );

int main ( int argc, char *argv[] )

{
  int id;
  int p;
  double wtime;

  MPI_Init ( &argc, &argv );
  MPI_Comm_rank ( MPI_COMM_WORLD, &id );
  MPI_Comm_size ( MPI_COMM_WORLD, &p );

  if ( id == 0 )
  {
    timestamp ( );
    printf ( "\n" );
    printf ( "Derivative_MPI:\n" );
    printf ( "  C/MPI version\n" );
    printf ( "  Compute Derivative of function y=sin(x).\n" );
  }

  if ( id == 0 ) 
  {
    wtime = MPI_Wtime ( );
  }

  update ( id, p );

  if ( id == 0 )
  {
    wtime = MPI_Wtime ( ) - wtime;

    printf ( "\n");       
    printf ( "  Wall clock elapsed seconds = %f\n", wtime );      
  }

  MPI_Finalize ( );

  if ( id == 0 )
  {
    printf ( "\n" );
    printf ( "Derivative_MPI:\n" );
    printf ( "  Normal end of execution.\n" );
    printf ( "\n" );
    timestamp ( );
  }

  return 0;
}

void update ( int id, int p )

{
  double cfl;

  double *h;
  FILE *h_file;
  double *h_new;
  int i;
  int j;
  int j_min = 0;
  int j_max = 99;
  int n = 100;
  MPI_Status status;
  int tag;
  double time;
  double time_delta;
  double time_max = 10.0;
  double time_min = 0.0;
  double time_new;
  double *x;
  double x_delta;

  x = ( double * ) malloc ( ( n + 2 ) * sizeof ( double ) );

  for ( i = 0; i <= n + 1; i++ )
  {
    x[i] = ( ( double ) (         id * n + i - 1 ) * x_max
           + ( double ) ( p * n - id * n - i     ) * x_min )
           / ( double ) ( p * n              - 1 );
  }
  if ( p == 1 )

  time = time_min;
  h = ( double * ) malloc ( ( n + 2 ) * sizeof ( double ) );
  h_new = ( double * ) malloc ( ( n + 2 ) * sizeof ( double ) );
  h[0] = 0.0;
  for ( i = 1; i <= n; i++ )
  {
    h[i] = initial_condition ( x[i], time );
  }
  h[n+1] = 0.0;
  
  time_delta = ( time_max - time_min ) / ( double ) ( j_max - j_min );
  x_delta = ( x_max - x_min ) / ( double ) ( p * n - 1 );

  cfl = k * time_delta / x_delta / x_delta;

  if ( id == 0 ) 
  {
    printf ( "\n" );
    printf ( "UPDATE\n" );
    printf ( "  CFL stability criterion value = %f\n", cfl );
  }

  if ( 0.5 <= cfl ) 
  {
    if ( id == 0 )
    {
      printf ( "\n" );
      printf ( "UPDATE - Warning!\n" );
      printf ( "  Computation cancelled!\n" );
      printf ( "  CFL condition failed.\n" );
    }
    return;
  }


  for ( j = 1; j <= j_max; j++ )
  {

    time_new = ( ( double ) (         j - j_min ) * time_max
               + ( double ) ( j_max - j         ) * time_min )
               / ( double ) ( j_max     - j_min );

    if ( 0 < id )
    {
      tag = 1;
      MPI_Send ( &h[1], 1, MPI_DOUBLE, id-1, tag, MPI_COMM_WORLD );
    }

    if ( id < p-1 )
    {
      tag = 1;
      MPI_Recv ( &h[n+1], 1,  MPI_DOUBLE, id+1, tag, MPI_COMM_WORLD, &status );
    }

    if ( id < p-1 )
    {
      tag = 2;
      MPI_Send ( &h[n], 1, MPI_DOUBLE, id+1, tag, MPI_COMM_WORLD );
    }

    if ( 0 < id )
    {
      tag = 2;
      MPI_Recv ( &h[0], 1, MPI_DOUBLE, id-1, tag, MPI_COMM_WORLD, &status );
    }

    for ( i = 1; i <= n; i++ )
    {
      h_new[i] = h[i] 
      + ( time_delta * k / x_delta / x_delta ) * ( h[i-1] - 2.0 * h[i] + h[i+1] ) 
      + time_delta * rhs ( x[i], time );
    }

    if ( 0 == id )
    {
      h_new[1] = boundary_condition ( x[1], time_new );
    }
    if ( id == p - 1 )
    {
      h_new[n] = boundary_condition ( x[n], time_new );
    }

    time = time_new;

    for ( i = 1; i <= n; i++ )
    {
      h[i] = h_new[i];
    }

    if ( p == 1 )
    {
      for ( i = 1; i <= n; i++ )
      {
        fprintf ( h_file, "  %f", h[i] );
      }
      fprintf ( h_file, "\n" );
    }
  }

  if ( p == 1 )
  {
    fclose ( h_file );
  }

  free ( h );
  free ( h_new );
  free ( x );

  return;
}