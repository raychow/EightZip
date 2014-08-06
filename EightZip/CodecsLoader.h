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
    
    std::shared_ptr<SevenZipCore::Codecs> GetCodecs() const
    {
        return m_cpCodecs;
    }

private:
    CodecsLoader();
    CodecsLoader(const CodecsLoader &);
    CodecsLoader& operator=(const CodecsLoader &);

    std::shared_ptr<SevenZipCore::Codecs> m_cpCodecs;

};

#endif // CODECSLOADER_H
