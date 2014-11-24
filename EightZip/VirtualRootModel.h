// VirtualRootModel.h

#ifndef VIRTUALROOTMODEL_H
#define VIRTUALROOTMODEL_H

#include <memory>

#include "VirtualModel.h"

class ArchiveHandler
{
public:
    ArchiveHandler(std::unique_ptr<SevenZipCore::Archive> upArchive)
        : m_upArchive(move(upArchive))
    { }

    SevenZipCore::Archive &GetArchive() const
    {
        return *m_upArchive;
    }
        
private:
    std::unique_ptr<SevenZipCore::Archive> m_upArchive;

};

class VirtualRootModel
    : private ArchiveHandler
    , public VirtualModel
{
public:
    // Open stream.
    VirtualRootModel(TString tstrLocation,
        TString tstrName,
        std::shared_ptr<ModelBase> spParent,
        std::shared_ptr<SevenZipCore::IInStream> cpStream,
        SevenZipCore::IArchiveOpenCallback *pCallback);
    // Open real file.
    VirtualRootModel(TString tstrLocation,
        TString tstrName,
        TString tstrRealPath,
        std::shared_ptr<ModelBase> spParent,
        SevenZipCore::IArchiveOpenCallback *pCallback);
    virtual ~VirtualRootModel() {}

    using VirtualModel::GetArchive;

};

#endif // VIRTUALROOTMODEL_H
