#include "Renderer.h"
#include <string>

void draw_center(void)
{
	glBegin(GL_LINES);
	glColor3f(1.0f, 0.0f, 0.0f); /* R */
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.2f, 0.0f, 0.0f);
	glEnd();
	glRasterPos3f(0.2f, 0.0f, 0.0f);
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'x');

	glBegin(GL_LINES);
	glColor3f(0.0f, 1.0f, 0.0f); /* G */
	glVertex3f(0.0f, 0.2f, 0.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glEnd();
	glRasterPos3f(0.0f, 0.2f, 0.0f);
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'y');

	glBegin(GL_LINES);
	glColor3f(0.0f, 0.0f, 1.0f); /* B */
	glVertex3f(0.0f, 0.0f, -0.2f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glEnd();
	glRasterPos3f(0.0f, 0.0f, -0.2f);
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'z');
}

void idle() {
	static GLuint previousClock = glutGet(GLUT_ELAPSED_TIME);
	static GLuint currentClock = glutGet(GLUT_ELAPSED_TIME);
	static GLfloat deltaT;

	currentClock = glutGet(GLUT_ELAPSED_TIME);
	deltaT = currentClock - previousClock;
	if (deltaT < 1000.0 / 20.0) { return; }
	else { previousClock = currentClock; }

	//char buff[256];
	//sprintf_s(buff, "Frame Rate = %f", 1000.0 / deltaT);
	//frameRate = buff;

	glutPostRedisplay();
}

void close()
{
	glDeleteTextures(1, &dispBindIndex);
	glutLeaveMainLoop();
	CloseHandle(hMutex);
}

void add_quats(float q1[4], float q2[4], float dest[4])
{
	static int count = 0;
	float t1[4], t2[4], t3[4];
	float tf[4];

	vcopy(q1, t1);
	vscale(t1, q2[3]);

	vcopy(q2, t2);
	vscale(t2, q1[3]);

	vcross(q2, q1, t3);
	vadd(t1, t2, tf);
	vadd(t3, tf, tf);
	tf[3] = q1[3] * q2[3] - vdot(q1, q2);

	dest[0] = tf[0];
	dest[1] = tf[1];
	dest[2] = tf[2];
	dest[3] = tf[3];

	if (++count > RENORMCOUNT) {
		count = 0;
		normalize_quat(dest);
	}
}

void reshape(int width, int height)
{
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(58, (double)width / height, 0.1, 100);
	glMatrixMode(GL_MODELVIEW);
}

void motion(int x, int y)
{
	GLfloat spin_quat[4];
	float gain;
	gain = 2.0; /* trackball gain */

	if (drag_state == GLUT_DOWN)
	{
		if (button_state == GLUT_LEFT_BUTTON)
		{
			trackball(spin_quat,
				(gain * rot_x - 500) / 500,
				(500 - gain * rot_y) / 500,
				(gain * x - 500) / 500,
				(500 - gain * y) / 500);
			add_quats(spin_quat, quat, quat);
		}
		else if (button_state == GLUT_RIGHT_BUTTON)
		{
			t[0] -= (((float)trans_x - x) / 500);
			t[1] += (((float)trans_y - y) / 500);
		}
		else if (button_state == GLUT_MIDDLE_BUTTON)
			t[2] -= (((float)trans_z - y) / 500 * 4);
		else if (button_state == 3 || button_state == 4) // scroll
		{

		}
		//glutPostRedisplay();
	}

	rot_x = x;
	rot_y = y;

	trans_x = x;
	trans_y = y;
	trans_z = y;
}

void mouse(int button, int state, int x, int y)
{
	if (state == GLUT_DOWN)
	{
		if (button == GLUT_LEFT_BUTTON)
		{
			rot_x = x;
			rot_y = y;

			//t[0] = t[0] + 1;


		}
		else if (button == GLUT_RIGHT_BUTTON)
		{
			trans_x = x;
			trans_y = y;
		}
		else if (button == GLUT_MIDDLE_BUTTON)
		{
			//trcon = trcon + 1;
			trans_z = y;
		}
		else if (button == 3 || button == 4)
		{
			const float sign = (static_cast<float>(button) - 3.5f) * 2.0f;
			t[2] -= sign * 500 * 0.00015f;
		}
	}

	drag_state = state;
	button_state = button;
}

void vzero(float* v)
{
	v[0] = 0.0f;
	v[1] = 0.0f;
	v[2] = 0.0f;
}

void vset(float* v, float x, float y, float z)
{
	v[0] = x;
	v[1] = y;
	v[2] = z;
}

void vsub(const float* src1, const float* src2, float* dst)
{
	dst[0] = src1[0] - src2[0];
	dst[1] = src1[1] - src2[1];
	dst[2] = src1[2] - src2[2];
}

void vcopy(const float* v1, float* v2)
{
	register int i;
	for (i = 0; i < 3; i++)
		v2[i] = v1[i];
}

void vcross(const float* v1, const float* v2, float* cross)
{
	float temp[3];

	temp[0] = (v1[1] * v2[2]) - (v1[2] * v2[1]);
	temp[1] = (v1[2] * v2[0]) - (v1[0] * v2[2]);
	temp[2] = (v1[0] * v2[1]) - (v1[1] * v2[0]);
	vcopy(temp, cross);
}

float vlength(const float* v)
{
	return sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
}

void vscale(float* v, float div)
{
	v[0] *= div;
	v[1] *= div;
	v[2] *= div;
}

void vnormal(float* v)
{
	vscale(v, 1.0f / vlength(v));
}

float vdot(const float* v1, const float* v2)
{
	return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
}

void vadd(const float* src1, const float* src2, float* dst)
{
	dst[0] = src1[0] + src2[0];
	dst[1] = src1[1] + src2[1];
	dst[2] = src1[2] + src2[2];
}

