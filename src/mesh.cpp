#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "../include/mesh.h"

// Constructor
mesh::mesh()
{
	float t = (1.+sqrt(5.))/2.;
	float tau = t/sqrt(1.+t*t);
        float one = 1./sqrt(1.+t*t); // Unit sphere
        

        // Twelve vertices of icosahedron on unit sphere
        // Creacion dinamica de arreglos al apuntador
        // Apuntadores a filas
        nNodos = 12;       
        vertex = new float*[12];
        
        // Apuntadores a columnas
        for(int i = 0; i<12 ; i++){
        	vertex[i] = new float[3];
        } 

	float nodos[12][3] ={{  tau,  one,    0},
                     { -tau,  one,    0 },
                     { -tau, -one,    0 },
                     {  tau, -one,    0 },
                     {  one,   0 ,  tau },
                     {  one,   0 , -tau },
                     { -one,   0 , -tau },
                     { -one,   0 ,  tau },
                     {   0 ,  tau,  one },
                     {   0 , -tau,  one },
                     {   0 , -tau, -one },
                     {   0 ,  tau, -one }};

	for(int i = 0; i<nNodos ; i++)
		for(int j = 0 ; j<3 ; j++)
		{
			vertex[i][j]=nodos[i][j];
		}

        		
        // Structure for unit icosahedron
        nCeldas = 20;
	faces = new int*[20];
	for(int i=0;i<nCeldas;i++)
	{
		faces[i]=new int[3];
	}
        int prueba[3][2]={(1,2,3),
        		  (4,5,6)};
        int f[20][3]= {{4,  7,  8},
                        {4,  9,  7},
                        {5, 11,  6},
                        {5,  6, 10},
                        {0,  3,  4},
                        {0,  5,  3},
                        {2,  1,  7},
                        {2,  6,  1},
                        {8, 11,  0},
                        {8,  1, 11},
                        {9,  3, 10},
                        {9, 10,  2},
                        {8,  0,  4},
                        {11,  5, 0},
                        {4,  3, 9},
                        {5, 10,  3},
                        {7,  1,  8},
                        {6, 11,  1},
                        {7, 9,  2},
                        {6,  2, 10}};
  	printf("Nodo: %d\n",prueba[0][1]);
	for(int i = 0; i<nCeldas ; i++)
		for(int j = 0 ; j<3 ; j++)
		{
			faces[i][j]=f[i][j];
		}
}

//Destructor: Destruye los elementos de la malla nodos y celdas
mesh::~mesh()
{
	
}

// Trasladar en el espacio el centro de la esfera
void mesh::moverCentro(float x, float y, float z)
{
	for(int i=0;i<nNodos;i++)
	{
		vertex[i][0] = vertex[i][0]  + x;
		vertex[i][1] = vertex[i][1]  + y;
		vertex[i][2] = vertex[i][2]  + z;
	}
}

// Rotar la estructura completa dados tres angulos 
// theta, phi, psi
void mesh::rotarEstructura(float alpha, float phi, float theta)
{
	float Rx[3][3] = {{1,          0,          0},
			  {0, cos(alpha), -sin(alpha)},
			  {0, sin(alpha),  cos(alpha)}};	

	float Ry[3][3] = {{ cos(phi), 0, sin(phi)},
			  {        0, 1,        0},
			  {-sin(phi), 0, cos(phi)}};	
			  
        float Rz[3][3] = {{cos(theta), -sin(theta),0},
			  {sin(theta), cos(theta), 0},
			  {0, 0, 1}};		
	float Rxy[3][3];
	float Rxyz[3][3];
	
	// Realizar composición de rotaciones
	/* Realiza el producto de matrices y guarda el resultado en una tercera matriz*/
	for(int i=0;i<3;i++){ 
	      for(int j=0;j<3;j++){
		  Rxy[i][j]=0;
		  for(int k=0;k<3;k++){
		      Rxy[i][j]=Rxy[i][j]+(Rx[i][k]*Ry[k][j]);
		  }
	      }
	  }
	  
	  for(int i=0;i<3;i++){ 
	      for(int j=0;j<3;j++){
		  Rxyz[i][j]=0;
		  for(int k=0;k<3;k++){
		      Rxyz[i][j]=Rxyz[i][j]+(Rxy[i][k]*Rz[k][j]);
		  }
	      }
	  }
	  
	  // Rotar cada uno de los nodos por la matriz
	  for(int i = 0; i<nNodos ; i++)
	  {
	  	float x = Rxyz[0][0]*vertex[i][0] + Rxyz[0][1]*vertex[i][1] + Rxyz[0][2]*vertex[i][2];
	  	float y = Rxyz[1][0]*vertex[i][0] + Rxyz[1][1]*vertex[i][1] + Rxyz[1][2]*vertex[i][2];
	  	float z = Rxyz[2][0]*vertex[i][0] + Rxyz[2][1]*vertex[i][1] + Rxyz[2][2]*vertex[i][2];
	  	vertex[i][0] = x;
	  	vertex[i][1] = y;
	  	vertex[i][2] = z;
	  }
		
}


