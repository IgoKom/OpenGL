
#include "pch.h"
#include <iostream>

char sModel[8] = "smooth", buf[4] = "on";
float
ax = 90, aEarth = 0, aEarthOrbit = 0, ay = 0, aSun = 0, aMoon = 0, aMoonOrbit = 0, DeathStar = 0, X = 0, del = 0.001,
dx, dy, dz = -6,
rotX, rotY,
rotMatrix[16] = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 },
color[] = { 1.0f, 1.0f, 0.1f },
posLight1[] = { -0.5, 0.5, 1, 0 },
posSun[] = { 0, 0, 0, 1 },
amb[] = { 0.05f, 0.05f, 0.05f },
dif[] = { 0.5f, 0.5f, 0.5f },
spec[] = { 0.5, 0.5, 0.5 },
sunEmis[] = { 0.5, 0.4, 0 },
earthDif[] = { 0.0f, 0.9f, 0.0f },
moonDif[] = { 0.3f, 0.3f, 0.6f },
specE[] = { 0.7f, 0.7f, 0.7f },
specM[] = { 0.5f, 0.5f, 0.5f },
col[] = { 0.5,0.5,0.8 },
col1[] = {1,1,1},
col2[] = { 0.1,0.1,0.2 };

short posX, posY, shineE = 50, shineM = 10;
bool leftButton, twoSide, stop = true, enLight1 = true, enEarth = false, enEarthOrbit = false,
		enMoon = false, enMoonOrbit = false, enSun = false, go = false;
	   

	void Print(float x, float y, const char *format, ...)
	{
		va_list args;
		char buffer[200];
		va_start(args, format);
		vsprintf(buffer, format, args);
		va_end(args);
		glPushMatrix();
		glTranslatef(x, y, 0);
		for (char* p = buffer; *p; p++)
			glutStrokeCharacter(GLUT_STROKE_ROMAN, *p); // GLUT_STROKE_MONO_ROMAN // GLUT_STROKE_ROMAN
		glPopMatrix();
	}

	void DrawInfo()
	{
		glPushAttrib(GL_ENABLE_BIT);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_LIGHTING);
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		gluOrtho2D(0, 3000, 0, 3000);
		glMatrixMode(GL_MODELVIEW);
		glColor3f(0.7f, 1, 0);
		Print(80, 2800, "Sun (1): %d", enSun);
		Print(80, 2650, "Earth (2): %d", enEarth);
		Print(80, 2500, "Moon (3): %d", enMoon);
		Print(80, 2350, "EOrbit (e): %d", enEarthOrbit);
		Print(80, 2200, "MOrbit (m): %d", enMoonOrbit);
		Print(80, 480, "Extrn light (w): %s", buf);
		Print(80, 340, "Shade model (q): %s", sModel);
		Print(80, 200, "Sun coordinates (shift): (%3.1f, %3.1f, %3.1f)", dx, dy, dz);
		Print(80, 60, "Light source coordinates: (%3.1f, %3.1f, %3.1f)", posLight1[0], posLight1[1], posLight1[2]);
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
		glPopAttrib();
	}
	
	void AddRotation(float angle, float x, float y, float z)
	{
		glPushMatrix();
		glLoadIdentity();
		glRotatef(angle, x, y, z);
		glMultMatrixf(rotMatrix);
		glGetFloatv(GL_MODELVIEW_MATRIX, rotMatrix);
		glPopMatrix();
	}

	void DrawScene()
	{
		glLightfv(GL_LIGHT1, GL_POSITION, posLight1);
		glLightfv(GL_LIGHT1, GL_DIFFUSE, dif);
		//glLightfv(GL_LIGHT1, GL_SPECULAR, dif);

		if (enLight1) glEnable(GL_LIGHT1);
		else glDisable(GL_LIGHT1);
	
		glTranslatef(dx, dy, dz);
		/*glRotatef(ax, 1, 0, 0);
		glRotatef(ay, 0, 1, 0);*/
		glMultMatrixf(rotMatrix);
		
		if (go) {
		//UFO
		glPushAttrib(GL_LIGHTING_BIT);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, col1);
		glMaterialfv(GL_FRONT, GL_SPECULAR, specE);
		glMaterialf(GL_FRONT, GL_SHININESS, 200);

		glPushMatrix();
		glTranslatef(-3, 0, 2);
		glTranslatef(X, DeathStar, 0);
		glRotatef(90, 1, 0, 0);

		//head
		glPushMatrix();
		glTranslatef(0, 0, -0.15);
		glutSolidSphere(0.03, 10, 10);
		glPopMatrix();

		//right leg
		glPushMatrix();
		glTranslatef(0.1, 0, 0.1);
		glRotatef(90, 0, 0, 1);
		glRotatef(45, 1, 0, 0);
		glutSolidCone(0.02, 0.1, 10, 10);
		glPopMatrix();
		//left leg
		glPushMatrix();
		glTranslatef(-0.1, 0, 0.1);
		glRotatef(90, 0, 0, 1);
		glRotatef(-45, 1, 0, 0);
		glutSolidCone(0.02, 0.1, 10, 10);
		glPopMatrix();
		//body
		glPushAttrib(GL_LIGHTING_BIT);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, col);
		glutSolidSphere(0.15, 30, 30);
		glPopAttrib();
		glPushAttrib(GL_LIGHTING_BIT);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, col2);
		glMaterialfv(GL_FRONT, GL_SPECULAR, col2);
		glutSolidTorus(0.05, 0.2, 30, 30);
		glPopAttrib();
		glPopAttrib();
		glPopMatrix();
		}
		
		//SUN
		glPushMatrix();
		glLightfv(GL_LIGHT0, GL_POSITION, posSun);			
		glRotatef(aSun, 0, 0, 1);
		glPushAttrib(GL_LIGHTING_BIT);
		glMaterialfv(GL_FRONT, GL_EMISSION, sunEmis);
		glCallList(1);
		glPopAttrib();
		glPopMatrix(); //1
		//earth orbit
		
		glRotatef(30, 0, 0, 1);
		glRotatef(aEarthOrbit, 0, 1, 0);
		glTranslatef(2.5, 0, 0);
		glPushMatrix();
		//earth
		glPushMatrix(); //1
		glRotatef(aEarth, 0, 0, 1);
		glPushAttrib(GL_LIGHTING_BIT);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, earthDif);
		glMaterialfv(GL_FRONT, GL_SPECULAR, specE);
		glMaterialf(GL_FRONT, GL_SHININESS, shineE);
		glCallList(2);
		glPopAttrib();
		glPopMatrix(); //1
		//moon 
		glRotatef(135, 0, 1, 1);
		glRotatef(aMoonOrbit, 1, 0, 0);
		glTranslatef(0, 0, 0.3);
			
		glRotatef(aMoon, 0, 0, 1);
		glPushAttrib(GL_LIGHTING_BIT);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, moonDif);
		glMaterialfv(GL_FRONT, GL_SPECULAR, specM);
		glMaterialf(GL_FRONT, GL_SHININESS, shineM);
		glCallList(3);
		glPopAttrib();
		glPopMatrix();

		glNewList(1, GL_COMPILE);				
		glutSolidSphere(1.2, 50, 50);		
		glEndList();

		glNewList(2, GL_COMPILE);		
		glutSolidSphere(0.2, 30, 30);
		glEndList();

		glNewList(3, GL_COMPILE);		
		glutSolidSphere(0.05, 20, 20);
		glEndList();
	}

	void OnPaint()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		DrawInfo();
		DrawScene();
		glutSwapBuffers();
	}

	void OnSize(int w, int h)
	{
		glViewport(0, 0, w, h);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(45, double(w) / h, 0.6, 100);
	}

	void OnKey(byte ch, int x, int y)
	{
		int a[1] = { 0 };
		switch (ch)
		{
		case 27: exit(0); break;
		case '0': glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, (twoSide = !twoSide)); break;
		case 'p': glPolygonMode(GL_FRONT_AND_BACK, GL_POINT); break;
		case 'f': glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); break;
		case 'l': glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); break;
		case 's': stop = !stop; break;
		case 'q':
			glGetIntegerv(GL_SHADE_MODEL, a);
			if (a[0] == GL_SMOOTH) {
				glShadeModel(GL_FLAT);
				strcpy(sModel, "flat");
			}
			else {
				glShadeModel(GL_SMOOTH);
				strcpy(sModel, "smooth");
			}
			break;
		case 'w': enLight1 = !enLight1; 
			if(enLight1) strcpy(buf, "on");
			else strcpy(buf, "off");
			break;
		case '2': enEarth = !enEarth; break;
		case '3': enMoon = !enMoon; break;
		case '1': enSun = !enSun; break;
		case 'e': enEarthOrbit = !enEarthOrbit; break;
		case 'm': enMoonOrbit = !enMoonOrbit; break;
		case '-': posLight1[2] -= 0.1; break;
		case '+': posLight1[2] += 0.1; break;
		case 'c': posLight1[0] = 0; posLight1[1] = 0; posLight1[2] = 0.1;
			dx = 0; dy = 0; dz = -6;
			break;

		case 'g': go = !go; break;
		default: break;
		}
		glutPostRedisplay();
	}

	void OnSpecialKey(int key, int x, int y) {
		switch (key) {
			case GLUT_KEY_UP: 
				if (glutGetModifiers() == GLUT_ACTIVE_SHIFT) dy += 0.1;
				else posLight1[1] += 0.1;
				break;
			case GLUT_KEY_DOWN:
				if (glutGetModifiers() == GLUT_ACTIVE_SHIFT) dy -= 0.1;
				else posLight1[1] -= 0.1; 
				break;
			case GLUT_KEY_LEFT:
				if (glutGetModifiers() == GLUT_ACTIVE_SHIFT) dx -= 0.1;
				else  posLight1[0] -= 0.1;
				break;
			case GLUT_KEY_RIGHT: 
				if (glutGetModifiers() == GLUT_ACTIVE_SHIFT) dx += 0.1;
				else posLight1[0] += 0.1;
				break;
			default: break;
		}
		posX = x;
		posY = y;
		glutPostRedisplay();
	}	

	void OnIdle()
	{
		
		if (go) {
			X += del; DeathStar = sin(X);
		}
		if (X > 6 || X < 0 ) del = -del;
		if (stop) {
			if (enSun) aSun += 0.01;
			if (enEarth) aEarth += 0.3;
			if (enEarthOrbit) aEarthOrbit += 0.2;
			if (enMoon) aMoon += 0.5;
			if (enMoonOrbit) aMoonOrbit += 0.3;
			if (aSun > 360) aSun -= 360;
			if (aEarth > 360) aEarth -= 360;
			if (aEarthOrbit > 360) aEarthOrbit -= 360;
			if (aMoon > 360) aMoon -= 360;
			if (aMoonOrbit > 360) aMoonOrbit -= 360;
		}
		glutPostRedisplay();
	}

	void OnMouse(int button, int state, int x, int y)
	{
		leftButton = button == GLUT_LEFT_BUTTON;
		if (state == GLUT_DOWN)
		{
			//glutIdleFunc(0);
			rotX = rotY = 0;
		}
		else
		{
			if (fabs(rotX) > 1 || fabs(rotY) > 1)
				glutIdleFunc(OnIdle);
			/*else
				glutIdleFunc(0);*/
		}
		posX = x;
		posY = y;
	}

	void OnMouseMove(int x, int y)
	{
		rotX = y - posY;
		rotY = x - posX;

		if (leftButton)
		{
			AddRotation(rotX / 10, 1, 0, 0);
			AddRotation(rotY / 10, 0, 1, 0);
		}
		else
			dz += (rotX + rotY) / 60;
		posX = x;
		posY = y;
		glutPostRedisplay();
	}
	
	void Init()
	{
		glEnable(GL_DEPTH_TEST);
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
		glShadeModel(GL_SMOOTH);
		//glEnable(GL_CULL_FACE);		
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);		//SUN
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, amb);			
		DrawScene();
	}

	int main()
	{
		glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
		glutInitWindowSize(1200, 800);
		glutCreateWindow("lab_3");

		Init();
		glutMouseFunc(OnMouse);
		glutMotionFunc(OnMouseMove);
		glutSpecialFunc(OnSpecialKey);
		glutKeyboardFunc(OnKey);
		glutIdleFunc(OnIdle);
		glutReshapeFunc(OnSize);
		glutDisplayFunc(OnPaint);
		glutMainLoop();
		return 0;
	}






