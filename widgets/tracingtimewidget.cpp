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

#include "tracingtimewidget.h"
#include <QLabel>
#include <QVBoxLayout>
#include <QTimer>
#include <QSettings>
#include <QSpacerItem>
#include <QGroupBox>
#include <math.h>
#include "knossos-global.h"

extern  stateInfo *state;

TracingTimeWidget::TracingTimeWidget(QWidget *parent) :
    QDialog(parent)
{
    this->setWindowTitle("Tracing Time");
    this->setStyleSheet("");

    this->runningTimeLabel = new QLabel("Running Time: 00:00:00");
    this->tracingTimeLabel = new QLabel("Tracing Time: 00:00:00");
    this->idleTimeLabel = new QLabel("Idle Time: 00:00:00");

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(runningTimeLabel);
    layout->addWidget(tracingTimeLabel);
    layout->addWidget(idleTimeLabel);
    layout->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Minimum, QSizePolicy::Expanding));
    this->setLayout(layout);

    timer = new QTimer();
    connect(timer, SIGNAL(timeout()), this, SLOT(refreshTime()));
    timer->start(1000);

}

void TracingTimeWidget::closeEvent(QCloseEvent *event) {
    this->hide();
}

void TracingTimeWidget::refreshTime() {
    int time = state->time.elapsed();

    int hoursRunningTime = (int)(time * 0.001 / 3600.0);//
    int minutesRunningTime = (int)(time * 0.001/60.0 - hoursRunningTime * 60);
    int secondsRunningTime = (int)(time * 0.001 - hoursRunningTime * 3600 - minutesRunningTime * 60);

    QString forLabel = QString().sprintf("Running Time: \t%02d:%02d:%02d", hoursRunningTime, minutesRunningTime, secondsRunningTime);

    this->runningTimeLabel->setText(forLabel);

}

void TracingTimeWidget::checkIdleTime() {

    int time = state->time.elapsed();

    state->skeletonState->idleTimeLast = state->skeletonState->idleTimeNow;
    state->skeletonState->idleTimeNow = time;
    if (state->skeletonState->idleTimeNow - state->skeletonState->idleTimeLast > 600000) { //tolerance of 10 minutes
        state->skeletonState->idleTime += state->skeletonState->idleTimeNow - state->skeletonState->idleTimeLast;
        state->skeletonState->idleTimeSession += state->skeletonState->idleTimeNow - state->skeletonState->idleTimeLast;
    }

    state->skeletonState->unsavedChanges = true;

    int hoursIdleTime = (int)(floor(state->skeletonState->idleTimeSession * 0.001) / 3600.0);
    int minutesIdleTime = (int)(floor(state->skeletonState->idleTimeSession * 0.001) / 60.0 - hoursIdleTime * 60);
    int secondsIdleTime = (int)(floor(state->skeletonState->idleTimeSession * 0.001) - hoursIdleTime * 3600 - minutesIdleTime * 60);

    QString idleString = QString().sprintf("Idle Time: \t%02d:%02d:%02d", hoursIdleTime, minutesIdleTime, secondsIdleTime);
    this->idleTimeLabel->setText(idleString);

    int hoursTracingTime = (int)((floor(time *0.001) - floor(state->skeletonState->idleTimeSession *0.001)) / 3600.0);
    int minutesTracingTime = (int)((floor(time *0.001) - floor(state->skeletonState->idleTimeSession *0.001)) /60.0 - hoursTracingTime * 60);
    int secondsTracingTime = (int)((floor(time *0.001) - floor(state->skeletonState->idleTimeSession *0.001)) - hoursTracingTime * 3600 - minutesTracingTime * 60);

    QString tracingString = QString().sprintf("Tracing Time: \t%02d:%02d:%02d", hoursTracingTime, minutesTracingTime, secondsTracingTime);
    this->tracingTimeLabel->setText(tracingString);

    state->viewerState->lastIdleTimeCall = QDateTime::currentDateTimeUtc();
    state->viewerState->renderInterval = FAST;
}
