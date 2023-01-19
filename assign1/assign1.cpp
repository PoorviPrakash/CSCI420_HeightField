/*
  CSCI 420 Computer Graphics
  Assignment 1: Height Fields
  Poorvi Burly Prakash
*/

// Including the relevant libraries
#include <stdlib.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#include <pic.h>
#include<iostream>

using namespace std;
int g_iMenuId;

int g_vMousePos[2] = {0, 0};
int g_iLeftMouseButton = 0;    /* 1 if pressed, 0 if not */
int g_iMiddleMouseButton = 0;
int g_iRightMouseButton = 0;

typedef enum { ROTATE, TRANSLATE, SCALE } CONTROLSTATE;

CONTROLSTATE g_ControlState = ROTATE;

/* state of the world */
float g_vLandRotate[3] = {0.0, 0.0, 0.0};
float g_vLandTranslate[3] = {0.0, 0.0, 0.0};
float g_vLandScale[3] = {1.0, 1.0, 1.0};

/* see <your pic directory>/pic.h for type Pic */
Pic * g_pHeightData;

/* Image Details Variables */
int imageH,imageW, imageHW, imageHH, numVertices;

// Boolean Flags to for mode tracking
bool colourImage = FALSE, pinkColourImage = FALSE, inputColor = FALSE;

//Flag for rendering a Solid Wireframe
bool wireSolid = FALSE;

/* Write a screenshot to the specified filename */
void saveScreenshot (char *filename)
{
  int i, j;
  Pic *in = NULL;

  if (filename == NULL)
    return;

  /* Allocate a picture buffer */
  in = pic_alloc(640, 480, 3, NULL);

  printf("File to save to: %s\n", filename);

  for (i=479; i>=0; i--) {
    glReadPixels(0, 479-i, 640, 1, GL_RGB, GL_UNSIGNED_BYTE,
                 &in->pix[i*in->nx*in->bpp]);
  }

  if (jpeg_write(filename, in))
    printf("File saved Successfully\n");
  else
    printf("Error in Saving\n");

  pic_free(in);
}

// Set image details
void setImageDetails(){
    imageH = g_pHeightData->ny;
    imageW = g_pHeightData->nx;
    imageHW = g_pHeightData->nx/2;
    imageHH = g_pHeightData->ny/2;
    numVertices = imageH * imageW;
}

//Function to get heightmap for a grayscale image
void displayImageHeightMap()
{
  setImageDetails();
  float heightValTop, heightValBtm;
  float xScaleFactor = imageHW*0.7;
  float zScaleFactor = imageHH*0.7;
  float yScaleFactor = 500.0f;
  for(int i=0;i<imageH-1;i++){ 
      //To generate Triangle strips 
      glBegin(GL_TRIANGLE_STRIP);
      for(int j=0;j<imageW;j++){
          //Top vertex height
          heightValTop = PIC_PIXEL(g_pHeightData,j,i,0)/255.0; //Dividing by 255 as pic_pixel gives output in range 0-255 and we need value between 0-1 for glcolour.
          //Bottom vertex height
          heightValBtm = PIC_PIXEL(g_pHeightData,j,i+1,0)/255.0; //Dividing by 255 as pic_pixel gives output in range 0-255 and we need value between 0-1 for glcolour.
          if(colourImage){
            //rendering vertex 1 in blue hue
            pinkColourImage = FALSE;
            glColor4f(heightValTop*0.3, heightValTop*0.8, heightValTop*0.9,1); //Setting ratios for blue hue using vertex height value
          }else if(pinkColourImage){
            //rendering vertex 1 in pink hue
            colourImage = FALSE;
            glColor4f(heightValTop*0.6, heightValTop*0.1, heightValTop*0.4,1); //Setting ratios for pink hue using vertex height value
          }
          else{
            //rendering vertex in black and white gradient
            glColor3f(heightValTop, heightValTop, heightValTop);
          }
          //creating a vertex 1 with the z-axis corresponding to the height.
          glVertex3f((float)(j-imageHH)*0.003,(float)(i-imageHH)*0.003, heightValTop*0.2);

          if(colourImage){
            //rendering vertex 2 in blue hue
            pinkColourImage = FALSE;
            glColor4f(heightValBtm*0.3, heightValBtm*0.8, heightValBtm*0.9,1);
          }else if(pinkColourImage){
            //rendering vertex 2 in pink hue
            colourImage = FALSE;
            glColor4f(heightValBtm*0.6, heightValBtm*0.1, heightValBtm*0.4,1);
          }else{
            //rendering vertex in black and white gradient
            glColor3f(heightValBtm, heightValBtm, heightValBtm);
          }
           //creating a vertex 2 with the z-axis corresponding to the height.
          glVertex3f((float)(j-imageHH)*0.003,(float)(i+1-imageHH)*0.003, heightValBtm*0.2); 
      }
    glEnd();
  }
}

