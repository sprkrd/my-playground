#include "include/Entity.hpp"

Entity::Entity()
    : mPosition(0, 0)
    , mVelocity(0, 0)
{

}

void Entity::setVelocity(sf::Vector2f velocity)
{
    mVelocity = velocity;
}

void Entity::setVelocity(float vx, float vy)
{
    mVelocity = sf::Vector2f(vx, vy);
}

sf::Vector2f Entity::getVelocity() const
{
    return mVelocity;
}

Aircraft::Aircraft(Type type)
: mType(type)
{
}
