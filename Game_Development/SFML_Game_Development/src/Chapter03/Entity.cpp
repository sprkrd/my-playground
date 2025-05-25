#include "include/Entity.hpp"


const sf::Texture& toTexture(Aircraft::Type type)
{
    const auto& resourceManager = ResourceManager::getGlobalResourceManager();
    TextureId textureId;
    switch (type)
    {
        case Aircraft::Eagle:
            textureId = TextureId::Eagle;
            break;
        case Aircraft::Raptor:
            textureId = TextureId::Raptor;
            break;
        default:
            textureId = TextureId::Null;
            break;
    }
    return resourceManager.get<sf::Texture>(textureId);
}

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
    , mSprite(toTexture(type))
{
    auto bounds = mSprite.getLocalBounds();
    this->setOrigin(bounds.size/2.f);
    //mSprite.setOrigin(bounds.size/2.f);
}

void Aircraft::drawCurrent(sf::RenderTarget& target,
		const sf::RenderStates& states) const
{
    target.draw(mSprite, states);
    sf::RectangleShape bb(mSprite.getLocalBounds().size);
    bb.setFillColor(sf::Color::Transparent);
    bb.setOutlineColor(sf::Color::Red);
    bb.setOutlineThickness(1);
    target.draw(bb, states);
}


