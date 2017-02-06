// Starter program for HW 1. 
// Angel.h is homegrown include file that also includes glew and freeglut

#include <stdlib.h>
#include <time.h>
#include "Angel.h"

//----------------------------------------------------------------------------

// remember to prototype
void initGPUBuffers( void );
void shaderSetup( void );
void display( void );
void keyboard( unsigned char key, int x, int y );
void tEventHandler( void );
void drawPolylineFile(char *);
void setOrtho(GLfloat ,GLfloat ,GLfloat ,GLfloat );
void drawMode(int , int , int , int);
void myMouse(int , int , int , int );
void gingerbreadMan( void );
void myReshape(int , int  );
void glViewportMain(int , int );
void fernFractal(void);

typedef vec2 point2;

using namespace std;
//windows Size
static int width = 640;
static int height = 480;

//flags and indexs for Drawing Event
static int isBKeyPressed = 0;
static int hasPrepoint = 0;
static int polylineIndex = 0;
static int pointIndex = 0;

// Array for polyline
point2 points[5000];
static point2 pointsForDrawMode[2];
static point2 pointsForPolyline[100][100];
static int isIndrawMode = 0;
static char currentKeyboardEvent = 'h';

static int countOfPointsForPolyline[100] = { 0 };

//store the origin size when e event happend
static int originWidthForE = 0;
static int originHeightForE = 0;
// the following two are not used for this time
static int lastWidthForE = 0;
static int lastHeightForE = 0;

// store the random picked file index, used for reshape
static int currentDatInMainScreen;
//ratio is not used this time
static float ratio = 0; 

GLuint program;
GLuint ProjLoc;

//get the color
GLint vertexColorLoaction;
static int currentColor = 0;

//all file in one list for randome and traverse
static char files[10][20] = {
	"dino.dat", 
	"birdhead.dat", 
	"dragon.dat", 
	"house.dat", 
	"knight.dat", 
	"rex.dat", 
	"scene.dat", 
	"usa.dat", 
	"vinci.dat",
	"dino.dat"
};

void initGPUBuffers( void )
{
	// Create a vertex array object
    GLuint vao;
    glGenVertexArrays( 1, &vao );
    glBindVertexArray( vao );

    // Create and initialize a buffer object
    GLuint buffer;
    glGenBuffers( 1, &buffer );
    glBindBuffer( GL_ARRAY_BUFFER, buffer );
    glBufferData( GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW );
}

void setOrtho(GLfloat left,GLfloat right,GLfloat bottom,GLfloat top)
{
	mat4 ortho = Ortho2D( left, right, bottom, top );
	ProjLoc = glGetUniformLocation(program, "Proj");
    glUniformMatrix4fv( ProjLoc, 1, GL_FALSE, ortho );
}

void drawPolylineFile(char *files)
{
	char line[256];
	GLfloat L, T, R, B;
	FILE *inStream;
	GLint countOfPolylines, countOfPointsForPolyline; 
	GLfloat	x, y;

	if ((inStream = fopen(files, "rt")) == NULL) return;
	fscanf(inStream, "%f %f %f %f\n", &L, &T, &R, &B);
	// read point count for each polyline
	fscanf(inStream,"%d", &countOfPolylines);			
	for(int j = 0; j < countOfPolylines; j++)
	{	//read each polyline
		fscanf(inStream,"%d", &countOfPointsForPolyline);
		for (int i = 0; i < countOfPointsForPolyline; i++)
		{
			fscanf(inStream,"%f %f", &x, &y);
			points[i] = point2( x , y);
		}
		setOrtho(L, R, B, T);
		glBufferData( GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW );
		glDrawArrays( GL_LINE_STRIP, 0, countOfPointsForPolyline ); 
		glFlush();
	}
	fclose(inStream);
}

void shaderSetup( void )
{
	// Load shaders and use the resulting shader program
    program = InitShader( "vshader1.glsl", "fshader1.glsl" );
	
    glUseProgram( program );

    // Initialize the vertex position attribute from the vertex shader
    GLuint loc = glGetAttribLocation( program, "vPosition" );
    glEnableVertexAttribArray( loc );
    glVertexAttribPointer( loc, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );

	// sets the default color to clear screen
    glClearColor( 1.0, 1.0, 1.0, 1.0 ); // white background
	GLint vertexColorLoaction = glGetUniformLocation(program, "myColor");
	glUniform4f(vertexColorLoaction, 1.0f, 0.0f, 0.0f, 1.0f);
}

