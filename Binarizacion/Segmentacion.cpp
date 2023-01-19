#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>

using namespace cv;
using namespace std;

Mat PasarKernelGaussImagen(Mat imagenAmpleada, Mat kernel, int filasImagen, int columnasImagen, int tamKernel, float sumKernel);
Mat matrizPrueba();
Mat ExpandirImagen(Mat imagen, int tamKernel);
Mat umbralizar(Mat imagen, int umbral);
vector<int> histogram(Mat matriz, int nivelGrises);
int isoData(float T, vector<int> histograma);
int picosMaximos(vector<int> histograma);
int otsu(vector<int> histograma);
int maxArreglo(vector<int> arreglo);
int minArreglo(vector<float> arreglo);
int redondearNumero(float numero);

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

	imagen = imread(NombreImagen, 0); // Lectura de la imagen
	if (!imagen.data)
	{
		cout << "Error al cargar la imagen: " << NombreImagen << endl;
		exit(1);
	}

	int filasImagen = imagen.rows; // Lectura de cuantas filas tiene la imagen
	int columnasImagen = imagen.cols; // Lectura de cuantas columnas tiene la imagen

	// Expandimos imagen
	Mat imagenAmpleada = ExpandirImagen(imagen, tamKernel);

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

	Mat matrizTest = matrizPrueba();
	// cout << "\nMatriz de prueba\n" << matrizTest << endl;

	cout << "\nHistograma" << endl;
	vector<int> histograma = histogram(imagenFiltroGauss, 256);
	for (i = 0; i < histograma.size(); i++) {
		cout << i << " " << histograma[i] << endl;
	}
	cout << "\n" << endl;

	cout << "\nUmbral ISODATA" << endl;
	int umbralIsoData = isoData(128, histograma);
	cout << "Umbral: " << umbralIsoData << endl;

	Mat imagenUmbralizadaIsoData = umbralizar(imagenFiltroGauss, umbralIsoData);

	cout << "\nUmbral picos maximos" << endl;
	int umbralPicosMaximos = picosMaximos(histograma);
	cout << "Umbral: " << umbralPicosMaximos << endl;

	Mat imagenUmbralizadaPicosMaximos = umbralizar(imagenFiltroGauss, umbralPicosMaximos);

	cout << "\nUmbral OTSU" << endl;
	int umbralOtsu = otsu(histograma);
	cout << "Umbral: " << umbralOtsu << endl;

	Mat imagenUmbralizadaOtsu = umbralizar(imagenFiltroGauss, umbralOtsu);

	namedWindow("Umbralizado ISODATA", WINDOW_AUTOSIZE);
	imshow("Umbralizado ISODATA", imagenUmbralizadaIsoData);

	namedWindow("Umbralizado picos maximos", WINDOW_AUTOSIZE);
	imshow("Umbralizado picos maximos", imagenUmbralizadaPicosMaximos);

	namedWindow("Umbralizado Otsu", WINDOW_AUTOSIZE);
	imshow("Umbralizado Otsu", imagenUmbralizadaOtsu);

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
					resultadoKernel += float(imagenAmpleada.at<uchar>(Point(indexAmpleadaj, indexAmpleadai))) * kernel.at<float>(Point(l, k));
				}
			}
			imagenFiltroGauss.at<float>(Point(j, i)) = resultadoKernel / sumKernel;
		}
	}

	return imagenFiltroGauss;
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

Mat umbralizar(Mat imagen, int umbral)
{
	for (int i = 0; i < imagen.rows; i++)
	{
		for (int j = 0; j < imagen.cols; j++)
		{
			if (int(imagen.at<uchar>(Point(j, i))) <= umbral) {
				imagen.at<uchar>(Point(j, i)) = uchar(0);
			}
			else if (int(imagen.at<uchar>(Point(j, i))) > umbral) {
				imagen.at<uchar>(Point(j, i)) = uchar(255);
			}
		}
	}

	return imagen;
}

