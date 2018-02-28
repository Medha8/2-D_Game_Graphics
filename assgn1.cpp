#include <iostream>
#include <cmath>
#include <fstream>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>
#include <unistd.h>
#include <time.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace std;

struct VAO {
  GLuint VertexArrayID;
  GLuint VertexBuffer;
  GLuint ColorBuffer;

  GLenum PrimitiveMode;
  GLenum FillMode;
  int NumVertices;
};
typedef struct VAO VAO;

struct GLMatrices {
	glm::mat4 projection;
	glm::mat4 model;
	glm::mat4 view;
	GLuint MatrixID;
  } Matrices;

  GLuint programID;

  /* Function to load Shaders - Use it as it is */
  GLuint LoadShaders(const char * vertex_file_path,const char * fragment_file_path) {

   // Create the shaders
   GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
   GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

   // Read the Vertex Shader code from the file
   std::string VertexShaderCode;
   std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
   if(VertexShaderStream.is_open())
   {
    std::string Line = "";
    while(getline(VertexShaderStream, Line))
    VertexShaderCode += "\n" + Line;
    VertexShaderStream.close();
  }

  // Read the Fragment Shader code from the file
  std::string FragmentShaderCode;
  std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
  if(FragmentShaderStream.is_open()){
    std::string Line = "";
    while(getline(FragmentShaderStream, Line))
    FragmentShaderCode += "\n" + Line;
    FragmentShaderStream.close();
  }

  GLint Result = GL_FALSE;
  int InfoLogLength;

  // Compile Vertex Shader
  printf("Compiling shader : %s\n", vertex_file_path);
  char const * VertexSourcePointer = VertexShaderCode.c_str();
  glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL);
  glCompileShader(VertexShaderID);

  // Check Vertex Shader
  glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
  glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
  std::vector<char> VertexShaderErrorMessage(InfoLogLength);
  glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
  fprintf(stdout, "%s\n", &VertexShaderErrorMessage[0]);

  // Compile Fragment Shader
  printf("Compiling shader : %s\n", fragment_file_path);
  char const * FragmentSourcePointer = FragmentShaderCode.c_str();
  glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
  glCompileShader(FragmentShaderID);

  // Check Fragment Shader
  glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
  glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
  std::vector<char> FragmentShaderErrorMessage(InfoLogLength);
  glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
  fprintf(stdout, "%s\n", &FragmentShaderErrorMessage[0]);

  // Link the program
  fprintf(stdout, "Linking program\n");
  GLuint ProgramID = glCreateProgram();
  glAttachShader(ProgramID, VertexShaderID);
  glAttachShader(ProgramID, FragmentShaderID);
  glLinkProgram(ProgramID);

  // Check the program
  glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
  glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
  std::vector<char> ProgramErrorMessage( max(InfoLogLength, int(1)) );
  glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
  fprintf(stdout, "%s\n", &ProgramErrorMessage[0]);

  glDeleteShader(VertexShaderID);
  glDeleteShader(FragmentShaderID);

  return ProgramID;
}

static void error_callback(int error, const char* description)
{
  fprintf(stderr, "Error: %s\n", description);
}

void quit(GLFWwindow *window)
{
  glfwDestroyWindow(window);
  glfwTerminate();
  //    exit(EXIT_SUCCESS);
}


/* Generate VAO, VBOs and return VAO handle */
struct VAO* create3DObject (GLenum primitive_mode, int numVertices, const GLfloat* vertex_buffer_data, const GLfloat* color_buffer_data, GLenum fill_mode=GL_FILL)
{
  struct VAO* vao = new struct VAO;
  vao->PrimitiveMode = primitive_mode;
  vao->NumVertices = numVertices;
  vao->FillMode = fill_mode;

  // Create Vertex Array Object
  // Should be done after CreateWindow and before any other GL calls
  glGenVertexArrays(1, &(vao->VertexArrayID)); // VAO
  glGenBuffers (1, &(vao->VertexBuffer)); // VBO - vertices
  glGenBuffers (1, &(vao->ColorBuffer));  // VBO - colors

  glBindVertexArray (vao->VertexArrayID); // Bind the VAO 
  glBindBuffer (GL_ARRAY_BUFFER, vao->VertexBuffer); // Bind the VBO vertices 
  glBufferData (GL_ARRAY_BUFFER, 3*numVertices*sizeof(GLfloat), vertex_buffer_data, GL_STATIC_DRAW); // Copy the vertices into VBO
  glVertexAttribPointer(
    0,                  // attribute 0. Vertices
    3,                  // size (x,y,z)
    GL_FLOAT,           // type
    GL_FALSE,           // normalized?
    0,                  // stride
    (void*)0            // array buffer offset
    );

  glBindBuffer (GL_ARRAY_BUFFER, vao->ColorBuffer); // Bind the VBO colors 
  glBufferData (GL_ARRAY_BUFFER, 3*numVertices*sizeof(GLfloat), color_buffer_data, GL_STATIC_DRAW);  // Copy the vertex colors
  glVertexAttribPointer(
    1,                  // attribute 1. Color
    3,                  // size (r,g,b)
    GL_FLOAT,           // type
    GL_FALSE,           // normalized?
    0,                  // stride
    (void*)0            // array buffer offset
    );

  return vao;
}