void trackball(float q[4], float p1x, float p1y, float p2x, float p2y)
{
	float a[3]; /* Axis of rotation */
	float phi;  /* how much to rotate about axis */
	float p1[3], p2[3], d[3];
	float t;

	if (p1x == p2x && p1y == p2y) {
		/* Zero rotation */
		vzero(q);
		q[3] = 1.0;
		return;
	}

	/*
	 * First, figure out z-coordinates for projection of P1 and P2 to
	 * deformed sphere
	 */
	vset(p1, p1x, p1y, tb_project_to_sphere(TRACKBALLSIZE, p1x, p1y));
	vset(p2, p2x, p2y, tb_project_to_sphere(TRACKBALLSIZE, p2x, p2y));

	/*
	 *  Now, we want the cross product of P1 and P2
	 */
	vcross(p2, p1, a);

	/*
	 *  Figure out how much to rotate around that axis.
	 */
	vsub(p1, p2, d);
	t = vlength(d) / (2.0f * TRACKBALLSIZE);

	/*
	 * Avoid problems with out-of-control values...
	 */
	if (t > 1.0) t = 1.0;
	if (t < -1.0) t = -1.0;
	phi = 2.0f * asin(t);

	axis_to_quat(a, phi, q);
}

void axis_to_quat(float a[3], float phi, float q[4])
{
	vnormal(a);
	vcopy(a, q);
	vscale(q, sin(phi / 2.0f));
	q[3] = cos(phi / 2.0f);
}

float tb_project_to_sphere(float r, float x, float y)
{
	float d, t, z;

	d = sqrt(x * x + y * y);
	if (d < r * 0.70710678118654752440f) {    /* Inside sphere */
		z = sqrt(r * r - d * d);
	}
	else {           /* On hyperbola */
		t = r / 1.41421356237309504880f;
		z = t * t / d;
	}
	return z;
}

void normalize_quat(float q[4])
{
	int i;
	float mag;

	mag = (q[0] * q[0] + q[1] * q[1] + q[2] * q[2] + q[3] * q[3]);
	for (i = 0; i < 4; i++) q[i] /= mag;
}

void build_rotmatrix(float m[4][4], float q[4])
{
	m[0][0] = 1.0f - 2.0f * (q[1] * q[1] + q[2] * q[2]);
	m[0][1] = 2.0f * (q[0] * q[1] - q[2] * q[3]);
	m[0][2] = 2.0f * (q[2] * q[0] + q[1] * q[3]);
	m[0][3] = 0.0f;

	m[1][0] = 2.0f * (q[0] * q[1] + q[2] * q[3]);
	m[1][1] = 1.0f - 2.0f * (q[2] * q[2] + q[0] * q[0]);
	m[1][2] = 2.0f * (q[1] * q[2] - q[0] * q[3]);
	m[1][3] = 0.0f;

	m[2][0] = 2.0f * (q[2] * q[0] - q[1] * q[3]);
	m[2][1] = 2.0f * (q[1] * q[2] + q[0] * q[3]);
	m[2][2] = 1.0f - 2.0f * (q[1] * q[1] + q[0] * q[0]);
	m[2][3] = 0.0f;

	m[3][0] = 0.0f;
	m[3][1] = 0.0f;
	m[3][2] = 0.0f;
	m[3][3] = 1.0f;
}

void InitializeWindow(int argc, char* argv[])
{
	// initialize glut settings
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_ALPHA | GLUT_DEPTH);
	glutInitWindowSize(2000 / 2, 2000 / 2);

	glutInitWindowPosition(0, 0);

	dispWindowIndex = glutCreateWindow("3D Model");

	trackball(quat, 90.0, 0.0, 0.0, 0.0);

	glutIdleFunc(idle);
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutSpecialFunc(special);
	glutMotionFunc(motion);
	glutMouseFunc(mouse);
	glutCloseFunc(close);
	//GLuint image = load   ("./my_texture.bmp");

	//glBindTexture(1,)

	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);

	// bind textures
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glEnable(GL_DEPTH_TEST);

	reshape(1000, 1000);

	/*glGenTextures(1, &dispBindIndex);
	glBindTexture(GL_TEXTURE_2D, dispBindIndex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);*/
}


