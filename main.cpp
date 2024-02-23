#include <QApplication>
#include <QFile>
#include <QImage>
#include <QPainter>
#include <QMap>

#include "MapWidget.h"

#include "TileURL.h"

QString getPbfLink(const TileCoord &tileCoord, const QString &pbfLinkTemplate)
{
    // Exchange the {x, y z} in link
    auto copy = pbfLinkTemplate;
    copy.replace("{z}", QString::number(tileCoord.zoom));
    copy.replace("{x}", QString::number(tileCoord.x));
    copy.replace("{y}", QString::number(tileCoord.y));
    return copy;
}

QByteArray downloadTile(const QString &pbfLink)
{
    NetworkController controller;
    return controller.sendRequest(pbfLink);
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    // Gets different URLs to download PBF tiles.
    TileURL tileUrl;

     // Read key from file.
    QString mapTilerKey = tileUrl.readKey("key.txt");
    if(mapTilerKey == "")
        return 0;

    // Picks stylesheet to load and loads it.
    auto styleSheetType = TileURL::styleSheetType::basic_v2;
    auto styleSheetBytes = tileUrl.loadStyleSheetFromWeb(mapTilerKey, styleSheetType);

    // Gets the link template where we have to switch out x, y,z in the link.
    auto pbfLinkTemplate = tileUrl.getPbfLinkTemplate(styleSheetBytes, "maptiler_planet");

    // Creates the Widget that displays the map.
    MapWidget mapWidget;
    auto &styleSheet = mapWidget.styleSheet;
    auto &tileStorage = mapWidget.tileStorage;

    // Parses the bytes that form the stylesheet into a json-document object.
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(styleSheetBytes, &parseError);
    if (parseError.error != QJsonParseError::NoError) {
        qWarning() << "Parse error at" << parseError.offset << ":" << parseError.errorString();
        return 0;
    }
    // Then finally parse the JSonDocument into our StyleSheet.
    styleSheet.parseSheet(doc);

    // Download tiles, or load them from disk, then insert into the tile-storage map.
    auto tile000 = Bach::tileFromByteArray(downloadTile(getPbfLink({0, 0, 0}, pbfLinkTemplate)));
    tileStorage.insert({0, 0, 0}, &tile000);
    auto tile100 = Bach::tileFromByteArray(downloadTile(getPbfLink({1, 0, 0}, pbfLinkTemplate)));
    tileStorage.insert({1, 0, 0}, &tile100);
    auto tile101 = Bach::tileFromFile(Bach::testDataDir + "z1x0y1.mvt");
    tileStorage.insert({1, 0, 1}, &tile101);
    auto tile110 = Bach::tileFromByteArray(downloadTile(getPbfLink({1, 1, 0}, pbfLinkTemplate)));
    tileStorage.insert({1, 1, 0}, &tile110);
    auto tile111 = Bach::tileFromFile(Bach::testDataDir + "z1x1y1.mvt");
    tileStorage.insert({1, 1, 1}, &tile111);

    // This can download entire zoom levels and insert them
    // Temporarily commented out, can be used for testing.
    /*
    auto tileCount = 1 << 2;
    for (int x = 0; x < tileCount; x++) {
        for (int y = 0; y < tileCount; y++) {
            auto temp = new VectorTile(Bach::tileFromByteArray(download({2, x, y})));
            tileStorage.insert({2, x, y}, temp);
        }
    }
    {
        auto tileCount = 1 << 3;
        for (int x = 0; x < tileCount; x++) {
            for (int y = 0; y < tileCount; y++) {
                auto temp = new VectorTile(Bach::tileFromByteArray(download({3, x, y})));
                tileStorage.insert({3, x, y}, temp);
            }
        }
    }
    */

    mapWidget.show();

    return a.exec();
}
