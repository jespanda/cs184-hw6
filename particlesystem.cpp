#include "particlesystem.h"
#include "vec3f.h"
float randomFloat() {
	return (float)rand() / ((float)RAND_MAX + 1);
}
//Represents a single particle.
struct Particle {
	Vec3f pos;
	Vec3f velocity;
	Vec3f color;
	float timeAlive; //The amount of time that this particle has been alive.
	float lifespan;  //The total amount of time that this particle is to live.
};

//Rotates the vector by the indicated number of degrees about the specified axis
Vec3f rotate(Vec3f v, Vec3f axis, float degrees) {
	axis = axis.normalize();
	float radians = degrees * PI / 180;
	float s = sin(radians);
	float c = cos(radians);
	return v * c + axis * axis.dot(v) * (1 - c) + v.cross(axis) * s;
}

//Returns the position of the particle, after rotating the camera
Vec3f adjParticlePos(Vec3f pos) {
	return pos;
}

//Returns whether particle1 is in back of particle2
bool compareParticles(Particle* particle1, Particle* particle2) {
	return adjParticlePos(particle1->pos)[2] <
		adjParticlePos(particle2->pos)[2];
}


GLuint textureId;
Particle particles[NUM_PARTICLES];
//The amount of time until the next call to step().
float timeUntilNextStep;
//The color of particles that the fountain is currently shooting.  0
//indicates red, and when it reaches 1, it starts over at red again.  It
//always lies between 0 and 1.
float colorTime;
//The angle at which the fountain is shooting particles, in radians.
float angle;
//The origin
Vec3f origin;
//Returns the current color of particles produced by the fountain.
Vec3f curColor() {
	Vec3f color;
	if (colorTime < 0.166667f) {
		color = Vec3f(1.0f, colorTime * 6, 0.0f);
	}
	else if (colorTime < 0.333333f) {
		color = Vec3f((0.333333f - colorTime) * 6, 1.0f, 0.0f);
	}
	else if (colorTime < 0.5f) {
		color = Vec3f(0.0f, 1.0f, (colorTime - 0.333333f) * 6);
	}
	else if (colorTime < 0.666667f) {
		color = Vec3f(0.0f, (0.666667f - colorTime) * 6, 1.0f);
	}
	else if (colorTime < 0.833333f) {
		color = Vec3f((colorTime - 0.666667f) * 6, 0.0f, 1.0f);
	}
	else {
		color = Vec3f(1.0f, 0.0f, (1.0f - colorTime) * 6);
	}
	
	//Make sure each of the color's components range from 0 to 1
	for(int i = 0; i < 3; i++) {
		if (color[i] < 0) {
			color[i] = 0;
		}
		else if (color[i] > 1) {
			color[i] = 1;
		}
	}
	
	return color;
}

//Returns the average velocity of particles produced by the fountain.
Vec3f curVelocity() {
	return Vec3f(1 * cos(angle), 1.0f, 1 * sin(angle));
}

//Alters p to be a particle newly produced by the fountain.
void createParticle(Particle* p) {
	p->pos = Vec3f(0, 0, 0);
	p->velocity = curVelocity() + Vec3f(0.5f * randomFloat() - 0.25f,
										0.5f * randomFloat() - 0.25f,
										0.5f * randomFloat() - 0.25f);
	p->color = curColor();
	p->timeAlive = 0;
	p->lifespan = randomFloat();
}

//Advances the particle fountain by STEP_TIME seconds.
void step() {
	colorTime += STEP_TIME / 10;
	while (colorTime >= 1) {
		colorTime -= 1;
	}
	
	angle += 0.5f * STEP_TIME;
	while (angle > 2 * PI) {
		angle -= 2 * PI;
	}
	
	for(int i = 0; i < NUM_PARTICLES; i++) {
		Particle* p = particles + i;
		
		p->pos += p->velocity * STEP_TIME;
		p->velocity += Vec3f(0.0f, -GRAVITY * STEP_TIME, 0.0f);
		p->timeAlive += STEP_TIME;
		if (p->timeAlive > p->lifespan) {
			createParticle(p);
		}
	}
}

ParticleEngine::ParticleEngine(GLuint textureId1) {
	textureId = textureId1;
	timeUntilNextStep = 0;
	colorTime = 0;
	angle = 0;
	for(int i = 0; i < NUM_PARTICLES; i++) {
		createParticle(particles + i);
	}
	for(int i = 0; i < 10 / STEP_TIME; i++) {
		step();
	}
}

//Advances the particle fountain by the specified amount of time.
void ParticleEngine::advance(float dt) {
	while (dt > 0) {
		if (timeUntilNextStep < dt) {
			dt -= timeUntilNextStep;
			step();
			timeUntilNextStep = STEP_TIME;
		}
		else {
			timeUntilNextStep -= dt;
			dt = 0;
		}
	}
}

//Draws the particle fountain.
void ParticleEngine::draw() {
	vector<Particle*> ps;
	for(int i = 0; i < NUM_PARTICLES; i++) {
		ps.push_back(particles + i);
	}
	sort(ps.begin(), ps.end(), compareParticles);
	
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textureId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	glBegin(GL_QUADS);
	for(unsigned int i = 0; i < ps.size(); i++) {
		Particle* p = ps[i];
		glColor4f(p->color[0], p->color[1], p->color[2],
				  (1 - p->timeAlive / p->lifespan));
		float size = PARTICLE_SIZE / 2;
		
		Vec3f pos = p->pos;
		
		glTexCoord2f(0, 0);
		glVertex3f(pos[0] - size, pos[1] - size, pos[2]);
		glTexCoord2f(0, 1);
		glVertex3f(pos[0] - size, pos[1] + size, pos[2]);
		glTexCoord2f(1, 1);
		glVertex3f(pos[0] + size, pos[1] + size, pos[2]);
		glTexCoord2f(1, 0);
		glVertex3f(pos[0] + size, pos[1] - size, pos[2]);
	}
	glEnd();
}








