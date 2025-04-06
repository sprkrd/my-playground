#pragma once

#include <SFML/System.hpp>

#include "ResourceManager.hpp"
#include "SceneNode.hpp"

class Entity : public SceneNode
{
    public:

        typedef std::unique_ptr<Entity> Ptr;

        Entity();

        void setVelocity(sf::Vector2f velocity);

        void setVelocity(float vx, float vy);

        sf::Vector2f getVelocity() const;

        virtual ~Entity() = default;

    private:

        sf::Vector2f mPosition;
        sf::Vector2f mVelocity;

};

class Aircraft : public Entity
{
    public:
        enum Type
        {
            Eagle,
            Raptor
        };

    public:
        explicit Aircraft(Type type);

    private:

        virtual void drawCurrent(sf::RenderTarget& target,
                const sf::RenderStates& states) const override;

    private:
        Type mType;
        sf::Sprite mSprite;
};

