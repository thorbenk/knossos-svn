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

#include <curl/curl.h>
#include <QXmlStreamReader>
#include <QXmlAttributes>

#include <QEvent>
#include <QMenu>
#include <QAction>
#include <QLayout>
#include <QGridLayout>
#include <QMessageBox>
#include <QDebug>
#include <QFileDialog>
#include <QFile>
#include <QDir>
#include <QStringList>
#include <QToolBar>
#include <QSpinBox>
#include <QLabel>
#include <QQueue>
#include <QKeySequence>
#include <QSettings>
#include <QDir>
#include <QAction>
#include <QThread>
#include <QRegExp>
#include <QToolButton>
#include <QCheckBox>
#include <QtConcurrent/QtConcurrentRun>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "GUIConstants.h"
#include "knossos-global.h"
#include "knossos.h"
#include "viewport.h"
#include "widgetcontainer.h"

extern  stateInfo *state;

// -- Constructor and destroyer -- //
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("KnossosQT");
    this->setWindowIcon(QIcon(":/images/logo.ico"));
    this->setUnifiedTitleAndToolBarOnMac(true);

    skeletonFileHistory = new QQueue<QString>();
    skeletonFileHistory->reserve(FILE_DIALOG_HISTORY_MAX_ENTRIES);

    state->viewerState->gui->oneShiftedCurrPos.x =
        state->viewerState->currentPosition.x + 1;
    state->viewerState->gui->oneShiftedCurrPos.y =
        state->viewerState->currentPosition.y + 1;
    state->viewerState->gui->oneShiftedCurrPos.z =
        state->viewerState->currentPosition.z + 1;

    // for task management
    state->taskState->cookieFile = (char*)calloc(1, sizeof("cookie"));
    strcpy(state->taskState->cookieFile, "cookie");
    state->taskState->taskFile = (char*)calloc(1, 1024 * sizeof(char));
    state->taskState->taskName = (char*)calloc(1, 1024 *sizeof(char));
    state->taskState->host = (char*)calloc(1, 10240 * sizeof(char));
    strcpy(state->taskState->host, "149.217.51.57:8000");

    /* init here instead of initSkeletonizer to fix some init order issue */
    state->skeletonState->displayMode = 0;
    state->skeletonState->displayMode |= DSP_SKEL_VP_WHOLE;

    state->viewerState->gui->commentBuffer = (char*)malloc(10240 * sizeof(char));
    memset(state->viewerState->gui->commentBuffer, '\0', 10240 * sizeof(char));

    state->viewerState->gui->commentSearchBuffer = (char*)malloc(2048 * sizeof(char));
    memset(state->viewerState->gui->commentSearchBuffer, '\0', 2048 * sizeof(char));

    state->viewerState->gui->treeCommentBuffer = (char*)malloc(8192 * sizeof(char));
    memset(state->viewerState->gui->treeCommentBuffer, '\0', 8192 * sizeof(char));

    state->viewerState->gui->comment1 = (char*)malloc(10240 * sizeof(char));
    memset(state->viewerState->gui->comment1, '\0', 10240 * sizeof(char));

    state->viewerState->gui->comment2 = (char*)malloc(10240 * sizeof(char));
    memset(state->viewerState->gui->comment2, '\0', 10240 * sizeof(char));

    state->viewerState->gui->comment3 = (char*)malloc(10240 * sizeof(char));
    memset(state->viewerState->gui->comment3, '\0', 10240 * sizeof(char));

    state->viewerState->gui->comment4 = (char*)malloc(10240 * sizeof(char));
    memset(state->viewerState->gui->comment4, '\0', 10240 * sizeof(char));

    state->viewerState->gui->comment5 = (char*)malloc(10240 * sizeof(char));
    memset(state->viewerState->gui->comment5, '\0', 10240 * sizeof(char));

    createActions();
    createMenus();

    widgetContainer = new WidgetContainer(this);
    widgetContainer->createWidgets(this);

    createToolBar();
    mainWidget = new QWidget(this);
    setCentralWidget(mainWidget);
    setGeometry(0, 0, width(), height());

    //connect(widgetContainer->toolsWidget, SIGNAL(uncheckSignal()), this, SLOT(uncheckToolsAction()));
    connect(widgetContainer->viewportSettingsWidget, SIGNAL(uncheckSignal()), this, SLOT(uncheckViewportSettingAction()));
    connect(widgetContainer->commentsWidget, SIGNAL(uncheckSignal()), this, SLOT(uncheckCommentShortcutsAction()));
    connect(widgetContainer->console, SIGNAL(uncheckSignal()), this, SLOT(uncheckConsoleAction()));

    connect(widgetContainer->dataSavingWidget, SIGNAL(uncheckSignal()), this, SLOT(uncheckDataSavingAction()));
    connect(widgetContainer->navigationWidget, SIGNAL(uncheckSignal()), this, SLOT(uncheckNavigationAction()));
    connect(widgetContainer->synchronizationWidget, SIGNAL(uncheckSignal()), this, SLOT(uncheckSynchronizationAction()));
    updateTitlebar(false);
    createViewports();
    setAcceptDrops(true);

}