/* Generate VAO, VBOs and return VAO handle - Common Color for all vertices */
struct VAO* create3DObject (GLenum primitive_mode, int numVertices, const GLfloat* vertex_buffer_data, const GLfloat red, const GLfloat green, const GLfloat blue, GLenum fill_mode=GL_FILL)
{
  GLfloat* color_buffer_data = new GLfloat [3*numVertices];
  for (int i=0; i<numVertices; i++) {
    color_buffer_data [3*i] = red;
    color_buffer_data [3*i + 1] = green;
    color_buffer_data [3*i + 2] = blue;
  }

  return create3DObject(primitive_mode, numVertices, vertex_buffer_data, color_buffer_data, fill_mode);
}

/* Render the VBOs handled by VAO */
void draw3DObject (struct VAO* vao)
{
  // Change the Fill Mode for this object
  glPolygonMode (GL_FRONT_AND_BACK, vao->FillMode);

  // Bind the VAO to use
  glBindVertexArray (vao->VertexArrayID);

  // Enable Vertex Attribute 0 - 3d Vertices
  glEnableVertexAttribArray(0);
  // Bind the VBO to use
  glBindBuffer(GL_ARRAY_BUFFER, vao->VertexBuffer);

  // Enable Vertex Attribute 1 - Color
  glEnableVertexAttribArray(1);
  // Bind the VBO to use
  glBindBuffer(GL_ARRAY_BUFFER, vao->ColorBuffer);

  // Draw the geometry !
  glDrawArrays(vao->PrimitiveMode, 0, vao->NumVertices); // Starting from vertex 0; 3 vertices total -> 1 triangle
}

/**************************
* Customizable functions *
**************************/

float triangle_rot_dir = 1;
float gun2_rot_dir_pos = 1;
float gun2_rot_dir_neg = -1;
bool triangle_rot_status = false;
bool gun2_rot_status = false;
float rect1_xpos = 1.4;
float rect2_xpos = -1.4;
float gun_ypos = -0.3;
float increments = 6;
/* Executed when a regular key is pressed/released/held-down */
/* Prefered for Keyboard events */


/* Executed for character input (like in text boxes) */
void keyboardChar (GLFWwindow* window, unsigned int key)
{
 switch (key) {
  case 'Q':
  case 'q':
  quit(window);
  break;
  default:
  break;
}
}
/* Executed when a mouse button is pressed/released */

/* Executed when window is resized to 'width' and 'height' */
/* Modify the bounds of the screen here in glm::ortho or Field of View in glm::Perspective */
void reshapeWindow (GLFWwindow* window, int width, int height)
{
  int fbwidth=width, fbheight=height;
  /* With Retina display on Mac OS X, GLFW's FramebufferSize
  is different from WindowSize */
  glfwGetFramebufferSize(window, &fbwidth, &fbheight);

  GLfloat fov = 90.0f;

  // sets the viewport of openGL renderer
  glViewport (0, 0, (GLsizei) fbwidth, (GLsizei) fbheight);

  // set the projection matrix as perspective
  /* glMatrixMode (GL_PROJECTION);
  glLoadIdentity ();
  gluPerspective (fov, (GLfloat) fbwidth / (GLfloat) fbheight, 0.1, 500.0); */
  // Store the projection matrix in a variable for future use
  // Perspective projection for 3D views
  // Matrices.projection = glm::perspective (fov, (GLfloat) fbwidth / (GLfloat) fbheight, 0.1f, 500.0f);

  // Ortho projection for 2D views
  Matrices.projection = glm::ortho(-4.0f, 4.0f, -4.0f, 4.0f, 0.1f, 500.0f);
}

VAO *triangle, *rectangle1, *rectangle2, *line, *gun1, *gun2, *block1[6], *block2[6], *block3[6], *laser,*mirror1,*mirror2;

// Creates the triangle object used in this sample code

void createTriangle ()
{
  /* ONLY vertices between the bounds specified in glm::ortho will be visible on screen */

  /* Define vertex array as used in glBegin (GL_TRIANGLES) */
  static const GLfloat vertex_buffer_data [] = {
    0, 1,0, // vertex 0
    -1,-1,0, // vertex 1
    1,-1,0, // vertex 2
  };

  static const GLfloat color_buffer_data [] = {
    1,0,0, // color 0
    0,1,0, // color 1
    0,0,1, // color 2
  };

  // create3DObject creates and returns a handle to a VAO that can be used later
  triangle = create3DObject(GL_TRIANGLES, 3, vertex_buffer_data, color_buffer_data, GL_FILL);
}

