/*****************************************************************************/
/* This is the program skeleton for homework 2 in CS 184 by Ravi Ramamoorthi */
/* Extends HW 1 to deal with shading, more transforms and multiple objects   */
/*****************************************************************************/

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <deque>
#include <stack>
#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include <GL/glut.h>
#include "shaders.h"
#include "Transform.h"
#include <FreeImage.h>
#include "UCB/grader.h"

using namespace std ; 

// Main variables in the program.  
#define MAINPROGRAM 
#include "variables.h" 
#include "readfile.h" // prototypes for readfile.cpp  

//Particle System
#include "particlesystem.h"
#include "imageloader.h"

void display(void) ;  // prototype for display function.
void setFog(void);
/*
Grader grader;
bool allowGrader = false;
const char *scenefile = NULL;
*/
// Uses the Projection matrices (technically deprecated) to set perspective 
// We could also do this in a more modern fashion with glm.  
void reshape(int width, int height){
  w = width;
  h = height;
  mat4 mv ; // just like for lookat

  glMatrixMode(GL_PROJECTION);
  float aspect = w / (float) h, zNear = 0.1, zFar = 99.0 ;
  // I am changing the projection stuff to be consistent with lookat
  if (useGlu) mv = glm::perspective(fovy,aspect,zNear,zFar) ; 
  else {
    mv = Transform::perspective(fovy,aspect,zNear,zFar) ; 
    mv = glm::transpose(mv) ; // accounting for row major 
  }
  glLoadMatrixf(&mv[0][0]) ; 

  glViewport(0, 0, w, h);
}

void animation(void) {
	//animates sushi boats
	float max_y = -40;
	float max_x = 38.5;
	if (move_boats_y <= max_y) {
		if (move_boats_x<=0) {
			move_boats_x = 0;
			move_boats_y = 0;
		} else {
			move_boats_x-=0.1;
		}
	}else {
		if (move_boats_x>=max_x) {
			move_boats_y-=0.1;
		} else {
			move_boats_x+=0.1;
		}
	}
	//changes texture
	frame+=0.02;
	if (frame>3) frame = 0;
	glutPostRedisplay() ;
}

//typedef unsigned char BYTE;
void saveScreenshot(string fname) {
  int px = w * h;
  BYTE *pixels = new BYTE[3*px];  
  glReadBuffer(GL_FRONT);
  glReadPixels(0,0,w,h,GL_BGR,GL_UNSIGNED_BYTE, pixels);

  FIBITMAP *img = FreeImage_ConvertFromRawBits(pixels, w, h, w * 3, 24, 0xFF0000, 0x00FF00, 0x0000FF, false);
  
  std::cout << "Saving screenshot: " << fname << "\n";

  FreeImage_Save(FIF_PNG, img, fname.c_str(), 0);
}

void printHelp() {
  std::cout << "\npress 'h' to print this message again.\n" 
   << "press '+' or '-' to change the amount of rotation that\noccurs with each arrow press.\n" 
  << "press 'i' to run image grader test cases\n"
  << "press 'f' refresh scene from the file: " << scenefile << "\n"
  << "press 'g' to switch between using glm::lookAt and glm::Perspective or your own LookAt.\n"   
  << "press 's' to to save a screenshot.\n"
  << "press 'r' to reset the transformations.\n"
  << "press 'a' to toggle animation.\n"
  << "press 'e' to toggle textures.\n"
  << "press 'v' 't' 'x' to do view [default], translate, scale.\n"
  << "press ESC to quit.\n" ;  
}

