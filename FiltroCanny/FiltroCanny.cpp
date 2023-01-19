#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>

#include <math.h>
#include <cmath>
#include <iostream>

using namespace cv;
using namespace std;

Mat PasarKernelGaussImagen(Mat imagenAmpleada, Mat kernel, int filasImagen, int columnasImagen, int tamKernel, float sumKernel);
Mat PasarKernelImagen(Mat imagenAmpleada, Mat kernel, int filasImagen, int columnasImagen, int tamKernel);
Mat ExpandirImagen(Mat imagen, int tamKernel);
Mat SupresionNoMaxima(Mat imagenAmpleada, Mat angulosSobel, int filasImagenOriginal, int columnasImagenOriginal);
Mat Histeresis(Mat imagenAmpleada, int filasImagenOriginal, int columnasImagenOriginal, int max);
Mat DobleUmbral(Mat imagenAmpleda, int columanasImagenOriginal, int filasImagenOriginal, int max);
Mat CrearGx();
Mat CrearGy();
int ValorMaximoMatriz(Mat matriz);

int main()
{
	char NombreImagen[] = "Lenna.png";

	Mat imagen;
	Mat imagenGrises;

	int tamExpancion = 0;
	int tamKernel = 0;
	int centro = 0;
	int x, y;
	int i, j;

	float sigma = 0;
	float resultado = 0;

	// Constantes
	float pi = 3.14159265358979323846;
	float e = 2.71828182845904523536;

	double azul, verde, rojo;

	// Pedimos y validamos un tamaño de kernel impar 
	while (tamKernel % 2 != 1)
	{
		cout << "Ingresa un tamaño impar del kernel" << endl;
		cin >> tamKernel;

		if (tamKernel % 2 == 0) {
			cout << "Tamaño de kernel no valido\n" << endl;
		}
	}

	// Pedimos el valor de sigma
	cout << "Ingresa el valor de sigma" << endl;
	cin >> sigma;
	
	// Creamos kernel para Gauss
	Mat kernel(tamKernel, tamKernel, CV_32F); // Creamos la matriz de tamaño dinamico 
	centro = (tamKernel - 1) / 2; // Para dimensiones impares
	cout << "Centro kernel Gauss: " << centro << "\n" << endl;

	// Llenamos el kernel para Gauss
	cout << "\nCoordenadas kernel Gauss\n" << endl;
	for (i = 0; i < tamKernel; i++)
	{
		y = -1 * (i - centro);

		for (j = 0; j < tamKernel; j++)
		{
			x = j - centro;

			resultado = (1 / (2 * pi * sigma * sigma)) * pow(e, -((x * x + y * y) / (2 * sigma * sigma)));
			kernel.at<float>(Point(j, i)) = resultado;

			cout << "\t(" << x << "," << y << ")";
		}
		cout << endl;
	 }
	cout << "\nValores kernel Gauss\n" << endl;
	cout << kernel << "\n" << endl;
	
	imagen = imread(NombreImagen); // Lectura de la imagen
	if (!imagen.data)
	{
		cout << "Error al cargar la imagen: " << NombreImagen << endl;
		exit(1);
	}
	
	int filasImagen = imagen.rows; // Lectura de cuantas filas tiene la imagen
	int columnasImagen = imagen.cols; // Lectura de cuantas columnas tiene la imagen

	// Pasamos a escala de grises
	//cvtColor(imagen, imagenGrises, COLOR_BGR2GRAY); // Funcion para pasar a escala de grises con openCV
	// Mat imagenGrisesPromedio(filasOriginal, columnasOriginal, CV_8UC1); // CV_8UC1 Dato de tipo uchar de un solo canal
	Mat imagenGrisesNTSC(filasImagen, columnasImagen, CV_8UC1);
	for (i = 0; i < filasImagen; i++)
	{
		for (j = 0; j < columnasImagen; j++)
		{
			azul = imagen.at<Vec3b>(Point(j, i)).val[0];  // B
			verde = imagen.at<Vec3b>(Point(j, i)).val[1]; // G
			rojo = imagen.at<Vec3b>(Point(j, i)).val[2];  // R

			// imagenGrisesPromedio.at<uchar>(Point(j, i)) = uchar((rojo + verde + azul) / 3);
			imagenGrisesNTSC.at<uchar>(Point(j, i)) = uchar(0.299 * rojo + 0.587 * verde + 0.114 * azul);
		}
	}
	// cout << imagenGrisesNTSC << "\n" << endl;

	// Expandimos imagen
	Mat imagenAmpleada = ExpandirImagen(imagenGrisesNTSC, tamKernel);

	// Aplicamos filtro Gauss
	float sumKernel = 0;
	for (int i = 0; i < tamKernel; i++)
	{
		for (int j = 0; j < tamKernel; j++)
		{
			sumKernel += kernel.at<float>(Point(j, i)); // Suma de los valores del kernel para normalizar la imagen
		}
	}
	cout << "Suma kernel: " << sumKernel << "\n" << endl;

	Mat imagenFiltroGauss = PasarKernelGaussImagen(imagenAmpleada, kernel, filasImagen, columnasImagen, tamKernel, sumKernel);
	imagenFiltroGauss.convertTo(imagenFiltroGauss, CV_8UC1);
	// cout << imagenFiltroGauss << "\n" << endl;

	// Ecualizamos imagen
	Mat imagenEcu(imagenFiltroGauss.rows, imagenFiltroGauss.cols, CV_8UC1);
	equalizeHist(imagenFiltroGauss, imagenEcu);

	// Inicia sobel
	// Creamos los kernel Gx y Gy
	Mat kernelGx = CrearGx(); // 3x3
	Mat kernelGy = CrearGy(); // 3x3
	cout << "\nKenrnelGx\n" << kernelGx << "\n" << endl;
	cout << "\nKenrnelGy\n" << kernelGy << "\n" << endl;

	// Expandimos imagen
	Mat imagenAmpleada2 = ExpandirImagen(imagenEcu, 3);

	// Pasamos los dos kernel por la imagen
	Mat matrizGx = PasarKernelImagen(imagenAmpleada2, kernelGx, imagenEcu.rows, imagenEcu.cols, 3);
	Mat matrizGy = PasarKernelImagen(imagenAmpleada2, kernelGy, imagenEcu.rows, imagenEcu.cols, 3);
	matrizGx.convertTo(matrizGx, CV_8UC1);
	matrizGy.convertTo(matrizGy, CV_8UC1);

	// Sacamos la matriz |G|
	Mat sobel(imagenEcu.rows, imagenEcu.cols, CV_32F);
	for (int i = 0; i < imagenEcu.rows; i++)
	{
		for (int j = 0; j < imagenEcu.cols; j++)
		{
			sobel.at<float>(Point(j, i)) = sqrt(pow(matrizGx.at<uchar>(Point(j, i)), 2) + pow(matrizGy.at<uchar>(Point(j, i)), 2));
		}
	}
	sobel.convertTo(sobel, CV_8UC1);

	// Sacamos la matriz de los angulos (arctan(Gy / Gx))
	Mat angulosSobel(imagenEcu.rows, imagenEcu.cols, CV_32F);
	float angulo = 0;
	for (int i = 0; i < imagenEcu.rows; i++)
	{
		for (int j = 0; j < imagenEcu.cols; j++)
		{
			angulo = atan2(float(matrizGy.at<uchar>(Point(j, i))), float(matrizGx.at<uchar>(Point(j, i)))) * (180 / pi);
			angulosSobel.at<float>(Point(j, i)) = angulo; // Grados
		}
	}
	// cout << "\n"  << angulosSobel << endl;

	// Empieza Conny
	// Aplicamos la supresion no maxima a la imagen despues de aplicar sobel 
	 Mat imagenAmpleada3 = ExpandirImagen(sobel, 3);
	 Mat bordeDelgado = SupresionNoMaxima(imagenAmpleada3, angulosSobel, sobel.rows, sobel.cols);
	 bordeDelgado.convertTo(bordeDelgado, CV_8UC1);

	 // Aplicamos el doble umbral
	 int valorMaximo = ValorMaximoMatriz(bordeDelgado); // Sacamos el valor maximo de la imagen para definir el doble umbral 
	 // cout << "\nValor maximo matriz: \n" << valorMaximo << "\n" << endl;

	 Mat imagenDobleUmbral = DobleUmbral(bordeDelgado, bordeDelgado.rows, bordeDelgado.cols, valorMaximo);
	 imagenDobleUmbral.convertTo(imagenDobleUmbral, CV_8UC1);
	 // cout << "\n" << imagenDobleUmbral << "\n" << endl;

	 // Aplicamos Histeresis
	 Mat imagenAmpleada4 = ExpandirImagen(imagenDobleUmbral, 3);
	 Mat conny = Histeresis(imagenAmpleada4, imagenDobleUmbral.rows, imagenDobleUmbral.cols, valorMaximo);
	 conny.convertTo(conny, CV_8UC1);

	 // Tamaños ventanas
	 cout << "\nTamaño imagen original: " << imagen.rows * imagen.cols << endl;
	 cout << "Tamaño imagen grises: " << imagenGrisesNTSC.rows * imagenGrisesNTSC.cols << endl;
	 cout << "Tamaño imagen filtro Gauss: " << imagenFiltroGauss.rows * imagenFiltroGauss.cols << endl;
	 cout << "Tamaño imagen ecualizada: " << imagenEcu.rows * imagenEcu.cols << endl;
	 cout << "Tamaño imagen filtro Sobel: " << sobel.rows * sobel.cols << endl;
	 cout << "Tamaño imagen Conny: " << conny.rows * conny.cols << endl;

	namedWindow("Imagen original", WINDOW_AUTOSIZE); // Creación de una ventana
	imshow("Imagen original", imagen);

	namedWindow("Imagen escala de grises (NTSC)", WINDOW_AUTOSIZE);
	imshow("Imagen escala de grises (NTSC)", imagenGrisesNTSC);

	/*namedWindow("ImagenaAmpleada", WINDOW_AUTOSIZE);
	imshow("Imagen ampleada", imagenAmpleada);*/

	/*namedWindow("ImagenaAmpleada2", WINDOW_AUTOSIZE);
	imshow("Imagen ampleada 2", imagenAmpleada2);*/

	/*namedWindow("ImagenaAmpleada3", WINDOW_AUTOSIZE);
	imshow("Imagen ampleada 3", imagenAmpleada3);*/

	/*namedWindow("ImagenaAmpleada4", WINDOW_AUTOSIZE);
	imshow("Imagen ampleada 4", imagenAmpleada4);*/

	namedWindow("Imagen filtro gauss", WINDOW_AUTOSIZE);
	imshow("Imagen filtro gauss", imagenFiltroGauss);

	namedWindow("Imagen ecualizada", WINDOW_AUTOSIZE);
	imshow("Imagen ecualizada", imagenEcu);

	namedWindow("Imagen GX", WINDOW_AUTOSIZE);
	imshow("Imagen GX", matrizGx);

	namedWindow("Imagen GY", WINDOW_AUTOSIZE);
	imshow("Imagen GY", matrizGy);

	namedWindow("Imagen filtro sobel", WINDOW_AUTOSIZE);
	imshow("Imagen filtro sobel", sobel);

	/*namedWindow("ImagenAngulosSobel", WINDOW_AUTOSIZE);
	imshow("Imagen angulos Sobel", angulosSobel);*/

	namedWindow("Imagen supresion no maxima", WINDOW_AUTOSIZE);
	imshow("Imagen supresion no maxima", bordeDelgado);

	namedWindow("Imagen doble umbral", WINDOW_AUTOSIZE);
	imshow("Imagen doble umbral", imagenDobleUmbral);

	namedWindow("Imagen conny", WINDOW_AUTOSIZE);
	imshow("Imagen conny", conny);

	waitKey(0);
	
	return 1;
}