// Creates the rectangle object used in this sample code
void createRectangle1 ()
{
  // GL3 accepts only Triangles. Quads are not supported
  static const GLfloat vertex_buffer_data [] = {
    -0.45,-0.35,0, // vertex 1
    0.45,-0.35,0, // vertex 2
    0.45, 0.35,0, // vertex 3

    0.45, 0.35,0, // vertex 3
    -0.45, 0.35,0, // vertex 4
    -0.45,-0.35,0  // vertex 1
  };

  static const GLfloat color_buffer_data [] = {
    0.5019,1,0, // color 1
    0.5019,1,0, // color 2
    0.5019,1,0, // color 3

    0.5019,1,0, // color 3
    0.5019,1,0, // color 4
    0.5019,1,0, // color 1

  };
  // create3DObject creates and returns a handle to a VAO that can be used later
  rectangle1 = create3DObject(GL_TRIANGLES, 6, vertex_buffer_data, color_buffer_data, GL_FILL);
}
void createRectangle2 ()
{
  // GL3 accepts only Triangles. Quads are not supported
  static const GLfloat vertex_buffer_data [] = {
    -0.45,-0.35,0, // vertex 1
    0.45,-0.35,0, // vertex 2
    0.45, 0.35,0, // vertex 3

    0.45, 0.35,0, // vertex 3
    -0.45, 0.35,0, // vertex 4
    -0.45,-0.35,0  // vertex 1
  };

  static const GLfloat color_buffer_data [] = {
    0.8,0,0, // color 1
    0.8,0,0, // color 2
    0.8,0,0, // color 3

    0.8,0,0, // color 3
    0.8,0,0, // color 4
    0.8,0,0, // color 1
  };
  // create3DObject creates and returns a handle to a VAO that can be used later
  rectangle2 = create3DObject(GL_TRIANGLES, 6, vertex_buffer_data, color_buffer_data, GL_FILL);
}

void createLine ()
{
  // GL3 accepts only Triangles. Quads are not supported
  static const GLfloat vertex_buffer_data [] = {
    -5,-0.01,0, // vertex 1
    5,-0.01,0, // vertex 2
    5, 0.01,0, // vertex 3

    5, 0.01,0, // vertex 3
    -5, 0.01,0, // vertex 4
    -5,-0.01,0  // vertex 1
  };

  static const GLfloat color_buffer_data [] = {
    0,0,0, // color 1
    0,0,0, // color 2
    0,0,0, // color 3

    0,0,0, // color 3
    0,0,0, // color 4
    0,0,0, // color 1

  };
  // create3DObject creates and returns a handle to a VAO that can be used later
  line = create3DObject(GL_TRIANGLES, 6, vertex_buffer_data, color_buffer_data, GL_FILL);
}

void createGun1 ()
{
  // GL3 accepts only Triangles. Quads are not supported
  static const GLfloat vertex_buffer_data [] = {
    -0.3,-0.2,0, // vertex 1
    0.3,-0.2,0, // vertex 2
    0.3, 0.2,0, // vertex 3

    0.3, 0.2,0, // vertex 3
    -0.3, 0.2,0, // vertex 4
    -0.3,-0.2,0  // vertex 1
  };

  static const GLfloat color_buffer_data [] = {

    0.2,0.2,1, // color 1
    0.2,0.2,1, // color 2
    0.2,0.2,1, // color 3

    0.2,0.2,1, // color 3
    0.2,0.2,1, // color 4
    0.2,0.2,1  // color 1
  };
  // create3DObject creates and returns a handle to a VAO that can be used later
  gun1 = create3DObject(GL_TRIANGLES, 6, vertex_buffer_data, color_buffer_data, GL_FILL);
}

void createGun2 ()
{
  // GL3 accepts only Triangles. Quads are not supported
  static const GLfloat vertex_buffer_data [] = {
    0,-0.1,0, // vertex 1
    0.8,-0.1,0, // vertex 2
    0.8, 0.1,0, // vertex 3

    0.8, 0.1,0, // vertex 3
    0, 0.1,0, // vertex 4
    0,-0.1,0  // vertex 1
  };

  static const GLfloat color_buffer_data [] = {

    0.2,0.2,1, // color 1
    0.2,0.2,1, // color 2
    0.2,0.2,1, // color 3

    0.2,0.2,1, // color 3
    0.2,0.2,1, // color 4
    0.2,0.2,1  // color 1
  };
  // create3DObject creates and returns a handle to a VAO that can be used later
  gun2 = create3DObject(GL_TRIANGLES, 6, vertex_buffer_data, color_buffer_data, GL_FILL);
}

void createBlock1 ()
{
  // GL3 accepts only Triangles. Quads are not supported
  static const GLfloat vertex_buffer_data [] = {
    -0.05,-0.15,0, // vertex 1
    0.05,-0.15,0, // vertex 2
    0.05, 0.15,0, // vertex 3

    0.05, 0.15,0, // vertex 3
    -0.05, 0.15,0, // vertex 4
    -0.05,-0.15,0  // vertex 1
  };

  static const GLfloat color_buffer_data [] = {
    1,0,0, // color 1
    1,0,0, // color 2
    1,0,0, // color 3

    1,0,0, // color 3
    1,0,0, // color 4
    1,0,0, // color 1

  };
  // create3DObject creates and returns a handle to a VAO that can be used later
  int i;
  for (i=0;i<6;i++)
  block1[i] = create3DObject(GL_TRIANGLES, 6, vertex_buffer_data, color_buffer_data, GL_FILL);
}

void createBlock2 ()
{
  // GL3 accepts only Triangles. Quads are not supported
  static const GLfloat vertex_buffer_data [] = {
   -0.05,-0.15,0, // vertex 1
   0.05,-0.15,0, // vertex 2
   0.05, 0.15,0, // vertex 3

   0.05, 0.15,0, // vertex 3
   -0.05, 0.15,0, // vertex 4
   -0.05,-0.15,0  // vertex 1
 };

 static const GLfloat color_buffer_data [] = {
  0,1,0, // color 1
  0,1,0, // color 2
  0,1,0, // color 3

  0,1,0, // color 3
  0,1,0, // color 4
  0,1,0, // color 1

};
// create3DObject creates and returns a handle to a VAO that can be used later
int i;
for (i=0;i<6;i++)
block2[i] = create3DObject(GL_TRIANGLES, 6, vertex_buffer_data, color_buffer_data, GL_FILL);
}

