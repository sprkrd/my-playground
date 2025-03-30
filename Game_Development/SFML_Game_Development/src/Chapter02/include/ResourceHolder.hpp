#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include <array>
#include <memory>

enum class TextureId {
    Landscape,
    Airplane,
    Missile,
    Null
};

enum class FontId {
    GameOver,
    Null
};

enum class SoundBufferId {
    Null
};

enum class ShaderId {
    Null
};

constexpr int kNumberOfTextures = static_cast<int>(TextureId::Null);
constexpr int kNumberOfFonts = static_cast<int>(FontId::Null);
constexpr int kNumberOfSoundFx = static_cast<int>(SoundBufferId::Null);
constexpr int kNumberOfShaders = static_cast<int>(ShaderId::Null);

template<class Resource>
struct ResourceIdTrait;

template<>
struct ResourceIdTrait<sf::Texture>
{
        typedef TextureId Value;
};

template<>
struct ResourceIdTrait<sf::Font>
{
        typedef FontId Value;
};

template<>
struct ResourceIdTrait<sf::SoundBuffer>
{
        typedef SoundBufferId Value;
};

template<>
struct ResourceIdTrait<sf::Shader>
{
        typedef ShaderId Value;
};

template<class Resource>
using ResourceIdValue = typename ResourceIdTrait<Resource>::Value;

template<class Resource>
class ResourceHolder
{
    public:

        typedef ResourceIdValue<Resource> ResourceId;

        ResourceHolder(const std::filesystem::path& resourcePath)
            : mResourcePath(resourcePath)
        {

        }

        template<class... Args>
        void load(ResourceId id, const std::filesystem::path& resourceRelativePath, Args&& ...args)
        {
            int idIndex = static_cast<int>(id);
            assert(!mResources[idIndex] && "A resource with this ID has already been loaded");
            mResources[idIndex] = std::make_unique<Resource>(mResourcePath / resourceRelativePath,
                                                             std::forward<Args>(args)...);
        }

        const Resource& get(ResourceId id) const
        {
            int idIndex = static_cast<int>(id);
            assert(mResources[idIndex] && "Tried to get a resource that was not loaded");
            return *mResources[idIndex];
        }

        Resource& get(ResourceId id)
        {
            return const_cast<Resource&>(const_cast<const ResourceHolder*>(this)->get(id));
        }

        const std::filesystem::path& getResourcePath() const
        {
            return mResourcePath;
        }

    private:

        // [typedefs]

        typedef std::unique_ptr<Resource> ResourcePtr;

    private:

        // [static members]

        constexpr static int kNumberOfResources = static_cast<int>(ResourceId::Null);

    private:

        // [members]

        std::filesystem::path mResourcePath;
        std::array<ResourcePtr, kNumberOfResources> mResources;
};

typedef ResourceHolder<sf::Texture> TextureHolder;
typedef ResourceHolder<sf::Font> FontHolder;
typedef ResourceHolder<sf::SoundBuffer> SoundBufferHolder;
typedef ResourceHolder<sf::Shader> ShaderHolder;




