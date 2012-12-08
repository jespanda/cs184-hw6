#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <vector>

#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

using namespace std;
const float PI = 3.1415926535f;
const float GRAVITY = 3.0f;
const int NUM_PARTICLES = 100;
//The interval of time, in seconds, by which the particle engine periodically
//steps.
const float STEP_TIME = 0.01f;
//The length of the sides of the quadrilateral drawn for each particle.
const float PARTICLE_SIZE = 0.05f;

const int TIMER_MS = 50; //The number of milliseconds to which the timer is set
//Returns a random float from 0 to < 1

const GLuint _textureId = 1;

class ParticleEngine {

    public:
        ParticleEngine(GLuint textureId1); 
        //Advances the particle fountain by the specified amount of time.
        void advance(float dt); 
        //Draws the particle fountain.
        void draw(); 
        void drawScene();
};
