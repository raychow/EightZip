#include "ArchiveManager.h"

SevenZipCore::ArchiveManager::ArchiveManager(const TString &tstrBaseFolder)
    : m_cpCodecs(MakeComPtr(new Codecs(tstrBaseFolder)))
    , m_upArchive(new Archive(m_cpCodecs))
{

}

SevenZipCore::ArchiveManager::ArchiveManager(std::shared_ptr<Codecs> cpCodecs)
    : m_cpCodecs(cpCodecs)
    , m_upArchive(new Archive(cpCodecs))
{

}

SevenZipCore::ArchiveManager::~ArchiveManager()
{
    Close();
}

void SevenZipCore::ArchiveManager::Open(
    TString tstrPath,
    std::shared_ptr<IArchiveOpenCallback> cpCallback)
{
    m_upArchive.reset(new Archive(m_cpCodecs, tstrPath, move(cpCallback)));
}

void SevenZipCore::ArchiveManager::Close()
{
    m_upArchive.release();
}
