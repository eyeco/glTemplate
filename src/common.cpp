#include <common.h>

#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <string>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <functional>

#include <imgui.h>
#include <imgui_internal.h>

namespace glTemplate
{
	double globalTime = 0.0;
	double getGlobalTime() { return globalTime; }

	std::vector<std::string> split( const std::string &s, char delimiter )
	{
		//TODO: 
		// do this properly (this implementation has the following problem: if last substring is "", then it won't be added

		std::string outStr;
		std::stringstream sstr( s );

		std::vector<std::string> v;

		while( std::getline( sstr, outStr, delimiter ) )
			v.push_back( outStr );

		return v;
	}

	void trim( std::string &s )
	{
		trimLeft( s );
		trimRight( s );
	}

	void trimLeft( std::string &s )
	{
		if( s.empty() )
			return;

		auto p = s.begin();
		for( p; p != s.end() && isspace( *p ); p++ );

		s.erase( s.begin(), p );
	}

	void trimRight( std::string &s )
	{
		if( s.empty() )
			return;

		auto p = s.end();
		for( p; p != s.begin() && isspace( *--p ););

		if( !isspace( *p ) )
			p++;

		s.erase( p, s.end() );
	}

	void toLower( std::string &s )
	{
		std::transform( s.begin(), s.end(), s.begin(), tolower );
	}

	void toUpper( std::string &s )
	{
		std::transform( s.begin(), s.end(), s.begin(), toupper );
	}

	bool endsWith( const std::string &s, const std::string &end )
	{
		if( s.length() < end.length() )
			return false;
		return ( s.compare( s.length() - end.length(), end.length(), end ) == 0 );
	}

	ScopedImGuiDisable::ScopedImGuiDisable( bool disable ) :
		_disabled( false )
	{
		if( disable )
			this->disable();
	}

	ScopedImGuiDisable::~ScopedImGuiDisable()
	{
		enable();
	}

	void ScopedImGuiDisable::disable()
	{
		if( !_disabled )
		{
			ImGui::PushItemFlag( ImGuiItemFlags_Disabled, true );
			ImGui::PushStyleVar( ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f );

			_disabled = true;
		}
	}

	void ScopedImGuiDisable::enable()
	{
		if( _disabled )
		{
			ImGui::PopItemFlag();
			ImGui::PopStyleVar();

			_disabled = false;
		}
	}

#ifdef _WIN32
	std::string formatWinError( DWORD err )
	{
		if( err == 0 )
			return std::string( "no error" ); //No error message has been recorded

		LPSTR messageBuffer = nullptr;
		size_t size = FormatMessageA( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, err, MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ), (LPSTR) &messageBuffer, 0, NULL );

		std::string message( messageBuffer, size );

		//Free the buffer.
		LocalFree( messageBuffer );

		return message;
	}

	std::string formatLastWinError()
	{
		return formatWinError( GetLastError() );
	}

	void disableQuickEditMode()
	{
		HANDLE hStdin = GetStdHandle( STD_INPUT_HANDLE );
		unsigned long mode = 0;
		GetConsoleMode( hStdin, &mode );

		mode &= ~ENABLE_QUICK_EDIT_MODE;

		SetConsoleMode( hStdin, mode );
	}
#endif

	void printText(const char *string, float x, float y, void *font)
	{
		glRasterPos3f(x, y, 0.0);

		while(*string)
			glutBitmapCharacter(font, *(string++));
	}

	void drawAxes()
	{
		static glm::vec3 verts[] = {
			zero(), unitX(),
			zero(), unitY(),
			zero(), unitZ()
		};

		static glm::vec3 cols[] = {
			red(), red(),
			green(), green(),
			blue(), blue()
		};

		glEnableClientState( GL_VERTEX_ARRAY );
		glEnableClientState( GL_COLOR_ARRAY );

		glVertexPointer( 3, GL_FLOAT, 0, verts );
		glColorPointer( 3, GL_FLOAT, 0, cols );

		glDrawArrays( GL_LINES, 0, arraySize( verts ) );

		glDisableClientState( GL_VERTEX_ARRAY );
		glDisableClientState( GL_COLOR_ARRAY );
	}

	void drawLocator()
	{
		static glm::vec3 verts[] = {
			unitX() * -0.5f, unitX() * 0.5f,
			unitY() * -0.5f, unitY() * 0.5f,
			unitZ() * -0.5f, unitZ() * 0.5f
		};

		static glm::vec3 cols[] = {
			grey(), grey(),
			grey(), grey(),
			grey(), grey()
		};

		glEnableClientState( GL_VERTEX_ARRAY );
		glEnableClientState( GL_COLOR_ARRAY );

		glVertexPointer( 3, GL_FLOAT, 0, verts );
		glColorPointer( 3, GL_FLOAT, 0, cols );

		glDrawArrays( GL_LINES, 0, arraySize( verts ) );

		glDisableClientState( GL_VERTEX_ARRAY );
		glDisableClientState( GL_COLOR_ARRAY );
	}

#ifdef _DEBUG
	bool checkForGLError()
	{
		GLenum errCode;
		if( ( errCode = glGetError() ) != GL_NO_ERROR )
		{
			const GLubyte *str = gluErrorString( errCode );
			if( str )
				std::cerr << "<error> OpenGL error: " << str << std::endl;
			else
				std::cerr << "<error> unknown OpenGL error #" << std::hex << errCode << std::dec << std::endl;
			return false;
		}
		return true;
	}
#else
	bool checkForGLError() { return true; }
#endif //_DEBUG
}