void MainWindow::createViewports() {
    viewports = new Viewport*[NUM_VP];
    viewports[VP_UPPERLEFT] = new Viewport(this, VIEWPORT_XY, VP_UPPERLEFT);
    viewports[VP_LOWERLEFT] = new Viewport(this, VIEWPORT_XZ, VP_LOWERLEFT);
    viewports[VP_UPPERRIGHT] = new Viewport(this, VIEWPORT_YZ, VP_UPPERRIGHT);
    viewports[VP_LOWERRIGHT] = new Viewport(this, VIEWPORT_SKELETON, VP_LOWERRIGHT);

    viewports[VP_UPPERLEFT]->setGeometry(DEFAULT_VP_MARGIN, DEFAULT_VP_Y_OFFSET, DEFAULT_VP_SIZE, DEFAULT_VP_SIZE);
    viewports[VP_LOWERLEFT]->setGeometry(DEFAULT_VP_MARGIN, DEFAULT_VP_Y_OFFSET + DEFAULT_VP_SIZE + DEFAULT_VP_MARGIN, DEFAULT_VP_SIZE, DEFAULT_VP_SIZE);
    viewports[VP_UPPERRIGHT]->setGeometry(DEFAULT_VP_MARGIN*2 + DEFAULT_VP_SIZE, DEFAULT_VP_Y_OFFSET, DEFAULT_VP_SIZE, DEFAULT_VP_SIZE);
    viewports[VP_LOWERRIGHT]->setGeometry(DEFAULT_VP_MARGIN*2 + DEFAULT_VP_SIZE, DEFAULT_VP_Y_OFFSET + DEFAULT_VP_SIZE + DEFAULT_VP_MARGIN, DEFAULT_VP_SIZE, DEFAULT_VP_SIZE);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow:: createToolBar() {

    open = new QToolButton();
    open->setToolTip("Open");
    open->setIcon(QIcon(":/images/icons/document-open.png"));

    save = new QToolButton();
    save->setToolTip("Save");
    save->setIcon(QIcon(":/images/icons/document-save.png"));

    copyButton = new QToolButton();
    copyButton->setToolTip("Copy");
    copyButton->setIcon(QIcon(":/images/icons/edit-copy.png"));

    pasteButton = new QToolButton();
    pasteButton->setToolTip("Paste");
    pasteButton->setIcon(QIcon(":/images/icons/edit-paste.png"));

    this->toolBar = new QToolBar();
    this->toolBar->setMaximumHeight(45);
    this->addToolBar(toolBar);
    this->toolBar->addWidget(open);
    this->toolBar->addWidget(save);
    this->toolBar->addSeparator();
    this->toolBar->addWidget(copyButton);
    this->toolBar->addWidget(pasteButton);

    xField = new QSpinBox();
    xField->setMaximum(1000000);
    xField->setMinimum(1);
    xField->setMinimumWidth(75);
    xField->clearFocus();

    xField->setValue(state->viewerState->currentPosition.x + 1);
    yField = new QSpinBox();

    yField->setMaximum(1000000);
    yField->setMinimum(1);
    yField->setMinimumWidth(75);
    yField->setValue(state->viewerState->currentPosition.y + 1);
    yField->clearFocus();

    zField = new QSpinBox();
    zField->setMaximum(1000000);
    zField->setMinimum(1);
    zField->setMinimumWidth(75);
    zField->setValue(state->viewerState->currentPosition.z + 1);
    zField->clearFocus();

    xLabel = new QLabel("<font color='black'>x</font>");
    yLabel = new QLabel("<font color='black'>y</font>");
    zLabel = new QLabel("<font color='black'>z</font>");

    this->toolBar->setMovable(false);
    this->toolBar->setFloatable(false);

    this->toolBar->addWidget(xLabel);
    this->toolBar->addWidget(xField);
    this->toolBar->addWidget(yLabel);
    this->toolBar->addWidget(yField);
    this->toolBar->addWidget(zLabel);
    this->toolBar->addWidget(zField);    
    this->toolBar->addSeparator();


    pythonButton = new QToolButton();
    pythonButton->setToolTip("Python");
    pythonButton->setIcon(QIcon(":/images/python.png"));        
    this->toolBar->addWidget(pythonButton);    


    taskManagementButton = new QToolButton();
    taskManagementButton->setToolTip("Task Management Widget");
    taskManagementButton->setIcon(QIcon(":/images/icons/task.png"));
    this->toolBar->addWidget(taskManagementButton);

    tracingTimeButton = new QToolButton();
    tracingTimeButton->setToolTip("Tracing Time Widget");
    tracingTimeButton->setIcon(QIcon(":/images/icons/appointment.png"));
    this->toolBar->addWidget(tracingTimeButton);

    this->toolBar->setBackgroundRole(QPalette::Dark);

    zoomAndMultiresButton = new QToolButton();
    zoomAndMultiresButton->setToolTip("Zoom and Multiresolution Widget");
    zoomAndMultiresButton->setIcon(QIcon(":/images/icons/zoom-in.png"));
    this->toolBar->addWidget(zoomAndMultiresButton);

    /*
    syncButton = new QToolButton();
    syncButton->setToolTip("Dataset Synchronization Widget");
    syncButton->setIcon(QIcon(":images/icons/network-connect.png"));
    this->toolBar->addWidget(syncButton);
    */

    viewportSettingsButton = new QToolButton();
    viewportSettingsButton->setToolTip("Viewport Settings Widget");
    viewportSettingsButton->setIcon(QIcon(":/images/icons/view-list-icons-symbolic.png"));
    this->toolBar->addWidget(viewportSettingsButton);

//    toolsButton = new QToolButton();
//    toolsButton->setToolTip("Tools Widget");
//    toolsButton->setIcon(QIcon(":/images/icons/configure-toolbars.png"));
//    this->toolBar->addWidget(toolsButton);

    commentShortcutsButton = new QToolButton();
    commentShortcutsButton->setToolTip("Comments Shortcut Widget");
    commentShortcutsButton->setIcon(QIcon(":/images/icons/insert-text.png"));
    this->toolBar->addWidget(commentShortcutsButton);

    annotationButton = new QToolButton();
    annotationButton->setToolTip("Tools Widget");
    annotationButton->setIcon(QIcon(":/images/icons/graph.png"));
    annotationButton->setToolTip(("Tools Widget"));
    toolBar->addWidget(annotationButton);
    this->toolBar->addSeparator();
    resetVPsButton = new QPushButton("Reset VP Positions", this);
    resetVPsButton->setToolTip("Reset viewport positions and sizes");
    this->toolBar->addWidget(resetVPsButton);

    resetVPOrientButton = new QPushButton("Reset VP Orientation", this);
    resetVPOrientButton->setToolTip("Orientate viewports along xy, xz and yz axes.");
    this->toolBar->addWidget(resetVPOrientButton);
    lockVPOrientationCheckbox = new QCheckBox("lock VP orientation.");
    lockVPOrientationCheckbox->setToolTip("Lock viewports to current orientation");
    this->toolBar->addWidget(lockVPOrientationCheckbox);

    connect(open, SIGNAL(clicked()), this, SLOT(openSlot()));
    connect(save, SIGNAL(clicked()), this, SLOT(saveSlot()));

    connect(copyButton, SIGNAL(clicked()), this, SLOT(copyClipboardCoordinates()));
    connect(pasteButton, SIGNAL(clicked()), this, SLOT(pasteClipboardCoordinates())); 

    connect(xField, SIGNAL(editingFinished()), this, SLOT(coordinateEditingFinished()));
    connect(yField, SIGNAL(editingFinished()), this, SLOT(coordinateEditingFinished()));
    connect(zField, SIGNAL(editingFinished()), this, SLOT(coordinateEditingFinished()));

    //connect(syncButton, SIGNAL(clicked()), this, SLOT(synchronizationSlot()));

    //connect(pythonButton, SIGNAL(clicked()), this, SLOT());
    connect(tracingTimeButton, SIGNAL(clicked()), this, SLOT(tracingTimeSlot()));
    //connect(toolsButton, SIGNAL(clicked()), this, SLOT(toolsSlot()));
    connect(annotationButton, SIGNAL(clicked()), this, SLOT(annotationSlot()));
    connect(viewportSettingsButton, SIGNAL(clicked()), this, SLOT(viewportSettingsSlot()));
    connect(zoomAndMultiresButton, SIGNAL(clicked()), this, SLOT(zoomAndMultiresSlot()));
    connect(commentShortcutsButton, SIGNAL(clicked()), this, SLOT(commentShortcutsSlots()));
    connect(taskManagementButton, SIGNAL(clicked()), this, SLOT(taskSlot()));

    connect(resetVPsButton, SIGNAL(clicked()), this, SLOT(resetViewports()));
    connect(resetVPOrientButton, SIGNAL(clicked()), this, SLOT(resetVPOrientation()));
    connect(lockVPOrientationCheckbox, SIGNAL(clicked(bool)), this, SLOT(lockVPOrientation(bool)));

    connect(widgetContainer->viewportSettingsWidget->generalTabWidget->resetVPsButton, SIGNAL(clicked()), this, SLOT(resetViewports()));
    connect(widgetContainer->viewportSettingsWidget->generalTabWidget->showVPDecorationCheckBox, SIGNAL(clicked()), this, SLOT(showVPDecorationClicked()));
}

void MainWindow::updateTitlebar(bool useFilename) {
    QString title;
    if(!state->skeletonState->skeletonFileAsQString.isNull()) {
        title = QString("KNOSSOS %1 Revision %2 showing %3").arg(KVERSION).arg(REVISION).arg(state->skeletonState->skeletonFileAsQString);

    } else {
        title = QString("KNOSSOS %1 Revision %2 showing %3").arg(KVERSION).arg(REVISION).arg("no skeleton file");
    }

    setWindowTitle(title);

}

// -- static methods -- //

bool MainWindow::cpBaseDirectory(char *target, QString path){
    int hit;
#ifdef Q_OS_UNIX
    hit = path.indexOf('/');
#else
    hit = path.indexOf('\\');
#endif
    if(hit == -1) {
        qDebug() << "no path separator in " << path;
        return false;
    }
    if(hit > 2047) {
        qDebug("Path too long.");
        return false;
    }
    sprintf(target, "%s\0", path.mid(0, hit).toStdString().c_str());
    return true;
}

/** This method adds a file path to the queue data structure
    Use this method only after checking if the entry is already in the menu
*/

bool MainWindow::addRecentFile(const QString &fileName) {

    if(skeletonFileHistory->size() < FILE_DIALOG_HISTORY_MAX_ENTRIES) {
        skeletonFileHistory->enqueue(fileName);
    } else {
        skeletonFileHistory->dequeue();
        skeletonFileHistory->enqueue(fileName);
    }

    updateFileHistoryMenu();
    return true;
}

/**
  * @todo Replacements for the Labels
  * Maybe functionality of Viewport
  */
void MainWindow::reloadDataSizeWin(){
    float heightxy = state->viewerState->vpConfigs[VIEWPORT_XY].displayedlengthInNmY*0.001;
    float widthxy = state->viewerState->vpConfigs[VIEWPORT_XY].displayedlengthInNmX*0.001;
    float heightxz = state->viewerState->vpConfigs[VIEWPORT_XZ].displayedlengthInNmY*0.001;
    float widthxz = state->viewerState->vpConfigs[VIEWPORT_XZ].displayedlengthInNmX*0.001;
    float heightyz = state->viewerState->vpConfigs[VIEWPORT_YZ].displayedlengthInNmY*0.001;
    float widthyz = state->viewerState->vpConfigs[VIEWPORT_YZ].displayedlengthInNmX*0.001;

    if ((heightxy > 1.0) && (widthxy > 1.0)){
        //AG_LabelText(state->viewerState->gui->dataSizeLabelxy, "Height %.2f \u00B5m, Width %.2f \u00B5m", heightxy, widthxy);
    }
    else{
        //AG_LabelText(state->viewerState->gui->dataSizeLabelxy, "Height %.0f nm, Width %.0f nm", heightxy*1000, widthxy*1000);
    }
    if ((heightxz > 1.0) && (widthxz > 1.0)){
        //AG_LabelText(state->viewerState->gui->dataSizeLabelxz, "Height %.2f \u00B5m, Width %.2f \u00B5m", heightxz, widthxz);
    }
    else{
       // AG_LabelText(state->viewerState->gui->dataSizeLabelxz, "Height %.0f nm, Width %.0f nm", heightxz*1000, widthxz*1000);
    }

    if ((heightyz > 1.0) && (widthyz > 1.0)){
        //AG_LabelText(state->viewerState->gui->dataSizeLabelyz, "Height %.2f \u00B5m, Width %.2f \u00B5m", heightyz, widthyz);
    }
    else{
        //AG_LabelText(state->viewerState->gui->dataSizeLabelyz, "Height %.0f nm, Width %.0f nm", heightyz*1000, widthyz*1000);
    }
}

void MainWindow::treeColorAdjustmentsChanged() {
    //user lut activated
        if(state->viewerState->treeColortableOn) {
            //user lut selected
            if(state->viewerState->treeLutSet) {
                memcpy(state->viewerState->treeAdjustmentTable,
                state->viewerState->treeColortable,
                RGB_LUTSIZE * sizeof(float));
                emit updateTreeColorsSignal();
            }
            else {
                memcpy(state->viewerState->treeAdjustmentTable,
                state->viewerState->defaultTreeTable,
                RGB_LUTSIZE * sizeof(float));
            }
        }
        //use of default lut
        else {
            memcpy(state->viewerState->treeAdjustmentTable,
            state->viewerState->defaultTreeTable,
            RGB_LUTSIZE * sizeof(float));
                    emit updateTreeColorsSignal();
            }
}

void MainWindow::datasetColorAdjustmentsChanged() {
    bool doAdjust = false;
        int i = 0;
        int dynIndex;
        GLuint tempTable[3][256];

        if(state->viewerState->datasetColortableOn) {
            memcpy(state->viewerState->datasetAdjustmentTable,
                   state->viewerState->datasetColortable,
                   RGB_LUTSIZE * sizeof(GLuint));
            doAdjust = true;
        } else {
            memcpy(state->viewerState->datasetAdjustmentTable,
                   state->viewerState->neutralDatasetTable,
                   RGB_LUTSIZE * sizeof(GLuint));
        }

        /*
         * Apply the dynamic range settings to the adjustment table
         *
         */
        if((state->viewerState->luminanceBias != 0) ||
           (state->viewerState->luminanceRangeDelta != MAX_COLORVAL)) {
            for(i = 0; i < 256; i++) {
                dynIndex = (int)((i - state->viewerState->luminanceBias) /
                                     (state->viewerState->luminanceRangeDelta / MAX_COLORVAL));

                if(dynIndex < 0)
                    dynIndex = 0;
                if(dynIndex > MAX_COLORVAL)
                    dynIndex = MAX_COLORVAL;

                tempTable[0][i] = state->viewerState->datasetAdjustmentTable[0][dynIndex];
                tempTable[1][i] = state->viewerState->datasetAdjustmentTable[1][dynIndex];
                tempTable[2][i] = state->viewerState->datasetAdjustmentTable[2][dynIndex];
            }

            for(i = 0; i < 256; i++) {
                state->viewerState->datasetAdjustmentTable[0][i] = tempTable[0][i];
                state->viewerState->datasetAdjustmentTable[1][i] = tempTable[1][i];
                state->viewerState->datasetAdjustmentTable[2][i] = tempTable[2][i];
            }

            doAdjust = true;
        }
       state->viewerState->datasetAdjustmentOn = doAdjust;
}


void MainWindow::createActions()
{
    /* file actions */
    historyEntryActions = new QAction*[FILE_DIALOG_HISTORY_MAX_ENTRIES];
    for(int i = 0; i < FILE_DIALOG_HISTORY_MAX_ENTRIES; i++) {
        historyEntryActions[i] = new QAction(QIcon(":/images/icons/document-open-recent.png"), "", this);
        historyEntryActions[i]->setVisible(false);
        connect(historyEntryActions[i], SIGNAL(triggered()), this, SLOT(recentFileSelected()));
    }

    /* edit skeleton actions */
    addNodeAction = new QAction(tr("Add Node"), this);
    addNodeAction->setCheckable(true);
    linkWithActiveNodeAction = new QAction(tr("Link with Active Node(W)"), this);
    linkWithActiveNodeAction->setCheckable(true);
    dropNodesAction = new QAction(tr("Drop Nodes"), this);
    dropNodesAction->setCheckable(true);
    skeletonStatisticsAction = new QAction(tr("Skeleton Statistics"), this);


    if(state->skeletonState->workMode == SKELETONIZER_ON_CLICK_ADD_NODE) {
        addNodeAction->setChecked(true);
    } else if(state->skeletonState->workMode == SKELETONIZER_ON_CLICK_LINK_WITH_ACTIVE_NODE) {
        linkWithActiveNodeAction->setChecked(true);
    } else if(state->skeletonState->workMode == SKELETONIZER_ON_CLICK_DROP_NODE) {
        dropNodesAction->setChecked(true);
    }

    connect(addNodeAction, SIGNAL(triggered()), this, SLOT(addNodeSlot()));
    connect(linkWithActiveNodeAction, SIGNAL(triggered()), this, SLOT(linkWithActiveNodeSlot()));
    connect(dropNodesAction, SIGNAL(triggered()), this, SLOT(dropNodesSlot()));
    connect(skeletonStatisticsAction, SIGNAL(triggered()), this, SLOT(skeletonStatisticsSlot()));


    /* view actions */
    workModeViewAction = new QAction(tr("Work Mode"), this);
    dragDatasetAction = new QAction(tr("Drag Dataset"), this);
    dragDatasetAction->setCheckable(true);
    recenterOnClickAction = new QAction(tr("Recenter on Click"), this);
    recenterOnClickAction->setCheckable(true);

    if(state->viewerState->workMode == ON_CLICK_DRAG) {
        dragDatasetAction->setChecked(true);
    } else if(state->viewerState->workMode == ON_CLICK_RECENTER) {
        recenterOnClickAction->setChecked(true);
    }

    connect(dragDatasetAction, SIGNAL(triggered()), this, SLOT(dragDatasetSlot()));
    connect(recenterOnClickAction, SIGNAL(triggered()), this, SLOT(recenterOnClickSlot()));

    /* preferences actions */
    loadCustomPreferencesAction = new QAction(tr("Load Custom Preferences"), this);
    saveCustomPreferencesAction = new QAction(tr("Save Custom Preferences"), this);
    defaultPreferencesAction = new QAction(tr("Default Preferences"), this);
    datasetNavigationAction = new QAction(tr("Dataset Navigation"), this);
    datasetNavigationAction->setCheckable(true);
    synchronizationAction = new QAction(tr("Synchronization"), this);
    synchronizationAction->setCheckable(true);
    dataSavingOptionsAction = new QAction(tr("Data Saving Options"), this);
    dataSavingOptionsAction->setCheckable(true);
    viewportSettingsAction = new QAction(tr("Viewport Settings"), this);
    viewportSettingsAction->setCheckable(true);

    connect(loadCustomPreferencesAction, SIGNAL(triggered()), this, SLOT(loadCustomPreferencesSlot()));
    connect(saveCustomPreferencesAction, SIGNAL(triggered()), this, SLOT(saveCustomPreferencesSlot()));
    connect(defaultPreferencesAction, SIGNAL(triggered()), this, SLOT(defaultPreferencesSlot()));
    connect(datasetNavigationAction, SIGNAL(triggered()), this, SLOT(datatasetNavigationSlot()));    
    connect(dataSavingOptionsAction, SIGNAL(triggered()), this, SLOT(dataSavingOptionsSlot()));

    /* window actions */
    toolsAction = new QAction(tr("Tools"), this);
    toolsAction->setCheckable(true);
    //taskLoginAction = new QAction(tr("Task Management"), this);
    //taskLoginAction->setCheckable(true);
    logAction = new QAction(tr("Log"), this);
    logAction->setCheckable(true);
    commentShortcutsAction = new QAction(tr("Comment Settings"), this);
    commentShortcutsAction->setCheckable(true);

    connect(logAction, SIGNAL(triggered()), this, SLOT(logSlot()));

    annotationAction = new QAction(tr("Annotation Widget"), this);
    annotationAction->setCheckable(true);
    //connect(taskLoginAction, SIGNAL(triggered()), this, SLOT(taskLoginSlot()));

    /* Help actions */
    //aboutAction = new QAction(tr("About"), this);
    //connect(aboutAction, SIGNAL(triggered()), this, SLOT(aboutSlot()));

}

/** This slot is called if one of the entries is clicked in the recent file menue */
void MainWindow::recentFileSelected() {
    QAction *action = (QAction *)sender();

    QString fileName = action->text();
    if(!fileName.isNull()) {
        this->loadSkeletonAfterUserDecision(fileName);

    }
}

void MainWindow::createMenus()
{

    dataSetMenu = menuBar()->addMenu("Dataset");
    dataSetMenu->addAction(QIcon(":/images/icons/document-open.png"), "Open", this, SLOT(openDatasetSlot()));

    fileMenu = menuBar()->addMenu("Skeleton File");
    fileMenu->addAction(QIcon(":/images/icons/document-open.png"), "Open...", this, SLOT(openSlot()), QKeySequence(tr("CTRL+O", "File|Open")));
    recentFileMenu = fileMenu->addMenu(QIcon(":/images/icons/document-open-recent.png"), QString("Recent File(s)"));

    for(int i = 0; i < FILE_DIALOG_HISTORY_MAX_ENTRIES; i++) {
        recentFileMenu->addAction(historyEntryActions[i]);
    }

    fileMenu->addAction(QIcon(":/images/icons/document-save.png"), "Save", this, SLOT(saveSlot()), QKeySequence(tr("CTRL+S", "File|Save")));
    fileMenu->addAction(QIcon(":/images/icons/document-save-as.png"), "Save As...", this, SLOT(saveAsSlot()));
    fileMenu->addSeparator();
    fileMenu->addAction(QIcon(":/images/icons/system-shutdown.png"), "Quit", this, SLOT(quitSlot()), QKeySequence(tr("CTRL+Q", "File|Quit")));

    editMenu = menuBar()->addMenu("Edit Skeleton");
    workModeEditMenu = editMenu->addMenu("Work Mode");
        workModeEditMenu->addAction(addNodeAction);
        workModeEditMenu->addAction(linkWithActiveNodeAction);
        workModeEditMenu->addAction(dropNodesAction);
    //editMenu->addAction(skeletonStatisticsAction);

    newTreeAction = editMenu->addAction(QIcon(""), "New Tree", this, SLOT(newTreeSlot()));
    newTreeAction->setShortcut(QKeySequence(tr("C")));
    newTreeAction->setShortcutContext(Qt::ApplicationShortcut);

    moveToNextNodeAction = editMenu->addAction(QIcon(""), "Move To Next Node", this, SLOT(moveToNextNodeSlot()));
    moveToNextNodeAction->setShortcut(QKeySequence(tr("X")));
    moveToNextNodeAction->setShortcutContext(Qt::ApplicationShortcut);

    moveToPrevNodeAction = editMenu->addAction(QIcon(""), "Move To Previous Node", this, SLOT(moveToPrevNodeSlot()));
    moveToPrevNodeAction->setShortcut(QKeySequence(tr("SHIFT+X")));
    moveToPrevNodeAction->setShortcutContext(Qt::ApplicationShortcut);

    moveToNextTreeAction = editMenu->addAction(QIcon(""), "Move To Next Tree", this, SLOT(moveToNextTreeSlot()));
    moveToNextTreeAction->setShortcut(QKeySequence(tr("Z")));
    moveToNextTreeAction->setShortcutContext(Qt::ApplicationShortcut);

    moveToPrevTreeAction = editMenu->addAction(QIcon(""), "Move To Previous Tree", this, SLOT(moveToPrevTreeSlot()));
    moveToPrevTreeAction->setShortcut(QKeySequence(tr("SHIFT+Z")));
    moveToPrevTreeAction->setShortcutContext(Qt::ApplicationShortcut);

    pushBranchNodeAction = editMenu->addAction(QIcon(""), "Push Branch Node", this, SLOT(pushBranchNodeSlot()));
    pushBranchNodeAction->setShortcut(QKeySequence(tr("B")));
    pushBranchNodeAction->setShortcutContext(Qt::ApplicationShortcut);

    popBranchNodeAction = editMenu->addAction(QIcon(""), "Pop Branch Node", this, SLOT(popBranchNodeSlot()));
    popBranchNodeAction->setShortcut(QKeySequence(tr("J")));
    popBranchNodeAction->setShortcutContext(Qt::ApplicationShortcut);

    jumpToActiveNodeAction = editMenu->addAction(QIcon(""), "Jump To Active Node", this, SLOT(jumpToActiveNodeSlot()));
    jumpToActiveNodeAction->setShortcut(QKeySequence(tr("S")));
    jumpToActiveNodeAction->setShortcutContext(Qt::ApplicationShortcut);

    editMenu->addSeparator();

    nextCommentAction = editMenu->addAction(QIcon(""), "Next Comment", this, SLOT(nextCommentNodeSlot()));
    nextCommentAction->setShortcut(QKeySequence(tr("N")));
    nextCommentAction->setShortcutContext(Qt::ApplicationShortcut);

    previousCommentAction = editMenu->addAction(QIcon(""), "Previous Comment", this, SLOT(previousCommentNodeSlot()));
    previousCommentAction->setShortcut(QKeySequence(tr("P")));
    previousCommentAction->setShortcutContext(Qt::ApplicationShortcut);

    F1Action = editMenu->addAction(QIcon(""), "Comment Shortcut", this, SLOT(F1Slot()));
    F1Action->setShortcut(Qt::Key_F1);
    F1Action->setShortcutContext(Qt::ApplicationShortcut);

    F2Action = editMenu->addAction(QIcon(""), "Comment Shortcut", this, SLOT(F2Slot()));
    F2Action->setShortcut(Qt::Key_F2);
    F2Action->setShortcutContext(Qt::ApplicationShortcut);

    F3Action = editMenu->addAction(QIcon(""), "Comment Shortcut", this, SLOT(F3Slot()));
    F3Action->setShortcut(Qt::Key_F3);
    F3Action->setShortcutContext(Qt::ApplicationShortcut);

    F1Action = editMenu->addAction(QIcon(""), "Comment Shortcut", this, SLOT(F4Slot()));
    F1Action->setShortcut(Qt::Key_F4);
    F1Action->setShortcutContext(Qt::ApplicationShortcut);

    F5Action = editMenu->addAction(QIcon(""), "Comment Shortcut", this, SLOT(F5Slot()));
    F5Action->setShortcut(Qt::Key_F5);
    F5Action->setShortcutContext(Qt::ApplicationShortcut);

    editMenu->addAction(QIcon(":/images/icons/user-trash.png"), "Clear Skeleton", this, SLOT(clearSkeletonSlot()));

    viewMenu = menuBar()->addMenu("Navigation");
    workModeViewMenu = viewMenu->addMenu("Work Mode");
        workModeViewMenu->addAction(dragDatasetAction);
        workModeViewMenu->addAction(recenterOnClickAction);

    viewMenu->addAction(datasetNavigationAction);


    preferenceMenu = menuBar()->addMenu("Preferences");
    preferenceMenu->addAction(loadCustomPreferencesAction);
    preferenceMenu->addAction(saveCustomPreferencesAction);
    preferenceMenu->addAction(defaultPreferencesAction);
    //synchronizationAction = preferenceMenu->addAction(QIcon(":/images/icons/network-connect.png"), "Synchronization", this, SLOT(synchronizationSlot()));
    preferenceMenu->addAction(dataSavingOptionsAction);

    viewportSettingsAction = preferenceMenu->addAction(QIcon(":/images/icons/view-list-icons-symbolic.png"), "Viewport Settings", this, SLOT(viewportSettingsSlot()));

    windowMenu = menuBar()->addMenu("Windows");
    //toolsAction = windowMenu->addAction(QIcon(":/images/icons/configure-toolbars.png"), "Tools", this, SLOT(toolsSlot()));
    taskAction = windowMenu->addAction(QIcon(":/images/icons/task.png"), "Task Management", this, SLOT(taskSlot()));

    commentShortcutsAction = windowMenu->addAction(QIcon(":/images/icons/insert-text.png"), "Comment Settings", this, SLOT(commentShortcutsSlots()));
    annotationAction = windowMenu->addAction(QIcon(":/images/icons/graph.png"), "Tools", this, SLOT(annotationSlot()));
    this->zoomAndMultiresAction = windowMenu->addAction(QIcon(":/images/icons/zoom-in.png"), "Zoom and Multiresolution", this, SLOT(zoomAndMultiresSlot()));
    this->tracingTimeAction = windowMenu->addAction(QIcon(":/images/icons/appointment.png"), "Tracing Time", this, SLOT(tracingTimeSlot()));


    helpMenu = menuBar()->addMenu("Help");
    helpMenu->addAction(QIcon(":/images/icons/edit-select-all.png"), "About", this, SLOT(aboutSlot()));
    helpMenu->addAction(QIcon(":/images/icons/edit-select-all.png"), "Documentation", this, SLOT(documentationSlot()), QKeySequence(tr("CTRL+H")));
}

void MainWindow::closeEvent(QCloseEvent *event) {
    saveSettings();

    if(state->skeletonState->unsavedChanges) {
         QMessageBox question;
         question.setWindowFlags(Qt::WindowStaysOnTopHint);
         question.setIcon(QMessageBox::Question);
         question.setWindowTitle("Confirmation required.");
         question.setText("There are unsaved changes. Really Quit?");
         QPushButton *yes = question.addButton("Yes", QMessageBox::ActionRole);
         question.addButton("No", QMessageBox::ActionRole);
         question.exec();
         if(question.clickedButton() == yes) {
             event->accept();
         } else {
             event->ignore();
         }
    }
}

//file menu functionality

bool MainWindow::loadSkeletonAfterUserDecision
(const QString &fileName) {
    if(!fileName.isNull()) {
        QApplication::processEvents();
        QFileInfo info(fileName);
        QString path = info.canonicalPath();

        if(state->skeletonState->treeElements > 0) {
            QMessageBox prompt;
            prompt.setWindowFlags(Qt::WindowStaysOnTopHint);
            prompt.setIcon(QMessageBox::Question);
            prompt.setText("Which Action do you like to choose?<ul><li>Merge the new Skeleton into the current one ?</li><li>Override the current Skeleton</li><li>Cancel</li></ul>");
            QPushButton *merge = prompt.addButton("Merge", QMessageBox::ActionRole);
            QPushButton *override = prompt.addButton("Override", QMessageBox::ActionRole);
            prompt.addButton("Cancel", QMessageBox::ActionRole);
            prompt.exec();

            if(prompt.clickedButton() == merge) {
                state->skeletonState->mergeOnLoadFlag = true;
            } else if(prompt.clickedButton() == override) {
                state->skeletonState->mergeOnLoadFlag = false;
            } else {
                return false;
            }

        }

        state->skeletonState->skeletonFileAsQString = fileName;


        bool result = loadSkeletonSignal(fileName);
        //QFuture<bool> future = QtConcurrent::run(this, &MainWindow::loadSkeletonSignal, fileName);
        //future.waitForFinished();

        //emit updateCommentsTableSignal();
        updateTitlebar(true);
        linkWithActiveNodeSlot();

        if(!alreadyInMenu(fileName)) {
            addRecentFile(fileName);
        } else {
            becomeFirstEntry(fileName);
        }
        //emit updateToolsSignal();
        emit updateTreeviewSignal();
        return result;
    }
    return false;
}

/** if a queue´s entry is reused and not more at position zero it will moved to the first entry (most recent) and the
 *  menu will be updated
*/
void MainWindow::becomeFirstEntry(const QString &entry) {
    int index = skeletonFileHistory->indexOf(entry);
    if(index > 0) {
        skeletonFileHistory->move(index, 0);
        updateFileHistoryMenu();
    }
}

/**
  * This method opens the file dialog and receives a skeleton file name path. If the file dialog is not cancelled
  * the skeletonFileHistory Queue is updated with the file name entry. The history entries are compared to the the
  * selected file names. If the file is already loaded it will not be put to the queue
  * @todo lookup in skeleton directory, extend the file dialog with merge option
  *
  */
void MainWindow::openSlot() {
    state->viewerState->renderInterval = SLOW;
    QString fileName = QFileDialog::getOpenFileName(this, "Open Skeleton File", *this->openFileDirectory, "KNOSSOS Skeleton file(*.nml)");
    if(!fileName.isEmpty()) {
        QFileInfo info(fileName);
        openFileDirectory = new QString(info.dir().absolutePath());
        state->skeletonState->skeletonFileAsQString = fileName;
        loadSkeletonAfterUserDecision(fileName);
    }

    state->viewerState->renderInterval = FAST;
}

/** So far this variant is needed only for drag n drop skeleton files */
void MainWindow::openSlot(const QString &fileName) {
    state->viewerState->renderInterval = SLOW;
    loadSkeletonAfterUserDecision(fileName);
    state->viewerState->renderInterval = FAST;

}

/** This method checks if a candidate is already in the queue */
bool MainWindow::alreadyInMenu(const QString &path) {

    for(int i = 0; i < this->skeletonFileHistory->size(); i++) {
        qDebug() << skeletonFileHistory->at(i) << "_" << path;
        if(!QString::compare(skeletonFileHistory->at(i), path, Qt::CaseSensitive)) {
            return true;
        }
    }
    return false;
}


/**
  * This method puts the history entries of the loaded skeleton files to the recent file menu section
  * The order is: 1. AddRecentFile and then 2. updateFileHistoryMenu
  */
void MainWindow::updateFileHistoryMenu() {

    QQueue<QString>::iterator it;
    int i = 0;
    for(it = skeletonFileHistory->begin(); it != skeletonFileHistory->end(); it++) {
        QString path = *it;

        historyEntryActions[i]->setText(path);
        if(!historyEntryActions[i]->text().isEmpty()) {
            //recentFileMenu->addAction(QIcon(":/images/icons/document-open-recent.png"), historyEntryActions[i]->text(), this, SLOT(recentFileSelected()));
            historyEntryActions[i]->setVisible(true);
        } else {
            historyEntryActions[i]->setVisible(false);
        }
        i++;
    }
}

void MainWindow::saveSlot()
{

    if(state->skeletonState->firstTree != NULL) {
        if(state->skeletonState->unsavedChanges) {

            if(state->skeletonState->autoFilenameIncrementBool) {

                // solution for a clever recent file menu (not yet activated)
                int index = skeletonFileHistory->indexOf(state->skeletonState->skeletonFileAsQString);

                updateSkeletonFileName(state->skeletonState->skeletonFileAsQString);

                if(state->skeletonState->autoSaveBool) {
                    if(index >= 0) {
                        skeletonFileHistory->replace(index, state->skeletonState->skeletonFileAsQString);
                        historyEntryActions[index]->setText(skeletonFileHistory->at(index));
                        becomeFirstEntry(state->skeletonState->skeletonFileAsQString);


                    }
                }

            }

            emit saveSkeletonSignal(state->skeletonState->skeletonFileAsQString);

            if(!alreadyInMenu(state->skeletonState->skeletonFileAsQString)) {
                addRecentFile(state->skeletonState->skeletonFileAsQString);
            } else {
                becomeFirstEntry(state->skeletonState->skeletonFileAsQString);
            }

                updateTitlebar(true);
                state->skeletonState->unsavedChanges = false;
        }
    }
    emit idleTimeSignal();
}

void MainWindow::saveAsSlot()
{
    state->viewerState->renderInterval = SLOW;
    QApplication::processEvents();
    if(!state->skeletonState->firstTree) {
        QMessageBox prompt;
        prompt.setWindowFlags(Qt::WindowStaysOnTopHint);
        prompt.setIcon(QMessageBox::Information);
        prompt.setWindowTitle("Information");
        prompt.setText("No skeleton was found. Not saving!");
        prompt.exec();
        return;
    }

    QString fileName = QFileDialog::getSaveFileName(this, "Save the KNOSSOS Skeleton file", *saveFileDirectory, "KNOSSOS Skeleton file(*.nml)");
    if(!fileName.isEmpty()) {
        QFileInfo info(fileName);
        saveFileDirectory = new QString(info.dir().absolutePath());

        if(state->skeletonState->autoFilenameIncrementBool) {
            updateSkeletonFileName(fileName);
        }

        state->skeletonState->skeletonFileAsQString = fileName;

        emit saveSkeletonSignal(fileName);

        if(!alreadyInMenu(state->skeletonState->skeletonFileAsQString)) {
            addRecentFile(state->skeletonState->skeletonFileAsQString);
        } else {
            becomeFirstEntry(state->skeletonState->skeletonFileAsQString);
        }

        updateTitlebar(true);
        state->skeletonState->unsavedChanges = false;

    }
    state->viewerState->renderInterval = FAST;
}


void MainWindow::quitSlot()
{
    this->close();
}

/* edit skeleton functionality */

void MainWindow::addNodeSlot()
{
    state->skeletonState->workMode = SKELETONIZER_ON_CLICK_ADD_NODE;

    if(linkWithActiveNodeAction->isChecked()) {
        linkWithActiveNodeAction->setChecked(false);
    }
    if(dropNodesAction->isChecked()) {
        dropNodesAction->setChecked(false);
    }

    if(!addNodeAction->isChecked()) {
        addNodeAction->setChecked(true);
    }
}

void MainWindow::linkWithActiveNodeSlot()
{
    state->skeletonState->workMode = SKELETONIZER_ON_CLICK_LINK_WITH_ACTIVE_NODE;

    if(addNodeAction->isChecked()) {
        addNodeAction->setChecked(false);
    }
    if(dropNodesAction->isChecked()) {
        dropNodesAction->setChecked(false);
    }

    if(!linkWithActiveNodeAction->isChecked()) {
        linkWithActiveNodeAction->setChecked(true);
    }
}

void MainWindow::dropNodesSlot()
{
    state->skeletonState->workMode = SKELETONIZER_ON_CLICK_DROP_NODE;

    if(addNodeAction->isChecked()) {
        addNodeAction->setChecked(false);
    }

    if(linkWithActiveNodeAction->isChecked()) {
        linkWithActiveNodeAction->setChecked(false);
    }

    if(!dropNodesAction->isChecked()) {
        dropNodesAction->setChecked(true);
    }
}


void MainWindow::skeletonStatisticsSlot()
{
    QMessageBox info;
    info.setIcon(QMessageBox::Information);
    info.setWindowTitle("Information");
    info.setText("This feature is not yet implemented");
    info.setWindowFlags(Qt::WindowStaysOnTopHint);
    info.addButton(QMessageBox::Ok);
    info.exec();
}


void MainWindow::clearSkeletonWithoutConfirmation() {
    emit clearSkeletonSignal(CHANGE_MANUAL, false);
    updateTitlebar(false);
    emit updateToolsSignal();
    emit updateTreeviewSignal();
    emit updateCommentsTableSignal();
}

void MainWindow::clearSkeletonSlot()
{
    QMessageBox question;
    question.setWindowFlags(Qt::WindowStaysOnTopHint);
    question.setIcon(QMessageBox::Question);
    question.setWindowTitle("Confirmation required");
    question.setText("Really clear skeleton (you cannot undo this)?");
    QPushButton *ok = question.addButton(QMessageBox::Ok);
    question.addButton(QMessageBox::No);
    question.exec();
    if(question.clickedButton() == ok) {
        emit clearSkeletonSignal(CHANGE_MANUAL, false);
        updateTitlebar(false);
        emit updateTreeviewSignal();
    }
}

/* view menu functionality */

void MainWindow::dragDatasetSlot() {
   state->viewerState->workMode = ON_CLICK_DRAG;
   if(recenterOnClickAction->isChecked()) {
       recenterOnClickAction->setChecked(false);
   }
}

void MainWindow::recenterOnClickSlot() {
   state->viewerState->workMode = ON_CLICK_RECENTER;
   if(dragDatasetAction->isChecked()) {
       dragDatasetAction->setChecked(false);
   }
}

void MainWindow::zoomAndMultiresSlot() {
    this->widgetContainer->zoomAndMultiresWidget->show();
}

void MainWindow::tracingTimeSlot() {
    this->widgetContainer->tracingTimeWidget->show();
}

/* preference menu functionality */
void MainWindow::loadCustomPreferencesSlot()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Open Custom Preferences File", QDir::homePath(), "KNOSOS GUI preferences File (*.ini)");
    if(!fileName.isEmpty()) {      
        QSettings::setUserIniPath(fileName);
        loadSettings();

    }
}