Mat PasarKernelGaussImagen(Mat imagenAmpleada, Mat kernel, int filasImagen, int columnasImagen, int tamKernel, float sumKernel)
{
	Mat imagenFiltroGauss(filasImagen, columnasImagen, CV_32F);
	float resultadoKernel = 0;
	int indexAmpleadai = 0;
	int indexAmpleadaj = 0;

	for (int i = 0; i < filasImagen; i++)
	{
		for (int j = 0; j < columnasImagen; j++)
		{
			// Recorrer kernel 
			resultadoKernel = 0;
			for (int k = 0; k < tamKernel; k++)
			{
				for (int l = 0; l < tamKernel; l++)
				{
					indexAmpleadai = i + k;
					indexAmpleadaj = j + l;
					/*cout << indexAmpleadai << ", " << indexAmpleadaj << "\n" << endl;
					cout << kernel.at<float>(Point(k, l)) << endl;*/
					resultadoKernel += float(imagenAmpleada.at<uchar>(Point(indexAmpleadaj, indexAmpleadai))) * kernel.at<float>(Point(l, k));
					// cout << resultadoKernel << endl;
				}
			}
			// cout << "Resultado kernel: " << resultadoKernel << "\n" << endl;
			imagenFiltroGauss.at<float>(Point(j, i)) = resultadoKernel / sumKernel;
		}
	}

	return imagenFiltroGauss;
}

