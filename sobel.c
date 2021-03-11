#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <math.h>


char core[] = {1, 2, 1};
char brightness(char r, char g, char b);	//Яркость точки 

int main(int argc, char* argv[])
{
	if (argc != 3)
	{
		printf("А где аргумент\n");
		return 1;
	}
	int in_file = open(argv[1], O_RDONLY);
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
	//Запись производится следующим образом: края - черная рамка
	for (int i = 0; i < h; i++)
	{
		char sobel = 0;			
		for (int j = 0; j < w; j++)
		{
			if (i == 0 || i == h-1 || j == 0 || j == w-1)
				sobel = 0;
			else
			{
				int gy = - core[0]*pic[j-1][i-1] - core[1]*pic[j][i-1] - core[2]*pic[j+1][i-1] + core[0]*pic[j-1][i+1] + core[1]*pic[j][i+1] + core[2]*pic[j+1][i+1];
				int gx = - core[0]*pic[j-1][i-1] - core[1]*pic[j-1][i] - core[2]*pic[j-1][i+1] + core[0]*pic[j+1][i-1] + core[1]*pic[j+1][i] + core[2]*pic[j+1][i+1];
				sobel = hypot(gx, gy);
			}	
			for (int k = 0; k < 3; k++)
				write(out_file, &sobel, 1);
		}	
	}

	//Освобождение памяти
	for (int i = 0; i < w; i++)
		free(pic[i]);
	free(pic);
	//TODO - многопоточность
	//TODO - таймер
	return 0;
}

char brightness(char r, char g, char b)	//Яркость точки 
{
	return (0.2126*r + 0.7152*g + 0.0722*b); 	
}