// Proyectar los nodos a una superficie RCB
void mesh::proyectarRBC(float r)
{

	float c0 = 0.207;
        float c1 = 2.000;
        float c2 = -1.123;
        float R  = r;
        
        for(int i = 0;i<nNodos;i++)
        {
		float X = vertex[i][0];
		float Y = vertex[i][1];
		float a = ((X*X)+(Y*Y))/(R*R);
		float b = (c0 + c1*a + c2*(a*a));
		float Z = (0.5*R*(sqrt(fabs(1.0-a))))*b;
		vertex[i][0]=X;
		vertex[i][1]=Y;
		
		// Calcular magnitud del vector
		float x, y, z;
		x = vertex[i][0]*vertex[i][0];
		y = vertex[i][1]*vertex[i][1];
		z = vertex[i][2]*vertex[i][2];
		float mag = sqrt(x+y+z);
		if(vertex[i][2]/mag<0){
			vertex[i][2]=-Z + (X*X)/16;
		}else{
			vertex[i][2]=Z + (X*X)/16;
		}
	}
}


// Proyectar los nodos a una superficie esferica 
void mesh::proyectarEsfera(float r)
{
	for(int i=0; i<nNodos;i++)
	{
		float x, y, z;
		x = vertex[i][0]*vertex[i][0];
		y = vertex[i][1]*vertex[i][1];
		z = vertex[i][2]*vertex[i][2];
		float mag= sqrt(x+y+z);
		vertex[i][0] = vertex[i][0]*r/mag;
		vertex[i][1] = vertex[i][1]*r/mag;
		vertex[i][2] = vertex[i][2]*r/mag;
	}
}


// Funcion que agrega un nodo a la lista de nodos de la malla
// Retorna la posicion del nuevo nodo
int mesh::agregarNodo(float x, float y, float z)
{
	float **nuevaLista = new float*[nNodos+1];
	for(int i=0; i<nNodos+1; i++)
	{
		nuevaLista[i]=new float[3];
	}
	for(int i=0; i<nNodos; i++)
	{
		nuevaLista[i][0]=vertex[i][0];
		nuevaLista[i][1]=vertex[i][1];
		nuevaLista[i][2]=vertex[i][2];
	}
	nuevaLista[nNodos][0]=x;
	nuevaLista[nNodos][1]=y;
	nuevaLista[nNodos][2]=z;
	vertex = nuevaLista;
	nNodos=nNodos+1;
	return nNodos-1;
}

// Funcion que determina si un nodo existe en la lista
// retorna -1 si no existe
// retorna  N si existe
int mesh::existeNodo(float x, float y, float z)
{
	int existe = -1;
	for(int i=0;i<nNodos;i++)
	{
		bool a = (vertex[i][0]==x);
		bool b = (vertex[i][1]==y);
		bool c = (vertex[i][2]==z);
		if(a &&  b && c)
		{
			existe = i;
		}
	}
	return existe;
}