void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, 1, 0.1, 200);
	glTranslatef(t[0], t[1], t[2] - 1.0f);
	glScalef(1, 1, 1);
	GLfloat m[4][4], m1[4][4];
	build_rotmatrix(m, quat);
	gluLookAt(0, 2.0, 2.0, 0, 0, 0, 0, 1.0, 0);

	GLfloat r, g, b;
	glMultMatrixf(&m[0][0]);


	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	GLfloat diffuse0[4] = { 0.5, 0.5, 0.5, 1.0 };
	//GLfloat ambient0[4] = { 0.1, 0.1, 0.1, 1.0 };
	GLfloat ambient0[4] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat specular0[4] = { 0.5, 0.5, 0.5, 1.0 };
	GLfloat emission0[4] = { 0.3, 0.3, 0.3, 1.0 };
	GLfloat light0_pos[4] = { 2.0, 2.0, 12.0, 1.0 };

	glLightfv(GL_LIGHT0, GL_POSITION, light0_pos);
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient0);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse0);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular0);
	//glLightfv(GL_LIGHT0, GL_EMISSION, emission0);


	//glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 0.2);
	//glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.1);
	//glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.05);
	glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 0);
	glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0);
	glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0);


	//������ �ö�ƽ�� ������ ������ ������ ���� ����
	GLfloat mat_ambient[4] = { 0.8f, 0.8f, 0.8f, 1.0f };
	GLfloat mat_diffuse[4] = { 0.6f, 0.6f, 0.6f, 1.0f };
	GLfloat mat_specular[4] = { 0.6f, 0.6f, 0.6f, 1.0f };
	GLfloat mat_emission[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	GLfloat mat_shininess = 32.0;

	//// �������� �ո��� ������ ���� 
	//glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	//glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	//glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	//glMaterialf(GL_FRONT, GL_SHININESS, mat_shininess);



	/*GLenum format = (nm->nrChannels == 4) ? GL_RGBA : GL_RGB;
	glTexImage2D(GL_TEXTURE_2D, 0, format, nm->width, nm->height, 0, format, GL_UNSIGNED_BYTE, nm->texels);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);


	glEnable(GL_TEXTURE_2D);
	glBegin(GL_QUADS);*/
	for (auto& nm : models) {

		int mat_num = -1;

		for (MMesh const& mm : nm->mymesh)
		{
			if (mm.m != mat_num) {
				mat_num = mm.m;
				if (nm->material.at(mat_num)->iskdefined()) {
					glMaterialfv(GL_FRONT, GL_AMBIENT, nm->material.at(mat_num)->ka);
					glMaterialfv(GL_FRONT, GL_DIFFUSE, nm->material.at(mat_num)->kd);
					glMaterialfv(GL_FRONT, GL_SPECULAR, nm->material.at(mat_num)->ks);
					glMaterialf(GL_FRONT, GL_SHININESS, nm->material.at(mat_num)->ns);
					if (nm->material.at(mat_num)->eheight > 0)
						glMaterialfv(GL_FRONT, GL_EMISSION, nm->material.at(mat_num)->ke);
				}
				else {
					glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
					glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
					glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
					glMaterialf(GL_FRONT, GL_SHININESS, mat_shininess);
					if (nm->material.at(mat_num)->eheight > 0)
						glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission);
				}


				/*GLenum format = (nm->material.at(mat_num)->nrChannels == 4) ? GL_RGBA : GL_RGB;
				glTexImage2D(GL_TEXTURE_2D, 0, format, nm->material.at(mat_num)->width,
					nm->material.at(mat_num)->height, 0, format, GL_UNSIGNED_BYTE, nm->material.at(mat_num)->mkd);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);*/
				glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
				glEnable(GL_TEXTURE_2D);
				float matrix[16] = {
					nm->material.at(mat_num)->ts_x, 0.0f, 0.0f, 0.0f,
					0.0f, nm->material.at(mat_num)->ts_y, 0.0f, 0.0f,
					0.0f, 0.0f, 1.0f, 0.0f,
					0.0f, 0.0f, 0.0f, 1.0f
				};
				glMatrixMode(GL_TEXTURE);
				glLoadMatrixf(matrix);
				glShadeModel(GL_SMOOTH);
			}
			// ke
			// ���̼� �����ε� �� �̻��� �̰�
			if (nm->material.at(mat_num)->eheight > 0 && true) {
				glBindTexture(GL_TEXTURE_2D, nm->material.at(mat_num)->eid);
				//glEnable(GL_BLEND);
				//glBlendFunc(GL_ONE, GL_ONE); // Additive blending
				glDisable(GL_LIGHTING);
				if (mm.V4 > 0)
					glBegin(GL_QUADS);
				else
					glBegin(GL_TRIANGLES);
				if (mm.T1 > 0)
					glTexCoord2d(nm->vertex_color.at(mm.T1 - 1).X, nm->vertex_color.at(mm.T1 - 1).Y);
				glVertex3f(nm->vertex.at(mm.V1 - 1).X, nm->vertex.at(mm.V1 - 1).Y, nm->vertex.at(mm.V1 - 1).Z);
				if (mm.T2 > 0)
					glTexCoord2d(nm->vertex_color.at(mm.T2 - 1).X, nm->vertex_color.at(mm.T2 - 1).Y);
				glVertex3f(nm->vertex.at(mm.V2 - 1).X, nm->vertex.at(mm.V2 - 1).Y, nm->vertex.at(mm.V2 - 1).Z);
				if (mm.T3 > 0)
					glTexCoord2d(nm->vertex_color.at(mm.T3 - 1).X, nm->vertex_color.at(mm.T3 - 1).Y);
				glVertex3f(nm->vertex.at(mm.V3 - 1).X, nm->vertex.at(mm.V3 - 1).Y, nm->vertex.at(mm.V3 - 1).Z);
				if (mm.V4 > 0) {
					if (mm.T4 > 0) {
						glTexCoord2d(nm->vertex_color.at(mm.T4 - 1).X, nm->vertex_color.at(mm.T4 - 1).Y);
						glVertex3f(nm->vertex.at(mm.V4 - 1).X, nm->vertex.at(mm.V4 - 1).Y, nm->vertex.at(mm.V4 - 1).Z);
					}
				}
				//glDisable(GL_BLEND);
				glEnd();
				glEnable(GL_LIGHTING);
			}
			// kd
			if (nm->material.at(mat_num)->nrChannels > 3) {
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			}
			glBindTexture(GL_TEXTURE_2D, nm->material.at(mat_num)->did);
			if (mm.V4 > 0)
				glBegin(GL_QUADS);
			else
				glBegin(GL_TRIANGLES);
			if (mm.T1 > 0)
				glTexCoord2d(nm->vertex_color.at(mm.T1 - 1).X, nm->vertex_color.at(mm.T1 - 1).Y);
			glVertex3f(nm->vertex.at(mm.V1 - 1).X, nm->vertex.at(mm.V1 - 1).Y, nm->vertex.at(mm.V1 - 1).Z);
			if (mm.T2 > 0)
				glTexCoord2d(nm->vertex_color.at(mm.T2 - 1).X, nm->vertex_color.at(mm.T2 - 1).Y);
			glVertex3f(nm->vertex.at(mm.V2 - 1).X, nm->vertex.at(mm.V2 - 1).Y, nm->vertex.at(mm.V2 - 1).Z);
			if (mm.T3 > 0)
				glTexCoord2d(nm->vertex_color.at(mm.T3 - 1).X, nm->vertex_color.at(mm.T3 - 1).Y);
			glVertex3f(nm->vertex.at(mm.V3 - 1).X, nm->vertex.at(mm.V3 - 1).Y, nm->vertex.at(mm.V3 - 1).Z);
			if (mm.V4 > 0) {
				if (mm.T4 > 0) {
					glTexCoord2d(nm->vertex_color.at(mm.T4 - 1).X, nm->vertex_color.at(mm.T4 - 1).Y);
					glVertex3f(nm->vertex.at(mm.V4 - 1).X, nm->vertex.at(mm.V4 - 1).Y, nm->vertex.at(mm.V4 - 1).Z);
				}
			}
			glEnd();
			if (nm->material.at(mat_num)->nrChannels > 3) {
				glDisable(GL_BLEND);
			}

			
		}
		//glEnd();

	}
	glutSwapBuffers();
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glMatrixMode(GL_PROJECTION);
	//glLoadIdentity();
	//gluPerspective(60, 1, 0.1, 200);		
	//glTranslatef(t[0], t[1], t[2] - 1.0f);
	//glScalef(1, 1, 1);	
	//GLfloat m[4][4],m1[4][4];
	//build_rotmatrix(m, quat);
	//gluLookAt(0, 2.0, 2.0, 0, 0, 0, 0, 1.0, 0);

	//GLfloat r, g, b;
	//glMultMatrixf(&m[0][0]);


	//glEnable(GL_LIGHTING);
	//glEnable(GL_LIGHT0);
	//GLfloat diffuse0[4] = { 1.0, 1.0, 1.0, 1.0 };
	//GLfloat ambient0[4] = { 0.5, 0.5, 0.5, 1.0 };
	//GLfloat specular0[4] = { 1.0, 1.0, 1.0, 1.0 };
	//GLfloat light0_pos[4] = { 2.0, 2.0, 2.0, 1.0 };

	//glLightfv(GL_LIGHT0, GL_POSITION, light0_pos); 
	//glLightfv(GL_LIGHT0, GL_AMBIENT, ambient0);
	//glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse0);
	//glLightfv(GL_LIGHT0, GL_SPECULAR, specular0);
	//

	//glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 0.2);
	//glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.1);
	//glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.05);

	//
	////������ �ö�ƽ�� ������ ������ ������ ���� ����
	//GLfloat mat_ambient[4] = { 0.8f, 0.8f, 0.8f, 1.0f };
	//GLfloat mat_diffuse[4] = { 0.6f, 0.6f, 0.6f, 1.0f };
	//GLfloat mat_specular[4] = { 0.8f, 0.6f, 0.6f, 1.0f };
	//GLfloat mat_shininess = 32.0;

	//// �������� �ո��� ������ ���� 
	//glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	//glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	//glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	//glMaterialf(GL_FRONT, GL_SHININESS, mat_shininess);
	//
	//


	//glTexImage2D(GL_TEXTURE_2D, 0, 3, 2048, 2048, 0, GL_RGB, GL_UNSIGNED_BYTE, mytexels);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	//glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	//	
	//	
	//glEnable(GL_TEXTURE_2D);
	//glBegin(GL_QUADS);
	//for (int jj = 0; jj <6733; jj = jj + 1)
	//{
	//	glTexCoord2d(vertex_color[mymesh[jj].T1-1].X, vertex_color[mymesh[jj].T1 - 1].Y);
	//	glVertex3f(vertex[mymesh[jj].V1 - 1].X, vertex[mymesh[jj].V1 - 1].Y, vertex[mymesh[jj].V1 - 1].Z);
	//	glTexCoord2d(vertex_color[mymesh[jj].T2 - 1].X, vertex_color[mymesh[jj].T2 - 1].Y);
	//	glVertex3f(vertex[mymesh[jj].V2 - 1].X, vertex[mymesh[jj].V2 - 1].Y, vertex[mymesh[jj].V2 - 1].Z);
	//	glTexCoord2d(vertex_color[mymesh[jj].T3 - 1].X, vertex_color[mymesh[jj].T3 - 1].Y);
	//	glVertex3f(vertex[mymesh[jj].V3 - 1].X, vertex[mymesh[jj].V3 - 1].Y, vertex[mymesh[jj].V3 - 1].Z);
	//	glTexCoord2d(vertex_color[mymesh[jj].T4 - 1].X, vertex_color[mymesh[jj].T4 - 1].Y);
	//	glVertex3f(vertex[mymesh[jj].V4 - 1].X, vertex[mymesh[jj].V4 - 1].Y, vertex[mymesh[jj].V4 - 1].Z);
	//}
	//glEnd();


	//glutSwapBuffers();

}