void keyboard(unsigned char key, int x, int y) {
  switch(key) {
  case '+':
    amount++;
    std::cout << "amount set to " << amount << "\n" ;
    break;
  case '-':
    amount--;
    std::cout << "amount set to " << amount << "\n" ; 
    break;
  case 'i':
    if(allowGrader) {
      std::cout << "Running input script...\n";
      grader.runTests();
      std::cout << "Done! [ESC to quit]\n";
    } else {
      std::cout << "Error: no input file specified for grader\n";
    }
    break;
  case 'g':
    useGlu = !useGlu;
    reshape(w,h) ; 
    std::cout << "Using glm::LookAt and glm::Perspective set to: " << (useGlu ? " true " : " false ") << "\n" ; 
    break;
  case 'h':
    printHelp();
    break;
  case 27:  // Escape to quit
    exit(0) ;
    break ;
  case 'f':
    resetscene();
    readfile(scenefile.c_str());
    //break;
  case 'r': // reset eye and up vectors, scale and translate. 
    eye = eyeinit ; 
    up = upinit ; 
    sx = sy = sz = 1.0 ; 
    tx = ty = tz = 0.0 ; 
    break ;   
  case 's':
    saveScreenshot("screenshot.jpg");
    break;
  case 't':
    transop = translate ; 
    std::cout << "Operation is set to Translate\n" ; 
    break ; 
  case 'v': 
    transop = view ;
    std::cout << "Operation is set to View\n" ; 
    break ; 
  case 'x':
    transop = scale ; 
    std::cout << "Operation is set to Scale\n" ; 
    break ;
  case 'a':
	animate_obj = !animate_obj ;
	animate_tex = !animate_tex;
	if (animate_obj) {
		std::cout << "Animation: ON\n"; 
		glutIdleFunc(animation) ;
	} else {
		std::cout << "Animation: OFF\n";
		glutIdleFunc(NULL) ;
	}
	break ;
  case 'e':
	  enable_tex = !enable_tex;
	  std::cout << "Toggled texture (now " << (wireframes?"on":"off") << ")\n";
	  break;
  case 'w':
    wireframes = !wireframes ;
    std::cout << "Toggled wireframes (now " << (wireframes?"on":"off") << ")\n";
  }
  glutPostRedisplay();
}

void passive_motion(int x, int y )
{
  //std::cout << "PassiveMotion " << x << ", " << y << "\n";
}

int last_cursor_x = -1, last_cursor_y = -1;
void motion(int x, int y)
{
  //std::cout << "Motion " << x << ", " << y << "\n";

  int dx = 0, dy = 0;

  if( 0 <= last_cursor_x ) {
    dx = last_cursor_x - x;
  }
  if( 0 <= last_cursor_y ) {
    dy = - ( last_cursor_y - y );
  }

  /*
  if( x < last_cursor_x ) {
    dx = amount;
  } else if( x > last_cursor_x ) {
    dx = -amount;
  }

  if( y < last_cursor_y ) {
    dy = -amount;
  } else if( y > last_cursor_y ) {
    dy = amount;
  }*/

  if( 0 != dx ) Transform::left(dx, eye,  up);
  if( 0 != dy ) Transform::up(dy, eye,  up);

  last_cursor_x = x;
  last_cursor_y = y;

    //glutSetCursor(GLUT_CURSOR_NONE);//FULL_CROSSHAIR); 
  //glutWarpPointer( w/2, h/2 );//250, 250 );
  glutPostRedisplay();
}

inline int sign(int a) { return a<0?-1:1; }
void mouse(int button, int state, int x, int y)
{
  if( state==GLUT_UP ) {
    last_cursor_x = last_cursor_y = -1;
  }
   // Wheel reports as button 3(scroll up) and button 4(scroll down)
   if ((button == 3) || (button == 4)) // It's a wheel event
   {
       // Each wheel event reports like a button click, GLUT_DOWN then GLUT_UP
       if (state == GLUT_UP) return; // Disregard redundant GLUT_UP events
       if( button == 3 ) {
         sx += amount * 0.01;
         sy += amount * 0.01;
         sz += amount * 0.01;
         //eye -= ((amount*0.05f)*glm::normalize(eye));//eye.y += sign(eye.y) * amount * 0.1;
       } else {
         sx -= amount * 0.01;
         sy -= amount * 0.01;
         sz -= amount * 0.01;
         //eye += ((amount*0.05f)*glm::normalize(eye));//eye.y += sign(eye.y) * amount * 0.1;
         //eye.y -= sign(eye.y) * amount * 0.1;
       }
       glutPostRedisplay();
       //printf("Scroll %s At %d %d\n", (button == 3) ? "Up" : "Down", x, y);
   }else{  // normal button event
       //printf("Button %d: %s At %d %d\n", button, (state == GLUT_DOWN) ? "Down" : "Up", x, y);
   }
}
//  You will need to enter code for the arrow keys 
//  When an arrow key is pressed, it will call your transform functions

