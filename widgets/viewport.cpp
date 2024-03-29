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

#include "knossos-global.h"
#include "viewport.h"
#include "eventmodel.h"
#include "renderer.h"
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "sleeper.h"
#include "functions.h"
#include <QPainter>

extern stateInfo *state;

ResizeButton::ResizeButton(QWidget *parent) : QPushButton(parent) {}

void ResizeButton::enterEvent(QEvent *) {
    setCursor(Qt::SizeFDiagCursor);
}

void ResizeButton::leaveEvent(QEvent *) {
    setCursor(Qt::CrossCursor);
}

ViewportButton::ViewportButton(QString label, QWidget *parent) : QPushButton(label, parent) {}

void ViewportButton::enterEvent(QEvent *) {
    setCursor(Qt::ArrowCursor);
}

void ViewportButton::leaveEvent(QEvent *) {
    setCursor(Qt::CrossCursor);
}

Viewport::Viewport(QWidget *parent, int viewportType, uint newId) :
    QGLWidget(parent), viewportType(viewportType), id(newId), resizeButtonHold(false) {
    /* per default the widget only receives move event when at least one mouse button is pressed
    to change this behaviour we need to track the mouse position */

    //this->setMouseTracking(true);
    this->setCursor(Qt::CrossCursor);
    this->setFocusPolicy(Qt::WheelFocus); // this means the widget accepts mouse and keyboard focus.
                                          // This solves also the problem that viewports had to be clicked
                                          // before the widget know in which viewport the mouse click occured.

    resizeButton = new ResizeButton(this);
    resizeButton->setIcon(QIcon(":/images/icons/resize.gif"));
    resizeButton->show();

    connect(resizeButton, SIGNAL(pressed()), this, SLOT(resizeButtonClicked()));

    if(viewportType == VIEWPORT_SKELETON) {
        xyButton = new ViewportButton("xy", this);
        xzButton = new ViewportButton("xz", this);
        yzButton = new ViewportButton("yz", this);
        r90Button = new ViewportButton("r90", this);
        r180Button = new ViewportButton("r180", this);
        resetButton = new ViewportButton("reset", this);

        connect(xyButton, SIGNAL(clicked()), this, SLOT(xyButtonClicked()));
        connect(xzButton, SIGNAL(clicked()), this, SLOT(xzButtonClicked()));
        connect(yzButton, SIGNAL(clicked()), this, SLOT(yzButtonClicked()));
        connect(r90Button, SIGNAL(clicked()), this, SLOT(r90ButtonClicked()));
        connect(r180Button, SIGNAL(clicked()), this, SLOT(r180ButtonClicked()));
        connect(resetButton, SIGNAL(clicked()), this, SLOT(resetButtonClicked()));
    }



}

void Viewport::initializeGL() {
    // button geometry has to be defined here, because width() and height() return wrong information before initializeGL
    updateButtonPositions();
    if(viewportType != VIEWPORT_SKELETON) {
        glGenTextures(1, &state->viewerState->vpConfigs[id].texture.texHandle);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        glBindTexture(GL_TEXTURE_2D, state->viewerState->vpConfigs[id].texture.texHandle);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, state->viewerState->filterType);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, state->viewerState->filterType);

        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

        // loads an empty texture into video memory - during user movement, this
        // texture is updated via glTexSubImage2D in vpGenerateTexture
        // We need GL_RGB as texture internal format to color the textures

        glTexImage2D(GL_TEXTURE_2D,
                     0,
                     GL_RGB,
                     state->viewerState->vpConfigs[id].texture.edgeLengthPx,
                     state->viewerState->vpConfigs[id].texture.edgeLengthPx,
                     0,
                     GL_RGB,
                     GL_UNSIGNED_BYTE,
                     state->viewerState->defaultTexData);

        //Handle overlay textures.
    }

    // The following code configures openGL to draw into the current VP
    //set the drawing area in the window to our actually processed viewport.
    glViewport(state->viewerState->vpConfigs[id].upperLeftCorner.x,
               state->viewerState->vpConfigs[id].upperLeftCorner.y,
               state->viewerState->vpConfigs[id].edgeLength,
               state->viewerState->vpConfigs[id].edgeLength);
    //select the projection matrix
    glMatrixMode(GL_PROJECTION);
    //reset it
    glLoadIdentity();
    //define coordinate system for our viewport: left right bottom top near far
    //coordinate values
    glOrtho(0, state->viewerState->vpConfigs[id].edgeLength,
            state->viewerState->vpConfigs[id].edgeLength, 0, 25, -25);
    //select the modelview matrix for modification
    glMatrixMode(GL_MODELVIEW);
    //reset it
    glLoadIdentity();
    //glBlendFunc(GL_ONE_MINUS_DST_ALPHA,GL_DST_ALPHA);

    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

    QString versionString(QLatin1String(reinterpret_cast<const char*>(glGetString(GL_VERSION))));
    qDebug() << versionString;
}