void model_t::translation(const float t[]) {
	for (Vertex& v : vertex) {
		v.X += t[0];
		v.Y += t[1];
		v.Z += t[2];
	}
}
void model_t::translation(float a, float b, float c)
{
	for (Vertex& v : vertex) {
		v.X += a;
		v.Y += b;
		v.Z += c;
	}
}
void model_t::rotation(const float t[]) {
	float rad = dtor(t[3]);
	for (Vertex& v : vertex) {
		if (t[0] > 0) {
			float y = v.Y * cos(rad) - v.Z * sin(rad);
			float z = v.Y * sin(rad) + v.Z * cos(rad);
			v.Y = y;
			v.Z = z;
		}
		if (t[1] > 0) {
			float x = v.X * cos(rad) - v.Z * sin(rad);
			float z = v.X * sin(rad) + v.Z * cos(rad);
			v.X = x;
			v.Z = z;
		}
		if (t[2] > 0) {
			float x = v.X * cos(rad) - v.Y * sin(rad);
			float y = v.X * sin(rad) + v.Y * cos(rad);
			v.X = x;
			v.Y = y;
		}
	}
}

void model_t::rotation(float a, float b, float c, float r)
{
	float tf[4] = { a, b, c, r };
	rotation(tf);
}

void model_t::rotation_a(const float t[]) {
	float rad = dtor(t[3]);
	float xm, ym, zm;
	get_vertex_mean(xm, ym, zm);
	for (Vertex& v : vertex) {
		if (t[0] > 0) {
			float y = (v.Y - ym) * cos(rad) - (v.Z - zm) * sin(rad);
			float z = (v.Y - ym) * sin(rad) + (v.Z - zm) * cos(rad);
			v.Y = y + ym;
			v.Z = z + zm;
		}
		else if (t[1] > 0) {
			float x = (v.X - xm) * cos(rad) - (v.Z - zm) * sin(rad);
			float z = (v.X - xm) * sin(rad) + (v.Z - zm) * cos(rad);
			v.X = x + xm;
			v.Z = z + zm;
		}
		else if (t[2] > 0) {
			float x = (v.X - xm) * cos(rad) - (v.Y - ym) * sin(rad);
			float y = (v.X - xm) * sin(rad) + (v.Y - ym) * cos(rad);
			v.X = x + xm;
			v.Y = y + ym;
		}
	}
}

