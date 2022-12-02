#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <algorithm>

#include <gl/glew.h>

#include <time.h>

#ifdef _WIN32
#include <Windows.h>
#endif

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <imgui.h>

#include <GL/freeglut.h>

#include <vld.h>

//treat all 4715s as arrors
#pragma warning (error: 4715)
namespace glTemplate
{
	template<typename T>
	inline void safeDelete( T* &ptr )
	{
		if( ptr )
		{
			delete ptr;
			ptr = nullptr;
		}
	}

	template<typename T>
	inline void safeDeleteArray( T* &ptr )
	{
		if( ptr )
		{
			delete [] ptr;
			ptr = nullptr;
		}
	}

	//custom min function since std::min produced slow code on vc++
	// http://randomascii.wordpress.com/2013/11/24/stdmin-causing-three-times-slowdown-on-vc/
	template<typename T>
	inline const T min( const T left, const T right )
	{
		return ( right < left ? right : left );
	}

	//custom max function since std::max produced slow code on vc++
	// http://randomascii.wordpress.com/2013/11/24/stdmin-causing-three-times-slowdown-on-vc/
	template<typename T>
	inline const T max( const T left, const T right )
	{
		return ( left < right ? right : left );
	}

	template<typename T>
	inline T clamp( T value, T minValue, T maxValue )
	{
		return glTemplate::min( glTemplate::max( value, minValue ), maxValue );
	}

	template<typename T>
	inline T clamp01( T value )
	{
		return glTemplate::min( glTemplate::max( value, (T)0 ), (T)1 );
	}

	double getGlobalTime();

	std::vector<std::string> split( const std::string &s, char delimiter );

	void trim( std::string &s );
	void trimLeft( std::string &s );
	void trimRight( std::string &s );

	void toLower( std::string &s );
	void toUpper( std::string &s );

	bool endsWith( const std::string &s, const std::string &end );

	template<size_t S>
	void makeDateTimeString( char( &str )[S], time_t time, bool fileSafe = false )
	{
		struct tm *timeinfo;
		timeinfo = localtime( &time );

		sprintf_s( str, ( fileSafe ? "%d-%02d-%02d %02d-%02d-%02d" : "%d-%02d-%02d %02d:%02d:%02d" ),
			timeinfo->tm_year + 1900,
			timeinfo->tm_mon + 1,
			timeinfo->tm_mday,
			timeinfo->tm_hour,
			timeinfo->tm_min,
			timeinfo->tm_sec );
	}

	template<size_t S>
	void makeDateTimeString( char( &str )[S], bool fileSafe = false )
	{
		time_t rawtime;

		::time( &rawtime );

		makeDateTimeString( str, rawtime, fileSafe );
	}

	inline std::string	getDateTimeString( bool fileSafe = false )
	{
		char tempStr[32];
		makeDateTimeString( tempStr, fileSafe );
		return std::string( tempStr );
	}

	inline std::string toTimeString( double seconds )
	{
		char tempStr[64];

		sprintf( tempStr, "%02d:%02d:%02d:%03d", 
			(int)( seconds ) / ( 60 * 60 ),
			( (int)( seconds ) / 60 ) % 60,
			(int)( seconds ) % 60,
			(int)( seconds * 1000 ) % 1000 );

		return std::string( tempStr );
	}

	template<typename T> size_t arraySize( const T& t )
	{
		return sizeof( t ) / sizeof( *t );
	}

	inline unsigned int nextPo2( unsigned int x )
	{
		unsigned int power = 1;
		while( power < x && power )
			power <<= 1;

		if( !power )
			throw std::runtime_error( "nextpo2 overflow" );

		return power;
	}

	inline ImVec2 glm2im( const glm::ivec2 &v )
	{
		return ImVec2( (float)v.x, (float) v.y );
	}

	inline ImVec4 glm2im( const glm::ivec4 &v )
	{
		return ImVec4( (float)v.x, (float)v.y, (float)v.z, (float)v.w );
	}

	inline ImVec2 glm2im( const glm::vec2 &v )
	{
		return ImVec2( v.x, v.y );
	}

	inline ImVec4 glm2im( const glm::vec4 &v )
	{
		return ImVec4( v.x, v.y, v.z, v.w );
	}

	inline glm::vec2 im2glm( const ImVec2 &v )
	{
		return glm::vec2( v.x, v.y );
	}

	inline glm::vec4 im2glm( const ImVec4 &v )
	{
		return glm::vec4( v.x, v.y, v.z, v.w );
	}

