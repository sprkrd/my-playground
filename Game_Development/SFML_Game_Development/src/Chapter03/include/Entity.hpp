#pragma once

#include <SFML/System.hpp>


class Entity
{
    public:

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
        Type mType;
};

