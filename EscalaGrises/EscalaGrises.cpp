#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

int main()
{
	char NombreImagen[] = "Lenna.png";
	Mat imagen; // Matriz que contiene nuestra imagen sin importar el formato
	Mat imagenGrises;
	int i, j;
	double azul, verde, rojo;

	imagen = imread(NombreImagen); // Lectura de la imagen
	if (!imagen.data)
	{
		cout << "Error al cargar la imagen: " << NombreImagen << endl;
		exit(1);
	}

	int filasOriginal = imagen.rows; // Lectura de cuantas filas tiene la imagen
	int columnasOriginal = imagen.cols; // Lectura de cuantas columnas tiene la imagen

	Mat imagenGrisesPromedio(filasOriginal, columnasOriginal, CV_8UC1); // CV_8UC1 Dato de tipo uchar de un solo canal
	Mat imagenGrisesNTSC(filasOriginal, columnasOriginal, CV_8UC1);

	//cout << "filas: " << filasOriginal << endl;
	//cout << "columnas: " << columnasOriginal << endl;

	//cvtColor(imagen, imagenGrises, COLOR_BGR2GRAY); // Funcion para pasar a escala de grises con openCV

	for (i = 0; i < filasOriginal; i++)
	{
		for (j = 0; j < columnasOriginal; j++)
		{
			azul = imagen.at<Vec3b>(Point(j, i)).val[0];  // B
			verde = imagen.at<Vec3b>(Point(j, i)).val[1]; // G
			rojo = imagen.at<Vec3b>(Point(j, i)).val[2];  // R

			//azul = imagen.at<Vec3b>(Point(i, j)).val[0];  // B
			//verde = imagen.at<Vec3b>(Point(i, j)).val[1]; // G
			//rojo = imagen.at<Vec3b>(Point(i, j)).val[2];  // R

			imagenGrisesPromedio.at<uchar>(Point(j, i)) = uchar((rojo + verde + azul) / 3);
			imagenGrisesNTSC.at<uchar>(Point(j, i)) = uchar(0.299*rojo + 0.587*verde + 0.114*azul);
		}
	}

	FILE *matrizImgOriginal, *matrizImgPromedio, *matrizImgNTSC;

	matrizImgOriginal = fopen("imgOriginal.xls", "w");
	matrizImgPromedio = fopen("imgPromedio.xls", "w");
	matrizImgNTSC = fopen("imgNTSC.xls", "w");

	uchar valorOriginal, valorPromedio, valorNTSC;
	int valorEnteroOriginal, valorEnteroPromedio, valorEnteroNTSC;

	for (i = 0; i < filasOriginal; i++)
	{
		for (j = 0; j < columnasOriginal; j++)
		{
			valorOriginal = imagen.at<uchar>(Point(j, i));
			valorPromedio = imagenGrisesPromedio.at<uchar>(Point(j, i));
			valorNTSC = imagenGrisesNTSC.at<uchar>(Point(j, i));

			valorEnteroOriginal = static_cast<int>(valorOriginal);
			valorEnteroPromedio = static_cast<int>(valorPromedio);
			valorEnteroNTSC = static_cast<int>(valorNTSC);

			fprintf(matrizImgOriginal, "%d \t", valorEnteroOriginal);
			fprintf(matrizImgPromedio, "%d \t", valorEnteroPromedio);
			fprintf(matrizImgNTSC, "%d \t", valorEnteroNTSC);
		}
		fprintf(matrizImgOriginal, "\n");
		fprintf(matrizImgPromedio, "\n");
		fprintf(matrizImgNTSC, "\n");
	}
	fclose(matrizImgOriginal);
	fclose(matrizImgPromedio);
	fclose(matrizImgNTSC);

	namedWindow("Imagen original", WINDOW_AUTOSIZE); // Creación de una ventana
	imshow("Imagen original", imagen);

	//namedWindow("Imagen escala de grises", WINDOW_AUTOSIZE); 
	//imshow("Imagen escala de grises", imagenGrises);

	namedWindow("Imagen escala de grises (Promedio)", WINDOW_AUTOSIZE); 
	imshow("Imagen escala de grises (Promedio)", imagenGrisesPromedio);

	namedWindow("Imagen escala de grises (NTSC)", WINDOW_AUTOSIZE); 
	imshow("Imagen escala de grises (NTSC)", imagenGrisesNTSC);

	//imwrite("LennaGrises.png", imagenGrises); // Guardar imagen en disco

	waitKey(0); // Función para esperar
	return 1;
}