void MainWindow::saveCustomPreferencesSlot()
{   
    saveSettings();
    QSettings settings;
    QString originSettings = settings.fileName();

    QString fileName = QFileDialog::getSaveFileName(this, "Save Custom Preferences File As", QDir::homePath(), "KNOSSOS GUI preferences File (*.ini)");    
    if(!fileName.isEmpty()) {
        QFile file;
        file.setFileName(originSettings);
        file.copy(fileName);
    }
}


void MainWindow::defaultPreferencesSlot() {
    QMessageBox question;
    question.setWindowFlags(Qt::WindowStaysOnTopHint);
    question.setIcon(QMessageBox::Question);
    question.setWindowTitle("Confirmation required");
    question.setText("Do you really want to load the default preferences?");
    QPushButton *yes = question.addButton(QMessageBox::Yes);
    question.addButton(QMessageBox::No);
    question.exec();

    if(question.clickedButton() == yes) {
        clearSettings();
        loadSettings();
//        saveSettings();
//        widgetContainer->zoomAndMultiresWidget->lockDatasetCheckBox->setChecked(true);
//        //widgetContainer->toolsWidget->toolsNodesTabWidget->defaultNodeRadiusSpinBox->setValue(1.5);
//        widgetContainer->dataSavingWidget->autosaveIntervalSpinBox->setValue(5);
        emit loadTreeLUTFallback();
        treeColorAdjustmentsChanged();
        datasetColorAdjustmentsChanged();
        this->setGeometry(QApplication::desktop()->availableGeometry().topLeft().x() + 20,
                          QApplication::desktop()->availableGeometry().topLeft().y() + 50, 1024, 800);
    }
}

