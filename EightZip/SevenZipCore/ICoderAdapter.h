// ICoderAdapter.h

#ifndef SEVENZIPCORE_ICODERADAPTER_H
#define SEVENZIPCORE_ICODERADAPTER_H

#include <memory>

#include "Adapter.h"
#include "ICoder.h"

namespace SevenZipCore
{
    template<typename T = std::shared_ptr<ISetCompressCodecsInfo>>
    class ISetCompressCodecsInfoAdapter
        : protected virtual Adapter < T >
    {
    public:
        explicit ISetCompressCodecsInfoAdapter(T target)
            : Adapter(target)
        {
        }

        using Adapter<T>::QueryInterface;
        using Adapter<T>::GetTarget;

        void SetCompressCodecsInfo(ICompressCodecsInfo &compressCodecsInfo) const
        {
            EnsureOk<LibraryException>(
                GetTarget()->SetCompressCodecsInfo(&compressCodecsInfo),
                "Cannot set compress codecs info.");
        }
    };
}

#endif // SEVENZIPCORE_ICODERADAPTER_H
