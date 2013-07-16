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

#include "commentswidget.h"
#include "commentshortcuts/commentshortcutstab.h"
#include "commentshortcuts/commentspreferencestab.h"
#include "GUIConstants.h"
#include <QSettings>
#include <QEvent>
#include <QKeyEvent>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QFormLayout>
#include <QVariant>
#include <QTableWidget>
#include "knossos-global.h"

extern struct stateInfo *state;

CommentsWidget::CommentsWidget(QWidget *parent) :
    QDialog(parent)
{
    setWindowTitle("Comments Shortcuts");

    this->shortcutTab = new CommentShortCutsTab();
    this->preferencesTab = new CommentsPreferencesTab();

    tabs = new QTabWidget(this);
    tabs->addTab(shortcutTab, "shortcuts");
    tabs->addTab(preferencesTab, "preferences");

}

void CommentsWidget::loadSettings() {
    int width, height, x, y;
    bool visible;

    QSettings settings;
    settings.beginGroup(COMMENTS_WIDGET);
    width = settings.value(WIDTH).toInt();
    height = settings.value(HEIGHT).toInt();
    x = settings.value(POS_X).toInt();
    y = settings.value(POS_Y).toInt();
    visible = settings.value(VISIBLE).toBool();

    if(!settings.value(COMMENT1).isNull()) {
        this->shortcutTab->textFields[0]->setText(settings.value(COMMENT1).toString());
        state->viewerState->gui->comment1 = const_cast<char *>(settings.value(COMMENT1).toString().toStdString().c_str());
    } if(!settings.value(COMMENT2).isNull()) {
        this->shortcutTab->textFields[1]->setText(settings.value(COMMENT2).toString());
        state->viewerState->gui->comment2 = const_cast<char *>(settings.value(COMMENT2).toString().toStdString().c_str());
    } if(!settings.value(COMMENT3).isNull()) {
        this->shortcutTab->textFields[2]->setText(settings.value(COMMENT3).toString());
        state->viewerState->gui->comment3 = const_cast<char *>(settings.value(COMMENT3).toString().toStdString().c_str());
    } if(!settings.value(COMMENT4).isNull()) {
        this->shortcutTab->textFields[3]->setText(settings.value(COMMENT4).toString());
        state->viewerState->gui->comment4 = const_cast<char *>(settings.value(COMMENT4).toString().toStdString().c_str());
    } if(!settings.value(COMMENT5).isNull()) {
        this->shortcutTab->textFields[4]->setText(settings.value(COMMENT5).toString());
        state->viewerState->gui->comment5 = const_cast<char *>(settings.value(COMMENT5).toString().toStdString().c_str());
    }

    if(!settings.value(SUBSTR1).isNull())
        this->preferencesTab->substringFields[0]->setText(settings.value(SUBSTR1).toString());
    if(!settings.value(SUBSTR2).isNull())
        this->preferencesTab->substringFields[1]->setText(settings.value(SUBSTR2).toString());
    if(!settings.value(SUBSTR3).isNull())
        this->preferencesTab->substringFields[2]->setText(settings.value(SUBSTR3).toString());
    if(!settings.value(SUBSTR4).isNull())
        this->preferencesTab->substringFields[3]->setText(settings.value(SUBSTR4).toString());
    if(!settings.value(SUBSTR5).isNull())
        this->preferencesTab->substringFields[4]->setText(settings.value(SUBSTR5).toString());

    this->preferencesTab->colorComboBox[0]->setCurrentIndex(settings.value(COLOR1).toInt());
    this->preferencesTab->colorComboBox[1]->setCurrentIndex(settings.value(COLOR2).toInt());
    this->preferencesTab->colorComboBox[2]->setCurrentIndex(settings.value(COLOR3).toInt());
    this->preferencesTab->colorComboBox[3]->setCurrentIndex(settings.value(COLOR4).toInt());
    this->preferencesTab->colorComboBox[4]->setCurrentIndex(settings.value(COLOR5).toInt());

    this->preferencesTab->radiusSpinBox[0]->setValue(settings.value(RADIUS1).toDouble());
    this->preferencesTab->radiusSpinBox[1]->setValue(settings.value(RADIUS2).toDouble());
    this->preferencesTab->radiusSpinBox[2]->setValue(settings.value(RADIUS3).toDouble());
    this->preferencesTab->radiusSpinBox[3]->setValue(settings.value(RADIUS4).toDouble());
    this->preferencesTab->radiusSpinBox[4]->setValue(settings.value(RADIUS5).toDouble());

    settings.endGroup();

    //this->setGeometry(x, y, width, height);

}

void CommentsWidget::saveSettings() {
    QSettings settings;
    settings.beginGroup(COMMENTS_WIDGET);
    settings.setValue(WIDTH, this->width());
    settings.setValue(HEIGHT, this->height());
    settings.setValue(POS_X, this->x());
    settings.setValue(POS_Y, this->y());
    settings.setValue(VISIBLE, this->isVisible());

    settings.setValue(COMMENT1, this->shortcutTab->textFields[0]->text());
    settings.setValue(COMMENT2, this->shortcutTab->textFields[1]->text());
    settings.setValue(COMMENT3, this->shortcutTab->textFields[2]->text());
    settings.setValue(COMMENT4, this->shortcutTab->textFields[3]->text());
    settings.setValue(COMMENT5, this->shortcutTab->textFields[4]->text());

    settings.setValue(SUBSTR1, this->preferencesTab->substringFields[0]->text());
    settings.setValue(SUBSTR2, this->preferencesTab->substringFields[1]->text());
    settings.setValue(SUBSTR3, this->preferencesTab->substringFields[2]->text());
    settings.setValue(SUBSTR4, this->preferencesTab->substringFields[3]->text());
    settings.setValue(SUBSTR5, this->preferencesTab->substringFields[4]->text());

    settings.setValue(COLOR1, this->preferencesTab->colorComboBox[0]->currentIndex());
    settings.setValue(COLOR2, this->preferencesTab->colorComboBox[1]->currentIndex());
    settings.setValue(COLOR3, this->preferencesTab->colorComboBox[2]->currentIndex());
    settings.setValue(COLOR4, this->preferencesTab->colorComboBox[3]->currentIndex());
    settings.setValue(COLOR5, this->preferencesTab->colorComboBox[4]->currentIndex());

    settings.setValue(RADIUS1, this->preferencesTab->radiusSpinBox[0]->value());
    settings.setValue(RADIUS2, this->preferencesTab->radiusSpinBox[1]->value());
    settings.setValue(RADIUS3, this->preferencesTab->radiusSpinBox[2]->value());
    settings.setValue(RADIUS4, this->preferencesTab->radiusSpinBox[3]->value());
    settings.setValue(RADIUS5, this->preferencesTab->radiusSpinBox[4]->value());

    settings.endGroup();
}


void CommentsWidget::closeEvent(QCloseEvent *event) {
    this->hide();
    emit this->uncheckSignal();
}