//Extra Credit - Function to get the height map for a coloured image
void displayColouredImage()
{
  setImageDetails();
  float hTR, hTG,hTB,hBR,hBG,hBB,grayTopHeight,grayBtmHeight;
  float xScaleFactor = imageHW*0.7;
  float zScaleFactor = imageHH*0.7;
  float yScaleFactor = 500.0f;
  for(int i=0;i<imageH-1;i++){ 
      glBegin(GL_TRIANGLE_STRIP);
      for(int j=0;j<imageW;j++){
          //Getting the pixel density for channels 0(red), 1(green) and 2(blue) respectively for top vertex
          hTR = PIC_PIXEL(g_pHeightData,j,i,0)/255.0;
          hTG = PIC_PIXEL(g_pHeightData,j,i,1)/255.0;
          hTB = PIC_PIXEL(g_pHeightData,j,i,2)/255.0;
          //Getting the pixel density for channels 0(red), 1(green) and 2(blue) respectively for bottom vertex
          hBR = PIC_PIXEL(g_pHeightData,j,i+1,0)/255.0;
          hBG = PIC_PIXEL(g_pHeightData,j,i,1)/255.0;
          hBB = PIC_PIXEL(g_pHeightData,j,i,2)/255.0;
          //Using the formula to convert individual RGB height values to a equivalent weighted grayscale height value to pass the the vertex height
          grayTopHeight = 0.299*hTR + 0.587*hTG + 0.114*hTB;
          grayBtmHeight = 0.299*hBR + 0.587*hBG + 0.114*hBB;
          if(colourImage){
            pinkColourImage = FALSE;
            glColor4f(hTR*0.3, hTG*0.8, hTB*0.9,1);
          }else if(pinkColourImage){
            colourImage = FALSE;
            glColor4f(hTR*0.6, hTG*0.1, hTB*0.4,1);
          }
          else{
            glColor3f(hTR, hTG, hTB);
          }
          glVertex3f((float)(j-imageHH)*0.003,(float)(i-imageHH)*0.003, grayTopHeight*0.2);

          if(colourImage){
            pinkColourImage = FALSE;
            glColor4f(hBR*0.3, hBG*0.8, hBB*0.9,1);
          }else if(pinkColourImage){
            colourImage = FALSE;
            glColor4f(hBR*0.6, hBG*0.1, hBB*0.4,1);
          }else{
            glColor3f(hBR, hBG, hBB);
          }
          glVertex3f((float)(j-imageHH)*0.003,(float)(i+1-imageHH)*0.003, grayBtmHeight*0.2); 
      }
    glEnd();
  }
}

//Extra Credit - Function to render wireframe on top of solid triangles  
void displayWireframeFill()
{
  //Rendering image as solid fill first
  glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
  //Enabling Polygon Offset to avoid z-buffer fighting
  glEnable(GL_POLYGON_OFFSET_FILL);
  //Setting Polygon Offset values
  glPolygonOffset(1,1);
  //If it is a coloured image render a coloured solid fill image else a grayscale solid fill image
  if(inputColor){
    displayColouredImage();
  }else{
    displayImageHeightMap();
  }
  //Rendering image as lines for wireframe
  glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
  glDisable(GL_POLYGON_OFFSET_FILL);
  //If it is a coloured image render a coloured wireframe else a grayscale wireframe
  if(inputColor){
    displayColouredImage();
  }else{
    displayImageHeightMap();
  }
}

void myinit()
{
  // set background color
    glClearColor(0.0, 0.0, 0.0, 0.0);   
    //enable depth buffering
    glEnable(GL_DEPTH_TEST); 
    // interpolate colors during rasterization
    glShadeModel(GL_SMOOTH);            
}

void display()
{
  //clear buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  //Setting to projection mode to adjust perspective
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(-45.0,(float)640/480,0.01f,1000.0f);
  //Setting to model view mode to adjust perspective
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  //Adjusting the viewing matrix from an eye point, a reference point indicating the center of the scene, and an UP vector 
  gluLookAt(0, 2.6, 2.2, 0, 0, -1, 0, 1, 0);
  //Handling Rotation, Translation and scaling 
  glTranslatef(g_vLandTranslate[0],g_vLandTranslate[1],g_vLandTranslate[2]);
  glRotatef(g_vLandRotate[0],1,0,0);
  glRotatef(g_vLandRotate[1],0,1,0);
  glRotatef(g_vLandRotate[2],0,1,0);
  glScalef(-g_vLandScale[0],-g_vLandScale[1],g_vLandScale[2]);
  //Checking if the input image is coloured or grayscale using bpp value 
  if(g_pHeightData->bpp==3){
    inputColor=TRUE;
    if(wireSolid){
      displayWireframeFill();
    }else{
      displayColouredImage();
    }
  }else{
    if(wireSolid){
      displayWireframeFill();
    }else{
    displayImageHeightMap();
    }
  }
  //swap buffers
  glutSwapBuffers();
}

void menufunc(int value)
{
  switch (value)
  {
    case 0:
      exit(0);
      break;
  }
}