//Función para refinamiento de malla
void mesh::mesh_refine_tri4()
{
	// Dividir cada cara en 4 triangulos
	// Make new midpoints
	// a = (A+B)/2
	// b = (B+C)/2
	// c = (C+A)/2
	//        B
	//       /\
	//      /  \
	//    a/____\b       Construct new triangles
	//    /\    /\       [A,a,c]
	//   /  \  /  \      [a,B,b]
	//  /____\/____\     [c,b,C]
	// A      c     C    [a,b,c]
	// -------------------------------------------
	
	printf("Refinando...\n");
	// Crear la estructura para las nuevas celdas
	int **nuevaLista = new int*[4*nCeldas];
	for(int i=0; i<nCeldas*4;i++)
	{
		nuevaLista[i] = new int[3];
	}
	
	// Dividir todas las caras 
	for(int f=0;f<nCeldas;f++)
	{
		int NA, NB, NC;
		// Encontrar los vertices correspondientes a cada cara
		NA = faces[f][0];
		NB = faces[f][1];
		NC = faces[f][2];
		
		// Encontrar las coordenadas de cada vertice
		float A[3], B[3], C[3];
		A[0]=vertex[NA][0];
		A[1]=vertex[NA][1];
		A[2]=vertex[NA][2];
		
		B[0]=vertex[NB][0];
		B[1]=vertex[NB][1];
		B[2]=vertex[NB][2];
		
		C[0]=vertex[NC][0];
		C[1]=vertex[NC][1];
		C[2]=vertex[NC][2];
		
		// Hallar los puntos medios de cada coordenada
		float a[3], b[3], c[3];
		a[0] = (A[0]+B[0])/2.;
		a[1] = (A[1]+B[1])/2.;
		a[2] = (A[2]+B[2])/2.;
		
		b[0] = (B[0]+C[0])/2.;
		b[1] = (B[1]+C[1])/2.;
		b[2] = (B[2]+C[2])/2.;
		
		c[0] = (C[0]+A[0])/2.;
		c[1] = (C[1]+A[1])/2.;
		c[2] = (C[2]+A[2])/2.;
		
		// Hallar el indice de cada nodo de las nuevas celdas
		int Na, Nb, Nc;
		Na = posicionNodo(a[0], a[1], a[2]);
		Nb = posicionNodo(b[0], b[1], b[2]);
		Nc = posicionNodo(c[0], c[1], c[2]);
		
		// Crear las nuevas caras
		int C1[3]={NA, Na, Nc}, C2[3]={Na, NB, Nb};
		int C3[3]={Nc, Nb, NC}, C4[3]={Na, Nb, Nc};
		
		// Agregarlas a la nueva estructura de celdas
		nuevaLista[(f+1)*4-4][0] = C1[0];
		nuevaLista[(f+1)*4-4][1] = C1[1];
		nuevaLista[(f+1)*4-4][2] = C1[2];
		
		nuevaLista[(f+1)*4-3][0] = C2[0];
		nuevaLista[(f+1)*4-3][1] = C2[1];
		nuevaLista[(f+1)*4-3][2] = C2[2];
		
		nuevaLista[(f+1)*4-2][0] = C3[0];
		nuevaLista[(f+1)*4-2][1] = C3[1];
		nuevaLista[(f+1)*4-2][2] = C3[2];
		
		nuevaLista[(f+1)*4-1][0] = C4[0];
		nuevaLista[(f+1)*4-1][1] = C4[1];
		nuevaLista[(f+1)*4-1][2] = C4[2];
	}
	faces = nuevaLista;
	nCeldas = 4*nCeldas;
}

// Funcion auxiliar para obtener la posicion de los vertices
int mesh::posicionNodo(float x, float y, float z)
{
	//Decidir si existe o no existe en la lista
	int pos = existeNodo(x,y,z);
	if(pos>-1)
	{
		return pos;
	}
	return agregarNodo(x,y,z);
}



/**

*/

int mesh::guardarVTU(int t)
{
	printf("%d",t);
	FILE *archivo;/*El manejador de archivo*/
	char ruta[80];
	char numero[4];
	strcpy(ruta, "temp/esfera-");
	sprintf(numero,"%d",t);
	strcat(ruta,numero);
	strcat(ruta,".vtk");
        archivo=fopen(ruta, "w");
        if(archivo==NULL){/*Si no lo logramos abrir, salimos*/
		printf("No se puede guardar archivo");
		return 1;}
	else{
	// Escribir datos al archivo 
	// 1. Escribir cabecera.
	fprintf(archivo, "# vtk DataFile Version 3.0\n");
	fprintf(archivo, "vtk output\n");
	fprintf(archivo, "ASCII\n");
	fprintf(archivo, "DATASET UNSTRUCTURED_GRID\n");
	fprintf(archivo, "POINTS %i double\n",nNodos);
	// Escribir coordenadas de cada nodo
	for(int i = 0; i<nNodos ; i++){
		for(int j = 0; j<3 ; j++){
			fprintf(archivo, "%f ", vertex[i][j]);
		}
		fprintf(archivo, "\n");}
	// Escribir celdas 
	fprintf(archivo, "CELLS %d %d\n",nCeldas,nCeldas*4);
	for(int i = 0; i<nCeldas ; i++){
		fprintf(archivo, "3 ");
		for(int j = 0; j<3 ; j++){
			fprintf(archivo, "%i ", faces[i][j]);
		}
		fprintf(archivo, "\n");}
	fprintf(archivo,"CELL_TYPES %d\n",nCeldas);
	for(int i = 0; i<nCeldas ; i++){
		fprintf(archivo, "5 \n");}

	fclose(archivo);/*Cerramos el archivo*/
	return 0;
	}
}

