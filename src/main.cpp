#include <mutex>
#include <thread>

#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <filesystem>

#include <conio.h>

#include <cxxopts.hpp>

#include <common.h>

#include <vertexBuffer.h>

#include <trackball.h>

#ifdef _WIN32
#include <Windows.h>
#include <gl/wglew.h>
#endif

// OpenGL / glew Headers
//#define GL3_PROTOTYPES 1
#include <GL/glew.h>

#include <GL/freeglut.h>

#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_impl_glut.h>
#include <imgui_impl_opengl2.h>

using namespace glTemplate;


int glutWindowID = -1;

int windowX = 100;
int windowY = 100;

int windowWidth = 1024;
int windowHeight = 768;

float ar = 1.0f;

double timeAccu = 0.0;

glm::vec4 clearColor( 0.0f, 0.1f, 0.2f, 1.0f );
//glm::vec4 clearColor( 1.0f, 1.0f, 1.0f, 1.0f );

std::vector<VertexBuffer*> grids( 3 );
VertexBuffer *axis = nullptr;

Trackball3D trackball( glm::vec3( 0, 0, -0.5 ), glm::vec3( pi() * 0.25, pi() * 0.85, 0 ) );

namespace glTemplate
{
	extern double globalTime;

	struct StartupConfig
	{
	private:
		template<typename T>
		static std::string toString( const T &t )
		{
			std::stringstream sstr;
			sstr << t;

			return sstr.str();
		}

		template<>
		static std::string toString<bool>( const bool &t )
		{
			return std::string( t ? "true" : "false" );
		}

	public:
		StartupConfig()
		{}

		static bool readPA( int argc, char **argv, StartupConfig &cfg )
		{
			//generic options: 
			cxxopts::Options options( argv[0], " command line options" );
			options
				.positional_help( "[optional args]" )
				.show_positional_help();

			options.add_options()
				//TODO: incorporate version, run RCStamp tool as pre build step and generate include file with version number constants
				//http://www.codeproject.com/KB/dotnet/build_versioning.aspx
				//		( "version,v",										"print version string" )
				( "h,help", "print help" )
				;

			//positional options:
			//options.parse_positional( { "" } );

			try
			{
				cxxopts::ParseResult result = options.parse( argc, argv );

				//generic options
				if( result.count( "help" ) )
				{
					std::cout << options.help( { 
						""
					} ) << std::endl;

					return false;
				}

			}
			catch( std::exception &e )
			{
				std::cerr << e.what() << std::endl;

				std::stringstream sstr;
				sstr << std::endl << "An error occured, processing program options: " << std::endl
					<< e.what() << std::endl;

				std::cout << options.help();

				throw std::runtime_error( sstr.str().c_str() );
			}

			return true;
		}
	};
}

StartupConfig cfg;

bool quit = false;
bool cleaned = false;

bool uiActive = true;

//stupid ImGui is stupid. have to use flag.
bool imGuiInitialized = false;

/*
void displayQuad( float left, float bottom, float right, float top )
{
	std::vector<GLfloat> vertices( 8 );

	vertices[0] = left;
	vertices[1] = bottom;

	vertices[2] = right;
	vertices[3] = bottom;

	vertices[4] = right;
	vertices[5] = top;

	vertices[6] = left;
	vertices[7] = top;

	glEnableClientState( GL_VERTEX_ARRAY );
	glVertexPointer( 2, GL_FLOAT, 0, &vertices[0] );

	glDrawArrays( GL_TRIANGLE_FAN, 0, 8 );

	// deactivate vertex arrays after drawing
	glDisableClientState( GL_VERTEX_ARRAY );
}

void displayGraph( const std::vector<float> &values, unsigned int maxValues )//, float bottom, float top )
{
	if( values.size() )
	{
		std::vector<GLfloat> vertices( values.size() * 2 );

		for( int i = 0; i < values.size(); i++ )
		{
			vertices[i * 2 + 0] = (float) i / ( maxValues - 1.0f );
			vertices[i * 2 + 1] = values[i];
		}

		glEnableClientState( GL_VERTEX_ARRAY );
		glVertexPointer( 2, GL_FLOAT, 0, &vertices[0] );

		glDrawArrays( GL_LINE_STRIP, 0, (GLsizei)values.size() );
		//glDrawArrays( GL_POINTS, 0, values.size() );

		// deactivate vertex arrays after drawing
		glDisableClientState( GL_VERTEX_ARRAY );
	}
}
*/

void motion( int x, int y )
{
	if( uiActive )
		ImGui_ImplGLUT_MotionFunc( x, y );

	trackball.onMotion( x, y );
}

