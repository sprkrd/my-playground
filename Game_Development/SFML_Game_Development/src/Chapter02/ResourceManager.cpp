#include "ResourceManager.hpp"

#include <cassert>

template<class Resource>
ResourceHolder<Resource>::ResourceHolder(const std::filesystem::path& resourcePath)
    : mResourcePath(resourcePath)
{

}

template<class Resource>
const Resource& ResourceHolder<Resource>::get(ResourceId id) const
{
    int idIndex = static_cast<int>(id);
    assert(mResources[idIndex] && "Tried to get a resource that was not loaded");
    return *mResources[idIndex];
}

template<class Resource>
Resource& ResourceHolder<Resource>::get(ResourceId id)
{
    return const_cast<Resource&>(const_cast<const ResourceHolder*>(this)->get(id));
}

template class ResourceHolder<sf::Texture>;
template class ResourceHolder<sf::Font>;
template class ResourceHolder<sf::SoundBuffer>;
template class ResourceHolder<sf::Shader>;

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
