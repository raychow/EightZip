// VirtualRootModel.h

#ifndef VIRTUALROOTMODEL_H
#define VIRTUALROOTMODEL_H

#include <memory>

#include "ArchiveProperty.h"
#include "VirtualModel.h"

class ProgressDialog;

class ArchiveHandler
{
public:
    ArchiveHandler(TString tstrPath,
        ProgressDialog *pProgressDialog);
    ArchiveHandler(TString tstrPath,
        shared_ptr<SevenZipCore::IInStream> cpStream,
        ProgressDialog *pProgressDialog);

    inline SevenZipCore::Archive &GetArchive() const
    {
        return *m_upArchive;
    }

    inline ArchiveProperty &GetProperty()
    {
        return m_property;
    }
        
private:
    std::unique_ptr<SevenZipCore::Archive> m_upArchive;
    ArchiveProperty m_property;

};

class VirtualRootModel
    : private ArchiveHandler
    , public VirtualModel
{
public:
    // Open stream.
    VirtualRootModel(TString tstrLocation,
        TString tstrName,
        std::shared_ptr<VirtualModel> spParent,
        std::shared_ptr<SevenZipCore::IInStream> cpStream,
        ProgressDialog *pProgressDialog);
    // Open real file.
    VirtualRootModel(TString tstrLocation,
        TString tstrName,
        TString tstrRealPath,
        std::shared_ptr<VirtualModel> spParent,
        ProgressDialog *pProgressDialog);
    virtual ~VirtualRootModel() {}

    using VirtualModel::GetArchive;
    using VirtualModel::GetProperty;

};

#endif // VIRTUALROOTMODEL_H