bool Viewport::setOrientation(int orientation) {
    if(orientation != VIEWPORT_XY
       && orientation != VIEWPORT_XZ
       && orientation != VIEWPORT_YZ
       && orientation != VIEWPORT_ARBITRARY) {
        return false;
    }
    this->viewportType = orientation;
    state->viewerState->vpConfigs[id].type = orientation;
    return true;
}

void Viewport::initializeOverlayGL() {
    if(viewportType != VIEWPORT_SKELETON) {
        if(state->overlay) {
            glGenTextures(1, &state->viewerState->vpConfigs[id].texture.overlayHandle);
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

            glBindTexture(GL_TEXTURE_2D, state->viewerState->vpConfigs[id].texture.overlayHandle);

            //Set the parameters for the texture.
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, state->viewerState->filterType);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, state->viewerState->filterType);

            glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

            glTexImage2D(GL_TEXTURE_2D,
                         0,
                         GL_RGBA,
                         state->viewerState->vpConfigs[id].texture.edgeLengthPx,
                         state->viewerState->vpConfigs[id].texture.edgeLengthPx,
                         0,
                         GL_RGBA,
                         GL_UNSIGNED_BYTE,
                         state->viewerState->defaultOverlayData);
        }
    }
}

void Viewport::resizeGL(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    GLfloat x = (GLfloat)width() / height();

    glFrustum(-x, +x, -1.0, + 1.0, 0.1, 15.0);
    glMatrixMode(GL_MODELVIEW);

    SET_COORDINATE(state->viewerState->vpConfigs[id].upperLeftCorner,
                   geometry().topLeft().x(),
                   geometry().topLeft().y(),
                   0);
    state->viewerState->vpConfigs[id].edgeLength = width();
}

void Viewport::paintGL() {
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    if(state->viewerState->viewerReady) {
        if(this->viewportType != VIEWPORT_SKELETON) {
           this->drawViewport(id);
        }  else {
            this->drawSkeletonViewport();
        }
    }
}

//functions to determine position x/y relative to last position lastX, lastY
int Viewport::xrel(int x) {
    return (x - this->lastX);
}
int Viewport::yrel(int y) {
    return (y - this->lastY);
}


void Viewport::mouseMoveEvent(QMouseEvent *event) {
    bool clickEvent = false;

    if(QApplication::mouseButtons() == Qt::LeftButton) {
        Qt::KeyboardModifiers modifiers = QApplication::keyboardModifiers();
        bool ctrl = modifiers.testFlag(Qt::ControlModifier);
        bool alt = modifiers.testFlag(Qt::AltModifier);

        if(ctrl and alt) { // drag viewport around
            moveVP(event);
        }
        else if(resizeButtonHold) {// resize viewport
            resizeVP(event);
        }
        else {// delegate behaviour
            handleMouseMotionLeftHold(event, id);
            clickEvent = true;
        }
    } else if(QApplication::mouseButtons() == Qt::MidButton) {
        handleMouseMotionMiddleHold(event, id);
        clickEvent = true;
    } else if(QApplication::mouseButtons() == Qt::RightButton) {
        handleMouseMotionRightHold(event, id);
        clickEvent = true;
    }

    if(clickEvent) {
        eventDelegate->mouseX = event->x();
        eventDelegate->mouseY = event->y();
    }
}

void Viewport::mousePressEvent(QMouseEvent *event) {
    raise(); //bring this viewport to front
    eventDelegate->mouseX = event->x();
    eventDelegate->mouseY = event->y();
    if(event->button() == Qt::LeftButton) {
        Qt::KeyboardModifiers modifiers = QApplication::keyboardModifiers();
        bool ctrl = modifiers.testFlag(Qt::ControlModifier);
        bool alt = modifiers.testFlag(Qt::AltModifier);

        if(ctrl and alt) { // user wants to drag vp
            setCursor(Qt::ClosedHandCursor);
            lastX= event->x();
            lastY = event->y();
            return;
        }
        //this->move(event->x() - this->pos().x(), event->y() - pos().y());
        handleMouseButtonLeft(event, id);
    }
    else if(event->button() == Qt::MiddleButton) {
        handleMouseButtonMiddle(event, id);
    }
    else if(event->button() == Qt::RightButton) {
        handleMouseButtonRight(event, id);
    }
}