void createBlock3 ()
{
  // GL3 accepts only Triangles. Quads are not supported
  static const GLfloat vertex_buffer_data [] = {
   -0.05,-0.15,0, // vertex 1
   0.05,-0.15,0, // vertex 2
   0.05, 0.15,0, // vertex 3

   0.05, 0.15,0, // vertex 3
   -0.05, 0.15,0, // vertex 4
   -0.05,-0.15,0  // vertex 1
 };

 static const GLfloat color_buffer_data [] = {
  0,0,0, // color 1
  0,0,0, // color 2
  0,0,0, // color 3

  0,0,0, // color 3
  0,0,0, // color 4
  0,0,0, // color 1

};
// create3DObject creates and returns a handle to a VAO that can be used later
int i;
for(i=0;i<6;i++)
block3[i] = create3DObject(GL_TRIANGLES, 6, vertex_buffer_data, color_buffer_data, GL_FILL);
}

void createLaser ()
{
  // GL3 accepts only Triangles. Quads are not supported
  static const GLfloat vertex_buffer_data [] = {
    0.8,-0.03,0, // vertex 1
    1.1,-0.03,0, // vertex 2
    1.1, 0.03,0, // vertex 3

    1.1, 0.03,0, // vertex 3
    0.8, 0.03,0, // vertex 4
    0.8,-0.03,0  // vertex 1
  };

  static const GLfloat color_buffer_data [] = {
    1,0,1, // color 1
    1,0,1, // color 2
    1,0,1, // color 3

    1,0,1, // color 3
    1,0,1, // color 4
    1,0,1, // color 1

  };
  // create3DObject creates and returns a handle to a VAO that can be used later
  laser = create3DObject(GL_TRIANGLES, 6, vertex_buffer_data, color_buffer_data, GL_FILL);
}
void createMirror1()
{
  static const GLfloat vertex_buffer_data [] = {
    -0.45,-0.1,0, // vertex 1
    0.45,-0.1,0, // vertex 2
    0.45, 0.1,0, // vertex 3

    0.45, 0.1,0, // vertex 3
    -0.45, 0.1,0, // vertex 4
    -0.45,-0.1,0  // vertex 1
  };

  static const GLfloat color_buffer_data [] = {
    0.25,0.25,0.25, // color 1
    0.25,0.25,0.25, // color 2
    0.25,0.25,0.25, // color 3

    0.25,0.25,0.25, // color 1
    0.25,0.25,0.25, // color 2
    0.25,0.25,0.25, // color 1

  };
  // create3DObject creates and returns a handle to a VAO that can be used later
  mirror1 = create3DObject(GL_TRIANGLES, 6, vertex_buffer_data, color_buffer_data, GL_FILL);
}
void createMirror2()
{
  static const GLfloat vertex_buffer_data [] = {
    -0.45,-0.1,0, // vertex 1
    0.45,-0.1,0, // vertex 2
    0.45, 0.1,0, // vertex 3

    0.45, 0.1,0, // vertex 3
    -0.45, 0.1,0, // vertex 4
    -0.45,-0.1,0  // vertex 1
  };

  static const GLfloat color_buffer_data [] = {
    0.25,0.25,0.25, // color 1
    0.25,0.25,0.25, // color 2
    0.25,0.25,0.25, // color 3

    0.25,0.25,0.25, // color 1
    0.25,0.25,0.25, // color 2
    0.25,0.25,0.25, // color 1

  };
  // create3DObject creates and returns a handle to a VAO that can be used later
  mirror2 = create3DObject(GL_TRIANGLES, 6, vertex_buffer_data, color_buffer_data, GL_FILL);
}

void GameOver()
{
  exit(0);
}

float camera_rotation_angle = 90;
float rectangle_rotation = 0;
float mirror1_rotation = 0;
float mirror2_rotation = 90;
float gun2_rotation = 0;
float laser_rotation = gun2_rotation;
float triangle_rotation = 0;
float red_move[6]; //= 4.2;
float green_move[6]; //= 4.2;
float black_move[6];// = 4.2;
float increase = 0.003;
float decrease = -0.003;
float speed = 0.01;
float red_pos[6], green_pos[6], black_pos[6];
float laser_xpos=-3.5;
float laser_ypos=gun_ypos;
int spc=0;
float r=0.3;
float red_ypos,green_ypos,black_ypos;
int points=0;
int flag=0;
int reflected=0;
/* Render the scene with openGL */
/* Edit this function according to your assignment */
void keyboard (GLFWwindow* window, int key, int scancode, int action, int mods);
void mouseButton (GLFWwindow* window, int button, int action, int mods);

