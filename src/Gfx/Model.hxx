#pragma once

#include "Material.hxx"
#include "Mesh.hxx"

class Model
{
  public:
    Model(const std::shared_ptr<Material> &material,
          const std::shared_ptr<Mesh> &mesh);

    void Draw() const;

    const Material &GetMaterial() const;
    const Mesh &GetMesh() const;

  private:
    std::shared_ptr<Material> m_material;
    std::shared_ptr<Mesh> m_mesh;
};
