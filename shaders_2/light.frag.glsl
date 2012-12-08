# version 120 

/* This is the fragment shader for reading in a scene description, including 
   lighting.  Uniform lights are specified from the main program, and used in 
   the shader.  As well as the material parameters of the object.  */

// Mine is an old machine.  For version 130 or higher, do 
// in vec4 color ;  
// in vec4 mynormal ; 
// in vec4 myvertex ;
// That is certainly more modern

varying vec4 color ;
varying vec3 mynormal ; 
varying vec4 myvertex ; 

const int numLights = 10 ; 
uniform bool enablelighting ; // are we lighting at all (global).
uniform vec4 lightposn[numLights] ; // positions of lights 
uniform vec4 lightcolor[numLights] ; // colors of lights
uniform int numused ;               // number of lights used

// Now, set the material parameters.  These could be varying and/or bound to 
// a buffer.  But for now, I'll just make them uniform.  
// I use ambient, diffuse, specular, shininess as in OpenGL.  
// But, the ambient is just additive and doesn't multiply the lights.  
uniform int hastex ;
uniform sampler2D tex ;

uniform vec4 ambient ; 
uniform vec4 diffuse ; 
uniform vec4 specular ; 
uniform vec4 emission ; 
uniform float shininess ; 


vec4 handleAllLights();

void main (void) 
{       
	if ( hastex > 0)
    gl_FragColor = texture2D(tex, gl_TexCoord[0].st) ; 
	else if (!enablelighting)
    gl_FragColor = color ;
  else { 
    vec4 finalcolor = handleAllLights();
    gl_FragColor = finalcolor ;
  }
}

/**
I = A + E + sum(over lights) L[D max(N L, 0) + S max(N H, 0)^S]
A = ambient
E = self emission
L = direction to the light
H = half-angle
s = shininess
 */

vec4 ComputeLight (
  const in vec3 direction,
  const in vec4 lightcolor,
  const in vec3 normal,
  const in vec3 halfvec,
  const in vec4 mydiffuse,
  const in vec4 myspecular,
  const in float myshininess) {

  float nDotL = dot(normal, direction);
  vec4 lambert = mydiffuse * lightcolor * max (nDotL, 0.0);
  float nDotH = dot(normal, halfvec);
  vec4 phong = myspecular * lightcolor * pow (max(nDotH, 0.0), myshininess);

  vec4 retval = lambert + phong ; 
  return retval ;            
}

vec4 handleAllLights (void) 
{       
  // They eye is always at (0,0,0) looking down -z axis 
  // Also compute current fragment position and direction to eye 
  vec4 totalColor = vec4(0, 0, 0, 0);
  const vec3 eyepos = vec3(0,0,0) ; 
  vec4 _mypos = gl_ModelViewMatrix * myvertex ; 
  vec3 mypos = _mypos.xyz / _mypos.w ; // Dehomogenize current location 
  vec3 eyedirn = normalize(eyepos - mypos) ; 
  // Compute normal, needed for shading. 
  // Simpler is vec3 normal = normalize(gl_NormalMatrix * mynormal) ; 
  vec3 _normal = (gl_ModelViewMatrixInverseTranspose*vec4(mynormal,0.0)).xyz ; 
  vec3 normal = normalize(_normal) ; 
  for( int x = 0; x <= numused; x++ ) {
    // Light point
    vec3 direction;
    vec4 pos = lightposn[x].xyzw;

    if( 0.0 != pos.w ) {
      vec3 position = pos.xyz / pos.w ; 
      direction = normalize (position - mypos) ; // no attenuation 
    } else {
      direction = normalize( pos.xyz );
    }
    vec3 halfangle = normalize (direction + eyedirn) ;  
    vec4 col = ComputeLight(direction, lightcolor[x], normal, halfangle, diffuse, specular, shininess) ;
    totalColor += col;
  }
  return ambient + emission + totalColor; 
}