void draw ()
{
  srand(time(NULL));
  /* FTGLPixmapFont font("/home/user/Arial.ttf");

  // If something went wrong, bail out.
  if(font.Error())
  return -1;

  // Set the font size and render a small text.
  font.FaceSize(72);
  font.Render("Hello World!");
  */
  // clear the color and depth in the frame buffer
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // use the loaded shader program
  // Don't change unless you know what you are doing
  glUseProgram (programID);

  // Eye - Location of camera. Don't change unless you are sure!!
  glm::vec3 eye ( 5*cos(camera_rotation_angle*M_PI/180.0f), 0, 5*sin(camera_rotation_angle*M_PI/180.0f) );
  // Target - Where is the camera looking at.  Don't change unless you are sure!!
  glm::vec3 target (0, 0, 0);
  // Up - Up vector defines tilt of camera.  Don't change unless you are sure!!
  glm::vec3 up (0, 1, 0);

  // Compute Camera matrix (view)
  // Matrices.view = glm::lookAt( eye, target, up ); // Rotating Camera for 3D
  //  Don't change unless you are sure!!
  Matrices.view = glm::lookAt(glm::vec3(0,0,3), glm::vec3(0,0,0), glm::vec3(0,1,0)); // Fixed camera for 2D (ortho) in XY plane

  // Compute ViewProject matrix as view/camera might not be changed for this frame (basic scenario)
  //  Don't change unless you are sure!!
  glm::mat4 VP = Matrices.projection * Matrices.view;

  // Send our transformation to the currently bound shader, in the "MVP" uniform
  // For each model you render, since the MVP will be different (at least the M part)
  //  Don't change unless you are sure!!
  glm::mat4 MVP;  // MVP = Projection * View * Model

  // Load identity to model matrix
  
  //Matrices.model = glm::mat4(1.0f);

  /* Render your scene */

  // Pop matrix to undo transformations till last push matrix instead of recomputing model matrix
  // glPopMatrix ();
  float dist=0.8;
  int j,k,l,countr=0,countg=0,countb=0,x;
  for (j=0;j<6;j++)
  {
    int i;
    red_ypos = red_move[j]+red_pos[j];
    Matrices.model = glm::mat4(1.0f);

    glm::mat4 translateBlock1 = glm::translate (glm::vec3(-0.75+(dist*j),red_ypos , 0));        // glTranslatef
    glm::mat4 rotateBlock1 = glm::rotate((float)(rectangle_rotation*M_PI/180.0f), glm::vec3(0,0,1)); // rotate about vector (-1,1,1)
    Matrices.model *= (translateBlock1 * rotateBlock1);
    MVP = VP * Matrices.model;
    glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
    // draw3DObject draws the VAO given to it using current MVP matrix 
    if(laser_ypos<(red_ypos+0.145) && laser_ypos>(red_ypos-0.145) && (laser_xpos+1.1*cos((laser_rotation*M_PI/180.0f)))>-0.75+(dist*j) && -0.75+(dist*j)>(laser_xpos+0.8*cos((laser_rotation*M_PI/180.0f))))
    {
      flag=1;
      laser_xpos=5;
      laser_ypos=-5;
      red_move[j]=-15;
    }
    if(red_ypos<-3.32 && red_ypos>-3.34 && (-0.75+(dist*j))>rect2_xpos-0.45 && (-0.75+(dist*j))<rect2_xpos+0.45 )
    {
      flag=2;
      red_move[j]=-15;
    }
    if (red_ypos<-4.2)
    {
      countr+=1;
    }
    if(countr==6)
    { 
      for(i=0;i<6;i++)
      red_move[i] = 4.2;
      for (x=0;x<6;x++)
      red_pos[x] = rand() % 10; 
      countr=0;
    }

    draw3DObject(block1[j]);
    
  }
  for(k=0;k<6;k++)
  {
    int i;
    green_ypos=green_move[k]+green_pos[k];
    Matrices.model = glm::mat4(1.0f);

    glm::mat4 translateBlock2 = glm::translate (glm::vec3(-0.5+(dist*k),green_ypos, 0));        // glTranslatef
    glm::mat4 rotateBlock2 = glm::rotate((float)(rectangle_rotation*M_PI/180.0f), glm::vec3(0,0,1)); // rotate about vector (-1,1,1)
    Matrices.model *= (translateBlock2 * rotateBlock2);
    MVP = VP * Matrices.model;
    glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
    //(laser_xpos+0.8)==(-1.5+(dist*k)) && 
    // draw3DObject draws the VAO given to it using current MVP matrix
    if(laser_ypos<(green_ypos+0.145) && laser_ypos>(green_ypos-0.145) && (laser_xpos+1.1*cos((laser_rotation*M_PI/180.0f)))>-0.5+(dist*k) && -0.5+(dist*k)>(laser_xpos+0.8*cos((laser_rotation*M_PI/180.0f))))
    {
      flag=1;
      laser_xpos=5;
      laser_ypos=-5;
      green_move[k]=-15;
    }
    if(green_ypos<-3.32 && green_ypos>-3.34 && (-0.5+(dist*k))>rect1_xpos-0.45 && (-0.5+(dist*k))<rect1_xpos+0.45 )
    {
      flag=2;
      green_move[k]=-15;
    }
    if(green_ypos<-4.2)
    {
      countg+=1;
    }
    if(countg==6)
    { 
      for(i=0;i<6;i++)
      green_move[i] = 4.2;
      for (x=0;x<6;x++)
      green_pos[x] = rand() % 10; 
      countg=0;
    }

    draw3DObject(block2[k]);
  }
  for(l=0;l<6;l++)
  {
    int i;
    black_ypos = black_move[l]+black_pos[l];
    Matrices.model = glm::mat4(1.0f);

    glm::mat4 translateBlock3 = glm::translate (glm::vec3(-1+(dist*l),black_ypos , 0));        // glTranslatef
    glm::mat4 rotateBlock3 = glm::rotate((float)(rectangle_rotation*M_PI/180.0f), glm::vec3(0,0,1)); // rotate about vector (-1,1,1)
    Matrices.model *= (translateBlock3 * rotateBlock3);
    MVP = VP * Matrices.model;
    glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);

    // draw3DObject draws the VAO given to it using current MVP matrix
    if(laser_ypos<(black_ypos+0.145) && laser_ypos>(black_ypos-0.145) && (laser_xpos+1.1*cos((laser_rotation*M_PI/180.0f)))>-1+(dist*l) && -1+(dist*l)>(laser_xpos+0.8*cos((laser_rotation*M_PI/180.0f))))
    {
      flag=2;
      laser_xpos=5;
      laser_ypos=-5;
      black_move[l]=-15;
    }
    if(black_ypos<-3.32 && black_ypos>-3.34 && (-1+(dist*l))>(rect2_xpos-0.45) && (-1+(dist*l))< (rect2_xpos+0.45) )
    {
      flag=1;
      black_move[l]=-15;
    }
    else if ( black_ypos<-3.32 && black_ypos>-3.34 && (-1+(dist*l))>(rect1_xpos-0.45) && (-1+(dist*l))< (rect1_xpos+0.45) )
    {
      flag=1;
      black_move[l]=-15;
    }
    if (black_ypos<-4.2)
    {
      countb+=1;
    }
    if(countb==6)
    { 
      for(i=0;i<6;i++)
      black_move[i] = 4.2;
      for (x=0;x<6;x++)
      black_pos[x] = rand() % 10; 
      countb=0;
    }
    draw3DObject(block3[l]);
  }
  Matrices.model = glm::mat4(1.0f);

  glm::mat4 translateRectangle1 = glm::translate (glm::vec3(rect1_xpos, -3.67, 0));        // glTranslatef
  glm::mat4 rotateRectangle1 = glm::rotate((float)(rectangle_rotation*M_PI/180.0f), glm::vec3(0,0,1)); // rotate about vector (-1,1,1)
  Matrices.model *= (translateRectangle1 * rotateRectangle1);
  MVP = VP * Matrices.model;
  glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);

  // draw3DObject draws the VAO given to it using current MVP matrix
  draw3DObject(rectangle1);

  Matrices.model = glm::mat4(1.0f);

  glm::mat4 translateRectangle2 = glm::translate (glm::vec3(rect2_xpos, -3.67, 0));        // glTranslatef
  glm::mat4 rotateRectangle2 = glm::rotate((float)(rectangle_rotation*M_PI/180.0f), glm::vec3(0,0,1)); // rotate about vector (-1,1,1)
  Matrices.model *= (translateRectangle2 * rotateRectangle2);
  MVP = VP * Matrices.model;
  glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);

  // draw3DObject draws the VAO given to it using current MVP matrix
  draw3DObject(rectangle2);
  
  Matrices.model = glm::mat4(1.0f);

  glm::mat4 translateLine = glm::translate (glm::vec3(0, -3.2, 0));        // glTranslatef
  glm::mat4 rotateLine = glm::rotate((float)(rectangle_rotation*M_PI/180.0f), glm::vec3(0,0,1)); // rotate about vector (-1,1,1)
  Matrices.model *= (translateLine * rotateLine);
  MVP = VP * Matrices.model;
  glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);

  // draw3DObject draws the VAO given to it using current MVP matrix
  draw3DObject(line);
  
  Matrices.model = glm::mat4(1.0f);

  glm::mat4 translateGun1 = glm::translate (glm::vec3(-3.65, gun_ypos, 0));        // glTranslatef
  glm::mat4 rotateGun1 = glm::rotate((float)(rectangle_rotation*M_PI/180.0f), glm::vec3(0,0,1)); // rotate about vector (-1,1,1)
  Matrices.model *= (translateGun1 * rotateGun1);
  MVP = VP * Matrices.model;
  glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);

  // draw3DObject draws the VAO given to it using current MVP matrix
  draw3DObject(gun1);
  
  Matrices.model = glm::mat4(1.0f);

  glm::mat4 translateGun2 = glm::translate (glm::vec3(-3.5, gun_ypos, 0));        // glTranslatef
  glm::mat4 rotateGun2 = glm::rotate((float)(gun2_rotation*M_PI/180.0f), glm::vec3(0,0,1)); // rotate about vector (-1,1,1)
  Matrices.model *= (translateGun2 * rotateGun2);
  MVP = VP * Matrices.model;
  glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);

  // draw3DObject draws the VAO given to it using current MVP matrix
  draw3DObject(gun2);

  if (reflected==0) 
  laser_rotation=gun2_rotation;

  Matrices.model = glm::mat4(1.0f);

  glm::mat4 translateLaser = glm::translate (glm::vec3(laser_xpos, laser_ypos, 0));        // glTranslatef
  glm::mat4 rotateLaser = glm::rotate((float)(laser_rotation*M_PI/180.0f), glm::vec3(0,0,1)); // rotate about vector (-1,1,1)
  Matrices.model *= (translateLaser * rotateLaser);
  MVP = VP * Matrices.model;
  glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);

  // draw3DObject draws the VAO given to it using current MVP matrix
  draw3DObject(laser);

  Matrices.model = glm::mat4(1.0f);

  glm::mat4 translateMirror1 = glm::translate (glm::vec3(0, 3, 0));        // glTranslatef
  glm::mat4 rotateMirror1 = glm::rotate((float)(mirror1_rotation*M_PI/180.0f), glm::vec3(0,0,1)); // rotate about vector (-1,1,1)
  Matrices.model *= (translateMirror1 * rotateMirror1);
  MVP = VP * Matrices.model;
  glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);

  // draw3DObject draws the VAO given to it using current MVP matrix
  draw3DObject(mirror1);

  Matrices.model = glm::mat4(1.0f);

  glm::mat4 translateMirror2 = glm::translate (glm::vec3(3.2, 0.5, 0));        // glTranslatef
  glm::mat4 rotateMirror2 = glm::rotate((float)(mirror2_rotation*M_PI/180.0f), glm::vec3(0,0,1)); // rotate about vector (-1,1,1)
  Matrices.model *= (translateMirror2 * rotateMirror2);
  MVP = VP * Matrices.model;
  glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);

  // draw3DObject draws the VAO given to it using current MVP matrix
  draw3DObject(mirror2);

  if (laser_ypos+1.1*sin((laser_rotation*M_PI/180.0f))>3 && laser_ypos+1.1*sin((laser_rotation*M_PI/180.0f))<3.2 && (laser_xpos+1.1*cos((laser_rotation*M_PI/180.0f)))<0.45 && (laser_xpos+1.1*cos((laser_rotation*M_PI/180.0f)))>-0.5)
  {
    reflected=1;
    laser_rotation=-laser_rotation;
    laser_ypos=3.1;
    laser_xpos=laser_xpos+1.1*cos((laser_rotation*M_PI/180.0f));
  }
  if (laser_ypos+1.1*sin((laser_rotation*M_PI/180.0f))>0.05 && laser_ypos+1.1*sin((laser_rotation*M_PI/180.0f))<0.95 && (laser_xpos+1.1*cos((laser_rotation*M_PI/180.0f)))<3.3 && (laser_xpos+1.1*cos((laser_rotation*M_PI/180.0f)))>3)
  {
    reflected=1;
    if(laser_rotation<0)
    laser_rotation-=(180+2*laser_rotation);
    else if(laser_rotation>=0)
    laser_rotation=180-laser_rotation;
    
    laser_xpos=3.2;
    laser_ypos=laser_ypos+1.1*sin((laser_rotation*M_PI/180.0f));
  }
  if(spc==1 || reflected==1)
  {   
    laser_xpos+=r*cos(laser_rotation*M_PI/180.0f);
    laser_ypos+=r*sin(laser_rotation*M_PI/180.0f);
  }
  int y;
  for(y=0;y<6;y++)
  {
    red_move[y] =  red_move[y] - speed;
    green_move[y] = green_move[y] - speed;
    black_move[y] = black_move[y] - speed;
  }
  if(laser_xpos>12 || laser_ypos>12 || laser_xpos<-12 || laser_ypos<-12)
  {
        laser_xpos=-3.5;
        laser_ypos=gun_ypos;
        spc=0;
        reflected=0;
  }
  if(flag==1)
  {
    points-=5;
    flag=0;
  }
  if (flag==2)
  {
    points+=10;
    flag=0;
  }
  printf("points: %d\n",points);
  if (points<-40)
  GameOver();
}