void MainWindow::datatasetNavigationSlot() {
    this->widgetContainer->navigationWidget->show();
    datasetNavigationAction->setChecked(true);
}

void MainWindow::synchronizationSlot()
{
    this->widgetContainer->synchronizationWidget->show();
    this->widgetContainer->synchronizationWidget->adjustSize();
    if(this->widgetContainer->synchronizationWidget->pos().x() <= 0 or this->widgetContainer->synchronizationWidget->pos().y() <= 0)
        this->widgetContainer->synchronizationWidget->move(QWidget::mapToGlobal(mainWidget->pos()));
    this->widgetContainer->synchronizationWidget->move(QWidget::mapFromGlobal(mainWidget->pos()));
    synchronizationAction->setChecked(true);
    //this->widgetContainer->synchronizationWidget->setFixedSize(this->widgetContainer->);
}

void MainWindow::dataSavingOptionsSlot() {
    this->widgetContainer->dataSavingWidget->show();
    dataSavingOptionsAction->setChecked(true);
}

void MainWindow::viewportSettingsSlot() {
    this->widgetContainer->viewportSettingsWidget->show();
    viewportSettingsAction->setChecked(true);
}

/* window menu functionality */

void MainWindow::toolsSlot()
{
    this->widgetContainer->toolsWidget->show();
    this->widgetContainer->toolsWidget->adjustSize();
    if(widgetContainer->toolsWidget->pos().x() <= 0 or this->widgetContainer->toolsWidget->pos().y() <= 0)
        this->widgetContainer->toolsWidget->move(QWidget::mapToGlobal(mainWidget->pos()));
    toolsAction->setChecked(true);
    this->widgetContainer->toolsWidget->setFixedSize(this->widgetContainer->toolsWidget->size());
}

