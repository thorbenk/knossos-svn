#ifndef DATASAVINGWIDGET_H
#define DATASAVINGWIDGET_H

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
static int count;


#include <QDialog>

class QCheckBox;
class QLabel;
class QSpinBox;
class DataSavingWidget : public QDialog
{
    friend class MainWindow;
    Q_OBJECT
public:
    explicit DataSavingWidget(QWidget *parent = 0);
    void loadSettings();
    void saveSettings();
protected:
    QCheckBox *autosaveCheckbox;
    QLabel *autosaveIntervalLabel;
    QSpinBox *autosaveIntervalSpinBox;
    QLabel *autoincrementFileNameLabel;
    QCheckBox *autoincrementFileNameButton;

signals:
    void uncheckSignal();
public slots:
    void autosaveIntervalChanged(int value);
    void autosaveCheckboxChecked(bool on);
    void autonincrementFileNameButtonPushed(bool on);
protected:
    void closeEvent(QCloseEvent *event);

    
};

#endif // DATASAVINGWIDGET_H
