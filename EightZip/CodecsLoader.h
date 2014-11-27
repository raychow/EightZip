// CodecsLoader.h

#ifndef CODECSLOADER_H
#define CODECSLOADER_H

#include <memory>

#include "SevenZipCore/Codecs.h"

class CodecsLoader
{
public:
    static CodecsLoader &GetInstance()
    {
        static CodecsLoader codecsLoader;
        return codecsLoader;
    }
    
    std::shared_ptr<SevenZipCore::Codecs> GetCodecs() const;

private:
    CodecsLoader() {}
    CodecsLoader(const CodecsLoader &) = delete;
    CodecsLoader& operator=(const CodecsLoader &) = delete;

    mutable std::weak_ptr<SevenZipCore::Codecs> m_wpCodecs;

};

#endif // CODECSLOADER_H