void Viewport::mouseReleaseEvent(QMouseEvent *event) {
    resizeButtonHold = false; // can only be true, when left mouse button is pressed
    Qt::KeyboardModifiers modifiers = QApplication::keyboardModifiers();
    bool ctrl = modifiers.testFlag(Qt::ControlModifier);
    bool alt = modifiers.testFlag(Qt::AltModifier);

    if(ctrl and alt) {
        setCursor(Qt::OpenHandCursor);
    }
    else if(cursor().shape() != Qt::CrossCursor) {
        setCursor(Qt::CrossCursor);
    }
    if(event->button() == Qt::MiddleButton) {
        eventDelegate->handleMouseReleaseMiddle(event, id);
    }
    if(event->button() == Qt::LeftButton) {
        handleMouseReleaseLeft(event, id);
    }

    for(int i = 0; i < state->viewerState->numberViewports; i++) {
        state->viewerState->vpConfigs[i].draggedNode = NULL;
        state->viewerState->vpConfigs[i].motionTracking = false;
        state->viewerState->vpConfigs[i].VPmoves = false;
        state->viewerState->vpConfigs[i].VPresizes = false;
        state->viewerState->vpConfigs[i].userMouseSlideX = 0.;
        state->viewerState->vpConfigs[i].userMouseSlideY = 0.;
    }
}

void Viewport::keyReleaseEvent(QKeyEvent *event) {
    if(event->key() == Qt::Key_Control) {
        setCursor(Qt::CrossCursor);
    }

    if(state->keyD) {
        state->keyD = false;
        state->autorepeat = false;
    }else if(state->keyF) {
        state->keyF = false;
        state->autorepeat = false;
    } else if(state->keyE){
        state->keyE = false;
    } else if(state->keyR){
        state->keyR = false;
    }
    state->newCoord[0] = 0;
    state->newCoord[1] = 0;
    state->newCoord[2] = 0;

    if(state->modCtrl) {
        state->modCtrl = false;
    }
    if(state->modAlt) {
        state->modAlt = false;
    }
    if(state->modShift) {
        state->modShift = false;
    }
}

void Viewport::wheelEvent(QWheelEvent *event) {
    if(event->delta() > 0) {
        handleMouseWheelForward(event, id);
    } else {
        handleMouseWheelBackward(event, id);
    }
}

void Viewport::keyPressEvent(QKeyEvent *event) {
    if(event->key() == Qt::Key_Control) {
        if(Qt::KeyboardModifiers() == Qt::ALT) {
            setCursor(Qt::OpenHandCursor);
        }
    }
    this->eventDelegate->handleKeyboard(event, focus);
    if(event->isAutoRepeat()) {
        state->autorepeat = true;
        //event->ignore();
    }
}


void Viewport::drawViewport(int vpID) {
    reference->renderOrthogonalVP(vpID);
}

void Viewport::drawSkeletonViewport() {
    reference->renderSkeletonVP(VIEWPORT_SKELETON);
}

bool Viewport::handleMouseButtonLeft(QMouseEvent *event, int vpID) {
    return eventDelegate->handleMouseButtonLeft(event, vpID);
}

bool Viewport::handleMouseButtonMiddle(QMouseEvent *event, int vpID) {
    return eventDelegate->handleMouseButtonMiddle(event, vpID);

}

bool Viewport::handleMouseButtonRight(QMouseEvent *event, int vpID) {
    return eventDelegate->handleMouseButtonRight(event, vpID);
}


bool Viewport::handleMouseMotionLeftHold(QMouseEvent *event, int vpID) {
    return eventDelegate->handleMouseMotionLeftHold(event, vpID);
}

bool Viewport::handleMouseMotionMiddleHold(QMouseEvent *event, int vpID) {
    return eventDelegate->handleMouseMotionMiddleHold(event, vpID);
}

bool Viewport::handleMouseMotionRightHold(QMouseEvent *event, int vpID) {
    return eventDelegate->handleMouseMotionRightHold(event, vpID);
}

