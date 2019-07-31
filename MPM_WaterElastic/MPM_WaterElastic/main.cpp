#include <windows.h>
#include <iostream>
#include <cmath>
#include<stdlib.h>
#include"Parameters.h"
#include"Grid.h"
#include <fstream>
#define BMP_Header_Length 54
Grid* grid;
Particle_Cloud* bar;
Particle Object;
int N = 0;
FILE* pWritingFile;
GLubyte BMP_Header[BMP_Header_Length] = { 'B','M','~',';','b',0,0,0,0,0,
'6',0,0,0,'(',0,0,0,182,8,
0,0,194,3,0,0,1,0,24,0,
0,0,0,0,72,59,98,0,0,0,
0,0,0,0,0,0,0,0,0,0,
0,0,0,0 };
GLint i = WINDOW_WIDTH * 3, j;
GLint PixelDataLength = i * WINDOW_HEIGHT;
void grab(int k)
{
	GLubyte* pPixelData;
	// 分配内存和打开文件 
	pPixelData = (GLubyte*)malloc(PixelDataLength);
	if (pPixelData == 0)
		exit(0);
	stringstream ss;
	ss << "grab";
	ss << k;
	ss << ".bmp";
	string a = ss.str();
	const char* b = a.data();
	fopen_s(&pWritingFile, b, "wb");
	if (pWritingFile == 0)
		exit(0);

	// 读取像素 
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glReadPixels(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
		GL_BGR_EXT, GL_UNSIGNED_BYTE, pPixelData);

	fwrite(BMP_Header, sizeof(BMP_Header), 1, pWritingFile);
	fseek(pWritingFile, 0x0012, SEEK_SET);
	i = WINDOW_WIDTH;
	j = WINDOW_HEIGHT;
	fwrite(&i, sizeof(i), 1, pWritingFile);
	fwrite(&j, sizeof(j), 1, pWritingFile);

	// 写入像素数据 
	fseek(pWritingFile, 0, SEEK_END);
	fwrite(pPixelData, PixelDataLength, 1, pWritingFile);

	// 释放内存和关闭文件 
	fclose(pWritingFile);
	free(pPixelData);
}

void InitGL(void)
{
	glClearColor(0.9f, 0.9f, 0.9f, 1);
	glEnable(GL_POINT_SMOOTH);
	glPointSize(3);
	glMatrixMode(GL_PROJECTION);
}

void Update(void)
{
	grid->Update_Grid();
	grid->Update_Particle_Cloud();
	N++;
	if (N / 20 > 400) exit(0);
	
	if (!(N % 20))
	{
		//grab(N / 20);
	}
	glutPostRedisplay();
}

void Render(void)
{
	glClear(GL_COLOR_BUFFER_BIT);

	glLoadIdentity();
	glOrtho(0, VIEW_WIDTH, 0, VIEW_HEIGHT, 0, 1);

	glBegin(GL_POINTS);
	glColor4f(0.2f, 0.6f, 1.0f, 1);
	glEnd();
	bar->draw();
	glFlush();
}

void InitMPM(void)
{
	bar = new Particle_Cloud();
	/*double tmp1 = 0, tmp2 = 0;
	ifstream myfile("out.txt");
	if (!myfile.is_open())
	{
		cout << "can not open this file" << endl;
		return;
	}
	for (int i = 0; i < 14883; i++)
	{
		myfile >> tmp1;
		myfile >> tmp2;
		tmp1 /= 1000;
		tmp2 /= 1000;
		tmp2 += 0.1;
		tmp1 -= 0.1;
		Object = Particle(Vector2d(tmp2, tmp1), Vector2d((tmp2-0.5)*2, 1.0), 5.0, Elastic, 8000000);
		bar->points.push_back(Object);
	}*/
	for (int i = 0; i < 50; i++)
	{
		for (int j = 0; j < 50; j++)
		{
			Object = Particle(Vector2d(i/150.0+0.5, j/150.0+0.5), Vector2d(0.0, 1.0), 5.0, Water, 800000);
			bar->points.push_back(Object);
			Object = Particle(Vector2d(i / 150.0 + 0.1, j / 150.0 + 0.1), Vector2d(0.0, -1.0), 5.0, Elastic, 800000);
			bar->points.push_back(Object);
		}
	}
	grid = new Grid(Vector2d(0, 0), Vector2d(VIEW_WIDTH, VIEW_HEIGHT), Vector2i(101, 101), bar);
	grid->Init_Grid();
}

int main(int argc, char** argv)
{
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutInitWindowPosition(0, 0);
	glutInit(&argc, argv);
	glutCreateWindow("MPMElastic");
	glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
	InitGL();
	InitMPM();
	glutDisplayFunc(Render);
	glutIdleFunc(Update);
	glutMainLoop();
	delete bar;
	delete grid;
	return 0;
}