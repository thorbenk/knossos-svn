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

#include "vpskeletonviewportwidget.h"
#include <QLabel>
#include <QFrame>
#include <QCheckBox>
#include <QRadioButton>
#include <QVBoxLayout>
#include <QGridLayout>
#include "knossos-global.h"

extern  stateInfo *state;

VPSkeletonViewportWidget::VPSkeletonViewportWidget(QWidget *parent) :
    QWidget(parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout();
    QGridLayout *gridLayout = new QGridLayout();

    datasetVisualizationLabel = new QLabel("Dataset Visualization");
    showXYPlaneCheckBox = new QCheckBox("Show XY Plane");
    showXZPlaneCheckBox = new QCheckBox("Show XZ Plane");
    showYZPlaneCheckBox = new QCheckBox("Show YZ Plane");

    skeletonDisplayModesLabel = new QLabel("Skeleton Display Modes");
    wholeSkeletonRadioButton = new QRadioButton("Whole Skeleton");
    onlyCurrentCubeRadioButton = new QRadioButton("Only Current Cube");
    onlyActiveTreeRadioButton = new QRadioButton("Only Active Tree");
    hideSkeletonRadioButton = new QRadioButton("Hide Skeleton (fast)");

    view3dlabel = new QLabel("3D View");
    rotateAroundActiveNodeCheckBox = new QCheckBox("Rotate Around Active Node");
    rotateAroundActiveNodeCheckBox->setChecked(true);

    QFrame *line = new QFrame();
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);

    QFrame *line2 = new QFrame();
    line2->setFrameShape(QFrame::HLine);
    line2->setFrameShadow(QFrame::Sunken);

    QFrame *line3 = new QFrame();
    line3->setFrameShape(QFrame::HLine);
    line3->setFrameShadow(QFrame::Sunken);

    //gridLayout->addWidget(datasetVisualizationLabel, 0, 0);
    gridLayout->addWidget(skeletonDisplayModesLabel, 0, 0);
    gridLayout->addWidget(line, 1, 0);
    //gridLayout->addWidget(line2, 1, 1);
    //gridLayout->addWidget(showXYPlaneCheckBox, 2, 0);
    gridLayout->addWidget(wholeSkeletonRadioButton, 2, 0);
    //gridLayout->addWidget(showXZPlaneCheckBox, 3, 0);
    gridLayout->addWidget(onlyActiveTreeRadioButton, 3, 0);
    //gridLayout->addWidget(showYZPlaneCheckBox, 4, 0);
    gridLayout->addWidget(hideSkeletonRadioButton, 4, 0);

    gridLayout->addWidget(view3dlabel, 5, 0);
    gridLayout->addWidget(line3, 6, 0);
    gridLayout->addWidget(rotateAroundActiveNodeCheckBox, 7, 0);

    mainLayout->addLayout(gridLayout);
    setLayout(mainLayout);
    mainLayout->addStretch(50);

    connect(showXYPlaneCheckBox, SIGNAL(clicked(bool)), this, SLOT(showXYPlaneChecked(bool)));
    connect(showYZPlaneCheckBox, SIGNAL(clicked(bool)), this, SLOT(showYZPlaneChecked(bool)));
    connect(showXZPlaneCheckBox, SIGNAL(clicked(bool)), this, SLOT(showXZPlaneChecked(bool)));
    connect(wholeSkeletonRadioButton, SIGNAL(clicked()), this, SLOT(wholeSkeletonSelected()));
    connect(onlyCurrentCubeRadioButton, SIGNAL(clicked()), this, SLOT(onlyCurrentCubeSelected()));
    connect(onlyActiveTreeRadioButton, SIGNAL(clicked()), this, SLOT(onlyActiveTreeSelected()));
    connect(hideSkeletonRadioButton, SIGNAL(clicked()), this, SLOT(hideSkeletonSelected()));
    connect(rotateAroundActiveNodeCheckBox, SIGNAL(clicked(bool)), SLOT(rotateAroundActiveNodeChecked(bool)));
}

void VPSkeletonViewportWidget::showXYPlaneChecked(bool on) {
    state->skeletonState->showXYplane = on;
}

void VPSkeletonViewportWidget::showXZPlaneChecked(bool on) {
    state->skeletonState->showXZplane = on;
}

void VPSkeletonViewportWidget::showYZPlaneChecked(bool on) {
    state->skeletonState->showYZplane = on;
}

void VPSkeletonViewportWidget::wholeSkeletonSelected() {
   resetDisplayMode();
   state->skeletonState->displayMode |= DSP_SKEL_VP_WHOLE; 
   emit updateViewerStateSignal();
}

void VPSkeletonViewportWidget::onlyCurrentCubeSelected() {
    resetDisplayMode();
    state->skeletonState->displayMode |= DSP_SKEL_VP_CURRENTCUBE;    
    emit updateViewerStateSignal();
}

void VPSkeletonViewportWidget::onlyActiveTreeSelected() {
    resetDisplayMode();
    state->skeletonState->displayMode |= DSP_ACTIVETREE;
    emit updateViewerStateSignal();
}

void VPSkeletonViewportWidget::hideSkeletonSelected() {
    resetDisplayMode();
    state->skeletonState->displayMode |= DSP_SKEL_VP_HIDE;
    emit updateViewerStateSignal();
}

void VPSkeletonViewportWidget::rotateAroundActiveNodeChecked(bool on) {
    state->skeletonState->rotateAroundActiveNode = on;
}

void VPSkeletonViewportWidget::resetDisplayMode() {
    state->skeletonState->displayMode &=
            (~DSP_SKEL_VP_WHOLE &
            ~DSP_ACTIVETREE &
            ~DSP_SKEL_VP_HIDE &
            ~DSP_SKEL_VP_CURRENTCUBE);
}

void VPSkeletonViewportWidget::updateDisplayModeRadio() {
    if(state->skeletonState->displayMode & DSP_SKEL_VP_WHOLE) {
        wholeSkeletonRadioButton->setChecked(true);
    }
    if(state->skeletonState->displayMode & DSP_ACTIVETREE) {
        onlyActiveTreeRadioButton->setChecked(true);
    }
    if(state->skeletonState->displayMode & DSP_SKEL_VP_HIDE) {
        hideSkeletonRadioButton->setChecked(true);
    }
}