Mat PasarKernelImagen(Mat imagenAmpleada, Mat kernel, int filasImagen, int columnasImagen, int tamKernel)
{
	/*cout << "\imagenAmpleada\n" << imagenAmpleada << "\n" << endl;
	cout << "\kernel\n" << kernel << "\n" << endl;*/

	Mat imagenFiltro(filasImagen, columnasImagen, CV_32F);
	float resultadoKernel = 0;
	int indexAmpleadai = 0;
	int indexAmpleadaj = 0;

	for (int i = 0; i < filasImagen; i++)
	{
		for (int j = 0; j < columnasImagen; j++)
		{
			// Recorrer kernel 
			resultadoKernel = 0;
			for (int k = 0; k < tamKernel; k++)
			{
				for (int l = 0; l < tamKernel; l++)
				{
					indexAmpleadai = i + k;
					indexAmpleadaj = j + l;
					/*cout << indexAmpleadai << ", " << indexAmpleadaj << "\n" << endl;
					cout << kernel.at<float>(Point(k, l)) << endl;*/
					resultadoKernel += float(imagenAmpleada.at<uchar>(Point(indexAmpleadaj, indexAmpleadai))) * kernel.at<float>(Point(l, k));
					// cout << resultadoKernel << endl;
				}
			}
			// cout << "Resultado kernel: " << resultadoKernel << "\n" << endl;
			// imagenFiltroGauss.at<uchar>(Point(i, j)) = uchar(resultadoKernel / sumKernel);
			/*if (resultadoKernel <= 0) {
				resultadoKernel = 0;
			}
			else if (resultadoKernel >= 255) {
				resultadoKernel = 255;
			}
			imagenFiltro.at<float>(Point(j, i)) = resultadoKernel;*/
			imagenFiltro.at<float>(Point(j, i)) = abs(resultadoKernel);
		}
	}

	return imagenFiltro;
}