void model_t::rotation_a(float a, float b, float c, float r)
{
	float tf[4] = { a, b, c, r };
	rotation_a(tf);
}

void model_t::get_vertex_mean(float& a, float& b, float& c)
{
	float min_x = 1000000, min_y = 1000000, min_z = 1000000;
	float max_x = -1000000, max_y = -1000000, max_z = -1000000;
	for (Vertex& v : vertex) {
		min_x = min(min_x, v.X);
		min_y = min(min_y, v.Y);
		min_z = min(min_z, v.Z);
		max_x = max(max_x, v.X);
		max_y = max(max_y, v.Y);
		max_z = max(max_z, v.Z);
	}
	a = (min_x + max_x) / 2.0;
	b = (min_y + max_y) / 2.0;
	c = (min_z + max_z) / 2.0;
}


bool Material::loadTexture(const char* filename) {
	// �̹��� �ε�
	mkd = stbi_load(filename, &width, &height, &nrChannels, 0);
	if (!mkd) {
		std::cerr << "Failed to load texture: " << filename << std::endl;
		return false;
	}
	glGenTextures(1, &did);
	glBindTexture(GL_TEXTURE_2D, did);
	GLint format;
	if (nrChannels == 1) {
		format = GL_LUMINANCE;
	}
	else if (nrChannels == 3) {
		format = GL_RGB;
	}
	else if (nrChannels == 4) {
		format = GL_RGBA;
	}

	glTexImage2D(GL_TEXTURE_2D, 0, format, width,
		height, 0, format, GL_UNSIGNED_BYTE, mkd);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	return true;
}

bool Material::loadEmission(const char* filename)
{
	mke = stbi_load(filename, &ewidth, &eheight, &enrChannels, 0);
	if (!mkd) {
		std::cerr << "Failed to load texture: " << filename << std::endl;
		return false;
	}
	glGenTextures(1, &eid);
	glBindTexture(GL_TEXTURE_2D, eid);
	GLint format;
	if (enrChannels == 1) {
		format = GL_RED;
	}
	else if (enrChannels == 3) {
		format = GL_RGB;
	}
	else if (enrChannels == 4) {
		format = GL_RGBA;
	}
	glTexImage2D(GL_TEXTURE_2D, 0, format, ewidth,
		eheight, 0, format, GL_UNSIGNED_BYTE, mke);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	return true;
}

bool Material::iskdefined()
{
	bool correct = false;
	for (int i = 0; i < 3; i++) {
		if (ka[i] > 0 || kd[i] > 0 || ks[i] > 0)
			correct = true;
	}
	return correct;
}


void replaceNewlineWithNull(char* str) {
	size_t length = strlen(str); // ���ڿ��� ���̸� ���մϴ�.
	for (size_t i = 0; i < length; ++i) {
		if (str[i] == '\n') {
			str[i] = '\0';
		}
	}
}


double dtor(double degrees) {
	return degrees * 3.141592653589793 / 180.0;
}


