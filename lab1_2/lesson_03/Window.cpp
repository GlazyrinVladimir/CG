#include "Window.h"
#include <GL/glew.h>
#include <GL/gl.h>
#include <glm/vec2.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>

void StrokeEllipse(float xCenter, float yCenter, float rx, float ry, int pointCount = 360)
{
    const float step = float(2 * M_PI / pointCount);

    // Эллипс представлен в виде незамкнутой ломаной линии, соединяющей
    // pointCount точек на его границе с шагом 2*PI/pointCount
    glBegin(GL_LINE_STRIP);
    for (float angle = 0; angle < float(2 * M_PI); angle += step)
    {
        const float dx = rx * cosf(angle);
        const float dy = ry * sinf(angle);
        glVertex2f(dx + xCenter, dy + yCenter);
    }
    glEnd();
}

// Рисуем закрашенный эллипс
void FillEllipse(float xCenter, float yCenter, float rx, float ry, int pointCount = 360)
{
    const float step = float(2 * M_PI) / pointCount;

    // Эллипс представлет в виде "веера" из треугольников
    glBegin(GL_TRIANGLE_FAN);
    // Начальная точка веера располагается в центре эллипса
    glVertex2f(xCenter, yCenter);
    // Остальные точки - равномерно по его границе
    for (float angle = 0; angle <= float(2 * M_PI); angle += step)
    {
        float a = (fabsf(angle - float(2 * M_PI)) < 0.00001f) ? 0.f : angle;
        const float dx = rx * cosf(a);
        const float dy = ry * sinf(a);
        glVertex2f(dx + xCenter, dy + yCenter);
    }
    glEnd();
}

void DrawPolylineShapesUsingTriangles(std::string fileName)
{
	std::vector<double> coords;
	int countOfCoords;
	std::vector<double> color(3);
	std::ifstream coordFile(fileName);
	std::string line;
	int mode;
	while (getline(coordFile, line))
	{
		std::istringstream shapesInfo(line);
		shapesInfo >> mode;
		for (size_t i = 0; i < 3; i++)
		{
			shapesInfo >> color[i];
		}
		shapesInfo >> countOfCoords;
		for (size_t i = 0; i < countOfCoords*2; i++)
		{
			double number;
			shapesInfo >> number;
			coords.push_back(number);
		}

		glColor3f(color[0], color[1], color[2]);
		if (mode == 0)
		{
			glBegin(GL_TRIANGLE_FAN);
		}
		else
		{
			int lineSize;
			shapesInfo >> lineSize;
			glLineWidth(lineSize);
			glBegin(GL_LINE_LOOP);
		}
		for (size_t i = 0; i < countOfCoords * 2;)
		{
			glVertex2f(coords[i], coords[i + 1]);
			i = i + 2;
		}
		coords.clear();
		glEnd();
		glLineWidth(1);
	}	
}

CWindow::CWindow()
{
    SetBackgroundColor({1.0f, 1.0f, 1.0f, 1.0f});
}

void CWindow::OnUpdateWindow(float deltaSeconds)
{
    m_time += deltaSeconds;
}

void CWindow::OnDrawWindow(const glm::ivec2 &size)
{
    SetupView(size);
	glEnable(GL_POINT_SMOOTH);
	//ноги, колючки
	DrawPolylineShapesUsingTriangles("triangleShapes.txt");
	//тело
	//контур тела
	glColor3f(0.58, 0.13, 0.228);
	FillEllipse(size.x / 2.0, size.y / 2.0, 132, 102);

	//рисуем тень на теле
	glColor3f(0.78, 0.33, 0.424);
	FillEllipse(size.x / 2.0, size.y / 2.0, 129, 99);

	//рисуем светлую часть тела
	glColor3f(0.87, 0.4, 0.49);
	FillEllipse(size.x / 2.0 -3, size.y / 2.0, 125, 98);
	
	DrawPolylineShapesUsingTriangles("faceShapes.txt");
	glColor3f(0.35, 0.1, 0.35);

	glColor3f(0, 0, 0);
	FillEllipse(385, 278, 11, 11);
	FillEllipse(411, 278, 11, 11);
	//очки
	glColor3f(0.35, 0.1, 0.35);

	for (float i = 0; i < 8; i += 0.5)
	{
		StrokeEllipse(365, 280, 30 + i, 30 + i);
	}

	for (float i = 0; i < 8; i += 0.5)
	{
		StrokeEllipse(431, 277, 30 + i, 30 + i);
	}

	DrawPolylineShapesUsingTriangles("hands2.txt");	
}

void CWindow::SetupView(const glm::ivec2 &size)
{
    // Матрица ортографического проецирования изображения в трёхмерном пространстве
    // из параллелипипеда с размером, равным (size.X x size.Y x 2).
    const glm::mat4 matrix = glm::ortho<float>(0, float(size.x), float(size.y), 0);
    glViewport(0, 0, size.x, size.y);
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(glm::value_ptr(matrix));
    glMatrixMode(GL_MODELVIEW);
}