Mat ExpandirImagen(Mat imagen, int tamKernel)
{
	// Expandimos imagen
	int tamExpancion = ((tamKernel - 1) / 2) * 2;
	// cout << "Numero de filas y columnas a expandir: " << tamExpancion << "\n" << endl;

	int filasImagen = imagen.rows; // Lectura de cuantas filas tiene la imagen
	int columnasImagen = imagen.cols; // Lectura de cuantas columnas tiene la imagen
	int filasImagenAmpleada = filasImagen + tamExpancion;
	int columnasImagenAmpleada = filasImagen + tamExpancion;

	/*cout << "Tamaño imagen original: " << filasImagen * columnasImagen << endl;
	cout << "Tamaño imagen ampleada: " << filasImagenAmpleada * columnasImagenAmpleada << "\n" << endl;*/

	// Creamos una nueva imagen 
	Mat imagenAmpleada(filasImagenAmpleada, columnasImagenAmpleada, CV_8UC1);
	for (int i = 0; i < filasImagenAmpleada; i++) // Inicializamos la imagen ampleada con 0s
	{
		for (int j = 0; j < columnasImagenAmpleada; j++)
		{
			imagenAmpleada.at<uchar>(Point(j, i)) = uchar(0);
		}
	}

	// Pasamos la imagen dejando los espacios de la expancion
	for (int i = 0; i < filasImagen; i++)
	{
		for (int j = 0; j < columnasImagen; j++)
		{
			imagenAmpleada.at<uchar>(Point(j + (tamExpancion / 2), i + (tamExpancion / 2))) = imagen.at<uchar>(Point(j, i));
		}
	}
	// cout << imagenAmpleada << "\n" << endl;
	// cout << "Columnas imagen ampleada: " << columnasImagenAmpleada << "\n" << "Columnas imagen original: " << columnasImagen << "\n" << endl;

	return imagenAmpleada;
}