unique_ptr<model_t> load_model(const string objname, const string modeldir, float scale_factor) {
	auto nm = std::make_unique<model_t>();
	int count = 0;
	int num = 0;
	char ch;
	float x, y, z;
	int ix, iy, iz;
	float max_x = -100000, min_x = 100000, max_y = -100000, min_y = 100000, max_z = -100000, min_z = 100000;
	float x1, y1, z1, x2, y2, z2, x3, y3, z3, x4, y4, z4;
	bool start_flag = true;
	const char mtllib[] = "mtllib";
	char buffers[256];
	char buffer[256];  // ���� ���� ����

	string fullobjdir = modeldir + objname;
	FILE* fp;
	fp = fopen(fullobjdir.c_str(), "r");
	if (fp == 0)
		return nm;
	int lines = 0;
	int material_num = -1;
	while (fgets(buffer, sizeof(buffer), fp))
	{
		/*if (lines == 7150)
			cout << "asdf" << endl;*/
		if (strncmp(buffer, mtllib, strlen(mtllib)) == 0) {
			char mtlfilename[300];
			strcpy(mtlfilename, modeldir.c_str());
			strcat_s(mtlfilename, sizeof(mtlfilename), buffer + strlen(mtllib) + 1);
			FILE* fmp;
			replaceNewlineWithNull(mtlfilename);
			fmp = fopen(mtlfilename, "r");
			char bufferm[256];
			int mtx_idx = -1;
			while (fgets(bufferm, sizeof(bufferm), fmp)) {
				//cout << bufferm;
				count = sscanf(bufferm, "newmtl %s\n", buffers);
				if (count == 1) {
					nm->material.push_back(make_shared<Material>());
					mtx_idx++;
					nm->material.at(mtx_idx)->name.assign(buffers);
				}
				count = sscanf(bufferm, "Ns %f\n", &x);
				if (count == 1) {
					nm->material.at(mtx_idx)->ns = x;
				}
				count = sscanf(bufferm, "Ka %f %f %f\n", &x, &y, &z);
				if (count == 3) {
					nm->material.at(mtx_idx)->ka[0] = x;
					nm->material.at(mtx_idx)->ka[1] = y;
					nm->material.at(mtx_idx)->ka[2] = z;
					nm->material.at(mtx_idx)->ka[3] = 1;
				}
				count = sscanf(bufferm, "Kd %f %f %f\n", &x, &y, &z);
				if (count == 3) {
					nm->material.at(mtx_idx)->kd[0] = x;
					nm->material.at(mtx_idx)->kd[1] = y;
					nm->material.at(mtx_idx)->kd[2] = z;
					nm->material.at(mtx_idx)->kd[3] = 1;
				}
				count = sscanf(bufferm, "Ks %f %f %f\n", &x, &y, &z);
				if (count == 3) {
					nm->material.at(mtx_idx)->ks[0] = x;
					nm->material.at(mtx_idx)->ks[1] = y;
					nm->material.at(mtx_idx)->ks[2] = z;
					nm->material.at(mtx_idx)->ks[3] = 1;
				}
				count = sscanf(bufferm, "Ke %f %f %f\n", &x, &y, &z);
				if (count == 3) {
					nm->material.at(mtx_idx)->ke[0] = x;
					nm->material.at(mtx_idx)->ke[1] = y;
					nm->material.at(mtx_idx)->ke[2] = z;
					nm->material.at(mtx_idx)->ke[3] = 1;
				}
				count = sscanf(bufferm, "Ni %f\n", &x);
				if (count == 1) {
					nm->material.at(mtx_idx)->ni = x;
				}
				count = sscanf(bufferm, "d %f\n", &x);
				if (count == 1) {
					nm->material.at(mtx_idx)->d = x;
				}
				count = sscanf(bufferm, "map_Kd -o %f %f %f %s\n", &x, &y, &z, buffers);
				if (count == 4) {
					char mkdfilename[300];
					strcpy(mkdfilename, modeldir.c_str());
					strcat_s(mkdfilename, sizeof(mkdfilename), buffers);
					nm->material.at(mtx_idx)->loadTexture(mkdfilename);
					nm->material.at(mtx_idx)->texture_offset_x = x;
					nm->material.at(mtx_idx)->texture_offset_y = y;
					nm->material.at(mtx_idx)->texture_offset_z = z;
				}
				count = sscanf(bufferm, "map_Kd -s %d %d %d %s\n", &ix, &iy, &iz, buffers);
				if (count == 4) {
					char mkdfilename[300];
					strcpy(mkdfilename, modeldir.c_str());
					strcat_s(mkdfilename, sizeof(mkdfilename), buffers);
					nm->material.at(mtx_idx)->loadTexture(mkdfilename);
					nm->material.at(mtx_idx)->ts_x = ix;
					nm->material.at(mtx_idx)->ts_y = iy;
					nm->material.at(mtx_idx)->ts_z = iz;
				}
				else {
					count = sscanf(bufferm, "map_Kd %s\n", buffers);
					if (count == 1) {
						char mkdfilename[300];
						strcpy(mkdfilename, modeldir.c_str());
						strcat_s(mkdfilename, sizeof(mkdfilename), buffers);
						nm->material.at(mtx_idx)->loadTexture(mkdfilename);
					}
				}
				count = sscanf(bufferm, "map_Ke %s\n", buffers);
				if (count == 1) {
					char mkefilename[300];
					strcpy(mkefilename, modeldir.c_str());
					strcat_s(mkefilename, sizeof(mkefilename), buffers);
					nm->material.at(mtx_idx)->loadEmission(mkefilename);
				}

			}
		}
		count = sscanf(buffer, "v %f %f %f", &x, &y, &z);
		if (count == 3)
		{
			Vertex vt;
			vt.X = x / scale;
			vt.Y = y / scale;
			vt.Z = z / scale;
			if (vt.Z < nm->zmin) {
				nm->zmin = vt.Z;
				min_z = vt.Z;
			}
			if (vt.Z > nm->zmax) {
				nm->zmax = vt.Z;
				max_z = vt.Z;
			}
			if (vt.X < min_x)
				min_x = vt.X;
			if (vt.X > max_x)
				max_x = vt.X;
			if (vt.Y < min_y)
				min_y = vt.Y;
			if (vt.Y > max_y)
				max_y = vt.Y;
			nm->vertex.push_back(vt);
		}
		count = sscanf(buffer, "usemtl %s\n", buffers);
		if (count == 1)
		{
			string temp_material_name(buffers);
			for (int k = 0; k < nm->material.size(); k++) {
				if (nm->material.at(k)->name == temp_material_name)
					material_num = k;
			}
		}
		count = sscanf(buffer, "vt %f %f %f", &x, &y, &z);
		if (count == 3)
		{
			Vertex vt;
			vt.X = x;
			vt.Y = 1 - y;
			vt.Z = z;
			nm->vertex_color.push_back(vt);
		}
		else if (count == 2)
		{
			Vertex vt;
			vt.X = x;
			vt.Y = 1 - y;
			nm->vertex_color.push_back(vt);
		}
		count = sscanf(buffer, "f %f/%f/%f %f/%f/%f %f/%f/%f %f/%f/%f", &x1, &y1, &z1, &x2, &y2, &z2, &x3, &y3, &z3, &x4, &y4, &z4);
		if (count == 12)
		{
			MMesh mt;
			mt.V1 = x1;
			mt.V2 = x2;
			mt.V3 = x3;
			mt.V4 = x4;
			mt.T1 = y1;
			mt.T2 = y2;
			mt.T3 = y3;
			mt.T4 = y4;
			mt.N1 = z1;
			mt.N2 = z2;
			mt.N3 = z3;
			mt.N4 = z4;
			mt.m = material_num;
			nm->mymesh.push_back(mt);
		}
		else if (count == 9)
		{
			MMesh mt;
			mt.V1 = x1;
			mt.V2 = x2;
			mt.V3 = x3;
			mt.V4 = -1;
			mt.T1 = y1;
			mt.T2 = y2;
			mt.T3 = y3;
			mt.T4 = -1;
			mt.N1 = z1;
			mt.N2 = z2;
			mt.N3 = z3;
			mt.N4 = -1;
			mt.m = material_num;
			nm->mymesh.push_back(mt);
		}
		count = sscanf(buffer, "f %f//%f %f//%f %f//%f %f//%f", &x1, &z1, &x2, &z2, &x3, &z3, &x4, &z4);
		if (count == 8)
		{
			MMesh mt;
			mt.V1 = x1;
			mt.V2 = x2;
			mt.V3 = x3;
			mt.V4 = x4;
			mt.T1 = 0;
			mt.T2 = 0;
			mt.T3 = 0;
			mt.T4 = 0;
			mt.N1 = z1;
			mt.N2 = z2;
			mt.N3 = z3;
			mt.N4 = z4;
			mt.m = material_num;
			nm->mymesh.push_back(mt);
		}
		else if (count == 6)
		{
			MMesh mt;
			mt.V1 = x1;
			mt.V2 = x2;
			mt.V3 = x3;
			mt.V4 = -1;
			mt.T1 = 0;
			mt.T2 = 0;
			mt.T3 = 0;
			mt.T4 = -1;
			mt.N1 = z1;
			mt.N2 = z2;
			mt.N3 = z3;
			mt.N4 = -1;
			mt.m = material_num;
			nm->mymesh.push_back(mt);
		}
		lines++;
	}
	fclose(fp);

	float weight_center_x = (min_x + max_x) / 2.0;
	float weight_center_y = (min_y + max_y) / 2.0;
	float weight_center_z = (min_z + max_z) / 2.0;
	float max_length = max((max_x - min_x), max((max_y - min_y), (max_z - min_z)));
	float scale_calc = scale_factor / max_length;
	for (Vertex& vt : nm->vertex) {
		vt.X = (vt.X - weight_center_x) * scale_calc;
		vt.Y = (vt.Y - min_y) * scale_calc;
		vt.Z = (vt.Z - weight_center_z) * scale_calc;// z�� �ٴ��� ��������
	}

	return nm;
}

