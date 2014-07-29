// Exception.h

#ifndef SEVENZIPCORE_EXCEPTION_H
#define SEVENZIPCORE_EXCEPTION_H

#include <exception>
#include <memory>
#include <string>

#define SEVEN_ZIP_CORE_EXCEPTION(e) \
    class e \
        : public SevenZipCoreException \
    { \
    public: \
        e(const std::string& _Message) \
        : SevenZipCoreException(_Message) \
        {} \
        e(const char *_Message) \
        : SevenZipCoreException(_Message) \
        {} \
    };

namespace SevenZipCore
{
    class SevenZipCoreException
        : public std::runtime_error
    {
    public:
        SevenZipCoreException(const std::string& _Message)
            : runtime_error(_Message)
        {}

        SevenZipCoreException(const char *_Message)
            : runtime_error(_Message)
        {}

    protected:
        std::exception m_innerException;
    };

    SEVEN_ZIP_CORE_EXCEPTION(LibraryException);
    SEVEN_ZIP_CORE_EXCEPTION(CodecException);
    SEVEN_ZIP_CORE_EXCEPTION(StreamException);
    SEVEN_ZIP_CORE_EXCEPTION(ArchiveException);
    SEVEN_ZIP_CORE_EXCEPTION(PropertyException);
}

#endif // SEVENZIPCORE_EXCEPTION_H