bool Viewport::handleMouseWheelForward(QWheelEvent *event, int vpID) {
    return eventDelegate->handleMouseWheelForward(event, vpID);
}

bool Viewport::handleMouseWheelBackward(QWheelEvent *event, int vpID) {
    return eventDelegate->handleMouseWheelBackward(event, vpID);
}

bool Viewport::handleMouseReleaseLeft(QMouseEvent *event, int vpID) {
    return eventDelegate->handleMouseReleaseLeft(event, vpID);
}


void Viewport::enterEvent(QEvent *event) {
    entered = true;
    focus = this->id;
    this->setCursor(Qt::CrossCursor);
}

void Viewport::zoomOrthogonals(float step){
    int triggerMagChange = false;
    for(uint i = 0; i < state->viewerState->numberViewports; i++) {
        if(state->viewerState->vpConfigs[i].type != VIEWPORT_SKELETON) {
            /* check if mag is locked */
            if(state->viewerState->datasetMagLock) {
                if(!(state->viewerState->vpConfigs[i].texture.zoomLevel + step < VPZOOMMAX) &&
                   !(state->viewerState->vpConfigs[i].texture.zoomLevel + step > VPZOOMMIN)) {
                    state->viewerState->vpConfigs[i].texture.zoomLevel += step;
                }
            }
            else {
                /* trigger a mag change when possible */
                if((state->viewerState->vpConfigs[i].texture.zoomLevel + step < 0.5)
                    && (state->viewerState->vpConfigs[i].texture.zoomLevel >= 0.5)
                    && (state->magnification != state->lowestAvailableMag)) {
                    state->viewerState->vpConfigs[i].texture.zoomLevel += step;
                    triggerMagChange = MAG_DOWN;
                }
                if((state->viewerState->vpConfigs[i].texture.zoomLevel + step > 1.0)
                    && (state->viewerState->vpConfigs[i].texture.zoomLevel <= 1.0)
                    && (state->magnification != state->highestAvailableMag)) {
                    state->viewerState->vpConfigs[i].texture.zoomLevel += step;
                    triggerMagChange = MAG_UP;
                }
                /* performe normal zooming otherwise. This case also covers
                * the special case of zooming in further than 0.5 on mag1 */
                if(!triggerMagChange) {
                    float zoomLevel = state->viewerState->vpConfigs[i].texture.zoomLevel += step;
                    if(zoomLevel < VPZOOMMAX) {
                        state->viewerState->vpConfigs[i].texture.zoomLevel = VPZOOMMAX;
                    }
                    else if (zoomLevel > VPZOOMMIN) {
                        state->viewerState->vpConfigs[i].texture.zoomLevel = VPZOOMMIN;
                    }
                }
            }
        }
    }

   if(triggerMagChange) {
        emit changeDatasetMagSignal(triggerMagChange);
   }
   emit recalcTextureOffsetsSignal();
   emit updateZoomAndMultiresWidget();

}

void Viewport::zoomOutSkeletonVP() {
    if (state->skeletonState->zoomLevel >= SKELZOOMMIN) {
        state->skeletonState->zoomLevel -= (0.1* (0.5 - state->skeletonState->zoomLevel));
        if (state->skeletonState->zoomLevel < SKELZOOMMIN) {
            state->skeletonState->zoomLevel = SKELZOOMMIN;
        }
        state->skeletonState->viewChanged = true;
        emit updateZoomAndMultiresWidget();
    }
}
void Viewport::zoomInSkeletonVP() {
    if (state->skeletonState->zoomLevel <= SKELZOOMMAX){
        state->skeletonState->zoomLevel += (0.1 * (0.5 - state->skeletonState->zoomLevel));
        if(state->skeletonState->zoomLevel > SKELZOOMMAX) {
            state->skeletonState->zoomLevel = SKELZOOMMAX;
        }
        state->skeletonState->viewChanged = true;
        emit updateZoomAndMultiresWidget();
    }
}