bool is_able_place(const vector<object_t>& exsit_places, const object_t& now_place, float min_interval) {
	for (const object_t& exsit_place : exsit_places) {
		if (pow(exsit_place.x - now_place.x, 2) + pow(exsit_place.z - now_place.z, 2) < pow(min_interval, 2))
			return false;
	}
	return true;
}

vector<object_t> make_random_place(float min_x, float max_x, float min_z, float max_z, float max_num, int max_class, float min_interval, int rand_seed = 100000) {
	int iter_times = 0, max_iter_times = 1000;
	vector<object_t> places;
	mt19937 gen(rand_seed);
	uniform_real_distribution<float> dis_x(min_x, max_x);
	uniform_real_distribution<float> dis_z(min_z, max_z);
	uniform_int_distribution<int> dis_d(0, 360);
	uniform_int_distribution<int> dis_c(0, max_class);

	while (places.size() < max_num && iter_times < max_iter_times) {
		object_t obj_temp(dis_c(gen), dis_x(gen), 0, dis_z(gen), dis_d(gen));
		if (is_able_place(places, obj_temp, min_interval))
			places.push_back(obj_temp);
	}
	return places;
}

void load_tile() {
	auto tilec = load_model("tilec.obj", "models\\floor\\", 6);
	tilec->translation(0, -0.03, 0);
	tilec->rotation(0, 1, 0, -5);
	models.push_back(move(tilec));
	auto tile = load_model("tile.obj", "models\\floor\\", 1.3);
	tile->translation(8.0, 0, 0);
	for (int i = 0; i < 5; i++) {
		auto tile_temp = make_unique<model_t>(tile);
		tile_temp->translation(0, 0, i - 2.0f);
		models.push_back(move(tile_temp));
	}
	for (int i = 0; i < 15; i++) {
		auto tile_temp = make_unique<model_t>(tile);
		tile_temp->translation(5-i, 0, 0);
		models.push_back(move(tile_temp));
	}
	auto tile_temp = make_unique<model_t>(tile);
	tile_temp->rotation_a(0, 1, 0, 15);
	tile_temp->translation(0.11, 0, -2.95f);
	models.push_back(move(tile_temp));
	tile_temp = make_unique<model_t>(tile);
	tile_temp->rotation_a(0, 1, 0, -15);
	tile_temp->translation(0.11, 0, 2.95f);
	models.push_back(move(tile_temp));

	tile_temp = make_unique<model_t>(tile);
	tile_temp->rotation_a(0, 1, 0, 15);
	tile_temp->translation(0.37, 0, -3.95f);
	models.push_back(move(tile_temp));
	tile_temp = make_unique<model_t>(tile);
	tile_temp->rotation_a(0, 1, 0, -15);
	tile_temp->translation(0.37, 0, 3.95f);
	models.push_back(move(tile_temp));

	tile_temp = make_unique<model_t>(tile);
	tile_temp->rotation_a(0, 1, 0, 15);
	tile_temp->translation(0.63, 0, -4.95f);
	models.push_back(move(tile_temp));
	tile_temp = make_unique<model_t>(tile);
	tile_temp->rotation_a(0, 1, 0, -15);
	tile_temp->translation(0.63, 0, 4.95f);
	models.push_back(move(tile_temp));
	
	for (int i = 0; i < 4; i++) {
		tile_temp = make_unique<model_t>(tile);
		tile_temp->translation(0.76, 0, -5.95f - i);
		models.push_back(move(tile_temp));
		tile_temp = make_unique<model_t>(tile);
		tile_temp->translation(0.76, 0, 5.95f + i);
		models.push_back(move(tile_temp));
	}

	tile_temp = make_unique<model_t>(tile);
	tile_temp->rotation_a(0, 1, 0, -25);
	tile_temp->translation(0.56, 0, -9.85f);
	models.push_back(move(tile_temp));
	tile_temp = make_unique<model_t>(tile);
	tile_temp->rotation_a(0, 1, 0, 25);
	tile_temp->translation(0.56, 0, 9.85f);
	models.push_back(move(tile_temp));

	tile_temp = make_unique<model_t>(tile);
	tile_temp->rotation_a(0, 1, 0, -45);
	tile_temp->translation(0.06, 0, -10.55f);
	models.push_back(move(tile_temp));
	tile_temp = make_unique<model_t>(tile);
	tile_temp->rotation_a(0, 1, 0, 45);
	tile_temp->translation(0.06, 0, 10.55f);
	models.push_back(move(tile_temp));

	tile_temp = make_unique<model_t>(tile);
	tile_temp->rotation_a(0, 1, 0, -45);
	tile_temp->translation(-0.75, 0, -11.35f);
	models.push_back(move(tile_temp));
	tile_temp = make_unique<model_t>(tile);
	tile_temp->rotation_a(0, 1, 0, 45);
	tile_temp->translation(-0.75, 0, 11.35f);
	models.push_back(move(tile_temp));

	for (int i = 0; i < 11; i++) {
		tile_temp = make_unique<model_t>(tile);
		tile_temp->translation(-1.4-i, 0, -11.65f);
		models.push_back(move(tile_temp));
		tile_temp = make_unique<model_t>(tile);
		tile_temp->translation(-1.4-i, 0, 11.65f);
		models.push_back(move(tile_temp));
	}

	for (int i = -3; i < 11; i++) {
		tile_temp = make_unique<model_t>(tile);
		tile_temp->translation(-5.4, 0, -10.65f+i);
		models.push_back(move(tile_temp));
		tile_temp = make_unique<model_t>(tile);
		tile_temp->translation(-5.4, 0, 10.65f-i);
		models.push_back(move(tile_temp));

		tile_temp = make_unique<model_t>(tile);
		tile_temp->translation(-11.4, 0, -10.65f + i);
		models.push_back(move(tile_temp));
		tile_temp = make_unique<model_t>(tile);
		tile_temp->translation(-11.4, 0, 10.65f - i);
		models.push_back(move(tile_temp));
	}


}

