#pragma once

#include <memory>

class Mesh;

class PhysicsObject
{
public:
    PhysicsObject();
    PhysicsObject(const std::shared_ptr<Mesh> &mesh);

    // may be removed
    void setMesh(const std::shared_ptr<Mesh> &mesh);

    void draw();

private:
    std::shared_ptr<Mesh> m_mesh;
};
