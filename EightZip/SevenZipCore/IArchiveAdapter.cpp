#include "IArchiveAdapter.h"

#include "ComPtr.h"
#include "Exception.h"

namespace SevenZipCore
{
    IMPLEMENT_ADAPTER_CONSTRUCTOR(IInArchive)
    IMPLEMENT_IINARCHIVE_ADAPTER(IInArchive)

    IMPLEMENT_ADAPTER_CONSTRUCTOR(IInArchiveGetStream)
    IMPLEMENT_IINARCHIVEGETSTREAM_ADAPTER(IInArchiveGetStream)
}