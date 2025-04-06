#pragma once

#include <SFML/Graphics.hpp>

#include <memory>
#include <vector>

class SceneNode : public sf::Transformable, public sf::Drawable
{
    public:

        typedef std::unique_ptr<SceneNode> Ptr;

    public:

        SceneNode();

        SceneNode(const SceneNode&) = delete;

        SceneNode& operator=(const SceneNode&) = delete;

        void attachChild(Ptr child);

        bool hasChild(const SceneNode& node) const;

        Ptr detachChild(SceneNode& node);

        virtual ~SceneNode() = default;

    private:

        void draw(sf::RenderTarget& target, sf::RenderStates states) const final;

        virtual void drawCurrent(sf::RenderTarget& target,
                const sf::RenderStates& states) const;

    private:

        std::vector<Ptr> mChildren;
        SceneNode* mParent;
};