Mat matrizPrueba()
{
	Mat test(7, 8, CV_8UC1);
	test.at<uchar>(Point(0, 0)) = 2;
	test.at<uchar>(Point(1, 0)) = 2;
	test.at<uchar>(Point(2, 0)) = 8;
	test.at<uchar>(Point(3, 0)) = 3;
	test.at<uchar>(Point(4, 0)) = 0;
	test.at<uchar>(Point(5, 0)) = 7;
	test.at<uchar>(Point(6, 0)) = 9;
	test.at<uchar>(Point(7, 0)) = 6;

	test.at<uchar>(Point(0, 1)) = 4;
	test.at<uchar>(Point(1, 1)) = 5;
	test.at<uchar>(Point(2, 1)) = 9;
	test.at<uchar>(Point(3, 1)) = 0;
	test.at<uchar>(Point(4, 1)) = 6;
	test.at<uchar>(Point(5, 1)) = 8;
	test.at<uchar>(Point(6, 1)) = 2;
	test.at<uchar>(Point(7, 1)) = 4;

	test.at<uchar>(Point(0, 2)) = 7;
	test.at<uchar>(Point(1, 2)) = 8;
	test.at<uchar>(Point(2, 2)) = 3;
	test.at<uchar>(Point(3, 2)) = 2;
	test.at<uchar>(Point(4, 2)) = 1;
	test.at<uchar>(Point(5, 2)) = 7;
	test.at<uchar>(Point(6, 2)) = 9;
	test.at<uchar>(Point(7, 2)) = 8;

	test.at<uchar>(Point(0, 3)) = 0;
	test.at<uchar>(Point(1, 3)) = 1;
	test.at<uchar>(Point(2, 3)) = 4;
	test.at<uchar>(Point(3, 3)) = 6;
	test.at<uchar>(Point(4, 3)) = 1;
	test.at<uchar>(Point(5, 3)) = 1;
	test.at<uchar>(Point(6, 3)) = 8;
	test.at<uchar>(Point(7, 3)) = 1;

	test.at<uchar>(Point(0, 4)) = 3;
	test.at<uchar>(Point(1, 4)) = 4;
	test.at<uchar>(Point(2, 4)) = 0;
	test.at<uchar>(Point(3, 4)) = 7;
	test.at<uchar>(Point(4, 4)) = 2;
	test.at<uchar>(Point(5, 4)) = 0;
	test.at<uchar>(Point(6, 4)) = 2;
	test.at<uchar>(Point(7, 4)) = 8;

	test.at<uchar>(Point(0, 5)) = 6;
	test.at<uchar>(Point(1, 5)) = 2;
	test.at<uchar>(Point(2, 5)) = 2;
	test.at<uchar>(Point(3, 5)) = 3;
	test.at<uchar>(Point(4, 5)) = 2;
	test.at<uchar>(Point(5, 5)) = 7;
	test.at<uchar>(Point(6, 5)) = 7;
	test.at<uchar>(Point(7, 5)) = 3;

	test.at<uchar>(Point(0, 6)) = 5;
	test.at<uchar>(Point(1, 6)) = 0;
	test.at<uchar>(Point(2, 6)) = 1;
	test.at<uchar>(Point(3, 6)) = 9;
	test.at<uchar>(Point(4, 6)) = 9;
	test.at<uchar>(Point(5, 6)) = 7;
	test.at<uchar>(Point(6, 6)) = 9;
	test.at<uchar>(Point(7, 6)) = 0;

	return test;
}

vector<int> histogram(Mat matriz, int nivelGrises)
{
	matriz.convertTo(matriz, CV_32S);
	vector<int> intensidades(nivelGrises);
	int intensidad = 0;
	int pixeles = 0;

	/*while (intensidad < nivelGrises)
	{
		for (int i = 0; i < matriz.rows; i++)
		{
			for (int j = 0; j < matriz.cols; j++)
			{
				if (matriz.at<int>(Point(j, i)) == intensidad){
					intensidades[intensidad] += 1;
				}
			}
		}
		pixeles += intensidades[intensidad];
		intensidad++;
	}
	cout << "Pixeles: " << pixeles << "\n" << endl;*/

	for (int i = 0; i < matriz.rows; i++)
	{
		for (int j = 0; j < matriz.cols; j++)
		{
			intensidad = matriz.at<int>(Point(j, i));
			intensidades[intensidad] += 1;
		}
	}

	for (int i = 0; i < intensidades.size(); i++) // Pixeles totales
	{
		pixeles += intensidades[i];
	}
	cout << "Pixeles totales: " << pixeles << "\n" << endl;

	return intensidades;
}

int redondearNumero(float numero) 
{
	int numeroRedondeado = 0;
	float decimal = numero - int(numero);
	if (decimal <= 0.5) {
		numeroRedondeado = int(numero);
	}
	else {
		numeroRedondeado = round(numero);
	}

	return numeroRedondeado;
}

int isoData(float T, vector<int> histograma)
{
	float umbral = 0;
	float umbralAnterior = 0;
	float diferencia = 999999;
	int iteraciones = 0;
	float deltaT = 0.5;

	while (1)
	{
		float media1 = 0;
		float media2 = 0;
		float grupo1 = 0;
		float grupo2 = 0;
		float datosGrupo1 = 0;
		float datosGrupo2 = 0;
		
		iteraciones++;
		umbralAnterior = umbral;

		for (int i=0; i < histograma.size(); i++) // Sacamos los grupos
		{
			if (i <= T) 
			{
				grupo1 += histograma[i];
				media1 += i * histograma[i];
				datosGrupo1++;
			}
			else if (i > T) 
			{
				grupo2 += histograma[i];
				media2 += i * histograma[i];
				datosGrupo2++;
			}
		}
		cout << "grupo1: " << grupo1 << " " << "grupo2: " << grupo2 << endl;
		cout << "datosGrupo1: " << datosGrupo1 << " " << "datosGrupo2: " << datosGrupo2 << endl;

		// Calculamos el nuevo umbral 
		// media1 = redondearNumero(grupo1 / float(datosGrupo1));
		// media2 = redondearNumero(grupo2 / float(datosGrupo2));
		media1 = media1 / float(grupo1);
		media2 = media2 / float(grupo2);
		// umbral = redondearNumero((media1 + media2) / float(2));
		umbral = (media1 + media2) / float(2);
		diferencia = abs(umbral - umbralAnterior);
		cout << "media1: " << media1 << " " << "media2: " << media2 << endl;
		cout << "umbral: " << umbral << endl;
		cout << "diferencia: " << diferencia << "\n" << endl;

		if (diferencia < deltaT) {
			break;
		}

		T = umbral;
		cout << "Iteraciones: " << iteraciones + 1 << endl;
	}
	return int(umbral);
}

