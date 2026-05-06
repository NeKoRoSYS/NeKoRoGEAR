#include "Buffer.h"
#include "OpenGL/OpenGLBuffer.h"

std::unique_ptr<VertexBuffer> VertexBuffer::Create(float* vertices, uint32_t size) {
    return std::make_unique<OpenGLVertexBuffer>(vertices, size);
}

std::unique_ptr<IndexBuffer> IndexBuffer::Create(uint32_t* indices, uint32_t count) {
    return std::make_unique<OpenGLIndexBuffer>(indices, count);
}

std::unique_ptr<VertexArray> VertexArray::Create() {
    return std::make_unique<OpenGLVertexArray>();
}