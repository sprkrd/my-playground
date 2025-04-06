#include "ResourceManager.hpp"

#include <cassert>

std::optional<ResourceManager> ResourceManager::globalResourceManager;

ResourceManager& ResourceManager::getGlobalResourceManager()
{
	return globalResourceManager.value();
}

void ResourceManager::initGlobalResourceManager(const char* exePath)
{
    globalResourceManager = fromExePath(exePath);
}

ResourceManager ResourceManager::fromExePath(const char *exePath)
{
    std::filesystem::path assetsPath(exePath);
    assetsPath = assetsPath.parent_path().parent_path()/"assets";
    assetsPath = std::filesystem::absolute(assetsPath);
    return ResourceManager(assetsPath);
}

ResourceManager::ResourceManager(const std::filesystem::path& assetsPath)
    : mAssetsPath(assetsPath)
    , mTextureHolder(assetsPath/"textures")
    , mFontHolder(assetsPath/"fonts")
    , mSoundBufferHolder(assetsPath/"soundfx")
    , mShaderHolder(assetsPath/"shaders")
{

}

template<>
const ResourceHolder<sf::Texture>& ResourceManager::getResourceHolder<sf::Texture>() const
{
    return mTextureHolder;
}

template<>
const ResourceHolder<sf::Font>& ResourceManager::getResourceHolder<sf::Font>() const
{
    return mFontHolder;
}

template<>
const ResourceHolder<sf::SoundBuffer>& ResourceManager::getResourceHolder<sf::SoundBuffer>() const
{
    return mSoundBufferHolder;
}

template<>
const ResourceHolder<sf::Shader>& ResourceManager::getResourceHolder<sf::Shader>() const
{
    return mShaderHolder;
}
