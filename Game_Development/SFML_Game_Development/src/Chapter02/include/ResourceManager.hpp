#pragma once

#include <SFML/Graphics.hpp>

#include <array>
#include <memory>

enum class TextureId { Landscape, Airplane, Missile, Null };
constexpr int kNumberOfTextures = static_cast<int>(TextureId::Null);

typedef std::unique_ptr<sf::Texture> TexturePtr;

class TextureHolder
{
public:



private:

	std::array<TexturePtr, kNumberOfTextures> mTextures;

};

