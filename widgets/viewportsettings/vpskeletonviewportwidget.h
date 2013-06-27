#ifndef VPSKELETONVIEWPORTWIDGET_H
#define VPSKELETONVIEWPORTWIDGET_H

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

class QLabel;
class QCheckBox;
class QRadioButton;
class VPSkeletonViewportWidget : public QWidget
{
    friend class ViewportSettingsWidget;
    Q_OBJECT
public:
    explicit VPSkeletonViewportWidget(QWidget *parent = 0);
signals:
    void showXYPlaneSignal(bool on);
    void rotateAroundActiveNodeSignal(bool on);
public slots:
    void showXYPlaneChecked(bool on);
    void showXZPlaneChecked(bool on);
    void showYZPlaneChecked(bool on);
    void wholeSkeletonSelected();
    void onlyCurrentCubeSelected();
    void onlyActiveTreeSelected();
    void hideSkeletonSelected();
    void rotateAroundActiveNodeChecked(bool on);
protected:
    QLabel *datasetVisualizationLabel, *skeletonDisplayModesLabel;
    QCheckBox *showXYPlaneCheckBox, *showXZPlaneCheckBox, *showYZPlaneCheckBox;
    QRadioButton *wholeSkeletonRadioButton, *onlyCurrentCubeRadioButton, *onlyActiveTreeRadioButton, *hideSkeletonRadioButton;
    QLabel *view3dlabel;
    QCheckBox *rotateAroundActiveNodeCheckBox;
    
};

#endif // VPSKELETONVIEWPORTWIDGET_H