void MainWindow::logSlot()
{
    this->widgetContainer->console->show();
    this->widgetContainer->console->adjustSize();
    if(widgetContainer->console->pos().x() <= 0 or this->widgetContainer->console->pos().y() <= 0)
        this->widgetContainer->console->move(QWidget::mapToGlobal(mainWidget->pos()));
    logAction->setChecked(true);
}

void MainWindow::commentShortcutsSlots() {
    this->widgetContainer->commentsWidget->show();
}

void MainWindow::annotationSlot() {
    this->widgetContainer->annotationWidget->show();
}

/* help menu functionality */

void MainWindow::aboutSlot() {
    this->widgetContainer->splashWidget->show();
}

void MainWindow::documentationSlot() {
    this->widgetContainer->docWidget->show();
}

/* toolbar slots */

void MainWindow::copyClipboardCoordinates() {
   char copyString[8192];

   memset(copyString, '\0', 8192);

   snprintf(copyString,
                 8192,
                 "%d, %d, %d",
                 this->xField->value() + 1,
                 this->yField->value() + 1,
                 this->zField->value() + 1);
   QString coords(copyString);
   QApplication::clipboard()->setText(coords);
}

void MainWindow::pasteClipboardCoordinates(){
    QString text = QApplication::clipboard()->text();

    if(text.size() > 0) {
      char *pasteBuffer = const_cast<char *> (text.toStdString().c_str());

      Coordinate *extractedCoords = NULL;
      if((extractedCoords = Coordinate::parseRawCoordinateString(pasteBuffer))) {

            this->xField->setValue(extractedCoords->x);
            this->yField->setValue(extractedCoords->y);
            this->zField->setValue(extractedCoords->z);

            emit userMoveSignal(extractedCoords->x - 1 - state->viewerState->currentPosition.x,
                                extractedCoords->y - 1 - state->viewerState->currentPosition.y,
                                extractedCoords->z - 1 - state->viewerState->currentPosition.z,
                                TELL_COORDINATE_CHANGE);

            free(extractedCoords);

      } else {
          LOG("Unexpected Error in MainWindow::pasteCliboardCoordinates");
      }

    } else {
       LOG("Unable to fetch text from clipboard")
    }
}

