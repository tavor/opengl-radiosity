#include "GL/freeglut.h"
#include "GL/gl.h"
#include "struct.h"
#include "tiny_obj_loader.h"
#include <iostream>
#include <cmath>
#include <stdlib.h>

using namespace std;
using namespace radiosity_structs;

int WINDOW_HEIGHT = 512;
int WINDOW_WIDTH = 512;

enum HEMICUBE_VIEW { LEFT, RIGHT, TOP, BOTTOM, FRONT };

void 	checkWhetherPatchHasComponentMax(patch * p, light * totalLight);
void renderHemicubeView(point * center, Vector * up, Vector * direction, HEMICUBE_VIEW view);
light * calculauteIncidentLight(point * center, Vector * normal, Vector * up);
void getHemicubePixels();
hemicube * applyMultiplierHemicube(hemicube * h);
unsigned char * applyMultiplier(unsigned char * buffer, HEMICUBE_VIEW view);
void applyMultiplier(light * l, HEMICUBE_VIEW view, int pixelIndex);
unsigned char * getHemicubePixels(HEMICUBE_VIEW view);
light * getTotalLightOfView(hemicube * h, HEMICUBE_VIEW view);
void calculateExcident(patch * p);
void renderBakingScene();
void renderPatch(patch * p);
void renderScene();
void printLight(light * l);
int ITERATIONS = 0;
int NUMBER_OF_PATCHES = 0;
bool running = true;
vector<tinyobj::shape_t> shapes;
patch * patches;
bool DEBUG = false;
