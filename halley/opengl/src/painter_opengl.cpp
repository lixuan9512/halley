#include "painter_opengl.h"
#include "halley/core/graphics/material/material_definition.h"

using namespace Halley;

PainterOpenGL::PainterOpenGL()
{}

PainterOpenGL::~PainterOpenGL()
{
	if (vbo != 0) {
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDeleteBuffers(1, &vbo);
		vbo = 0;
	}
	if (veo != 0) {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glDeleteBuffers(1, &veo);
		veo = 0;
	}
	if (vao != 0) {
		glBindVertexArray(0);
		glDeleteVertexArrays(1, &vao);
		vao = 0;
	}
}

void PainterOpenGL::doStartRender()
{
	glCheckError();

	if (!glUtils) {
		glUtils = std::make_unique<GLUtils>();
	}

	if (vbo == 0) {
		glGenBuffers(1, &vbo);
		glCheckError();
	}

	if (veo == 0) {
		glGenBuffers(1, &veo);
		glCheckError();
	}

	if (vao == 0) {
		glGenVertexArrays(1, &vao);
		glCheckError();
		glBindVertexArray(vao);
		glCheckError();
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glCheckError();
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, veo);
		glCheckError();
	}

	glBindVertexArray(vao);
}

void PainterOpenGL::doEndRender()
{
	glBindVertexArray(0);
	glCheckError();
}

void PainterOpenGL::clear(Colour colour)
{
	glCheckError();
	glClearColor(colour.r, colour.g, colour.b, colour.a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glCheckError();
}

void PainterOpenGL::setBlend(BlendType blend)
{
	glUtils->setBlendType(blend);
}

void PainterOpenGL::setVertices(MaterialDefinition& material, size_t numVertices, void* vertexData)
{
	// Load vertices into VBO
	size_t bytesSize = numVertices * material.getVertexStride();
	glBufferData(GL_ARRAY_BUFFER, bytesSize, vertexData, GL_STREAM_DRAW);
	glCheckError();

	// Set attributes
	setupVertexAttributes(material);
}

void PainterOpenGL::setupVertexAttributes(MaterialDefinition& material)
{
	// Set vertex attribute pointers in VBO
	int vertexStride = material.getVertexStride();
	for (auto& attribute : material.getAttributes()) {
		int count = 0;
		int type = 0;
		switch (attribute.type) {
		case ShaderParameterType::Float:
			count = 1;
			type = GL_FLOAT;
			break;
		case ShaderParameterType::Float2:
			count = 2;
			type = GL_FLOAT;
			break;
		case ShaderParameterType::Float3:
			count = 3;
			type = GL_FLOAT;
			break;
		case ShaderParameterType::Float4:
			count = 4;
			type = GL_FLOAT;
			break;
		case ShaderParameterType::Int:
			count = 1;
			type = GL_INT;
			break;
		case ShaderParameterType::Int2:
			count = 2;
			type = GL_INT;
			break;
		case ShaderParameterType::Int3:
			count = 3;
			type = GL_INT;
			break;
		case ShaderParameterType::Int4:
			count = 4;
			type = GL_INT;
			break;
		default:
			break;
		}
		glEnableVertexAttribArray(attribute.location);
		size_t offset = attribute.offset;
		glVertexAttribPointer(attribute.location, count, type, GL_FALSE, vertexStride, reinterpret_cast<GLvoid*>(offset));
		glCheckError();
	}

	// TODO: disable positions not used by this program
}

void PainterOpenGL::drawQuads(size_t n)
{
	size_t sz = n * 6;

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, int(sz) * sizeof(short), getStandardQuadIndices(sz), GL_STREAM_DRAW);
	glDrawElements(GL_TRIANGLES, int(sz), GL_UNSIGNED_SHORT, 0);
	glCheckError();
}

void PainterOpenGL::setViewPort(Rect4i rect, bool enableScissor)
{
	glUtils->setViewPort(rect, enableScissor);
}