void MainWindow::coordinateEditingFinished() {
    emit userMoveSignal(xField->value()- 1 - state->viewerState->currentPosition.x,
                        yField->value()- 1 - state->viewerState->currentPosition.y,
                        zField->value()- 1 - state->viewerState->currentPosition.z,
                        TELL_COORDINATE_CHANGE);
}

void MainWindow::saveSettings() {
    QSettings settings;

    settings.beginGroup(MAIN_WINDOW);
    settings.setValue(WIDTH, this->geometry().width());
    settings.setValue(HEIGHT, this->geometry().height());
    settings.setValue(POS_X, this->geometry().x());
    settings.setValue(POS_Y, this->geometry().y());

    // viewport position and sizes
    settings.setValue(VP_DEFAULT_POS_SIZE, state->viewerState->defaultVPSizeAndPos);
    settings.setValue(VPXY_SIZE, viewports[VIEWPORT_XY]->size().height());
    settings.setValue(VPXZ_SIZE, viewports[VIEWPORT_XZ]->size().height());
    settings.setValue(VPYZ_SIZE, viewports[VIEWPORT_YZ]->size().height());
    settings.setValue(VPSKEL_SIZE, viewports[VIEWPORT_SKELETON]->size().height());

    settings.setValue(VPXY_COORD, viewports[VIEWPORT_XY]->pos());
    settings.setValue(VPXZ_COORD, viewports[VIEWPORT_XZ]->pos());
    settings.setValue(VPYZ_COORD, viewports[VIEWPORT_YZ]->pos());
    settings.setValue(VPSKEL_COORD, viewports[VIEWPORT_SKELETON]->pos());

    for(int i = 0; i < FILE_DIALOG_HISTORY_MAX_ENTRIES; i++) {
        if(i < skeletonFileHistory->size()) {
            settings.setValue(QString("loaded_file%1").arg(i+1), this->skeletonFileHistory->at(i));
        } else {
            settings.setValue(QString("loaded_file%1").arg(i+1), "");
        }
    }

    settings.setValue(OPEN_FILE_DIALOG_DIRECTORY, *this->openFileDirectory);
    settings.setValue(SAVE_FILE_DIALOG_DIRECTORY, *this->saveFileDirectory);


    settings.endGroup();

    widgetContainer->datasetPropertyWidget->saveSettings();
    widgetContainer->commentsWidget->saveSettings();
    widgetContainer->console->saveSettings();
    widgetContainer->dataSavingWidget->saveSettings();
    widgetContainer->zoomAndMultiresWidget->saveSettings();
    widgetContainer->viewportSettingsWidget->saveSettings();
    widgetContainer->navigationWidget->saveSettings();
    widgetContainer->annotationWidget->saveSettings();
    //widgetContainer->toolsWidget->saveSettings();    
}

/**
 * this method is a proposal for the qsettings variant
 */
