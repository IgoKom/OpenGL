// GL_lab1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"

void OnPaint()
{

	glClear(GL_COLOR_BUFFER_BIT);	
	//glBegin(GL_POINTS);		
	//glBegin(GL_LINES);
	//glBegin(GL_LINE_STRIP);
	//glBegin(GL_LINE_LOOP);
	//glBegin(GL_TRIANGLES);
	//glBegin(GL_TRIANGLE_STRIP);
	//glBegin(GL_TRIANGLE_FAN);
	glBegin(GL_QUADS);

	glColor3f(1, 0, 0);
	glVertex2f(-0.3, 0.6);
	glVertex2f(-0.8, 0.8);
	glVertex2f(-0.8, -0.8);
	glVertex2f(0, -0.7);
	glColor3f(0, 0, 1);
	glVertex2f(0.8, 0.8);
	glVertex2f(0.8, -0.8);
	/*glVertex2f(0.5, 0.1);
	glColor3f(0, 1, 0);
	glVertex2f(50, 50);
	glVertex2f(100, 200);
	glVertex2f(50, 200);*/
	glEnd();
	glFlush();
}
void OnSize(int w, int h)	
{
	glViewport(0, 0, w, h);
	//glMatrixMode(GL_PROJECTION);
	//glLoadIdentity();
	//gluOrtho2D(-1, 1, -1, 1);
	//gluOrtho2D(0, w, 0, h);
}
void Init()	// Настройки
{
	glClearColor(1, 1, 1, 0);	
	//glShadeModel(GL_FLAT);		
	glPointSize(7);					
	glEnable(GL_POINT_SMOOTH);		
}


void main()
{
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(500, 500);
	glutCreateWindow("lab_1");
	Init();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(-1, 1, -1, 1);
	glutReshapeFunc(OnSize);
	glutDisplayFunc(OnPaint);
	glutMainLoop();

}