void keyboard (GLFWwindow* window, int key, int scancode, int action, int mods)
{
 int ctrl_status_left = glfwGetKey(window, GLFW_KEY_LEFT_CONTROL);
 int ctrl_status_right = glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL);
 int alt_status_left = glfwGetKey(window, GLFW_KEY_LEFT_ALT);
 int alt_status_right = glfwGetKey(window, GLFW_KEY_RIGHT_ALT);
 // Function is called first on GLFW_PRESS.
 if (action == GLFW_RELEASE) {
  switch (key) {
    case GLFW_KEY_LEFT:
    if (rect2_xpos>=-2.2 && (ctrl_status_right==GLFW_PRESS || ctrl_status_left==GLFW_PRESS) )
    {
      rect2_xpos -= 0.2;
      ctrl_status_right==GLFW_RELEASE;
      ctrl_status_left==GLFW_RELEASE;
    }
    else if (rect1_xpos>=-2.2 && (alt_status_right==GLFW_PRESS || alt_status_left==GLFW_PRESS) )
    {
      rect1_xpos -= 0.2;
      alt_status_right==GLFW_RELEASE;
      alt_status_left==GLFW_RELEASE; 
    }          
    break;
    case GLFW_KEY_RIGHT:
    if (rect2_xpos<=3.4 && (ctrl_status_right==GLFW_PRESS || ctrl_status_left==GLFW_PRESS) )
    {
      rect2_xpos += 0.2;
      ctrl_status_right==GLFW_RELEASE;
      ctrl_status_left==GLFW_RELEASE;
    }
    else if (rect1_xpos<=3.4 && (alt_status_right==GLFW_PRESS || alt_status_left==GLFW_PRESS) )
    { 
      rect1_xpos += 0.2;
      alt_status_right==GLFW_RELEASE;
      alt_status_left==GLFW_RELEASE; 
    }
    break;
    case GLFW_KEY_S:
    if (gun_ypos<=2.5)
    {
      gun_ypos += 0.2;
      laser_ypos=gun_ypos;
    }
    break;
    case GLFW_KEY_F:
    if (gun_ypos>=-1.4)
    {
      gun_ypos -= 0.2;
      laser_ypos=gun_ypos;
    }
    break;
    case GLFW_KEY_M:
    if (speed>0.004)
    speed = speed + decrease;
    break;
    case GLFW_KEY_N:
    if(speed<3)
    speed = speed + increase;
    break;
    case GLFW_KEY_SPACE:
    spc=1;
    break;
    default:
    break;
  }
}
if (action == GLFW_RELEASE && key == GLFW_KEY_A && gun2_rotation <= 55)
gun2_rotation = gun2_rotation + increments*gun2_rot_dir_pos;  
if (action == GLFW_RELEASE && key == GLFW_KEY_D && gun2_rotation >= -55)
gun2_rotation = gun2_rotation + increments*gun2_rot_dir_neg; 
else if (action == GLFW_PRESS) 
{
  switch (key) 
  {
    case GLFW_KEY_ESCAPE:
    quit(window);
    break;
    default:
    break;
  }
}
}