void glViewportMain(int w, int h)
{
	if(width/(height-0.075*width) > 1.33)
	{
		glViewport((width-1.067*(height-0.08*width))/2, 0 , 1.067*(height-0.075*width) , height-0.08*width);
	}
	else
	{
		glViewport(0, (height-0.825*width)/2 , width , 0.8*width);
	}
}

void display( void )
{
	switch (currentColor)
	{
	case 1:
		vertexColorLoaction = glGetUniformLocation(program, "myColor");
		glUniform4f(vertexColorLoaction, 0.0f, 1.0f, 0.0f, 1.0f);
		break;
	case 2:
		vertexColorLoaction = glGetUniformLocation(program, "myColor");
		glUniform4f(vertexColorLoaction, 0.0f, 0.0f, 1.0f, 1.0f);
		break;
	default:
		vertexColorLoaction = glGetUniformLocation(program, "myColor");
		glUniform4f(vertexColorLoaction, 1.0f, 0.0f, 0.0f, 1.0f);
		currentColor = 0;
		break;
	}

	for(int i = 0; i < 10; i++)
	{
		glViewport(width/10*i,height-0.08*width,width/10 ,0.08*width);
		drawPolylineFile(files[i]);
	}
}

void tEventHandler( void )
{
	int randNum;
	srand(time(NULL));
	int randomArray[5][5] = { 0 };
	//generate random
	for(int y = 0; y < 5; y++)
	{
		for(int x = 0; x < 5; x++)
		{
			randNum = rand() % 10;
			randomArray[y][x] = randNum;
		}
	}

	for (int y = 0; y < 5; y++)
	{
		for (int x = 0; x < 5; x++)
		{
			int w = width / 5;
			int h = (height - 0.075*width) / 5;
			int leftOffset = (width - 5 * w) / 2;
			int topOffset = (height - 5 * h) / 2;
			glViewport(leftOffset + w*x, h*y, w, h);
			drawPolylineFile(files[randomArray[y][x]]);
		}
	}
}

void keyboard( unsigned char key, int x, int y )
{
	isBKeyPressed = 0; // Just in case
    switch ( key ) 
	{
		case 'h':
			currentKeyboardEvent = 'h';
			glClear( GL_COLOR_BUFFER_BIT );
			display();
			isIndrawMode = 0;
			break;
		
		case 'p':
			currentKeyboardEvent = 'p';
			srand(time(NULL));
			//generate a random number to select a random file
			currentDatInMainScreen = rand()%10;
			glClear( GL_COLOR_BUFFER_BIT );
			display();
			glViewportMain(width, height);
			
			drawPolylineFile(files[currentDatInMainScreen]);
			isIndrawMode = 0;
			glutMouseFunc(myMouse);
			break;

		case 't':
			currentKeyboardEvent = 't';
			glClear( GL_COLOR_BUFFER_BIT );
			display();
			tEventHandler();
			isIndrawMode = 0;
			break;
		case 'e':
			currentKeyboardEvent = 'e';
			glClear(GL_COLOR_BUFFER_BIT); // clear the window
			display();
			isIndrawMode = 1;
			polylineIndex = 0;
			isBKeyPressed = 0;
			hasPrepoint = 0;
			// Begin of drawing mode
			glutMouseFunc(drawMode); //After this, you must reset: glutMouseFunc(myMouse);
			// End of drawing mode
			break;
		case 'g':
			currentKeyboardEvent = 'g';
			glClear(GL_COLOR_BUFFER_BIT); // clear the window
			display();
			gingerbreadMan();
			isIndrawMode = 0;
			break;
		case 'f':
			currentKeyboardEvent = 'f';
			glClear(GL_COLOR_BUFFER_BIT); // clear the window
			display();
			fernFractal();
			isIndrawMode = 0;
			break;
		case 'b':
			currentKeyboardEvent = 'e';
			if(isIndrawMode == 1)
			{
				//a issue in E keyboard
				isBKeyPressed = 1;
				glutMouseFunc(drawMode);
			}
			break;
		case 'c':
			switch (currentColor)
			{
			case 0:
				vertexColorLoaction = glGetUniformLocation(program, "myColor");
				glUniform4f(vertexColorLoaction, 0.0f, 1.0f, 0.0f, 1.0f);
				currentColor = 1;
				break;
			case 1:
				vertexColorLoaction = glGetUniformLocation(program, "myColor");
				glUniform4f(vertexColorLoaction, 0.0f, 0.0f, 1.0f, 1.0f);
				currentColor = 2;
				break;
			case 2:
				vertexColorLoaction = glGetUniformLocation(program, "myColor");
				glUniform4f(vertexColorLoaction, 1.0f, 0.0f, 0.0f, 1.0f);
				currentColor = 0;
				break;
			default:
				vertexColorLoaction = glGetUniformLocation(program, "myColor");
				glUniform4f(vertexColorLoaction, 1.0f, 0.0f, 0.0f, 1.0f);
				currentColor = 0;
				break;
			}
			myReshape(width, height);
			display();
			break;
		case 033:
			exit( EXIT_SUCCESS );
			break;
		default:
			isBKeyPressed = 0;
			isIndrawMode = 0;
    }
}

