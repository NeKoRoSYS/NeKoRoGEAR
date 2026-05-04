#pragma once
#include <glad/glad.h>
#include <vector>
#include <cstddef>

struct Vertex {
    float position[3];
    float normal[3];
    float texCoords[2];
};

class Mesh {
public:
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
    ~Mesh();
    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;
    Mesh(Mesh&& other) noexcept;
    Mesh& operator=(Mesh&& other) noexcept;

    void Draw() const;

private:
    GLuint VAO, VBO, EBO;
    void SetupMesh();
};