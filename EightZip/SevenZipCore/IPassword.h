// IPassword.h

#ifndef IPASSWORD_H
#define IPASSWORD_H

#include "Platform.h"

#include "IDecl.h"

#define PASSWORD_INTERFACE(i, x) DECL_INTERFACE(i, 5, x)

PASSWORD_INTERFACE(ICryptoGetTextPassword, 0x10)
{
    STDMETHOD(CryptoGetTextPassword)(BSTR *password) PURE;
};

PASSWORD_INTERFACE(ICryptoGetTextPassword2, 0x11)
{
    STDMETHOD(CryptoGetTextPassword2)(INT32 *passwordIsDefined, BSTR *password) PURE;
};

#endif // IPASSWORD_H
