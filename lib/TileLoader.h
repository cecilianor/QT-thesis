#ifndef GETTILEURL_H
#define GETTILEURL_H

#include <QObject>
#include <QUrl>
#include <QJsonObject>
#include <QJsonArray>
#include <QMutex>
#include <QMutexLocker>
#include <QNetworkAccessManager>
#include <QThreadPool>

#include <set>
#include <functional>
#include <memory>
#include <map>

#include "TileCoord.h"
#include "VectorTiles.h"
#include "RequestTilesResult.h"

class UnitTesting;

namespace Bach {
    enum class LoadedTileState {
        Ok,
        Pending,
        ParsingFailed,
        Cancelled,
        UnknownError
    };
}

class TileLoader : public QObject
{
    Q_OBJECT

private:
    // Please use the static creator functions.
    // This constructor alone will not guarantee you a functional
    // TileLoader object.
    TileLoader();
public:

    ~TileLoader(){};

    static QString getGeneralCacheFolder();
    static QString getTileCacheFolder();

    static std::unique_ptr<TileLoader> fromPbfLink(
        const QString &pbfUrlTemplate,
        StyleSheet&& styleSheet);
    static std::unique_ptr<TileLoader> newLocalOnly(StyleSheet&& styleSheet);
    // We can't return by value, because TileLoader is a QObject and therefore
    // doesn't support move-semantics.
    static std::unique_ptr<TileLoader> newDummy(const QString &diskCachePath);

    /*!
     * @brief Gets the full file-path of a given tile.
     */
    QString getTileDiskPath(TileCoord coord);

    /*!
     * @brief Loads the tile-state of a given tile, if it has been loaded in some form.
     * Mostly used in tests to see if tiles were put into correct state.
     *
     * Thread safe.
     */
    std::optional<Bach::LoadedTileState> getTileState(TileCoord) const;

signals:
    void tileFinished(TileCoord);

private:
    //QByteArray styleSheet;
    //QByteArray JSONTileURL;
    //QUrl tileURL;
    //NetworkController networkController;


    StyleSheet styleSheet;
    QString pbfLinkTemplate;
    QNetworkAccessManager networkManager;
    bool useWeb = true;
    QString tileCacheDiskPath;

    struct StoredTile {
        Bach::LoadedTileState state = {};
        // We use std::unique_ptr over QScopedPointer
        // because QScopedPointer doesn't support move semantics.
        std::unique_ptr<VectorTile> tile;

        // Tells us whether this tile is safe to return to
        // rendering.
        bool isReadyToRender() const { return state == Bach::LoadedTileState::Ok; }

        static StoredTile newPending()
        {
            StoredTile temp;
            temp.state = Bach::LoadedTileState::Pending;
            return temp;
        }
    };
    /* This contains our memory tile-cache.
     *
     * The value is a pointer to the loaded tile.
     *
     * IMPORTANT! Only use when 'tileMemoryLock' is locked!
     *
     * We had to use std::map because QMap doesn't support move semantics,
     * which interferes with our automated resource cleanup.
     */
    std::map<TileCoord, StoredTile> tileMemory;
    // We use unique-ptr here to let use the lock in const methods.
    std::unique_ptr<QMutex> _tileMemoryLock = std::make_unique<QMutex>();
    QMutexLocker<QMutex> createTileMemoryLocker() const { return QMutexLocker(_tileMemoryLock.get()); }

public:
    using TileLoadedCallbackFn = std::function<void(TileCoord)>;

    /*!
     * @brief Grabs loaded tiles and enqueues loading tiles that are missing.
     *        This function does not block and is not re-entrant as
     *        this may get called from a QWidget paint event.
     *
     *        Returns the set of tiles that are already in memory at the
     *        time of the request. To grab new tiles as they are loaded,
     *        use the tileLoadedSignalFn parameter and call this function again.
     *
     * @param requestInput is a set of TileCoords that is requested.
     *
     * @param tileLoadedSignalFn is a function that will get called whenever
     *        a tile is loaded, will be called later in time,
     *        can be called from another thread. This function
     *        will be called once for each tile that was loaded successfully.
     *
     *        This function is only called if a tile is successfully loaded.
     *
     *        If this argument is set to null, the missing tiles will not be loaded.
     *
     * @return Returns a RequestTilesResult object containing
     *         the resulting map of tiles. The returned set of
     *         data will always be a subset of all currently loaded tiles.
     */
    QScopedPointer<Bach::RequestTilesResult> requestTiles(
        const std::set<TileCoord> &requestInput,
        TileLoadedCallbackFn tileLoadedSignalFn,
        bool loadMissingTiles);

    auto requestTiles(
        const std::set<TileCoord> &requestInput,
        bool loadMissingTiles)
    {
        return requestTiles(requestInput, nullptr, loadMissingTiles);
    }

    auto requestTiles(
        const std::set<TileCoord> &requestInput,
        TileLoadedCallbackFn tileLoadedSignalFn = nullptr)
    {
        return requestTiles(requestInput, tileLoadedSignalFn, static_cast<bool>(tileLoadedSignalFn));
    }

private:
    /*!
     * @brief
     * Loads the list of tiles into memory, corresponding to the list of
     * TileCoords inputted.
     *
     * This function launches asynchronous jobs, does not block execution!
     */
    void queueTileLoadingJobs(
        const QVector<TileCoord> &input,
        TileLoadedCallbackFn signalFn);

    QThreadPool threadPool;
    QThreadPool &getThreadPool() { return threadPool; }
    bool loadFromDisk(TileCoord coord, TileLoadedCallbackFn signalFn);
    void networkReplyHandler(
        QNetworkReply* reply,
        TileCoord coord,
        TileLoadedCallbackFn signalFn);
    void loadFromWeb(TileCoord coord, TileLoadedCallbackFn signalFn);
    void writeTileToDisk(TileCoord coord, const QByteArray &bytes);
    void queueTileParsing(
        TileCoord coord,
        QByteArray byteArray,
        TileLoadedCallbackFn signalFn);
    void insertTile(
        TileCoord coord,
        const QByteArray &byteArray,
        TileLoadedCallbackFn signalFn);
};

namespace Bach {
    QString setPbfLink(TileCoord tileCoord, const QString &pbfLinkTemplate);

    bool writeTileToDiskCache(
        const QString& basePath,
        TileCoord coord,
        const QByteArray &bytes);

    QString tileDiskCacheSubPath(TileCoord coord);
}


#endif // GETTILEURL_H