Mat SupresionNoMaxima(Mat imagenAmpleada, Mat angulosSobel, int filasImagenOriginal, int columnasImagenOriginal)
{
	Mat bordeDelgado(filasImagenOriginal, columnasImagenOriginal, CV_8UC1);
	int vecino1 = 0;
	int vecino2 = 0;
	float angulo = 0;

	for (int i = 0; i < filasImagenOriginal; i++)
	{
		for (int j = 0; j < columnasImagenOriginal; j++)
		{
			angulo = angulosSobel.at<float>(Point(j, i));
			if ((angulo >= 0 && angulo < 22.5) || (angulo >= 157.5 && angulo <= 180)) // 0 - 180 grados 
			{
				vecino1 = imagenAmpleada.at<uchar>(Point(j + 1, (i - 1) + 1)); // + 1 porque estamos recorriendo la matriz ampliada y se le aumentaron dos filas y dos columnas
				vecino2 = imagenAmpleada.at<uchar>(Point(j + 1, (i + 1) + 1)); // tamExpancion / 2
			}
			else if ((67.5 <= angulo < 112.5)) // 90 - 270 grados 
			{
				vecino1 = imagenAmpleada.at<uchar>(Point((j - 1) + 1, i + 1));
				vecino2 = imagenAmpleada.at<uchar>(Point((j + 1) + 1, i + 1));
			}
			else if ((22.5 <= angulo < 67.5)) // 45 - 225 grados
			{
				vecino1 = imagenAmpleada.at<uchar>(Point((j - 1) + 1, (i + 1) + 1));
				vecino2 = imagenAmpleada.at<uchar>(Point((j + 1) + 1, (i - 1) + 1));
			}
			else if ((112.5 <= angulo < 157.5)) // 135 - 315 grados
			{
				vecino1 = imagenAmpleada.at<uchar>(Point((j - 1) + 1, (i - 1) + 1));
				vecino2 = imagenAmpleada.at<uchar>(Point((j + 1) + 1, (i + 1) + 1));
			}

			if (imagenAmpleada.at<uchar>(Point(j + 1, i + 1)) >= vecino1 && imagenAmpleada.at<uchar>(Point(j + 1, i + 1)) >= vecino2){
				bordeDelgado.at<uchar>(Point(j, i)) = imagenAmpleada.at<uchar>(Point(j + 1, i + 1));
			}
			else {
				bordeDelgado.at<uchar>(Point(j, i)) = uchar(0);
			}
		}
	}

	return bordeDelgado;
}

Mat DobleUmbral(Mat imagenAmpleda, int columanasImagenOriginal, int filasImagenOriginal, int max)
{
	Mat imagenDobleUmbral(columanasImagenOriginal, filasImagenOriginal, CV_32F);
	float umbralAlto = max * 0.5;
	float umbralBajo = umbralAlto * 0.5;
	float pixel = 0;

	cout << "\nUmbral alto: " << umbralAlto << endl;
	cout << "Umbral bajo: " << umbralBajo << "\n" << endl;

	for (int i = 0; i < columanasImagenOriginal; i++)
	{
		for (int j = 0; j < filasImagenOriginal; j++)
		{
			pixel = float(imagenAmpleda.at<uchar>(Point(j, i)));
			// cout << pixel << endl;
			if (pixel <= umbralBajo) { // Irrelevante
				imagenDobleUmbral.at<float>(Point(j, i)) = 0;
			}
			else if  (pixel > umbralBajo && pixel < umbralAlto) { // Debil
				// cout << "Entro" << endl;
				imagenDobleUmbral.at<float>(Point(j, i)) = umbralBajo;
			}
			else if (pixel >= umbralAlto) { // Fuerte
				imagenDobleUmbral.at<float>(Point(j, i)) = 255;
			}
		}
	}

	return imagenDobleUmbral;
}

