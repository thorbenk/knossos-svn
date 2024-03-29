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
#include "commentshortcuts/commentshighlightingtab.h"
#include "commentshortcuts/commentsnodecommentstab.h"
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
#include <QApplication>
#include <QDesktopWidget>
#include "knossos-global.h"

extern  stateInfo *state;

CommentsWidget::CommentsWidget(QWidget *parent) :
    QDialog(parent)
{
    setWindowTitle("Comment Settings");
    this->shortcutTab = new CommentShortCutsTab();
    this->highlightingTab = new CommentsHighlightingTab();
//    this->nodeCommentsTab = new CommentsNodeCommentsTab();

    tabs = new QTabWidget(this);
    tabs->addTab(shortcutTab, "Shortcuts");
    tabs->addTab(highlightingTab, "Highlighting");
//    tabs->addTab(nodeCommentsTab, "Node Comments");
    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(tabs);
    setLayout(layout);
}

void CommentsWidget::loadSettings() {
    int width, height, x, y;
    bool visible;

    QSettings settings;
    settings.beginGroup(COMMENTS_WIDGET);
    width = (settings.value(WIDTH).isNull())? this->width() : settings.value(WIDTH).toInt();
    height = (settings.value(HEIGHT).isNull())? this->height() : settings.value(HEIGHT).toInt();
    if(settings.value(POS_X).isNull() or settings.value(POS_Y).isNull()) {
        x = QApplication::desktop()->screen()->rect().topRight().x() - width - 20;
        y = QApplication::desktop()->screen()->rect().topRight().y() + 50;
    }
    else {
        x = settings.value(POS_X).toInt();
        y = settings.value(POS_Y).toInt();
    }
    visible = (settings.value(VISIBLE).isNull())? false : settings.value(VISIBLE).toBool();

    if(settings.value(COMMENT1).isNull() == false) {
        this->shortcutTab->textFields[0]->setText(settings.value(COMMENT1).toString());
        strcpy(state->viewerState->gui->comment1, settings.value(COMMENT1).toString().toStdString().c_str());
    }
    else {
        this->shortcutTab->textFields[0]->clear();
        memset(state->viewerState->gui->comment1, '\0', sizeof(state->viewerState->gui->comment1));
    }
    if(settings.value(COMMENT2).isNull() == false) {
        this->shortcutTab->textFields[1]->setText(settings.value(COMMENT2).toString());
        strcpy(state->viewerState->gui->comment2, settings.value(COMMENT2).toString().toStdString().c_str());
    }
    else {
        this->shortcutTab->textFields[1]->clear();
        memset(state->viewerState->gui->comment2, '\0', sizeof(state->viewerState->gui->comment2));
    }
    if(settings.value(COMMENT3).isNull() == false) {
        this->shortcutTab->textFields[2]->setText(settings.value(COMMENT3).toString());
        strcpy(state->viewerState->gui->comment3, settings.value(COMMENT3).toString().toStdString().c_str());
    }
    else {
        this->shortcutTab->textFields[2]->clear();
        memset(state->viewerState->gui->comment3, '\0', sizeof(state->viewerState->gui->comment3));
    }
    if(settings.value(COMMENT4).isNull() == false) {
        this->shortcutTab->textFields[3]->setText(settings.value(COMMENT4).toString());
        strcpy(state->viewerState->gui->comment4, settings.value(COMMENT4).toString().toStdString().c_str());
    }
    else {
        this->shortcutTab->textFields[3]->clear();
        memset(state->viewerState->gui->comment4, '\0', sizeof(state->viewerState->gui->comment4));
    }
    if(settings.value(COMMENT5).isNull() == false) {
        this->shortcutTab->textFields[4]->setText(settings.value(COMMENT5).toString());
        strcpy(state->viewerState->gui->comment5, settings.value(COMMENT5).toString().toStdString().c_str());
    }
    else {
        this->shortcutTab->textFields[4]->clear();
        memset(state->viewerState->gui->comment5, '\0', sizeof(state->viewerState->gui->comment5));
    }

    if(settings.value(SUBSTR1).isNull() == false) {
        this->highlightingTab->substringFields[0]->setText(settings.value(SUBSTR1).toString());
    }
    else {
        this->highlightingTab->substringFields[0]->clear();
    }
    if(settings.value(SUBSTR2).isNull() == false) {
        this->highlightingTab->substringFields[1]->setText(settings.value(SUBSTR2).toString());
    }
    else {
        this->highlightingTab->substringFields[1]->clear();
    }
    if(settings.value(SUBSTR3).isNull() == false) {
        this->highlightingTab->substringFields[2]->setText(settings.value(SUBSTR3).toString());
    }
    else {
        this->highlightingTab->substringFields[2]->clear();
    }
    if(settings.value(SUBSTR4).isNull() == false) {
        this->highlightingTab->substringFields[3]->setText(settings.value(SUBSTR4).toString());
    }
    else {
        this->highlightingTab->substringFields[3]->clear();
    }
    if(settings.value(SUBSTR5).isNull() == false) {
        this->highlightingTab->substringFields[4]->setText(settings.value(SUBSTR5).toString());
    }
    else {
        this->highlightingTab->substringFields[4]->clear();
    }

    this->highlightingTab->colorComboBox[0]->setCurrentIndex(settings.value(COLOR1).toInt());
    this->highlightingTab->colorComboBox[1]->setCurrentIndex(settings.value(COLOR2).toInt());
    this->highlightingTab->colorComboBox[2]->setCurrentIndex(settings.value(COLOR3).toInt());
    this->highlightingTab->colorComboBox[3]->setCurrentIndex(settings.value(COLOR4).toInt());
    this->highlightingTab->colorComboBox[4]->setCurrentIndex(settings.value(COLOR5).toInt());

    if(settings.value(RADIUS1).isNull() == false) {
        this->highlightingTab->radiusSpinBox[0]->setValue(settings.value(RADIUS1).toDouble());
    }
    else {
        this->highlightingTab->radiusSpinBox[0]->setValue(1.5);
    }
    if(settings.value(RADIUS2).isNull() == false) {
        this->highlightingTab->radiusSpinBox[1]->setValue(settings.value(RADIUS2).toDouble());
    }
    else {
        this->highlightingTab->radiusSpinBox[1]->setValue(1.5);
    }
    if(settings.value(RADIUS3).isNull() == false) {
        this->highlightingTab->radiusSpinBox[2]->setValue(settings.value(RADIUS3).toDouble());
    }
    else {
        this->highlightingTab->radiusSpinBox[2]->setValue(1.5);
    }
    if(settings.value(RADIUS4).isNull() == false) {
        this->highlightingTab->radiusSpinBox[3]->setValue(settings.value(RADIUS4).toDouble());
    }
    else {
        this->highlightingTab->radiusSpinBox[3]->setValue(1.5);
    }
    if(settings.value(RADIUS5).isNull() == false) {
        this->highlightingTab->radiusSpinBox[4]->setValue(settings.value(RADIUS5).toDouble());
    }
    else {
        this->highlightingTab->radiusSpinBox[4]->setValue(1.5);
    }
    settings.endGroup();

    if(visible) {
        this->show();
    }
    else {
        this->hide();
    }
    this->setGeometry(x, y, width, height);
}

