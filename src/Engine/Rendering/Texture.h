#pragma once
#include <string>
#include <memory>

class Texture {
public:
    virtual ~Texture() = default;
    virtual void Bind(uint32_t slot = 0) const = 0;
    virtual void Unbind() const = 0;
    
    static std::unique_ptr<Texture> Load(const std::string& filepath);
};