Mat Histeresis(Mat imagenAmpleada, int filasImagenOriginal, int columnasImagenOriginal, int max)
{
	Mat imagenDobleUmbral(filasImagenOriginal, columnasImagenOriginal, CV_32F);
	float umbralAlto = max * 0.5;
	float umbralBajo = umbralAlto * 0.5;
	bool pixelFuerte = false;

	// cout << round(umbralBajo) << endl;

	for (int i = 0; i < filasImagenOriginal; i++)
	{
		for (int j = 0; j < columnasImagenOriginal; j++)
		{
			if (float(imagenAmpleada.at<uchar>(Point(j + 1, i + 1))) == round(umbralBajo))
			{
				// cout << "\nEntro" << endl;
				if (float(imagenAmpleada.at<uchar>(Point(j + 1, (i - 1) + 1))) == 255 || float(imagenAmpleada.at<uchar>(Point(j + 1, (i + 1) + 1))) == 255 ||
					float(imagenAmpleada.at<uchar>(Point((j - 1) + 1, i + 1))) == 255 || float(imagenAmpleada.at<uchar>(Point((j + 1) + 1, i + 1))) == 255 ||
					float(imagenAmpleada.at<uchar>(Point((j - 1) + 1, (i + 1) + 1))) == 255 || float(imagenAmpleada.at<uchar>(Point((j + 1) + 1, (i - 1) + 1))) == 255 ||
					float(imagenAmpleada.at<uchar>(Point((j - 1) + 1, (i - 1) + 1))) == 255 || float(imagenAmpleada.at<uchar>(Point((j + 1) + 1, (i + 1) + 1))) == 255)
				{
					pixelFuerte = true;
				}
				else {
					pixelFuerte = false;
				}

				if (pixelFuerte == true) {
					imagenDobleUmbral.at<float>(Point(j, i)) = 255;
				}
				else {
					imagenDobleUmbral.at<float>(Point(j, i)) = 0;
				}
			}
			else {
				imagenDobleUmbral.at<float>(Point(j, i)) = float(imagenAmpleada.at<uchar>(Point(j + 1, i + 1)));
			}
		}
	}

	return imagenDobleUmbral;
}

int ValorMaximoMatriz(Mat matriz)
{
	int actual = 0;
	int maximo = 0;

	for (int i = 0; i < matriz.rows; i++)
	{
		for (int j = 0; j < matriz.cols; j++)
		{
			actual = int(matriz.at<uchar>(Point(j, i)));
			if (actual >= maximo) {
				maximo = actual;
			}
		}
	}

	return maximo;
}

Mat CrearGx()
{
	Mat kernelGx(3, 3, CV_32F);
	kernelGx.at<float>(Point(0, 0)) = -1;
	kernelGx.at<float>(Point(1, 0)) = 0;
	kernelGx.at<float>(Point(2, 0)) = 1;

	kernelGx.at<float>(Point(0, 1)) = -2;
	kernelGx.at<float>(Point(1, 1)) = 0;
	kernelGx.at<float>(Point(2, 1)) = 2;

	kernelGx.at<float>(Point(0, 2)) = -1;
	kernelGx.at<float>(Point(1, 2)) = 0;
	kernelGx.at<float>(Point(2, 2)) = 1;

	return kernelGx;
}

Mat CrearGy()
{
	Mat kernelGy(3, 3, CV_32F);
	kernelGy.at<float>(Point(0, 0)) = -1;
	kernelGy.at<float>(Point(1, 0)) = -2;
	kernelGy.at<float>(Point(2, 0)) = -1;

	kernelGy.at<float>(Point(0, 1)) = 0;
	kernelGy.at<float>(Point(1, 1)) = 0;
	kernelGy.at<float>(Point(2, 1)) = 0;

	kernelGy.at<float>(Point(0, 2)) = 1;
	kernelGy.at<float>(Point(1, 2)) = 2;
	kernelGy.at<float>(Point(2, 2)) = 1;

	return kernelGy;
}




