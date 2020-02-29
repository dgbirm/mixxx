#include "test/librarytest.h"

#include <QThreadPool>

namespace {

const bool kInMemoryDbConnection = true;

void deleteTrack(Track* pTrack) {
    // Delete track objects directly in unit tests with
    // no main event loop
    delete pTrack;
};

std::unique_ptr<TrackCollectionManager> newTrackCollectionManager(
        UserSettingsPointer userSettings,
        mixxx::DbConnectionPoolPtr dbConnectionPool) {
    const auto dbConnection = mixxx::DbConnectionPooled(dbConnectionPool);
    if (!MixxxDb::initDatabaseSchema(dbConnection)) {
        return nullptr;
    }
    return std::make_unique<TrackCollectionManager>(
            nullptr,
            std::move(userSettings),
            std::move(dbConnectionPool),
            deleteTrack);
}

}

LibraryTest::LibraryTest()
    : m_mixxxDb(config(), kInMemoryDbConnection),
      m_dbConnectionPooler(m_mixxxDb.connectionPool()),
      m_pTrackCollectionManager(newTrackCollectionManager(config(), m_dbConnectionPooler)) {
}

LibraryTest::~LibraryTest() {
    // Ensure that all concurrent tasks have finished before
    // exiting. Otherwise test cases may fail with a SEGFAULT!
    QThreadPool::globalInstance()->clear();
    QThreadPool::globalInstance()->waitForDone();
}

TrackPointer LibraryTest::getOrAddTrackByLocationBlocking(
        const QString& trackLocation) {
    TrackPointer pTrack = internalCollection()->getOrAddTrack(
            TrackRef::fromFileInfo(trackLocation));
    QThreadPool::globalInstance()->waitForDone();
    return pTrack;
}
