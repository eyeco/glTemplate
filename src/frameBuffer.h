#pragma once

#include <common.h>

namespace glTemplate
{
	class FrameBuffer
	{
	private:
		std::string _name;

		bool _valid;

		GLuint _rtName;
		GLuint _fboID;

		//intentionally no implemented
		//prevent from copying via copy constructor -- have to use reference counting to not end up with invalid gl FBO IDs
		FrameBuffer( const FrameBuffer& );

	public:
		explicit FrameBuffer( const std::string &name );
		~FrameBuffer();

		bool build( size_t width, size_t height, GLint minFilter = GL_LINEAR, GLint magFilter = GL_LINEAR );
		void destroy();

		bool activate();
		bool deactivate();

		GLuint getRenderTextureName() const { return _rtName; }

		const std::string &getName() const { return _name; }
	};
}