#define MOVE_SCALE 0.2 
void specialKey(int key, int x, int y) {
  int mod = glutGetModifiers();
  switch(key) {
  case 100: //left
    if ( mod & GLUT_ACTIVE_SHIFT ) {//transop == rotate) {
      vec3 c = center - eye;
      Transform::left( amount, c, up );
      center = c + eye;
    } else {
      vec3 diff = mat3(MOVE_SCALE) * glm::normalize(glm::cross(center -eye, up) );///10;
      center -= diff;
      eye -= diff;
    }
    break;
  case 101: //up
    if( mod & GLUT_ACTIVE_SHIFT ) {
      vec3 c = center - eye;
      Transform::up(amount, c, up );
      center = c + eye;
    } else {
      vec3 diff = mat3(MOVE_SCALE) * glm::normalize(center - eye);///10;
      center += diff;
      eye += diff;
    }
    break;
  case 102: //right
    if ( mod & GLUT_ACTIVE_SHIFT ) {
      vec3 c = center - eye;
      Transform::left( -amount, c, up );
      center = c + eye;
    } else {
      vec3 diff = mat3(MOVE_SCALE) * glm::normalize(glm::cross(center -eye, up) );///10;
      center += diff;
      eye += diff;
    }
    break;
  case 103: //down
    if( mod & GLUT_ACTIVE_SHIFT ) {
      vec3 c = center - eye;
      Transform::up(-amount, c, up );
      center = c + eye;
    } else {
      vec3 diff = mat3(MOVE_SCALE) * glm::normalize(center - eye);///10;
      center -= diff;
      eye -= diff;
    }
    break;
  }
  glutPostRedisplay();
}
// Particle System
void update(int value) {
  _fountain->advance(TIMER_MS / 1000.0f);
  glutPostRedisplay();
  glutTimerFunc(TIMER_MS, update, 0);
}
GLuint _textureId;
//Returns an array indicating pixel data for an RGBA image that is the same as
//image, but with an alpha channel indicated by the grayscale image alphaChannel
char* addAlphaChannel(Image* image1, Image* alphaChannel) {
    char* pixels = new char[image1->width * image1->height * 4];
    for(int y = 0; y < image1->height; y++) {
        for(int x = 0; x < image1->width; x++) {
            for(int j = 0; j < 3; j++) {
                pixels[4 * (y * image1->width + x) + j] =
                    image1->pixels[3 * (y * image1->width + x) + j];
            }
            pixels[4 * (y * image1->width + x) + 3] =
                alphaChannel->pixels[3 * (y * image1->width + x)];
        }
    }
    
    return pixels;
}

//Makes the image into a texture, using the specified grayscale image as an
//alpha channel and returns the id of the texture
GLuint loadAlphaTexture(Image* image1, Image* alphaChannel) {
    char* pixels = addAlphaChannel(image1, alphaChannel);
    
    GLuint textureId;
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RGBA,
                 image1->width, image1->height,
                 0,
                 GL_RGBA,
                 GL_UNSIGNED_BYTE,
                 pixels);
    
    delete pixels;
    return textureId;
}

