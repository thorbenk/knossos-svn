#ifndef ZOOMANDMULTIRESWIDGET_H
#define ZOOMANDMULTIRESWIDGET_H



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

#include <QDialog>

class QLabel;
class QDoubleSpinBox;
class QCheckBox;
class QPushButton;

class ZoomAndMultiresWidget : public QDialog
{
    friend class TestZoomAndMultiresWidget;
    friend class MainWindow;
    Q_OBJECT
public:
    explicit ZoomAndMultiresWidget(QWidget *parent = 0);
        
public slots:
    void zoomDefaultsClicked();
    void lockDatasetMagChecked(bool on);
    void orthogonalSpinBoxChanged(double value);
    void skeletonSpinBoxChanged(double value);
    void update();
    void loadSettings();
    void saveSettings();
protected:
    void closeEvent(QCloseEvent *event);
protected:
    /*! Necessary helper variables to enable relative (instead of constant) incrementation/decrementation of zoom spin boxes.
     * Zoom steps become smaller with higher zoom levels and vice versa (smoother impression to the user).
     * See the spinBoxChanged slots for more details. */
    float lastZoomSkel;
    bool userZoomSkel;

    // top layout
    QLabel *orthogonalDataViewportLabel;
    QLabel *skeletonViewportLabel;
    QDoubleSpinBox *orthogonalDataViewportSpinBox;
    QDoubleSpinBox *skeletonViewportSpinBox;
    QPushButton *zoomDefaultsButton;

    // bottom layout
    QLabel *lockDatasetLabel;
    QCheckBox *lockDatasetCheckBox;
    QLabel *currentActiveMagDatasetLabel;
    QLabel *highestActiveMagDatasetLabel;
    QLabel *lowestActiveMagDatasetLabel;
signals:
    void zoomInSkeletonVPSignal();
    void zoomOutSkeletonVPSignal();
    void uncheckSignal();
    void zoomLevelSignal(float value);
};

#endif // ZOOMANDMULTIRESWIDGET_H
