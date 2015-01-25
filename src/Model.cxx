#include "Model.hxx"

Model::Model(const std::shared_ptr<Material> &material,
             const std::shared_ptr<Mesh> &mesh)
    : m_material{material}, m_mesh{mesh}
{
}

void Model::Draw() const
{
    m_material->Use();
    m_mesh->Draw();
}

const Material &Model::GetMaterial() const { return *m_material.get(); }

const Mesh &Model::GetMesh() const { return *m_mesh.get(); }