void CommentsWidget::saveSettings() {
    QSettings settings;
    settings.beginGroup(COMMENTS_WIDGET);
    settings.setValue(WIDTH, this->geometry().width());
    settings.setValue(HEIGHT, this->geometry().height());
    settings.setValue(POS_X, this->geometry().x());
    settings.setValue(POS_Y, this->geometry().y());
    settings.setValue(VISIBLE, this->isVisible());

    settings.setValue(COMMENT1, this->shortcutTab->textFields[0]->text());
    settings.setValue(COMMENT2, this->shortcutTab->textFields[1]->text());
    settings.setValue(COMMENT3, this->shortcutTab->textFields[2]->text());
    settings.setValue(COMMENT4, this->shortcutTab->textFields[3]->text());
    settings.setValue(COMMENT5, this->shortcutTab->textFields[4]->text());

    settings.setValue(SUBSTR1, this->highlightingTab->substringFields[0]->text());
    settings.setValue(SUBSTR2, this->highlightingTab->substringFields[1]->text());
    settings.setValue(SUBSTR3, this->highlightingTab->substringFields[2]->text());
    settings.setValue(SUBSTR4, this->highlightingTab->substringFields[3]->text());
    settings.setValue(SUBSTR5, this->highlightingTab->substringFields[4]->text());

    settings.setValue(COLOR1, this->highlightingTab->colorComboBox[0]->currentIndex());
    settings.setValue(COLOR2, this->highlightingTab->colorComboBox[1]->currentIndex());
    settings.setValue(COLOR3, this->highlightingTab->colorComboBox[2]->currentIndex());
    settings.setValue(COLOR4, this->highlightingTab->colorComboBox[3]->currentIndex());
    settings.setValue(COLOR5, this->highlightingTab->colorComboBox[4]->currentIndex());

    settings.setValue(RADIUS1, this->highlightingTab->radiusSpinBox[0]->value());
    settings.setValue(RADIUS2, this->highlightingTab->radiusSpinBox[1]->value());
    settings.setValue(RADIUS3, this->highlightingTab->radiusSpinBox[2]->value());
    settings.setValue(RADIUS4, this->highlightingTab->radiusSpinBox[3]->value());
    settings.setValue(RADIUS5, this->highlightingTab->radiusSpinBox[4]->value());

    settings.endGroup();
}

void CommentsWidget::closeEvent(QCloseEvent *event) {
    this->hide();
    emit this->uncheckSignal();
}