	inline const glm::vec3 &one()
	{
		static const glm::vec3 o( 1, 1, 1 );
		return o;
	}

	inline const glm::vec3 &unit()
	{
		static const glm::vec3 u( glm::normalize( glm::vec3( 1 ) ) );
		return u;
	}

	inline const glm::vec3 &zero()
	{
		static const glm::vec3 z( 0 );
		return z;
	}

	inline const glm::vec3 &unitX()
	{
		static const glm::vec3 x( 1, 0, 0 );
		return x;
	}

	inline const glm::vec3 &unitY()
	{
		static const glm::vec3 y( 0, 1, 0 );
		return y;
	}

	inline const glm::vec3 &unitZ()
	{
		static const glm::vec3 z( 0, 0, 1 );
		return z;
	}

	inline const glm::vec3 &black()
	{
		static const glm::vec3 b( 0 );
		return b;
	}

	inline const glm::vec3 &grey()
	{
		static const glm::vec3 g( 0.5 );
		return g;
	}

	inline const glm::vec3 &white()
	{
		static const glm::vec3 w( 1 );
		return w;
	}

	inline const glm::vec3 &red()
	{
		static const glm::vec3 r( 1, 0, 0 );
		return r;
	}

	inline const glm::vec3 &green()
	{
		static const glm::vec3 g( 0, 1, 0 );
		return g;
	}

	inline const glm::vec3 &blue()
	{
		static const glm::vec3 b( 0, 0, 1 );
		return b;
	}

	inline const glm::vec3 &cyan()
	{
		static const glm::vec3 c( 0, 1, 1 );
		return c;
	}

	inline const glm::vec3 &magenta()
	{
		static const glm::vec3 b( 1, 0, 1 );
		return b;
	}

	inline const glm::vec3 &yellow()
	{
		static const glm::vec3 y( 1, 1, 0 );
		return y;
	}

	inline long double pi()
	{
		return 3.141592653589793238462643383279502884L;
	}

	inline long double pi2()
	{
		return 2 * 3.141592653589793238462643383279502884L;
	}

	inline double toRad( double deg )
	{
		static double s = pi2() / 360.0;
		return deg * s;
	}

	inline double toDeg( double rad )
	{
		static double s = 360.0 / pi2();
		return rad * s;
	}

	void drawAxes();
	void drawLocator();

	bool checkForGLError();

	struct IndexedLine
	{
	public:
		int A;
		int B;

		IndexedLine() :
			A( -1 ), B( -1 )
		{}

		IndexedLine( int A, int B ) :
			A( A ), B( B )
		{}
	};

	struct IndexedFace
	{
	public:
		IndexedFace()
		{}

		explicit IndexedFace( unsigned int vertexCount ) :
			indices( vertexCount )
		{}

		IndexedFace( unsigned int a, unsigned int b, unsigned int c ) :
			indices( 3 )
		{
			this->indices[0] = a;
			this->indices[1] = b;
			this->indices[2] = c;
		}

		IndexedFace( unsigned int a, unsigned int b, unsigned int c, unsigned int d ) :
			indices( 4 )
		{
			this->indices[0] = a;
			this->indices[1] = b;
			this->indices[2] = c;
			this->indices[3] = d;
		}

		std::vector<unsigned int> indices;

		bool isTriangle() const { return this->indices.size() == 3; }
		bool isQuad() const { return this->indices.size() == 4; }
	};

	typedef std::vector<glm::vec3>		VertexList;
	typedef std::vector<glm::vec3>		NormalList;
	typedef std::vector<glm::vec3>		ColorList;
	typedef std::vector<glm::vec4>		ColorAlphaList;
	typedef std::vector<glm::vec2>		TexCoordList;
	typedef std::vector<unsigned int>	ArrayList;
	typedef std::vector<IndexedLine>	IndexedLineList;
	typedef std::vector<IndexedFace>	IndexedFaceList;

	class ScopedGlAttribs
	{
	public:
		ScopedGlAttribs( GLbitfield bits )
		{
			glPushAttrib( bits );
		}

		~ScopedGlAttribs()
		{
			glPopAttrib();
		}
	};

	class ScopedImGuiDisable
	{
	private:
		bool _disabled;

	public:
		explicit ScopedImGuiDisable( bool disable = true );
		~ScopedImGuiDisable();

		void disable();
		void enable();
	};

#ifdef _WIN32
	std::string formatWinError( DWORD err );
	std::string formatLastWinError();

	void disableQuickEditMode();
#endif

	void printText( const char *string, float x, float y, void *font = GLUT_BITMAP_HELVETICA_12 );
}
