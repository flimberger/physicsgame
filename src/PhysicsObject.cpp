#include "PhysicsObject.hpp"
#include "Mesh.hpp"

PhysicsObject::PhysicsObject()
{}

PhysicsObject::PhysicsObject(const std::shared_ptr<Mesh> &mesh)
  : m_mesh { mesh }
{}

void PhysicsObject::setMesh(const std::shared_ptr<Mesh> &mesh)
{
    m_mesh = mesh;
}

void PhysicsObject::draw()
{
    m_mesh->draw();
}