void Viewport::resizeVP(QMouseEvent *event) {
    if(event->x() >= event->y()) {
        resize(event->x(), event->x());
    }
    else {
        resize(event->y(), event->y());
    }
    if(height() < MIN_VP_SIZE) {
        resize(MIN_VP_SIZE, MIN_VP_SIZE);
    }
    else if(pos().y() + height() > parentWidget()->height() - 60) {
        resize(parentWidget()->height()- pos().y(), parentWidget()->height() - pos().y());
    }
    if(width() < MIN_VP_SIZE) {
        resize(MIN_VP_SIZE, MIN_VP_SIZE);
    }
    else if(pos().x() + width() > parentWidget()->width()) {
        resize(parentWidget()->width() - pos().x(), parentWidget()->width() - pos().x());
    }
    updateButtonPositions();
    state->viewerState->defaultVPSizeAndPos = false;
}

void Viewport::updateButtonPositions() {
    resizeButton->setGeometry(width() - ResizeButton::SIZE,
                              height() - ResizeButton::SIZE,
                              ResizeButton::SIZE,
                              ResizeButton::SIZE);
    if(viewportType == VIEWPORT_SKELETON) {
        xyButton->setGeometry(width() - (ViewportButton::WIDTH - 2)*6, 2, ViewportButton::WIDTH, ViewportButton::HEIGHT);
        xzButton->setGeometry(width() - (ViewportButton::WIDTH - 2)*5, 2, ViewportButton::WIDTH, ViewportButton::HEIGHT);
        yzButton->setGeometry(width() - (ViewportButton::WIDTH - 2)*4, 2, ViewportButton::WIDTH, ViewportButton::HEIGHT);
        r90Button->setGeometry(width() - (ViewportButton::WIDTH - 2)*3, 2, ViewportButton::WIDTH, ViewportButton::HEIGHT);
        r180Button->setGeometry(width() - (ViewportButton::WIDTH - 2)*2, 2, ViewportButton::WIDTH, ViewportButton::HEIGHT);
        resetButton->setGeometry(width() - ViewportButton::WIDTH - 2, 2, ViewportButton::WIDTH, ViewportButton::HEIGHT);
    }
}

void Viewport::moveVP(QMouseEvent *event) {
    raise();
    int x = pos().x() + xrel(event->x());
    int y = pos().y() + yrel(event->y());

    if(x >= 0 && x <= (parentWidget()->width() - width())
       && y >= 0 && y <= (parentWidget()->height() - height())) {
        move(x, y);
        state->viewerState->defaultVPSizeAndPos = false;
    }
    else if(x >= 0 && x <= (parentWidget()->width() - width())) {
        move(x, pos().y());
        state->viewerState->defaultVPSizeAndPos = false;
    }
    else if(y >= 0 && y <= (parentWidget()->height() - height())) {
        move(pos().x(), y);
        state->viewerState->defaultVPSizeAndPos = false;
    }
}

void Viewport::hideButtons() {
    resizeButton->hide();
    if(viewportType == VIEWPORT_SKELETON) {
        xyButton->hide();
        xzButton->hide();
        yzButton->hide();
        r90Button->hide();
        r180Button->hide();
        resetButton->hide();
    }
}

void Viewport::showButtons() {
    resizeButton->show();
    if(viewportType == VIEWPORT_SKELETON) {
        xyButton->show();
        xzButton->show();
        yzButton->show();
        r90Button->show();
        r180Button->show();
        resetButton->show();
    }
}

void Viewport::resizeButtonClicked() {
    resizeButtonHold = true;
    raise();
}

void Viewport::xyButtonClicked() {
    if(state->skeletonState->rotationcounter == 0) {
        state->skeletonState->definedSkeletonVpView = SKELVP_XY_VIEW;
    }
}

void Viewport::xzButtonClicked() {
    if(state->skeletonState->rotationcounter == 0) {
        state->skeletonState->definedSkeletonVpView = SKELVP_XZ_VIEW;
    }
}

void Viewport::yzButtonClicked() {
    if(state->skeletonState->rotationcounter == 0) {
        state->skeletonState->definedSkeletonVpView = SKELVP_YZ_VIEW;
    }
}

void Viewport::r90ButtonClicked() {
    if(state->skeletonState->rotationcounter == 0) {
        state->skeletonState->definedSkeletonVpView = SKELVP_R90;
    }
}

void Viewport::r180ButtonClicked() {
    if(state->skeletonState->rotationcounter == 0) {
        state->skeletonState->definedSkeletonVpView = SKELVP_R180;
    }
}

void Viewport::resetButtonClicked() {
    if(state->skeletonState->rotationcounter == 0) {
        state->skeletonState->definedSkeletonVpView = SKELVP_RESET;
    }
}
