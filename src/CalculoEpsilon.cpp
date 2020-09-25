/*
 ============================================================================
 Name        : Calculo del valor de Epsilon
 Author      : Cesar Condori
 Version     :
 Copyright   : Your copyright notice
 Description : Calculo del valor de Epsilon
 Objetivos	 : Repartir trabajo entre procesos, Aprender a usar MPI_Bcast y
 	 	 	   Aprender a usar MPI_Reduce.
 Compile Command:
 	$ mpiCC -g -Wall -o <CodeName> <CodeName.cpp>
	$ mpiexec  ./<CodeName>
	$ mpiexec -n 10 ./<CodeName>

 ============================================================================
 */

#include <math.h>
#include "mpi.h"   // Biblioteca de MPI
#include <cstdlib> // Incluido para el uso de atoi
#include <iostream>
using namespace std;

int main(int argc, char *argv[])
{
    int n, // Numero de iteraciones
		rank, // Identificador de proceso
		size; // Numero de procesos
    double EPSILON = 2.71828182845904523536;
    double my_epsilon, // Valor local de Epsilon
		e;    //valor de epsilon
	//double long long sum;  // Acumulador para la suma de la factoriales inversos

    MPI_Init(&argc, &argv); // Inicializamos los procesos
    MPI_Comm_size(MPI_COMM_WORLD, &size); // Obtenemos el numero total de procesos
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); // Obtenemos el valor de nuestro identificador

    // Solo el proceso 0 va a conocer el numero de iteraciones que vamos a
    // ejecutar para la aproximacion de Epsilon
	if (rank == 0) {
		cout<<"introduce la precision del calculo (n > 0): ";
		cin>>n;
	}

	// El proceso 0 reparte al resto de procesos el numero de iteraciones
	// que calcularemos para la aproximacion de Epsilon
	MPI_Bcast(&n, // Puntero al dato que vamos a enviar
			  1,  // Numero de datos a los que apunta el puntero
			  MPI_INT, // Tipo del dato a enviar
			  0, // Identificacion del proceso que envia el dato
			  MPI_COMM_WORLD);

	if (n <= 0){
		MPI_Finalize();
		exit(0);
	}else {
		unsigned long long fact = 1.0;
		double long sum = 0.0;
		//unsigned long long x;
		//sum = 0.0;
		for (int i = n-1; i >= rank+1; i -= size) {
			double long x = fact*i;
			sum += 1.0 / x;
			}
		my_epsilon = sum;


		MPI_Reduce(&my_epsilon, // Valor local de EPSILON
					&e,  // Dato sobre el que vamos a reducir el resto
					1,	  // Numero de datos que vamos a reducir
					MPI_DOUBLE,  // Tipo de dato que vamos a reducir
					MPI_SUM,  // Operacion que aplicaremos
					0, // proceso que va a recibir el dato reducido
					MPI_COMM_WORLD);

		// Solo el proceso 0 imprime el mensaje, ya que es la unica que
		// conoce el valor de Epsilon aproximado.
		if (rank == 0)
		{
			printf("EPSILON: %.30f \n", EPSILON);
			printf("My EPSILON: %.30f \n \n", e);
			printf("El valor aproximado de EPSILON es: %f, con un error de %.30f \n", e,fabs(e - EPSILON));
		}
	}

	// Terminamos la ejecucion de los procesos, despues de esto solo existira
	// el proceso 0
	// ¡Ojo! Esto no significa que los demas procesos no ejecuten el resto
	// de codigo despues de "Finalize", es conveniente asegurarnos con una
	// condicion si vamos a ejecutar mas codigo (Por ejemplo, con "if(rank==0)".
    MPI_Finalize();
    return 0;
}
