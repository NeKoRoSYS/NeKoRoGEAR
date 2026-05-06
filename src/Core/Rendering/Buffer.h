#pragma once
#include <cstdint>
#include <memory>

class VertexBuffer {
public:
    virtual ~VertexBuffer() = default;
    virtual void Bind() const = 0;
    virtual void Unbind() const = 0;

    static std::unique_ptr<VertexBuffer> Create(float* vertices, uint32_t size);
};

class IndexBuffer {
public:
    virtual ~IndexBuffer() = default;
    virtual void Bind() const = 0;
    virtual void Unbind() const = 0;
    virtual uint32_t GetCount() const = 0;

    static std::unique_ptr<IndexBuffer> Create(uint32_t* indices, uint32_t count);
};

class VertexArray {
public:
    virtual ~VertexArray() = default;
    virtual void Bind() const = 0;
    virtual void Unbind() const = 0;
    
    virtual void AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer) = 0;
    virtual void SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer) = 0;
    virtual const std::shared_ptr<IndexBuffer>& GetIndexBuffer() const = 0;

    static std::unique_ptr<VertexArray> Create();
};