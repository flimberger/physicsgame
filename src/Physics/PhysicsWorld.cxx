#include "PhysicsWorld.hxx"

PhysicsWorld::PhysicsWorld(const btVector3 &gravity)
    : m_broadphaseAlgorithm{new btDbvtBroadphase},
      m_collisionConfiguration{new btDefaultCollisionConfiguration},
      m_dispatcher{new btCollisionDispatcher{m_collisionConfiguration.get()}},
      m_solver{new btSequentialImpulseConstraintSolver},
      m_dynamicsWorld{new btDiscreteDynamicsWorld{
          m_dispatcher.get(), m_broadphaseAlgorithm.get(), m_solver.get(),
          m_collisionConfiguration.get()}}
{
    m_dynamicsWorld->setGravity(gravity);
}

btDiscreteDynamicsWorld *PhysicsWorld::GetDynamicsWorld() const
{
    return m_dynamicsWorld.get();
}
