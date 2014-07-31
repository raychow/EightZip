// Exception.h

#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <exception>
#include <memory>
#include <string>

#define EIGHT_ZIP_EXCEPTION(e) \
class e \
    : public EightZipException \
            { \
public: \
    e(const std::string& _Message) \
    : EightZipException(_Message) \
    {} \
    e(const char *_Message) \
    : EightZipException(_Message) \
    {} \
};

class EightZipException
    : public std::runtime_error
{
public:
    EightZipException(const std::string& _Message)
        : runtime_error(_Message)
    {}

    EightZipException(const char *_Message)
        : runtime_error(_Message)
    {}

protected:
    std::exception m_innerException;
};

EIGHT_ZIP_EXCEPTION(ModelException);
EIGHT_ZIP_EXCEPTION(SystemException);

#endif // EXCEPTION_H
