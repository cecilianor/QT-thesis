#ifndef MAPWIDGET_H
#define MAPWIDGET_H

#include <QWidget>
#include <QScopedPointer>


#include <functional>
#include <set>

#include "TileCoord.h"
#include "RequestTilesResult.h"

/* Widget class responsible for displaying the actual map.
 * Should be used as a smaller widget within a larger Widget hierarchy.
 *
 * This MapWidget has a built-in viewport configuration (zoom level and center coordinates).
 */
class MapWidget : public QWidget
{
    Q_OBJECT

public:
    MapWidget(QWidget *parent = nullptr);
    ~MapWidget();

    void paintEvent(QPaintEvent*) override;
    void keyPressEvent(QKeyEvent*) override;

    /* Zoom level of viewport. This is a floating number and can be partially zoomed between
     * discrete steps.
     *
     * Range [0, 16]
     */
    double getViewportZoomLevel() const;

    /* Returns the zoom level of the map based on the widget's
     * current viewport configuration.
     *
     * Always an integer in the range [0, 16]
     */
    int getMapZoomLevel() const;

    /* Calculates the set of visible tiles
     * based on the widget's current viewport configuration.
     */
    QVector<TileCoord> calcVisibleTiles() const;

    /* Returns how long the viewport counts as one step
     * during panning.
     *
     * The returned value is a distance in world-normalized coordinates, range [0, 1].
     */
    double getPanStepAmount() const;

    /*! Function signature of the tile-request callback.
     *
     * Returns a ScopedPointer containing a RequestTilesResult object.
     * RequestTilesResult object contains map of returned tiles.
     * May have custom logic in destructor.
     * MapWidget should release this object as early as possible.
     *
     * \param First parameter is a set of TileCoordinates to request.
     *
     * \param Second parameter is a callback to signal when a tile is loaded later.
     *        For this widget, it will signal the widget to redraw itself with the new result.
     */
    using RequestTilesFnT =
        QScopedPointer<Bach::RequestTilesResult>(
            const std::set<TileCoord>&,
            std::function<void(TileCoord)>);
    std::function<RequestTilesFnT> requestTilesFn;

private:
    // Zoom level of viewport
    // Range [0, 16]
    // This should never go below 0 in final release, but
    // is useful for testing.
    double viewportZoomLevel = 0;

    // Default should be 0 for final release.
    // This offsets the map zoom compared to viewport zoom.
    double mapZoomLevelOffset = 0;

    // Mostly useful for debugging.
    // Allows you to lock the zoom level of the tiles
    // that are displayed.
    bool overrideMapZoom = false;
    int overrideMapZoomLevel = 0;

    // Center of viewport X
    // Range [0, 1]
    double x = 0.5;
    // Center of viewport Y
    // Range [0, 1]
    double y = 0.5;

    // Pass in true if you want to zoom.
    // Applies a single zoom step to the viewport.
    void genericZoom(bool magnify);


    // Controls whether debug lines should be shown.
    bool showDebug = true;
    bool renderVectorTile = true;
public:
    bool isShowingDebug() const { return showDebug; }
    bool isRenderingVector() const { return renderVectorTile; }
public slots:
    void toggleIsShowingDebug();
    void toggleIsRenderingVectorTile();

public:
    /* We run into problems with UI keyboard navigation
     * when we want to use arrows for panning our MapWidget.
     *
     * This class filters the arrow key events of the QApplication
     * and feeds them directly to the MapWidget whenever it's alive.
     *
     * This could be problematic if we want to support UI keyboard navigation
     * in the future, should be adjusted to allow keyboard navigation with tab,
     * and only intercept these particular events when MapWidget is focused.
     */
    class KeyPressFilter : public QObject {
    public:
        KeyPressFilter(MapWidget *mapWidget) : mapWidget{ mapWidget } {}
    protected:
        /* This is the function that intercepts the events that go to the QApplication.
         */
        bool eventFilter(QObject*, QEvent*) override;
    private:
        MapWidget *mapWidget = nullptr;
    };
private:
    // Handle to the installed event-filter.
    std::unique_ptr<KeyPressFilter> keyPressFilter = nullptr;

public slots:
    /* Updates the center coordinates and the zoom-level of the viewport.
     */
    void setViewport(double x, double y, double zoom);

    // Slot for zooming in a single step.
    void zoomIn();
    // Slot for zooming out a single step.
    void zoomOut();
    // Slot for panning up a single step.
    void panUp();
    // Slot for panning down a single step.
    void panDown();
    // Slot for panning left a single step.
    void panLeft();
    // Slot for panning right a single step.
    void panRight();
};

#endif // MAPWIDGET_H
