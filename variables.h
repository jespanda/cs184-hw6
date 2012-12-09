/*****************************************************************************/
/* This is the program skeleton for homework 2 in CS 184 by Ravi Ramamoorthi */
/* Extends HW 1 to deal with shading, more transforms and multiple shapes   */
/*****************************************************************************/

// This is the basic include file for the global variables in the program.  
// Since all files need access to it, we define EXTERN as either blank or 
// extern, depending on if included in the main program or not.  

#ifdef MAINPROGRAM 
#define EXTERN 
#else 
#define EXTERN extern 
#endif

#include "glm.h"

#include <list>
#include <map>
#include <string.h>



class Grader;
class ParticleEngine;

EXTERN int amount; // The amount of rotation for each arrow press
EXTERN vec3 eye; // The (regularly updated) vector coordinates of the eye 
EXTERN vec3 up;  // The (regularly updated) vector coordinates of the up 

#ifdef _WINDOWS
#define DIRSEP "\\"
#else
#define DIRSEP "/"
#endif


#ifdef MAINPROGRAM 
vec3 eyeinit(0.0,0.0,5.0) ; // Initial eye position, also for resets
vec3 upinit(0.0,1.0,0.0) ; // Initial up position, also for resets
vec3 center(0.0,0.0,0.0) ; // Center look at point 
int w = 600, h = 400 ; // width and height 
float fovy = 90.0 ; // For field of view
Grader grader;
bool allowGrader = false;
string scenefile;

bool wireframes = false;
int lightingFlags = 0;
bool fileexists( string fn ) {
  ifstream f(fn.c_str());
  return f != 0;
}
#else 
EXTERN vec3 eyeinit ; 
EXTERN vec3 upinit ; 
EXTERN vec3 center ; 
EXTERN int w, h ; 
EXTERN float fovy ; 
EXTERN Grader grader;
EXTERN bool allowGrader;
EXTERN string scenefile;

EXTERN bool wireframes;
EXTERN int lightingFlags;
EXTERN bool fileexists( string fn );
#endif 

EXTERN float max_x; //dimensions of the scene
EXTERN float max_y; 
EXTERN float max_z;

EXTERN float move_boats_x;
EXTERN float move_boats_y;
EXTERN float spin_degrees;
EXTERN float frame;
EXTERN bool discomode;
EXTERN bool enable_tex;

EXTERN ParticleEngine* _fountain;

EXTERN bool useGlu; // Toggle use of "official" opengl/glm transform vs user 
EXTERN GLuint vertexshader, fragmentshader, shaderprogram ; // shaders
static enum {rotate, translate, view, scale} transop ; // which operation to transform 
enum {Vertices, Colors, Elements, TexCoords} ; // For arrays for object 
enum shape_type {cube, sphere, teapot, glitem, glmodel} ;
enum lighting_flag { lightingenabled = 1, texturesenabled = 2 } ;
EXTERN float sx, sy, sz ; // the scale in x, y and z
EXTERN float tx, ty, tz ; // the translation in x, y and z

// Lighting parameter array, similar to that in the fragment shader
const int numLights = 10 ; 
EXTERN GLfloat lightposn [4*numLights] ; // Light Positions
EXTERN GLfloat lightcolor[4*numLights] ; // Light Colors
EXTERN GLfloat lightransf[4*numLights] ; // Lights transformed by modelview
EXTERN int numused ;                     // How many lights are used 

// Fog Parameters
EXTERN GLfloat fogCol[3];

// Materials (read from file) 
// With multiple shapes, these are colors for each.
EXTERN GLfloat ambient[4] ; 
EXTERN GLfloat diffuse[4] ; 
EXTERN GLfloat specular[4] ; 
EXTERN GLfloat emission[4] ; 
EXTERN GLfloat shininess ; 
EXTERN GLuint animate_obj;
EXTERN GLuint animate_tex;


