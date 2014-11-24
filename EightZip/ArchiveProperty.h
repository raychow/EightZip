// ArchiveProperty.h

#ifndef ARCHIVEPROPERTY_H
#define ARCHIVEPROPERTY_H

#include <boost/optional.hpp>

#include "SevenZipCore/TString.h"

class ArchiveProperty
{
public:
    boost::optional<TString> GetPassword() const;
    void SetPassword(TString tstrPassword)
    {
        m_otstrPassword = move(tstrPassword);
    }
    void ClearPassword() { m_otstrPassword.reset(); }
    bool IsSetPassword() const { return m_otstrPassword; }

private:
    mutable boost::optional<TString> m_otstrPassword;

};

#endif // ARCHIVEPROPERTY_H