void fernFractal( void )
{
	//give credits to poeple.cs.fsu.edu
	double prob[4] = { 0.85, 0.92, 0.99, 1.00 };
	point2 pointsFern[50000];
	double r;
	double x;
	double y;
	//setup random
	int randNum;
	srand(time(NULL));
	//clear window and prepare the menu
	glClear(GL_COLOR_BUFFER_BIT);
	display();

	pointsFern[0].x = width / 2;
	pointsFern[0].y = 0;

	for (int i = 1; i < 50000; i++) {
		/*r = rand() % 100 / 100;
		if (r < prob[0])
		{
			x = 0.85 * pointsFern[i - 1].x + 0.04 * pointsFern[i - 1].y + 0.0;
			y = -0.04 * pointsFern[i - 1].x + 0.85 * pointsFern[i - 1].y + 1.6;
		}
		else if (r < prob[1])
		{
			x = 0.20 * pointsFern[i - 1].x - 0.26 * pointsFern[i - 1].y + 0.0;
			y = 0.23 * pointsFern[i - 1].x + 0.22 * pointsFern[i - 1].y + 1.6;
		}
		else if (r < prob[2])
		{
			x = -0.15 * pointsFern[i - 1].x + 0.28 * pointsFern[i - 1].y + 0.0;
			y = 0.26 * pointsFern[i - 1].x + 0.24 * pointsFern[i - 1].y + 0.44;
		}
		else
		{
			x = 0.00 * pointsFern[i - 1].x + 0.00 * pointsFern[i - 1].y + 0.0;
			y = 0.00 * pointsFern[i - 1].x + 0.16 * pointsFern[i - 1].y + 0.0;
		}*/

		pointsFern[i].x = pointsFern[i - 1].x * 0.85 + pointsFern[i - 1].y * 0.04;
		pointsFern[i].y = pointsFern[i - 1].x * (-0.04) + pointsFern[i - 1].y * 0.85 + 1.6;
	}
	glViewport(0, 0, width, height);
	setOrtho(0, 640, 0, 480);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pointsFern), pointsFern, GL_STATIC_DRAW);
	glDrawArrays(GL_POINTS, 0, 50000);
	glFlush();
}

void gingerbreadMan( void )
{
	point2 p = point2(115, 121);
	for(int j = 0; j < 1000; j++)
	{
		for(int i = 0; i < 5000; i++)
		{
			points[i].x = 40*(1+2*3)-p.y+abs(p.x-3*40);
			points[i].y = p.x;
		
			p = points[i];
		}

		glViewport(0, 0 , width , height);
		setOrtho(0, 640, 0, 480);
		glBufferData( GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW );
		glDrawArrays( GL_POINTS, 0, 5000 ); 
		glFlush();
	}

}