void passiveMotion( int x, int y )
{
	if( uiActive )
		ImGui_ImplGLUT_MotionFunc( x, y );

	trackball.onPassiveMotion( x, y );
}

void mouse( int button, int state, int x, int y )
{
	if( uiActive )
	{
		ImGui_ImplGLUT_MouseFunc( button, state, x, y );

		if( ImGui::GetIO().WantCaptureMouse )
			return;
	}

	trackball.onMouse( button, state, x, y );
}

void mouseWheel( int wheel, int direction, int x, int y )
{
	if( uiActive )
		ImGui_ImplGLUT_MouseWheelFunc( wheel, direction, x, y );

	trackball.onMouseWheel( wheel, direction, x, y );
}

void keyDown( unsigned char key, int x, int y )
{
	if( trackball.onKeyDown( key, x, y ) )
		return;

	if( uiActive )
		ImGui_ImplGLUT_KeyboardFunc( key, x, y );

	switch( key )
	{
	case ' ':
		//uiActive = !uiActive;
		//std::cout << ( uiActive ? "activated" : "deactivated" ) << " UI" << std::endl;
		break;
	case 27:
		quit = true;
		break;
	}
}

void keyUp( unsigned char key, int x, int y )
{
	if( trackball.onKeyUp( key, x, y ) )
		return;

	if( uiActive )
		ImGui_ImplGLUT_KeyboardUpFunc( key, x, y );
}

void specialDown( int key, int x, int y )
{
	if( trackball.onSpecialDown( key, x, y ) )
		return;

	if( uiActive )
		ImGui_ImplGLUT_SpecialFunc( key, x, y );

	switch( key )
	{
	case GLUT_KEY_RIGHT:
		break;
	case GLUT_KEY_LEFT:
		break;
	}
}

void specialUp( int key, int x, int y )
{
	if( trackball.onSpecialUp( key, x, y ) )
		return;

	if( uiActive )
		ImGui_ImplGLUT_SpecialUpFunc( key, x, y );
}

void update()
{
	static auto prevTime = std::chrono::system_clock::now();
	auto currentTime = std::chrono::system_clock::now();
	std::chrono::duration<double> diff = currentTime - prevTime;
	prevTime = currentTime;

	double dt = diff.count();
	timeAccu += dt;
	::globalTime += dt;

	//update your time-dependent objects
}

void display()
{
	glViewport( 0, 0, windowWidth, windowHeight );

	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();

	glm::mat4 proj = glm::perspective<float>( toRad( 60.0f ), ar, 0.1f, 300.0f );
	glMultMatrixf( glm::value_ptr( proj ) );

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

	glPushAttrib( GL_ALL_ATTRIB_BITS );
	{
		glm::mat4 view = trackball.mat();

		glMultMatrixf( glm::value_ptr( view ) );

		if( axis )
			axis->draw();

		glPushMatrix();
		{
			glRotatef( -90, 1, 0, 0 );

			for( int i = 0; i < grids.size(); i++ )
			{
				glLineWidth( grids.size() - i );
				if( grids[i] )
					grids[i]->draw();
			}
		}
		glPopMatrix();

		glPushAttrib( GL_ALL_ATTRIB_BITS );
		{
			//draw 3D stuff
		}
		glPopAttrib();
	}
	glPopAttrib();

	static auto prevTime = std::chrono::system_clock::now();
	auto currentTime = std::chrono::system_clock::now();
	std::chrono::duration<double> diff = currentTime - prevTime;
	prevTime = currentTime;

	double dt = diff.count();

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glOrtho( 0, windowWidth, windowHeight, 0, -1.0, 1.0 );
	//glMultMatrixf( glm::value_ptr( glm::ortho<float>( 0, windowWidth, windowHeight, 0 ) ) );

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

	//draw HUD

	if( uiActive )
	{
		//draw UI/ImGUI
	}
	
	glutSwapBuffers();

	checkForGLError();
}

void idle()
{
	if( quit )
		exit( 0 );

	update();

	glutPostRedisplay();

	GLenum err = glGetError();
	if( err != GL_NO_ERROR )
		std::cerr << "openGL error: " << glewGetErrorString( err );
}

void moved( int x, int y )
{
	std::cout << "moved to: " << x << ", " << y << std::endl;
}

void sizeChanged( int width, int height )
{
//#if _DEBUG
//	std::cout << "sizechanged: " << width << ", " << height << std::endl;
//#endif

	windowWidth = width;
	windowHeight = height;

	ar = (float) windowWidth / windowHeight;

	ImGui_ImplGLUT_ReshapeFunc( width, height );
}

