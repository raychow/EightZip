// Exception.h

#ifndef SEVENZIPCORE_EXCEPTION_H
#define SEVENZIPCORE_EXCEPTION_H

#include <exception>
#include <memory>
#include <string>

#define DECLARE_EXCEPTION(exception, base) \
    class exception \
        : public base \
    { \
    public: \
        exception(const char *pchMessage) \
            : base(pchMessage) \
        {} \
        \
    };

#define DECLARE_EXCEPTION_WITH_ERROR_CODE(exception, base, error_code_type) \
    class exception \
        : public base \
    { \
    public: \
        exception(error_code_type errorCode, const char *pchMessage) \
            : base(pchMessage) \
            , m_errorCode(errorCode) \
        {} \
        \
        error_code_type GetErrorCode() const { return m_errorCode; } \
        \
    private: \
        error_code_type m_errorCode; \
        \
    };

namespace SevenZipCore
{
    enum PropertyErrorCode
    {
        INVALID_VALUE_TYPE,
        EMPTY_VALUE,
    };

    class SevenZipCoreException
        : public std::runtime_error
    {
    public:
        SevenZipCoreException(const char *pchMessage)
            : runtime_error(pchMessage)
        {
        }

    };

    class SystemException
        : public SevenZipCoreException
    {
    public:
        SystemException(const char *pchMessage)
            : SevenZipCoreException(pchMessage)
            , m_nErrorCode(::GetLastError())
        {
        }

        int GetErrorCode() const { return m_nErrorCode; }

    private:
       int m_nErrorCode;

    };

    DECLARE_EXCEPTION(LibraryException, SevenZipCoreException);
    DECLARE_EXCEPTION(StreamException, SevenZipCoreException);
    DECLARE_EXCEPTION(ArchiveException, SevenZipCoreException);
    DECLARE_EXCEPTION(FileException, SevenZipCoreException);
    DECLARE_EXCEPTION_WITH_ERROR_CODE(
        PropertyException,
        SevenZipCoreException,
        PropertyErrorCode);
}

#endif // SEVENZIPCORE_EXCEPTION_H