void init() {
  // Initialize shaders
  vertexshader = initshaders(GL_VERTEX_SHADER, "shaders" DIRSEP "light.vert.glsl") ;
  fragmentshader = initshaders(GL_FRAGMENT_SHADER, "shaders" DIRSEP "light.frag.glsl") ;
  // do we need this?
  //GLuint program = glCreateProgram() ;
  shaderprogram = initprogram(vertexshader, fragmentshader) ; 
  // do we need this too?
  //GLint linked;
  //glGetProgramiv(shaderprogram, GL_LINK_STATUS, &linked) ;  
  animate_obj = 0;
  animate_tex = 0;
  enablelighting = glGetUniformLocation(shaderprogram,"enablelighting") ;
  lightpos = glGetUniformLocation(shaderprogram,"lightposn") ;   
  lightcol = glGetUniformLocation(shaderprogram,"lightcolor") ;   
  numusedcol = glGetUniformLocation(shaderprogram,"numused") ;   
  ambientcol = glGetUniformLocation(shaderprogram,"ambient") ;   
  diffusecol = glGetUniformLocation(shaderprogram,"diffuse") ;   
  specularcol = glGetUniformLocation(shaderprogram,"specular") ;   
  emissioncol = glGetUniformLocation(shaderprogram,"emission") ;   
  shininesscol = glGetUniformLocation(shaderprogram,"shininess") ;


  hastex = glGetUniformLocation(shaderprogram,"hastex");
  texsampler = glGetUniformLocation(shaderprogram,"tex");
  // Enable the depth test
  glEnable(GL_DEPTH_TEST) ;
  glDepthFunc (GL_LESS) ; // The default option
  glEnable(GL_COLOR_MATERIAL);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
  Image* image1 = loadBMP("circle.bmp");
  Image* alphaChannel = loadBMP("circlealpha.bmp");
  _textureId = loadAlphaTexture(image1, alphaChannel);
  delete image1;
  delete alphaChannel;
}
int main(int argc, char* argv[]) {

  /*
  if (argc < 2) {
    cerr << "Usage: transforms [scenefile] [grader input (optional)]\n"; 
    exit(-1); 
  }//*/
  srand((unsigned int)time(0)); //Seed the random number generator
  FreeImage_Initialise();
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
  glutCreateWindow("HW6: City");

#ifdef _WINDOWS
  	GLenum err = glewInit() ; 
	if (GLEW_OK != err) { 
		std::cerr << "Error: " << glewGetString(err) << std::endl;
	}
#endif

  init();
  if( argc < 2 ) {
    cout << "Loading local objects function\n";
    loadObjects();
  } else {
    scenefile = argv[1];
    if( !fileexists( scenefile ) ) {
      string alt = "scenes";
      alt += DIRSEP;
      alt += scenefile;
      if( fileexists( alt ) ) {
        scenefile=alt;
	  } else {
        printf( "Can't find scene file %s, (tried %s and %s)\n", argv[1], scenefile.c_str(), alt.c_str() );
      }
    }
    cout << "Reading scene file " << scenefile << "\n";
    readfile(scenefile.c_str());
  }

  glutDisplayFunc(display);
  glutSpecialFunc(specialKey);
  glutKeyboardFunc(keyboard);
  glutMouseFunc(mouse);
  glutMotionFunc(motion);
  glutPassiveMotionFunc(passive_motion);
  glutReshapeFunc(reshape);
  glutReshapeWindow(w, h);
  _fountain = new ParticleEngine(_textureId, 1.0f, 1.0f, 2.0f);
  //Particle
  glutTimerFunc(TIMER_MS, update, 0);

  if (argc > 2) {
    allowGrader = true;
    stringstream tcid;
    tcid << argv[1] << "." << argv[2];
    grader.init(tcid.str());
    grader.loadCommands(argv[2]);
    grader.bindDisplayFunc(display);
    grader.bindSpecialFunc(specialKey);
    grader.bindKeyboardFunc(keyboard);
    grader.bindScreenshotFunc(saveScreenshot);
  }
  printHelp();
  glutMainLoop();
  FreeImage_DeInitialise();
  return 0;
}