//Handling the different rendering modes using keyboard input
void keyBoardControls(unsigned char key, int x, int y){
  key = tolower(key); 
  switch (key)
  {
  case '1':
    glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
    wireSolid = FALSE;
    break;
  
  case '2':
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    wireSolid = FALSE;
    break;

  case '3':
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    wireSolid = FALSE;
    break;

  case '4':
    pinkColourImage = FALSE;
    colourImage = !colourImage;
    break;

  case '5':
    colourImage = FALSE;
    pinkColourImage = !pinkColourImage;
    break;  

  case '6':
    wireSolid = !wireSolid;
    break;  

  case ('t'):
    g_ControlState = TRANSLATE;
    break;
  }
}

float scaleMax = 1.5, scaleMin = 0, delta = 0.005f, checker = scaleMin;
void doIdle()
{
  //Scaling up and scaling down the image by delta value
  if(checker<=scaleMax && checker>=scaleMin){
    g_vLandScale[0]= g_vLandScale[0]  + delta;
    g_vLandScale[1]= g_vLandScale[1] + delta;
    checker += delta;
  }else if(checker>scaleMax || checker<scaleMin){
    delta = -delta;
    checker += delta;
  }
  /* make the screen update */
  glutPostRedisplay();
}

/* converts mouse drags into information about 
rotation/translation/scaling */
void mousedrag(int x, int y)
{
  int vMouseDelta[2] = {x-g_vMousePos[0], y-g_vMousePos[1]};
  
  switch (g_ControlState)
  {
    case TRANSLATE:  
      if (g_iLeftMouseButton)
      {
        g_vLandTranslate[0] += vMouseDelta[0]*0.01;
        g_vLandTranslate[1] -= vMouseDelta[1]*0.01;
      }
      if (g_iMiddleMouseButton)
      {
        g_vLandTranslate[2] += vMouseDelta[1]*0.01;
      }
      break;
    case ROTATE:
      if (g_iLeftMouseButton)
      {
        g_vLandRotate[0] += vMouseDelta[1];
        g_vLandRotate[1] += vMouseDelta[0];
      }
      if (g_iMiddleMouseButton)
      {
        g_vLandRotate[2] += vMouseDelta[1];
      }
      break;
    case SCALE:
      if (g_iLeftMouseButton)
      {
        g_vLandScale[0] *= 1.0+vMouseDelta[0]*0.01;
        g_vLandScale[1] *= 1.0-vMouseDelta[1]*0.01;
      }
      if (g_iMiddleMouseButton)
      {
        g_vLandScale[2] *= 1.0-vMouseDelta[1]*0.01;
      }
      break;
  }
  g_vMousePos[0] = x;
  g_vMousePos[1] = y;
}

void mouseidle(int x, int y)
{
  g_vMousePos[0] = x;
  g_vMousePos[1] = y;
}

void mousebutton(int button, int state, int x, int y)
{

  switch (button)
  {
    case GLUT_LEFT_BUTTON:
      g_iLeftMouseButton = (state==GLUT_DOWN);
      break;
    case GLUT_MIDDLE_BUTTON:
      g_iMiddleMouseButton = (state==GLUT_DOWN);
      break;
    case GLUT_RIGHT_BUTTON:
      g_iRightMouseButton = (state==GLUT_DOWN);
      break;
  }
 
  switch(glutGetModifiers())
  {
    case GLUT_ACTIVE_SHIFT:
      g_ControlState = SCALE;
      break;
    default:
      g_ControlState = ROTATE;
      break;
  }

  g_vMousePos[0] = x;
  g_vMousePos[1] = y;
}

int main (int argc, char ** argv)
{
  if (argc<2)
  {  
    printf ("usage: %s heightfield.jpg\n", argv[0]);
    exit(1);
  }

  g_pHeightData = jpeg_read(argv[1], NULL);
  if (!g_pHeightData)
  {
    printf ("error reading %s.\n", argv[1]);
    exit(1);
  }
// initialize GLUT
  glutInit(&argc, argv);
  
  // request double buffer
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGBA);
  
  // set window size
  glutInitWindowSize(640, 480);
  
  // set window position
  glutInitWindowPosition(0, 0);
  
  // creates a window
  glutCreateWindow("CSCI Assignment 1");

  /* do initialization */
  myinit();

  /* tells glut to use a particular display function to redraw */
  glutDisplayFunc(display);
  
  /* allow the user to quit using the right mouse button menu */
  g_iMenuId = glutCreateMenu(menufunc);
  glutSetMenu(g_iMenuId);
  glutAddMenuEntry("Quit",0);
  glutAttachMenu(GLUT_RIGHT_BUTTON);
  
  /* replace with any animate code */
  glutIdleFunc(doIdle);

  /* callback for mouse drags */
  glutMotionFunc(mousedrag);
  /* callback for idle mouse movement */
  glutPassiveMotionFunc(mouseidle);
  /* callback for mouse button changes */
  glutMouseFunc(mousebutton);

  /* callback for key press */
  glutKeyboardFunc(keyBoardControls);

  glutMainLoop();
  return(0);
}
