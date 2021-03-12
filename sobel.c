#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <math.h>

int core[3][3] = {	{ -1, -2, -1 },
					{  0,  0,  0 },
					{  1,  2,  1 }};
/*
int core[3][3] = { 	{  -3, -10,  -3},
					{   0,   0,   0},
					{   3,  10,   3}}; 
*/

float intensity[3]	=	{0.2126, 0.7152, 0.0722};	//Интенсивность разных цветов
//float intensity[3] = {0.33, 0.33, 0.33};
char brightness(char r, char g, char b);	//Яркость точки 
int convolution(char** arr, char direction, int x, int y);	//Свертка изображения.

//Аргументы:
//tsobel <input file> <output file> [threads]

int main(int argc, char* argv[])
{
	int thread_count = 1;
	if (argc < 3)
	{
		printf("Недостаточно аргументов.\n");
		return 1;
	}
	if (argc > 3)
	{
		thread_count = atoi(argv[3]);
		if (thread_count < 1)	//TODO - максимальное кол-во потоков
		{
			printf("Непрально указано количество потоков.\n");
			return 1;
		}
	}
	int in_file = open(argv[1], O_RDONLY);
	if (in_file < 0)
	{
		perror("Ошибка открытия файла.");
		return errno;
	}
	int out_file = open(argv[2], O_CREAT | O_WRONLY, 0666);
	//Все хранится в памяти
	char** pic;
	char buf[6];
	char temp;
	read(in_file, buf, 3);
	if (buf[0] == 'P' && buf[1] == '6')
		write(out_file, "P6\n", 3);
	else
	{
		printf("не P6\n");
		return 2;
	}
	//TODO - Пропуск коммента
	for (int i = 0; ; i++)	//Вставить проверку на конец файла и 
	{
		read(in_file, &temp, 1);
		if (temp < '0' || temp > '9')
		{
			buf[i] = '\0';
			break;
		}
		else 
			buf[i] = temp;
	}
	write(out_file, &buf, strlen(buf));
	write(out_file, " ", 1);
	int w = atoi(buf);
	for (int i = 0; ; i++)	//Вставить проверку на конец файла и 
	{
		read(in_file, &temp, 1);
		if (temp < '0' || temp > '9')
		{
			buf[i] = '\0';
			break;
		}
		else 
			buf[i] = temp;
	}
	write(out_file, &buf, strlen(buf));
	write(out_file, "\n", 1);
	int h = atoi(buf);
	pic = malloc(w * sizeof(void*));
	for (int i = 0; i < w; i++)
		pic[i] = malloc(h * sizeof(char));	//pic[w][h]
	printf("%ix%i\n", w, h);
	read(in_file, buf, 4);
	write(out_file, "255\n", 4);
	for (int i = 0; i < h; i++)
	{
		for (int j = 0; j < w; j++)
		{
			int r, g, b;
			r = 0;
			g = 0;
			b = 0;
			read(in_file, &r, 1);
			read(in_file, &g, 1);
			read(in_file, &b, 1);
			pic[j][i] = brightness(r, g, b);	//Запись в ч/б
		}
	}
	//Непосредственно сама обработка
	printf("Количество потоков: %i\n", thread_count);
	clock_t start = clock();
	for (int i = 0; i < h; i++)
	{
		char sobel = 0;			
		for (int j = 0; j < w; j++)
		{
			if (i == 0 || i == h-1 || j == 0 || j == w-1)	//Края не обрабатываются
				sobel = 0;
			else
			{
				int gy = convolution(pic, 0, j, i);
				int gx = convolution(pic, 1, j, i);
				sobel = hypot(gx, gy);
			}	
			for (int k = 0; k < 3; k++)
				write(out_file, &sobel, 1);
		}	
	}
	double time = (double)(clock() - start) / CLOCKS_PER_SEC;
	printf("Прошло %f с.", time);
	//Освобождение памяти
	for (int i = 0; i < w; i++)
		free(pic[i]);
	free(pic);
	//TODO - многопоточность
	return 0;
}

char brightness(char r, char g, char b)	//Яркость точки 
{
	return r * intensity[0] + g * intensity[1] + b * intensity[2];
}

int convolution(char** arr, char direction, int x, int y)	//Свертка изображения
{
	int result = 0;
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			if (direction == 0)	//Вертикальная свертка
				result += core[i][j] * arr[x+i-1][y+i-1];
			else
				result += core[j][i] * arr[x+i-1][y+i-1];
	return result;
}