void MainWindow::loadSettings() {
    QSettings settings;
    settings.beginGroup(MAIN_WINDOW);
    int width = (settings.value(WIDTH).isNull())? 1024 : settings.value(WIDTH).toInt();
    int height = (settings.value(HEIGHT).isNull())? 800 : settings.value(HEIGHT).toInt();
    int x, y;
    if(settings.value(POS_X).isNull() or settings.value(POS_Y).isNull()) {
        x = QApplication::desktop()->screen()->rect().topLeft().x() + 20;
        y = QApplication::desktop()->screen()->rect().topLeft().y() + 50;
    }
    else {
        x = settings.value(POS_X).toInt();
        y = settings.value(POS_Y).toInt();
    }

    state->viewerState->defaultVPSizeAndPos = settings.value(VP_DEFAULT_POS_SIZE, true).toBool();
    if(state->viewerState->defaultVPSizeAndPos == false) {
        viewports[VIEWPORT_XY]->resize(settings.value(VPXY_SIZE).toInt(), settings.value(VPXY_SIZE).toInt());
        viewports[VIEWPORT_XZ]->resize(settings.value(VPXZ_SIZE).toInt(), settings.value(VPXZ_SIZE).toInt());
        viewports[VIEWPORT_YZ]->resize(settings.value(VPYZ_SIZE).toInt(), settings.value(VPYZ_SIZE).toInt());
        viewports[VIEWPORT_SKELETON]->resize(settings.value(VPSKEL_SIZE).toInt(), settings.value(VPSKEL_SIZE).toInt());

        viewports[VIEWPORT_XY]->move(settings.value(VPXY_COORD).toPoint());
        viewports[VIEWPORT_XZ]->move(settings.value(VPXZ_COORD).toPoint());
        viewports[VIEWPORT_YZ]->move(settings.value(VPYZ_COORD).toPoint());
        viewports[VIEWPORT_SKELETON]->move(settings.value(VPSKEL_COORD).toPoint());
    }

    if(!settings.value(OPEN_FILE_DIALOG_DIRECTORY).isNull() and !settings.value(OPEN_FILE_DIALOG_DIRECTORY).toString().isEmpty()) {
        openFileDirectory = new QString(settings.value(OPEN_FILE_DIALOG_DIRECTORY).toString());
    } else {
        openFileDirectory = new QString(QDir::homePath());
    }

    if(!settings.value(SAVE_FILE_DIALOG_DIRECTORY).isNull() and !settings.value(SAVE_FILE_DIALOG_DIRECTORY).toString().isEmpty()) {
        saveFileDirectory = new QString(settings.value(SAVE_FILE_DIALOG_DIRECTORY).toString());
    } else {
        saveFileDirectory = new QString(QDir::homePath());
    }


    if(!settings.value(LOADED_FILE1).toString().isNull() and !settings.value(LOADED_FILE1).toString().isEmpty()) {
        this->skeletonFileHistory->enqueue(settings.value(LOADED_FILE1).toString());

    }
    if(!settings.value(LOADED_FILE2).toString().isNull() and !settings.value(LOADED_FILE2).toString().isEmpty()) {
        this->skeletonFileHistory->enqueue(settings.value(LOADED_FILE2).toString());

    }
    if(!settings.value(LOADED_FILE3).isNull() and !settings.value(LOADED_FILE3).toString().isEmpty()) {
        this->skeletonFileHistory->enqueue(settings.value(LOADED_FILE3).toString());

    }
    if(!settings.value(LOADED_FILE4).isNull() and !settings.value(LOADED_FILE4).toString().isEmpty()) {
        this->skeletonFileHistory->enqueue(settings.value(LOADED_FILE4).toString());

    }
    if(!settings.value(LOADED_FILE5).isNull() and !settings.value(LOADED_FILE5).toString().isEmpty()) {
        this->skeletonFileHistory->enqueue(settings.value(LOADED_FILE5).toString());

    }
    if(!settings.value(LOADED_FILE6).isNull() and !settings.value(LOADED_FILE6).toString().isEmpty()) {
        this->skeletonFileHistory->enqueue(settings.value(LOADED_FILE6).toString());

    }
    if(!settings.value(LOADED_FILE7).isNull() and !settings.value(LOADED_FILE7).toString().isEmpty()) {
        this->skeletonFileHistory->enqueue(settings.value(LOADED_FILE7).toString());

    }
    if(!settings.value(LOADED_FILE8).isNull() and !settings.value(LOADED_FILE8).toString().isEmpty()) {
        this->skeletonFileHistory->enqueue(settings.value(LOADED_FILE8).toString());      
    }
    if(!settings.value(LOADED_FILE9).isNull() and !settings.value(LOADED_FILE9).toString().isEmpty()) {
        this->skeletonFileHistory->enqueue(settings.value(LOADED_FILE9).toString());
    }
    if(!settings.value(LOADED_FILE10).isNull() and !settings.value(LOADED_FILE10).toString().isEmpty()) {
        this->skeletonFileHistory->enqueue(settings.value(LOADED_FILE10).toString());      
    }
    this->updateFileHistoryMenu();

    settings.endGroup();
    this->setGeometry(x, y, width, height);

    widgetContainer->datasetPropertyWidget->loadSettings();
    widgetContainer->commentsWidget->loadSettings();
    widgetContainer->console->loadSettings();
    widgetContainer->dataSavingWidget->loadSettings();
    widgetContainer->zoomAndMultiresWidget->loadSettings();
    widgetContainer->viewportSettingsWidget->loadSettings();
    widgetContainer->navigationWidget->loadSettings();
    widgetContainer->annotationWidget->loadSettings();

}

void MainWindow::clearSettings() {
    QSettings settings;

    skeletonFileHistory->clear();

    for(int i = 0; i < FILE_DIALOG_HISTORY_MAX_ENTRIES; i++) {
        historyEntryActions[i]->setText("");
        historyEntryActions[i]->setVisible(false);
    }

    QStringList keys = settings.allKeys();
    for(int i = 0; i < keys.size(); i++) {
        settings.remove(keys.at(i));
    }
}

void MainWindow::uncheckToolsAction() {
    this->toolsAction->setChecked(false);
}

void MainWindow::uncheckViewportSettingAction() {
    this->viewportSettingsAction->setChecked(false);
}

void MainWindow::uncheckCommentShortcutsAction() {
    this->commentShortcutsAction->setChecked(false);
}

void MainWindow::uncheckConsoleAction() {
    this->logAction->setChecked(false);
}

void MainWindow::uncheckDataSavingAction() {
    this->dataSavingOptionsAction->setChecked(false);
}

void MainWindow::uncheckSynchronizationAction() {
    this->synchronizationAction->setChecked(false);
}

void MainWindow::uncheckNavigationAction() {
    this->datasetNavigationAction->setChecked(false);
}

void MainWindow::updateCoordinateBar(int x, int y, int z) {
    xField->setValue(x + 1);
    yField->setValue(y + 1);
    zField->setValue(z + 1);
}

/** This is a replacement for the old updateSkeletonFileName
    It decides if a skeleton file has a revision(case 1) or not(case2).
    if case1 the revision substring is extracted, incremented and will be replaced.
    if case2 an initial revision will be inserted.
    This method is actually only needed for the save or save as slots, if incrementFileName is selected
*/
void MainWindow::updateSkeletonFileName(QString &fileName) {
    qDebug() <<"string to parse: " << fileName;
    QRegExp withVersion("[a-zA-Z0-9/_-\]+\\.[0-9]{3}\\.nml$");
    QRegExp withoutVersion("[a-zA-Z0-9/_-\]+.nml$");

    if(fileName.contains(withVersion)) {
        QString versionString = fileName.section("", fileName.length() - 6, fileName.length() - 4);
        int version = versionString.toInt();
        version += 1;
        state->skeletonState->skeletonRevision +=1;
        versionString = QString("%1").arg(version);
        while(versionString.length() < 3) {
            versionString.push_front("0");
        }
        fileName = fileName.replace(fileName.length() - 7, 3, versionString);
        qDebug() << fileName;

    } else if(fileName.contains(withoutVersion)) {
        //fileName = fileName.insert(fileName.length() - 3, "001.");
        state->skeletonState->skeletonRevision +=1;
        qDebug() << fileName;
    } else {
        qDebug() << "gnaaa";
    }
}

void MainWindow::resizeEvent(QResizeEvent *event) {
    if(state->viewerState->defaultVPSizeAndPos) {
        // don't resize viewports when user positioned and resized them manually
        int width = event->size().width();
        int height = event->size().height();
        resizeViewports(width, height);
    }
}


void MainWindow::dropEvent(QDropEvent *event) {
    if(event->mimeData()->hasFormat("text/uri-list")) {
        QList<QUrl> urls = event->mimeData()->urls();
        if(urls.size() != 1) {
            qDebug() << "error";
            return;
        }

        QUrl url = urls.first();
        QString fileName(url.toLocalFile());
        qDebug() << fileName;

        if(!fileName.endsWith(".nml")) {
            return;
        } else {
           openSlot(fileName);
           event->accept();
        }

    }
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event) {
    event->accept();
}

void MainWindow::dragLeaveEvent(QDragLeaveEvent *event) {
    qDebug() << "drag leave";
}

void MainWindow::openDatasetSlot() {
   this->widgetContainer->datasetPropertyWidget->show();
}

void MainWindow::taskSlot() {
    CURLcode code;
    long httpCode = 0;
    struct httpResponse response;
    char url[1024];

    // build url to send to
    memset(url, '\0', 1024);
    sprintf(url, "%s%s", state->taskState->host, "/knossos/session/");
    // prepare http response object
    response.length = 0;
    response.content = (char*)calloc(1, response.length+1);
    setCursor(Qt::WaitCursor);
    bool result = taskState::httpGET(url, &response, &httpCode, state->taskState->cookieFile, &code, 2);
    setCursor(Qt::ArrowCursor);
    if(result == false) {
        widgetContainer->taskLoginWidget->setResponse("Please login.");
        widgetContainer->taskLoginWidget->show();
        free(response.content);
        return;
    }
    if(code != CURLE_OK) {
        widgetContainer->taskLoginWidget->setResponse("Please login.");
        widgetContainer->taskLoginWidget->show();
        free(response.content);
        return;
    }
    if(httpCode != 200) {
        widgetContainer->taskLoginWidget->setResponse(QString("<font color='red'>%1</font>").arg(response.content));
        widgetContainer->taskLoginWidget->show();
        free(response.content);
        return;
    }
    // find out, which user is logged in
    QXmlStreamReader xml(response.content);
    if(xml.hasError()) { // response is broke.
        widgetContainer->taskLoginWidget->setResponse("Please login.");
        widgetContainer->taskLoginWidget->show();
        return;
    }
    xml.readNextStartElement();
    if(xml.isStartElement() == false) { // response is broke.
        widgetContainer->taskLoginWidget->setResponse("Please login.");
        widgetContainer->taskLoginWidget->show();
        free(response.content);
        return;
    }
    bool activeUser = false;
    if(xml.name() == "session") {
        QXmlStreamAttributes attributes = xml.attributes();
        QString attribute = attributes.value("username").toString();
        if(attribute.isNull() == false) {
            activeUser = true;
            widgetContainer->taskManagementWidget->mainTab->setActiveUser(attribute);
            widgetContainer->taskManagementWidget->mainTab->setResponse("Hello " + attribute + "!");
        }
        attribute = attributes.value("task").toString();
        if(attribute.isNull() == false) {
            widgetContainer->taskManagementWidget->mainTab->setTask(attribute);
        }
        attribute = attributes.value("taskFile").toString();
        if(attribute.isNull() == false) {
            memset(state->taskState->taskFile, '\0', sizeof(state->taskState->taskFile));
            strcpy(state->taskState->taskFile, attribute.toStdString().c_str());
        }
        attribute = attributes.value("description").toString();
        if(attribute.isNull() == false) {
            emit updateTaskDescriptionSignal(attribute);
        }
        attribute = attributes.value("comment").toString();
        if(attribute.isNull() == false) {
            emit updateTaskCommentSignal(attribute);
        }
    }
    if(activeUser) {
        widgetContainer->taskManagementWidget->show();
        free(response.content);
        return;
    }
    widgetContainer->taskLoginWidget->setResponse("Please login.");
    widgetContainer->taskLoginWidget->show();
    this->widgetContainer->taskLoginWidget->adjustSize();
    if(widgetContainer->taskLoginWidget->pos().x() <= 0 or this->widgetContainer->taskLoginWidget->pos().y() <= 0)
        this->widgetContainer->taskLoginWidget->move(QWidget::mapToGlobal(mainWidget->pos()));

    free(response.content);
    return;
}