void drawMode(int button, int state, int x, int y)
{
	switch ( button ) 
	{
		case GLUT_LEFT_BUTTON:
			if(state == GLUT_DOWN) 
			{
				if(hasPrepoint == 0 || isBKeyPressed == 1)
				{
					if(hasPrepoint == 0)
					{
						originWidthForE = width;
						originHeightForE = height;
					}
					if(width/height > originWidthForE/originHeightForE)
					{
						ratio = 1.0 * originHeightForE / height;
					}
					else
					{
						ratio = 1.0 * originWidthForE / width;
					}
					pointsForDrawMode[1] = point2( 1.0*x*originWidthForE/width , 1.0*originHeightForE - 1.0*y*originHeightForE/height);
					if(hasPrepoint != 0) 
					{
						polylineIndex++;
					}
					hasPrepoint = 1; 
					pointIndex = 0;
					
					pointsForPolyline[polylineIndex][pointIndex] = point2( 1.0*x*originWidthForE/width , 1.0*originHeightForE - 1.0*y*originHeightForE/height);
					pointIndex++;
					lastWidthForE = width;
					lastHeightForE = height;
				}
				else
				{
					if(width/height > originWidthForE/originHeightForE)
					{
						ratio = 1.0 * originHeightForE / height;
					}
					else
					{
						ratio = 1.0 * originWidthForE / width;
					}
					pointsForPolyline[polylineIndex][pointIndex] = point2( 1.0*x*originWidthForE/width , 1.0*originHeightForE - 1.0*y*originHeightForE/height); //point2( x*ratio , originHeightForE - y*ratio); //point2( x , height - y);
					pointIndex++;

					countOfPointsForPolyline[polylineIndex] = pointIndex;

					pointsForDrawMode[0] = pointsForDrawMode[1];

					pointsForDrawMode[1] = point2( 1.0*x*originWidthForE/width , 1.0*originHeightForE - 1.0*y*originHeightForE/height); //point2( x , height - y); //point2( x*ratio , (height - y)*ratio);  
					
					glViewport(0, 0 , width , height);

					setOrtho(0, originWidthForE, 0, originHeightForE);
					glBufferData( GL_ARRAY_BUFFER, sizeof(pointsForDrawMode), pointsForDrawMode, GL_STATIC_DRAW );
					glDrawArrays( GL_LINE_STRIP, 0, 2 ); 
					glFlush();
					lastWidthForE = width;
					lastHeightForE = height;
				}
				isBKeyPressed = 0;
			}
			break;
	}
}

void myMouse(int button, int state, int x, int y)
{
	int index = 0;
	currentKeyboardEvent = 'p';
	switch ( button ) 
	{
		case GLUT_LEFT_BUTTON:
			// determine which thumbnail is tapped on adn show it on main screen
			if(state == GLUT_DOWN && y <= 0.075*width) 
			{
				index = 10*x/width;
				currentDatInMainScreen = index; // for reshape use
				glClear( GL_COLOR_BUFFER_BIT );
				display();
				glViewportMain(width, height);
				drawPolylineFile(files[index]);
			};
			break;
    }
}

void myReshape(int reshapeWidth, int reshapeHeight )
{
	width = reshapeWidth;
	height = reshapeHeight;
	switch(currentKeyboardEvent)
	{
		case 'p':
			glClear( GL_COLOR_BUFFER_BIT );
			display();
			glViewportMain(width, height);
			drawPolylineFile(files[currentDatInMainScreen]);
			break;

		case 't':
			glClear( GL_COLOR_BUFFER_BIT );
			display();
			glViewportMain(width, height);
			tEventHandler();
			break;
		case 'g':
			//glClear( GL_COLOR_BUFFER_BIT );
			gingerbreadMan();
			break;
		case 'f' :
			fernFractal();
			break;
		case 'e':
			isIndrawMode = 1;
			glClear( GL_COLOR_BUFFER_BIT );
			display();
			//glViewport(0, 0 , width , height);
			int tempWidth, tempHeight;
			if(width/height > originWidthForE/originHeightForE)
			{
				tempWidth = height * originWidthForE/originHeightForE;
				tempHeight = height;
			}
			else
			{
				tempWidth = width;
				tempHeight = width * originHeightForE/originWidthForE ;
			}
			//maintain correct aspect ratio
			glViewport(0, 0 , tempWidth , tempHeight);
			setOrtho(0, originWidthForE, 0, originHeightForE);
			for(int i =0; i <= polylineIndex; i++)
			{
				glBufferData( GL_ARRAY_BUFFER, sizeof(pointsForPolyline[0]), pointsForPolyline[i], GL_STATIC_DRAW );
				glDrawArrays( GL_LINE_STRIP, 0, countOfPointsForPolyline[i] ); 
				glFlush();
			}
			//glutMouseFunc(drawMode);
			break;

		default:
			glutDisplayFunc( display );
			currentDatInMainScreen = 8; // for reshape use
			glClear(GL_COLOR_BUFFER_BIT);
			glViewportMain(width, height);
			drawPolylineFile(files[8]);
			glutKeyboardFunc( keyboard );
			glutMouseFunc(myMouse);
	}
	
}
//----------------------------------------------------------------------------
// entry point
int main( int argc, char **argv )
{
	// init glut
    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_RGBA | GLUT_SINGLE );
    glutInitWindowSize( width, height );
    glutCreateWindow( "Jarway's POLIBOOK" );
	// init glew
    glewInit();
    initGPUBuffers( );
    shaderSetup( );
	// assign default handlers
    glutDisplayFunc( display );
    glutKeyboardFunc( keyboard );
	glutMouseFunc(myMouse);
	glutReshapeFunc(myReshape);

    glutMainLoop();
    return 0;
}
