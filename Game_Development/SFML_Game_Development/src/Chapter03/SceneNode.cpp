#include "SceneNode.hpp"

#include <algorithm>
#include <cassert>

namespace
{

template<class Iterator>
Iterator findChild(Iterator begin, Iterator end, const SceneNode& node)
{
    return std::find_if(begin, end,
            [&](const SceneNode::Ptr& p) -> bool { return p.get() == &node; });
}

} // unnamed ns


SceneNode::SceneNode()
    : mParent(nullptr)
{
}

void SceneNode::attachChild(Ptr child)
{
    child->mParent = this;
    mChildren.push_back(std::move(child));
}

bool SceneNode::hasChild(const SceneNode& node) const
{
	return findChild(mChildren.begin(), mChildren.end(), node) != mChildren.end();
}

SceneNode::Ptr SceneNode::detachChild(SceneNode& node)
{
    auto found = findChild(mChildren.begin(), mChildren.end(), node);
    assert(found != mChildren.end());
    Ptr result = std::move(*found);
    result->mParent = nullptr;
    mChildren.erase(found);
    return result;
}

void SceneNode::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    states.transform *= getTransform();
    drawCurrent(target, states);
    for (const auto& child : mChildren)
    {
        child->draw(target, states);
    }
}

void SceneNode::drawCurrent(sf::RenderTarget& target, const sf::RenderStates& states) const
{
}