// Variables to set uniform params for lighting fragment shader 
EXTERN GLuint lightcol ; 
EXTERN GLuint lightpos ; 
EXTERN GLuint numusedcol ; 
EXTERN GLuint enablelighting ;
EXTERN GLuint ambientcol ; 
EXTERN GLuint diffusecol ; 
EXTERN GLuint specularcol ; 
EXTERN GLuint emissioncol ; 
EXTERN GLuint shininesscol ; 

EXTERN GLuint hastex ;
EXTERN GLuint texsampler ;

struct texture {
  int     id;
  string  name;
  int     xdim;
  int     ydim;
  GLuint  texName;
  GLubyte *texturedata;

  texture( int i, string n, int x, int y );
  virtual ~texture();

  void read(const char *filename );
  void glinit(GLuint program);
};

EXTERN std::map<string,texture*> allTextures;


/*


const GLfloat floorverts[4][3] = {
	{2.0, 2.0, 0.0}, {-2.0, 2.0, 0.0}, {-2.0, -2.0, 0.0}, {2.0, -2.0, 0.0}
} ; 
const GLfloat floorcol[4][3] = {
	{1.0, 1.0, 1.0}, {1.0, 1.0, 1.0}, {1.0, 1.0, 1.0}, {1.0, 1.0, 1.0}
} ; 
const GLubyte floorinds[1][4] = { {0, 1, 2, 3} } ; 
const GLfloat floortex[4][2] = { 
	{1.0, 1.0}, {0.0, 1.0}, {0.0, 0.0}, {1.0, 0.0}
} ;
const GLfloat floorverts[4][3] = {
	{2.0, 2.0, 0.0}, {-2.0, 2.0, 0.0}, {-2.0, -2.0, 0.0}, {2.0, -2.0, 0.0}
} ; 
const GLfloat floorcol[4][3] = {
	{1.0, 1.0, 1.0}, {1.0, 1.0, 1.0}, {1.0, 1.0, 1.0}, {1.0, 1.0, 1.0}
} ; 
const GLubyte floorinds[1][4] = { {0, 1, 2, 3} } ; 
const GLfloat floortex[4][2] = { 
	{1.0, 1.0}, {0.0, 1.0}, {0.0, 0.0}, {1.0, 0.0}
} ;

 * */
// For multiple shapes, read from a file.  
//const int maxshapes = 50 ;
EXTERN int numshapes ; 

struct shape {
  shape_type kind ;
  GLfloat size ;

  // look & feel
  GLfloat ambient[4] ; 
  GLfloat diffuse[4] ; 
  GLfloat specular[4] ;
  GLfloat emission[4] ; 
  GLfloat shininess ;
  GLint animate;

  mat4 transform ; 

#define ppcolor 3
  //shape() : tex(0) {}
  shape( shape_type _kind, GLfloat _size, const mat4& _transform );
  shape( const shape &src );
  virtual ~shape();
  virtual void dump();
  virtual void glinit(GLuint program);
  virtual void draw( );
};

EXTERN std::list<shape*> shapes;


struct cshape : shape {
  static GLenum toShapeMode( string k );
  GLenum  mode ;

  int     numverts;
  int     numcolors;
  int     numelts;
  int     numtexcoords; // should be 0 or match numverts

  GLfloat *verts;
  GLubyte *elts;
  GLfloat *colors;    // RGBA
  GLfloat *texcoords;

  texture *tex;

  GLuint  buffer[4];  // for
  cshape(
      //string n,
      string k,
      list<float>  &source_verts,
      list<float>  &source_colors,
      list<int>    &source_elts,
      list<float>  &source_texcoords,
      texture*     active_texture,
      mat4         &source_transform);
  virtual ~cshape();

  virtual void dump();
  virtual void glinit(GLuint program);
  virtual void draw( );
};



struct model : shape {
  GLMmodel *_model;

  model(GLMmodel *m,const mat4& source_transform);
  virtual ~model();

  virtual void glinit(GLuint program);
  virtual void draw( );
};

struct object {
  string name;
  std::list<shape*> shapes;
  object(string s)
   : name(s)
  {
  }
};

EXTERN std::map<string,object*> objects;