void mouseButton (GLFWwindow* window, int button, int action, int mods)
{
  switch (button) {
    case GLFW_MOUSE_BUTTON_LEFT:
    if (action == GLFW_RELEASE)
    spc=1;
    break;
    case GLFW_MOUSE_BUTTON_RIGHT:
    if (action == GLFW_RELEASE) {
      gun2_rot_dir_pos *= -1;
    }
    break;
    default:
    break;
  }
}
/* Initialise glfw window, I/O callbacks and the renderer to use */
/* Nothing to Edit here */
GLFWwindow* initGLFW (int width, int height)
{
  GLFWwindow* window; // window desciptor/handle

  glfwSetErrorCallback(error_callback);
  if (!glfwInit()) {
    //        exit(EXIT_FAILURE);
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  window = glfwCreateWindow(width, height, "Sample OpenGL 3.3 Application", NULL, NULL);

  if (!window) {
    glfwTerminate();
    //        exit(EXIT_FAILURE);
  }

  glfwMakeContextCurrent(window);
  gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
  glfwSwapInterval( 1 );

  /* --- register callbacks with GLFW --- */

  /* Register function to handle window resizes */
  /* With Retina display on Mac OS X GLFW's FramebufferSize
  is different from WindowSize */
  glfwSetFramebufferSizeCallback(window, reshapeWindow);
  glfwSetWindowSizeCallback(window, reshapeWindow);

  /* Register function to handle window close */
  glfwSetWindowCloseCallback(window, quit);

  /* Register function to handle keyboard input */
  glfwSetKeyCallback(window, keyboard);
  //glfwSetKeyCallback(window, key_callback);      // general keyboard input
  glfwSetCharCallback(window, keyboardChar);  // simpler specific character handling
  /* Register function to handle mouse click */
  glfwSetMouseButtonCallback(window, mouseButton);  // mouse button clicks

  return window;
}

/* Initialize the OpenGL rendering properties */
/* Add all the models to be created here */
void initGL (GLFWwindow* window, int width, int height)
{
  /* Objects should be created before any other gl function and shaders */
  // Create the models
  //createTriangle (); // Generate the VAO, VBOs, vertices data & copy into the array buffer
  createRectangle1 ();createRectangle2 ();
  createLine ();
  createGun1 ();createGun2 ();
  createBlock1 ();createBlock2 ();createBlock3 ();
  createLaser (); createMirror1(); createMirror2();
  //drawCircle(0,0,0,5,360);
  // Create and compile our GLSL program from the shaders
  programID = LoadShaders( "Sample_GL.vert", "Sample_GL.frag" );
  // Get a handle for our "MVP" uniform
  Matrices.MatrixID = glGetUniformLocation(programID, "MVP");


  reshapeWindow (window, width, height);

  // Background color of the scene
  glClearColor (0.7f, 0.7f, 0.7f, 0.0f); // R, G, B, A
  glClearDepth (1.0f);

  glEnable (GL_DEPTH_TEST);
  glDepthFunc (GL_LEQUAL);

  cout << "VENDOR: " << glGetString(GL_VENDOR) << endl;
  cout << "RENDERER: " << glGetString(GL_RENDERER) << endl;
  cout << "VERSION: " << glGetString(GL_VERSION) << endl;
  cout << "GLSL: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;
}
int main (int argc, char** argv)
{
	int width = 750;
	int height = 650;

  GLFWwindow* window = initGLFW(width, height);

  initGL (window, width, height);

  double last_update_time = glfwGetTime(), current_time;
  srand(time(NULL));
  int x,y;
  for (x=0;x<6;x++)
  {
    red_pos[x] = rand() % 10;
    green_pos[x] = rand() % 10; 
    black_pos[x] = rand() %10;
  }
  for(y=0;y<6;y++)
  {
    red_move[y]=4.2;
    green_move[y]=4.2;
    black_move[y]=4.2;
  }
  
  /* Draw in loop */
  while (!glfwWindowShouldClose(window)) {

    // OpenGL Draw commands
    draw();

    // Swap Frame Buffer in double buffering
    glfwSwapBuffers(window);

    // Poll for Keyboard and mouse events
    glfwPollEvents();

    // Control based on time (Time based transformation like 5 degrees rotation every 0.5s)
    current_time = glfwGetTime(); // Time in seconds
    if ((current_time - last_update_time) >= 0.5) { // atleast 0.5s elapsed since last frame
      // do something every 0.5 seconds ..
      last_update_time = current_time;
    }     
  }

  glfwTerminate();
  //    exit(EXIT_SUCCESS);
}
