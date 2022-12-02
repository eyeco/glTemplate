#include <frameBuffer.h>

#include <iostream>

namespace glTemplate
{
	FrameBuffer::FrameBuffer( const std::string &name ) :
		_name( name ),
		_valid( false ),
		_rtName( ~0x00 ),
		_fboID( ~0x00 )
	{}

	FrameBuffer::~FrameBuffer()
	{
		destroy();
	}

	bool FrameBuffer::activate()
	{
		if( _valid )
			glBindFramebuffer( GL_FRAMEBUFFER, _fboID );

		return _valid;
	}

	bool FrameBuffer::deactivate()
	{
		if( _valid )
			glBindFramebuffer( GL_FRAMEBUFFER, 0 );

		return _valid;
	}

	bool FrameBuffer::build( size_t width, size_t height, GLint minFilter, GLint magFilter )
	{
		if( _valid )
			destroy();

		_valid = true;

		glGenTextures( 1, &_rtName );

		glGenFramebuffers( 1, &_fboID );
		glBindFramebuffer( GL_FRAMEBUFFER, _fboID );

		glBindTexture( GL_TEXTURE_2D, _rtName );
		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_FLOAT, 0 );

		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter );

		// set renderTexture as colour attachement #0
		glFramebufferTexture( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, _rtName, 0 );

		// Set the list of draw buffers.
		GLenum drawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
		glDrawBuffers( 1, drawBuffers ); // "1" is the size of DrawBuffers

		// Always check that our framebuffer is ok
		if( glCheckFramebufferStatus( GL_FRAMEBUFFER ) != GL_FRAMEBUFFER_COMPLETE )
		{
			std::cerr << "<error> fbo incomplete" << std::endl;
			destroy();
		}

		glBindFramebuffer( GL_FRAMEBUFFER, 0 );

		return _valid;
	}

	void FrameBuffer::destroy()
	{
		if( _valid )
		{
			glDeleteFramebuffers( 1, &_fboID );
			_fboID = ~0x00;

			glDeleteTextures( 1, &_rtName );
			_rtName = ~0x00;

			_valid = false;
		}
	}
}