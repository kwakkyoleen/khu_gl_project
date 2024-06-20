#pragma once
#define STB_IMAGE_IMPLEMENTATION
#include <math.h>
#include <iostream>
#include <fstream>
#include "gl\freeglut.h"		// OpenGL header files
#include "stb_image.h"
#include <list>
#include <vector>
#include <utility>
#include <random>
#include <memory>
//#define TIME_CHECK_
#define DEPTH_CALIB_
#pragma warning(disable:4996)
#define BLOCK 4
#define READ_SIZE 110404//5060//110404
#define scale 20
//#define TEST 14989
//#define STATIC_

using namespace std;

struct Vertex{
	float X;
	float Y;
	float Z;
	int index_1;
	int index_2;
	int index_3;
	Vertex(){}
	Vertex(float x, float y, float z) :
		X(x), Y(y), Z(z) {}
};

class Material {
public:
	unsigned char* mkd;
	unsigned char* mke;
	string name;
	float ns=1;
	GLfloat ka[4] = {0.0f,0.0f,0.0f,1.0f};
	GLfloat kd[4] = { 0.0f,0.0f,0.0f,1.0f };
	GLfloat ks[4] = { 0.0f,0.0f,0.0f,1.0f };
	GLfloat ke[4] = { 0.0f,0.0f,0.0f,1.0f };
	float ni = 0;
	float d = 0;
	int illum = 0;
	int width=0, height=0, nrChannels=0;
	int ewidth=0, eheight=0, enrChannels=0;
	float texture_offset_x = 0, texture_offset_y = 0, texture_offset_z = 0;
	int ts_x = 1, ts_y = 1, ts_z = 1; // texture scale
	bool loadTexture(const char* filename);
	bool loadEmission(const char* filename);
	bool iskdefined();
	GLuint did;
	GLuint eid;
	~Material() {
		stbi_image_free(mkd);
		if(ewidth > 0)
			stbi_image_free(mke);
	}
};

struct MMesh {
	int V1;
	int V2;
	int V3;
	int V4;
	int T1;
	int T2;
	int T3;
	int T4;
	int N1;
	int N2;
	int N3;
	int N4;
	int m;
};

class model_t {
public :
	vector<Vertex> vertex;
	vector<Vertex> vertex_color;
	vector<MMesh> mymesh;
	vector<shared_ptr<Material>> material;
	float zmin = 100000;
	float zmax = -100000;
	void translation(const float t[]);
	void translation(float a, float b, float c);
	void rotation(const float t[]);
	void rotation(float a, float b, float c, float r);
	void rotation_a(const float t[]);
	void rotation_a(float a, float b, float c, float r);
	void get_vertex_mean(float& a, float& b, float& c);

	~model_t() {
	}
	model_t() {}
	model_t(const unique_ptr<model_t>& other) {
		vertex.assign(other->vertex.begin(), other->vertex.end());
		vertex_color.assign(other->vertex_color.begin(), other->vertex_color.end());
		mymesh.assign(other->mymesh.begin(), other->mymesh.end());
		material.assign(other->material.begin(), other->material.end());
		zmin = other->zmin;
		zmax = other->zmax;
	}

};

class object_t {
public:
	float x, y, z;
	int r, c;
	object_t(int c_, float x_, float y_, float z_, int r_) :
		c(c_), x(x_), y(y_), z(z_), r(r_) {}
};

// variables for GUI
const float TRACKBALLSIZE = 0.8f;
const int RENORMCOUNT = 97;
//const int width = KinectBasic::nColorWidth;
//const int height = KinectBasic::nColorHeight;

GLint drag_state = 0;
GLint button_state = 0;

GLint rot_x = 0;
GLint rot_y = 0;
GLint trans_x = 0;
GLint trans_y = 0;
GLint trans_z = 0;

GLubyte mytexels[2048][2048][3];

int add_depth_flag = 0;
int model_flag = 0;
int depth_display_flag = 0;
int geodesic_skel[23][5] = { 0 };
int trcon = 0;
float zmin = 100000, zmax = -100000;

int side_status[50] = { 0 };

float quat[4] = {0};
float t[3] = {0};

Vertex skt[23];
BOOLEAN bTracked = false;
bool checkt = false;
Vertex *vertex;
Vertex *vertex_color;
MMesh *mymesh;
vector<unique_ptr<model_t>> models;
vector<Vertex> lamp_loc;

bool recheck;

// variables for display OpenGL based point viewer
int dispWindowIndex = 0;
GLuint dispBindIndex = 0;
const float dispPointSize = 2.0f;

// variables for display text
string dispString = "";
const string dispStringInit = "Depth Threshold: D\nInfrared Threshold: I\nNonlocal Means Filter: N\nPick BodyIndex: P\nAccumulate Mode: A\nSelect Mode: C,B(select)\nSave: S\nReset View: R\nQuit: ESC";
string frameRate;

HANDLE hMutex;
//KinectBasic kinect;

// functions for GUIs
void InitializeWindow(int argc, char* argv[]);

// high-level functions for GUI
void draw_center();
void idle();
void display();
void close();
void special(int, int, int) {}
void keyboard(unsigned char, int, int);
void mouse(int, int, int, int);
void reshape(int, int);
void motion(int, int);


// basic functions for computation/GUI
// trackball codes were imported from those of Gavin Bell
// which appeared in SIGGRAPH '88
void vzero(float*);
void vset(float*, float, float, float);
void vsub(const float*, const float*, float*);
void vcopy(const float*, float*);
void vcross(const float *v1, const float *v2, float *cross);
float vlength(const float *v);
void vscale(float *v, float div);
void vnormal(float *v);
float vdot(const float *v1, const float *v2);
void vadd(const float *src1, const float *src2, float *dst);

void trackball(float q[4], float, float, float, float);
//void add_quats(float*, float*, float*);
void axis_to_quat(float a[3], float phi, float q[4]);
void normalize_quat(float q[4]);
float tb_project_to_sphere(float, float, float);
void build_rotmatrix(float m[4][4], float q[4]);
void Reader();
void DrawObj();
void DrawMeshObj();
void Setskt();

//CameraSpacePoint m_SpacePoint[JointType::JointType_Count];
void Track();


double dtor(double degrees);