// GL_lab1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>

float gx = 0.35f, gy = 0.58f, gw, gh;

void OnPaint()
{
	glClearColor(1, 1, 1, 0);
	glClear(GL_COLOR_BUFFER_BIT);
	//glBegin(GL_POINTS);		
	//glBegin(GL_LINES);
	//glBegin(GL_LINE_STRIP);
	//glBegin(GL_LINE_LOOP);
	glBegin(GL_TRIANGLES);
	//glBegin(GL_TRIANGLE_STRIP);
	//glBegin(GL_TRIANGLE_FAN);
	//glBegin(GL_QUADS);

	glColor3f(1, 0, 0);
	glVertex2f(-0.3, 0.6);
	glVertex2f(-0.8, 0.8);
	glVertex2f(-0.8, -0.8);
	glVertex2f(0, -0.7);
	glColor3f(0, 0, 1);
	glVertex2f(0.8, 0.8);
	glVertex2f(0.8, -0.8);
	glVertex2f(0.5, 0.1);
	glColor3f(0, 1, 0);
	glVertex2f(0.1, 0.1);
	glVertex2f(-0.1, -0.2);
	glVertex2f(250, 250);
	glEnd();

	glEnable(GL_SCISSOR_TEST); 
	glClearColor(0.9f, 0.8f, 0.7f, 0); 
	glClear(GL_COLOR_BUFFER_BIT);
	glBegin(GL_POINTS);
	glColor3f(0, 0, 1);
	glVertex2f(-0.2f, 0.2f);
	glVertex2f(0, 0.2f);
	glVertex2f(0, 0.1f);
	glVertex2f(0.2f, 0.2f);
	glVertex2f(0, -0.4f);
	glEnd();
	glDisable(GL_SCISSOR_TEST);
	glutSwapBuffers();

	glFlush();
}
void OnSize(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(-1, 1, -1, 1);	
	gw = w;
	gh = h;
	glScissor((int)(gx * w), (int)(gy * h), (int)(0.3f * w), (int)(0.3f * h));

}
void Init()	// Настройки
{	
	//glShadeModel(GL_FLAT);		
	glPointSize(7);
	glEnable(GL_POINT_SMOOTH);
}

void OnSpecialKey(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_LEFT: gx -= 0.005f; break;
	case GLUT_KEY_RIGHT: gx += 0.005f; break;
	case GLUT_KEY_DOWN: gy -= 0.005f; break;
	case GLUT_KEY_UP:  gy += 0.005f; break;
	}
	glScissor((int)(gx * gw), (int)(gy * gh), (int)(0.3f * gw), (int)(0.3f * gh));
	glutPostRedisplay();
}

void main() {
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(500, 500);
	glutCreateWindow("lab_1");
	glutSpecialFunc(OnSpecialKey);
	Init();
	glutReshapeFunc(OnSize);
	glutDisplayFunc(OnPaint);
	glutMainLoop();
}