void load_bench() {
	auto bench = load_model("bench.obj", "models\\bench\\", 1);
	auto bench_temp = make_unique<model_t>(bench);
	bench_temp->translation(9.3, 0, -3.5);
	bench_temp->rotation_a(0, 1, 0, 105);
	models.push_back(move(bench_temp));
	bench_temp = make_unique<model_t>(bench);
	bench_temp->translation(9.6, 0, -4.7);
	bench_temp->rotation_a(0, 1, 0, 105);
	models.push_back(move(bench_temp));
}

void load_tree() {
	auto tree1 = load_model("tree1.obj", "models\\tree\\", 2);
	auto tree2 = load_model("tree2.obj", "models\\tree\\", 2);
	auto tree3 = load_model("tree3.obj", "models\\tree\\", 2);
	auto tree4 = load_model("tree4.obj", "models\\tree\\", 2);
	auto tree5 = load_model("tree5.obj", "models\\tree\\", 2);
	auto gress1 = load_model("gress1.obj", "models\\gress\\", 2);
	auto rand_palace = make_random_place(4.0f, 7.0f, -7.0f, -1.5f, 5, 4, 0.5, 14523423);
	unique_ptr<model_t> object_temp;
	for (const auto& p : rand_palace) {
		if(p.c == 0)
			object_temp = make_unique<model_t>(tree1);
		else if(p.c == 1)
			object_temp = make_unique<model_t>(tree2);
		else
			object_temp = make_unique<model_t>(tree1);
		object_temp->translation(p.x, 0.05, p.z);
		object_temp->rotation_a(0, 1, 0, p.r);
		models.push_back(move(object_temp));
	}
	rand_palace = make_random_place(4.0f, 7.0f, -7.0f, -1.5f, 4, 0, 0.5, 523421);
	for (const auto& p : rand_palace) {
		if (p.c == 0)
			object_temp = make_unique<model_t>(gress1);
		else
			object_temp = make_unique<model_t>(gress1);
		object_temp->translation(p.x, 0.05, p.z);
		object_temp->rotation_a(0, 1, 0, p.r);
		models.push_back(move(object_temp));
	}
}

void load_models() {
	//Ÿ�� �ε�
	//load_tile();

	//load_bench();

	//oad_tree();

	/*models.push_back(load_model("Echidna.obj", "models\\", 1.0));
	
	auto kamen = load_model("Kamen.obj", "models\\Kamen\\", 1.0);
	kamen->translation(1, 0, 0);
	models.push_back(move(kamen));*/

	auto ground = load_model("ground_sim.obj", "models\\ground\\", 10);
	ground->translation(0, -1.1, 0);
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 4; j++) {
			auto ground_temp = make_unique<model_t>(ground);
			ground_temp->translation(i * 9, 0, j*7-10.5);
			models.push_back(move(ground_temp));
		}
	}

	auto forutain = load_model("forutain.obj", "models\\forutain\\", 2.3);
	forutain->translation(0, 0.1, 0);
	models.push_back(move(forutain));

	auto basketball = load_model("basketball.obj", "models\\basketball\\", 3);
	basketball->translation(10, 0.5, 2);
	basketball->rotation_a(0, 1, 0, 90);
	models.push_back(move(basketball));
	
	auto toilet = load_model("public_toilet_sim.obj", "models\\public_toilet\\", 3.5);
	toilet->translation(5.5, 0.05, -9);
	models.push_back(move(toilet));

}

int main(int argc, char* argv[])
{
	vertex = new Vertex[100000];
	vertex_color = new Vertex[100000];
	mymesh = new MMesh[100000];

	InitializeWindow(argc, argv);
	load_models();

	display();

	glutMainLoop();
	delete[] vertex;
	delete[] mymesh;
	delete[] vertex_color;
	return 0;
}