bool ctrlHandler( DWORD fdwCtrlType )
{
	switch( fdwCtrlType )
	{
	case CTRL_C_EVENT:
	case CTRL_BREAK_EVENT:
	case CTRL_CLOSE_EVENT:
	case CTRL_LOGOFF_EVENT:
	case CTRL_SHUTDOWN_EVENT:
		quit = true;
		return true;
	}

	return false;
}

void initGL( int argc, char **argv )
{
	glutInit( &argc, argv );
	glutInitDisplayMode( GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH );
	glutInitWindowPosition( windowX, windowY );
	glutInitWindowSize( windowWidth, windowHeight );
	glutWindowID = glutCreateWindow( "glTemplate" );

	glutDisplayFunc( display );
	glutReshapeFunc( sizeChanged );
	glutIdleFunc( idle );
	glutMotionFunc( motion );
	glutPassiveMotionFunc( passiveMotion );
	glutMouseFunc( mouse );
	glutMouseWheelFunc( mouseWheel );
	glutKeyboardFunc( keyDown );
	glutKeyboardUpFunc( keyUp );
	glutSpecialFunc( specialDown );
	glutSpecialUpFunc( specialUp );

	GLenum glErr = glewInit();
	if( glErr != GLEW_OK )
	{
		std::stringstream sstr;
		sstr << "Error initializing glew: \"" << glewGetErrorString( glErr ) << "\"";
		throw std::runtime_error( sstr.str() );
	}

#ifdef _WIN32
	//enable v-sync
	wglSwapIntervalEXT( 1 );
#endif

	glPointSize( 4 );
	glLineWidth( 1 );

	glClearColor( clearColor[0], clearColor[1], clearColor[2], clearColor[3] );

	glDisable( GL_TEXTURE_2D );
	glDisable( GL_LIGHTING );
	glEnable( GL_COLOR_MATERIAL );
	glEnable( GL_CULL_FACE );
	glEnable( GL_DEPTH_TEST );

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void) io;

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	// Setup Platform/Renderer bindings
	ImGui_ImplGLUT_Init();
	ImGui_ImplOpenGL2_Init();

	//stupid ImGui is stupid. have to use flag so it won't crash at shutdown when we did not make it until here.
	imGuiInitialized = true;


	moved( windowX, windowY );
	sizeChanged( windowWidth, windowHeight );

	for( int i = 0; i < grids.size(); i++ )
		grids[i] = VertexBuffer::createGrid( glm::vec2( 2.0f ), ( 10 * pow( 2, ( i ) ) ) - 1, clearColor * 1.5f );
		//grids[i] = VertexBuffer::createGrid( glm::vec2( 2.0f ), ( 10 * pow( 2, ( i ) ) ) - 1, glm::vec3( 0.9f, 0.9f, 0.9f ) );

	axis = VertexBuffer::createAxis();
}

void cleanup()
{
	if( cleaned )
		return;
	cleaned = true;

	//clean up all your dynamic ressources here

	for( auto &it : grids )
		safeDelete( it );

	safeDelete( axis );

	//stupid ImGui is stupid. have to use flag to avoid potential crash.
	if( imGuiInitialized )
	{
		ImGui_ImplOpenGL2_Shutdown();
		ImGui_ImplGLUT_Shutdown();
		ImGui::DestroyContext();
	}

	glutExit();

	glutWindowID = -1;
}


void __cdecl onExit()
{
	std::cerr << "on exit called" << std::endl;

	cleanup();
}

void __cdecl onTerminate()
{
	std::cerr << "process terminated" << std::endl;
	abort();
}

int main( int argc, char **argv )
{
	atexit( onExit );
	std::set_terminate( onTerminate );

	srand( 0 );

	try
	{
		if( !SetConsoleCtrlHandler( (PHANDLER_ROUTINE) ctrlHandler, true ) )
			throw std::runtime_error( "registering ctrl-handler failed" );

		disableQuickEditMode();

		if( StartupConfig::readPA( argc, argv, cfg ) )
		{
			trackball.setMotionSpeed( 0.001f );

			//init non-gl-dependent objects

			initGL( argc, argv );

			//init gl-dependent objects

			glutMainLoop();
		}
	}
	catch( std::exception &e )
	{
		std::cerr << "caught exception: " << e.what() << std::endl;

		cleanup();

#ifdef _DEBUG
#ifdef _WIN32
		std::cout << "press ENTER" << std::endl;
		getchar();
#endif
#endif
		return EXIT_FAILURE;
	}

	cleanup();

	return EXIT_SUCCESS;
}