void MainWindow::resetViewports() {
    resizeViewports(width(), height());
    state->viewerState->defaultVPSizeAndPos = true;
}

void MainWindow::resetVPOrientation() {
    if(state->viewerState->vpOrientationLocked == false) {
        viewports[VP_UPPERLEFT]->setOrientation(VIEWPORT_XY);
        viewports[VP_LOWERLEFT]->setOrientation(VIEWPORT_XZ);
        viewports[VP_UPPERRIGHT]->setOrientation(VIEWPORT_YZ);
        state->alpha = state->beta = state->viewerState->alphaCache = state->viewerState->betaCache = 0;
    }
    else {
        QMessageBox prompt;
        prompt.setWindowFlags(Qt::WindowStaysOnTopHint);
        prompt.setIcon(QMessageBox::Information);
        prompt.setWindowTitle("Information");
        prompt.setText("Viewport orientation is still locked. Uncheck 'Lock VP Orientation' first.");
        prompt.exec();
    }
}

void MainWindow::lockVPOrientation(bool lock) {
    state->viewerState->vpOrientationLocked = lock;
}

void MainWindow::showVPDecorationClicked() {
    if(widgetContainer->viewportSettingsWidget->generalTabWidget->showVPDecorationCheckBox->isChecked()) {
        for(int i = 0; i < NUM_VP; i++) {
            viewports[i]->showButtons();
        }
    }
    else {
        for(int i = 0; i < NUM_VP; i++) {
            viewports[i]->hideButtons();
        }
    }
}

void MainWindow::newTreeSlot() {
    color4F treeCol;
    treeCol.r = -1.;
    treeListElement *tree = addTreeListElementSignal(true, CHANGE_MANUAL, 0, treeCol, true);
    emit updateToolsSignal();
    //emit updateTreeviewSignal();
    treeAddedSignal(tree);
    state->skeletonState->workMode = SKELETONIZER_ON_CLICK_ADD_NODE;
}

void MainWindow::nextCommentNodeSlot() {
    emit nextCommentSignal(state->viewerState->gui->commentSearchBuffer);
}

void MainWindow::previousCommentNodeSlot() {
    emit previousCommentSignal(state->viewerState->gui->commentSearchBuffer);
}

void MainWindow::pushBranchNodeSlot() {
    if(pushBranchNodeSignal(CHANGE_MANUAL, true, true, state->skeletonState->activeNode, 0, true)) {
        emit branchPushedSignal();
    }
}

void MainWindow::popBranchNodeSlot() {
    if(popBranchNodeSignal()) {
        emit branchPoppedSignal();
    }
}

void MainWindow::moveToNextNodeSlot() {
    emit moveToNextNodeSignal();
    emit updateTools();
    emit updateTreeviewSignal();
}

void MainWindow::moveToPrevNodeSlot() {
    emit moveToPrevNodeSignal();
    emit updateTools();
    emit updateTreeviewSignal();
}

void MainWindow::moveToPrevTreeSlot() {
    emit moveToPrevTreeSignal();
    emit updateTools();
    emit updateTreeviewSignal();
}

void MainWindow::moveToNextTreeSlot() {
    emit moveToNextTreeSignal();
    emit updateTools();
    emit updateTreeviewSignal();
}

void MainWindow::jumpToActiveNodeSlot() {
    emit jumpToActiveNodeSignal();
}

void MainWindow::F1Slot() {   
    if(!state->skeletonState->activeNode)
        return;

    QString comment(state->viewerState->gui->comment1);

    if((!state->skeletonState->activeNode->comment) && (!comment.isEmpty())) {
        emit addCommentSignal(CHANGE_MANUAL, state->viewerState->gui->comment1, state->skeletonState->activeNode, 0, true);
    } else{
        if (!comment.isEmpty()) {
            emit editCommentSignal(CHANGE_MANUAL, state->skeletonState->activeNode->comment, 0, state->viewerState->gui->comment1, state->skeletonState->activeNode, 0, true);
        }
    }
    emit nodeCommentChangedSignal(state->skeletonState->activeNode);
}

void MainWindow::F2Slot() {
    if(!state->skeletonState->activeNode)
        return;

    if((!state->skeletonState->activeNode->comment) && (strncmp(state->viewerState->gui->comment2, "", 1) != 0)){
        emit addCommentSignal(CHANGE_MANUAL, state->viewerState->gui->comment2, state->skeletonState->activeNode, 0, true);
    }
    else{
        if(strncmp(state->viewerState->gui->comment2, "", 1) != 0)
            emit editCommentSignal(CHANGE_MANUAL, state->skeletonState->activeNode->comment, 0, state->viewerState->gui->comment2, state->skeletonState->activeNode, 0, true);
    }
    emit nodeCommentChangedSignal(state->skeletonState->activeNode);
}

void MainWindow::F3Slot() {
    if(!state->skeletonState->activeNode)
        return;

    if((!state->skeletonState->activeNode->comment) && (strncmp(state->viewerState->gui->comment3, "", 1) != 0)){
        emit addCommentSignal(CHANGE_MANUAL, state->viewerState->gui->comment3, state->skeletonState->activeNode, 0, true);
    }
    else{
       if(strncmp(state->viewerState->gui->comment3, "", 1) != 0)
            emit editCommentSignal(CHANGE_MANUAL, state->skeletonState->activeNode->comment, 0, state->viewerState->gui->comment3, state->skeletonState->activeNode, 0, true);
    }
    emit nodeCommentChangedSignal(state->skeletonState->activeNode);
}

void MainWindow::F4Slot() {
    if(!state->skeletonState->activeNode)
        return;

    if((!state->skeletonState->activeNode->comment) && (strncmp(state->viewerState->gui->comment4, "", 1) != 0)){
        emit addCommentSignal(CHANGE_MANUAL, state->viewerState->gui->comment4, state->skeletonState->activeNode, 0, true);
    }
    else{
       if (strncmp(state->viewerState->gui->comment4, "", 1) != 0)
        emit editCommentSignal(CHANGE_MANUAL, state->skeletonState->activeNode->comment, 0, state->viewerState->gui->comment4, state->skeletonState->activeNode, 0, true);
    }
    emit nodeCommentChangedSignal(state->skeletonState->activeNode);
}

void MainWindow::F5Slot() {
    if(!state->skeletonState->activeNode)
        return;

    if((!state->skeletonState->activeNode->comment) && (strncmp(state->viewerState->gui->comment5, "", 1) != 0)){
        emit addCommentSignal(CHANGE_MANUAL, state->viewerState->gui->comment5, state->skeletonState->activeNode, 0, true);
    }
    else {
        if (strncmp(state->viewerState->gui->comment5, "", 1) != 0)
        emit editCommentSignal(CHANGE_MANUAL, state->skeletonState->activeNode->comment, 0, state->viewerState->gui->comment5, state->skeletonState->activeNode, 0, true);
    }
    emit nodeCommentChangedSignal(state->skeletonState->activeNode);
}

void MainWindow::resizeViewports(int width, int height) {
    int sizeW = (width - 15)  / 2 ;
    int sizeH = (height - 70) / 2;

    if(width < height) {
        viewports[VIEWPORT_XY]->move(5, 60);
        viewports[VIEWPORT_XZ]->move(5, sizeW+60+5);
        viewports[VIEWPORT_YZ]->move(10 + sizeW, 60);
        viewports[VIEWPORT_SKELETON]->move(10 + sizeW, sizeW + 60 + 5);
        for(int i = 0; i < 4; i++) {
            viewports[i]->resize(sizeW, sizeW);

        }
    } else if(width > height) {
        viewports[VIEWPORT_XY]->move(5, 60);
        viewports[VIEWPORT_XZ]->move(5, sizeH+60+5);
        viewports[VIEWPORT_YZ]->move(10 + sizeH, 60);
        viewports[VIEWPORT_SKELETON]->move(10 + sizeH, sizeH + 60 + 5);
        for(int i = 0; i < 4; i++) {
            viewports[i]->resize(sizeH, sizeH);
            viewports[i]->updateButtonPositions();
        }
    }
}
