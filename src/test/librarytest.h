#pragma once

#include <memory>

#include "test/mixxxtest.h"

#include "database/mixxxdb.h"
#include "library/trackcollection.h"
#include "library/trackcollectionmanager.h"
#include "util/db/dbconnectionpooler.h"
#include "util/db/dbconnectionpooled.h"

class LibraryTest : public MixxxTest {
  protected:
    LibraryTest();
    ~LibraryTest() override;

    const mixxx::DbConnectionPoolPtr& dbConnectionPool() const {
        return m_dbConnectionPooler;
    }

    QSqlDatabase dbConnection() const {
        return mixxx::DbConnectionPooled(m_dbConnectionPooler);
    }

    TrackCollectionManager* trackCollections() {
        return m_pTrackCollectionManager.get();
    }

    TrackCollection* internalCollection() {
        return trackCollections()->internalCollection();
    }

    // Blocks until all background tasks after adding the
    // track (like cover art guessing) have been finished.
    // Otherwise timing issues may cause test failures!
    TrackPointer getOrAddTrackByLocationBlocking(
            const QString& trackLocation);

  private:
    const MixxxDb m_mixxxDb;
    const mixxx::DbConnectionPooler m_dbConnectionPooler;
    const std::unique_ptr<TrackCollectionManager> m_pTrackCollectionManager;
};
