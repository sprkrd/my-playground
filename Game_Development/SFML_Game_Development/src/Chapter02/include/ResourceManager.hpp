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
constexpr int kNumberOfTextures = static_cast<int>(TextureId::Null);

enum class FontId {
    GameOver,
    Null
};
constexpr int kNumberOfFonts = static_cast<int>(FontId::Null);

enum class SoundBufferId {
    Null
};
constexpr int kNumberOfSoundFx = static_cast<int>(SoundBufferId::Null);

enum class ShaderId {
    Null
};
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

        ResourceHolder(const std::filesystem::path& resourcePath);

        template<class... Args>
        void load(ResourceId id, const std::filesystem::path& resourceRelativePath, Args&& ...args)
        {
            int idIndex = static_cast<int>(id);
            assert(!mResources[idIndex] && "A resource with this ID has already been loaded");
            mResources[idIndex] = std::make_unique<Resource>(mResourcePath / resourceRelativePath,
                                                             std::forward<Args>(args)...);
        }

        const Resource& get(ResourceId id) const;

        Resource& get(ResourceId id);

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

class ResourceManager
{
    public:

        static ResourceManager fromExePath(const char* exePath);

        ResourceManager(const std::filesystem::path& assetsPath);

        template<class Resource, class... Args>
        void load(ResourceIdValue<Resource> id, const std::filesystem::path& relativePath, Args&& ...args)
        {
            getResourceHolder<Resource>().load(id, relativePath, std::forward<Args>(args)...);
        }

        template<class Resource>
        const Resource& get(ResourceIdValue<Resource> id) const
        {
            return getResourceHolder<Resource>().get(id);
        }

        template<class Resource>
        Resource& get(ResourceIdValue<Resource> id)
        {
            return getResourceHolder<Resource>().get(id);
        }

        const std::filesystem::path& getAssetsPath() const
        {
            return mAssetsPath;
        }

    private:

        // [private methods]

        template<class Resource>
        const ResourceHolder<Resource>& getResourceHolder() const;

        template<class Resource>
        ResourceHolder<Resource>& getResourceHolder()
        {
            const auto* const_this = this;
            return const_cast<ResourceHolder<Resource>&>(const_this->getResourceHolder<Resource>());
        }

    private:

        // [private members]

        std::filesystem::path mAssetsPath;
        ResourceHolder<sf::Texture> mTextureHolder;
        ResourceHolder<sf::Font> mFontHolder;
        ResourceHolder<sf::SoundBuffer> mSoundBufferHolder;
        ResourceHolder<sf::Shader> mShaderHolder;
};