int picosMaximos(vector<int> histograma)
{
	int max = maxArreglo(histograma); // Maximo histograma
	int max2 = 0;
	int max2Aux = 0;
	int indexMax2 = 0;
	int valoresEntreMax = 0;
	int umbral = 0;

	cout << "Maximo: " << max << endl;
	// Buscamos el segundo maximo
	for (int i = 0; i < histograma.size(); i++)
	{
		if (i != max) {
			max2Aux = pow((i - max), 2) * histograma[i];
		}

		if (max2Aux >= max2) 
		{
			max2 = max2Aux;
			indexMax2 = i;
		}
	}
	cout << "Segundo maximo: " << indexMax2 << endl;

	valoresEntreMax = indexMax2 - max;
	umbral = redondearNumero((valoresEntreMax / float(2)) + max);
	return umbral;
}

int otsu(vector<int> histograma)
{
	vector<float> varianzas(0);
	for (int i = 0; i < histograma.size(); i++)
	{
		float varianza = 0;
		float varianzaB = 0;
		float varianzaF = 0;
		float pesoB = 0;
		float pesoF = 0;
		float mediaB = 0;
		float mediaF = 0;
		int sumIntensidadesB = 0;
		int sumIntensidadesF = 0;
		int pixelesB = 0;
		int pixelesF = 0;

		for (int j = 0; j < histograma.size(); j++) // Sacamos pesos
		{
			if (j <= i) {
				sumIntensidadesB += histograma[j];
			}
			else if (j > i) {
				sumIntensidadesF += histograma[j];
			}
		}
		pixelesB = sumIntensidadesB;
		pixelesF = sumIntensidadesF;
		pesoB = sumIntensidadesB / float(pixelesB + pixelesF);
		pesoF = sumIntensidadesF / float(pixelesB + pixelesF);
		/*cout << "Total pixeles: " << pixelesB + pixelesF << endl;
		cout << "PixelesB: " << pixelesB << " " << "PixelesF: " << pixelesF << endl;
		cout << "PesoB: " << pesoB << " " << "PesoF: " << pesoF << endl;*/

		for (int j = 0; j < histograma.size(); j++) // Sacamos medias
		{
			if (j <= i) {
				mediaB += j * histograma[j];
			}
			else if (j > i) {
				mediaF += j * histograma[j];
			}
		}

		if (pixelesF > 0) {
			mediaF = mediaF / sumIntensidadesF;
		}
		else {
			mediaF = 0;
		}

		if (pixelesB > 0) {
			mediaB = mediaB / sumIntensidadesB;
		}
		else {
			mediaB = 0;
		}
		// cout << "MediaB: " << mediaB << " " << "MediaF: " << mediaF << endl;

		for (int j = 0; j < histograma.size(); j++) // Sacamos Varianzas
		{
			if (j <= i) {
				varianzaB += pow((j - mediaB), 2) * histograma[j];
			}
			else if (j > i) {
				varianzaF += pow((j - mediaF), 2) * histograma[j];
			}
		}
		
		if (pixelesF > 0) {
			varianzaF = varianzaF / pixelesF;
		}
		else {
			varianzaF = 0;
		}

		if (pixelesB > 0) {
			varianzaB = varianzaB / pixelesB;
		}
		else {
			varianzaB = 0;
		}
		varianza = (pesoB * varianzaB) + (pesoF * varianzaF);
		varianzas.push_back(varianza);
		/*cout << "Varianza: " << varianza << endl;
		cout << "VarianzaB: " << varianzaB << " " << "VarianzaF: " << varianzaF << "\n" << endl;*/
	}

	cout << "\nVarianzas" << endl;
	for (int i = 0; i < varianzas.size(); i++) {
		cout << i << " " << varianzas[i] << endl;
	}
	cout << "\n" << endl;

	return minArreglo(varianzas);
}

int maxArreglo(vector<int> arreglo)
{
	int max = 0;
	int index = 0;
	for (int i = 0; i < arreglo.size(); i++)
	{
		if (arreglo[i] >= max) 
		{
			max = arreglo[i];
			index = i;
		}
	}

	return index;
}

int minArreglo(vector<float> arreglo)
{
	int min = 999999;
	int index = 0;
	for (int i = 0; i < arreglo.size(); i++)
	{
		if (arreglo[i] <= min)
		{
			min = arreglo[i];
			index = i;
		}
	}

	return index;
}