#ifndef VIEWPORT_H
#define VIEWPORT_H

/*
 *  This file is a part of KNOSSOS.
 *
 *  (C) Copyright 2007-2013
 *  Max-Planck-Gesellschaft zur Foerderung der Wissenschaften e.V.
 *
 *  KNOSSOS is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 of
 *  the License as published by the Free Software Foundation.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * For further information, visit http://www.knossostool.org or contact
 *     Joergen.Kornfeld@mpimf-heidelberg.mpg.de or
 *     Fabian.Svara@mpimf-heidelberg.mpg.de
 */

#include <QWidget>
#include <QtOpenGL/QGLWidget>
#include <QtOpenGL>
#include <QDebug>
#include <QFont>
#include "eventmodel.h"

#define VP_UPPERLEFT 0
#define VP_LOWERLEFT 1
#define VP_UPPERRIGHT 2
#define VP_LOWERRIGHT 3
static int focus; /* This variable is needed to distinguish the viewport in case of key events. Needed for OSX, don´t remove */

class QPushButton;
class Renderer;

class ResizeButton : public QPushButton {
public:
    static const int SIZE = 22;
    explicit ResizeButton(QWidget *parent);
protected:
    void enterEvent(QEvent *);
    void leaveEvent(QEvent *);
};

class ViewportButton : public QPushButton {
public:
    static const int WIDTH = 35;
    static const int HEIGHT = 20;

    explicit ViewportButton(QString label, QWidget *parent);

protected:
    void enterEvent(QEvent *);
    void leaveEvent(QEvent *);
};

class Viewport : public QGLWidget
{
    Q_OBJECT
public:
    explicit Viewport(QWidget *parent, int viewportType, uint newId);
    void drawViewport(int vpID);
    void drawSkeletonViewport();
    void hideButtons();
    void showButtons();
    void updateButtonPositions();
    Renderer *reference;
    EventModel *eventDelegate;

    static const int MIN_VP_SIZE = 50;

protected:
    void initializeGL();
    void initializeOverlayGL();
    void resizeGL(int w, int h);
    void paintGL();
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
    void enterEvent(QEvent *event);
    //void leaveEvent(QEvent *event);

    int xrel(int x);
    int yrel(int y);
    uint id; // VP_UPPERLEFT, ...
    int viewportType; // XY_VIEWPORT, ...
    int lastX; //last x position
    int lastY; //last y position

    bool entered;
    QPushButton *resizeButton;
    QPushButton *xyButton, *xzButton, *yzButton, *r90Button, *r180Button, *resetButton;

private:
    bool resizeButtonHold;
    void resizeVP(QMouseEvent *event);
    void moveVP(QMouseEvent *event);
    bool handleMouseButtonLeft(QMouseEvent *event, int vpID);
    bool handleMouseButtonMiddle(QMouseEvent *event, int vpID);
    bool handleMouseButtonRight(QMouseEvent *event, int vpID);
    bool handleMouseMotionLeftHold(QMouseEvent *event, int vpID);
    bool handleMouseMotionMiddleHold(QMouseEvent *event, int vpID);
    bool handleMouseMotionRightHold(QMouseEvent *event, int vpID);
    bool handleMouseWheelForward(QWheelEvent *event, int vpID);
    bool handleMouseWheelBackward(QWheelEvent *event, int vpID);
    bool handleMouseReleaseLeft(QMouseEvent *event, int vpID);
signals:    
    void recalcTextureOffsetsSignal();
    void runSignal();
    void changeDatasetMagSignal(uint upOrDownFlag);
    void updateZoomAndMultiresWidget();
    void loadSkeleton(const QString &path);
public slots:
    void zoomOrthogonals(float step);
    void zoomInSkeletonVP();
    void zoomOutSkeletonVP();
    void resizeButtonClicked();
    void xyButtonClicked();
    void xzButtonClicked();
    void yzButtonClicked();
    void r90ButtonClicked();
    void r180ButtonClicked();
    void resetButtonClicked();
    bool setOrientation(int orientation);
};

#endif // VIEWPORT_H
