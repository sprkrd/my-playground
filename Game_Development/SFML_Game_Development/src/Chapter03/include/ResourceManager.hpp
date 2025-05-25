#pragma once

#include "ResourceHolder.hpp"

class ResourceManager
{
    public:

        static ResourceManager& getGlobalResourceManager();

        static void initGlobalResourceManager(const char* exePath);

        static ResourceManager fromExePath(const char* exePath);

    public:

        explicit ResourceManager(const std::filesystem::path& assetsPath);

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

        template<class Resource>
        const std::filesystem::path& getResourcePath() const
        {
            return getResourceHolder<Resource>().getResourcePath();
        }

    private:

        static std::optional<ResourceManager> globalResourceManager;

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
        TextureHolder mTextureHolder;
        FontHolder mFontHolder;
        SoundBufferHolder mSoundBufferHolder;
        ShaderHolder mShaderHolder;
};
