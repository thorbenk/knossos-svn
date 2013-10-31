#include "taskloginwidget.h"
#include <QLabel>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QPushButton>
#include <QFormLayout>
#include <QXmlStreamReader>
#include <QXmlStreamAttributes>
#include <QString>
#include "taskManagementWidget.h"
#include "knossos-global.h"
#include <curl/curl.h>

extern stateInfo *state;

TaskLoginWidget::TaskLoginWidget(QWidget *parent) :
    QWidget(parent), taskManagementWidget(NULL)
{
    setWindowTitle("Task Login");

    QVBoxLayout *mainLayout = new QVBoxLayout;
    QFormLayout *formLayout = new QFormLayout();

    urlField = new QLineEdit();
    usernameField = new QLineEdit();
    passwordField = new QLineEdit();
    serverStatus = new QLabel("Please Login");
    passwordField->setEchoMode(QLineEdit::Password);
    loginButton = new QPushButton("Login");

    QLabel *hostLabel = new QLabel("Host:");
    QLabel *usernameLabel = new QLabel("Username:");
    QLabel *passwordLabel = new QLabel("Password");

    QFrame *line = new QFrame();
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);

    formLayout->addWidget(serverStatus);
    formLayout->addRow(hostLabel, urlField);
    formLayout->addWidget(line);
    formLayout->addRow(usernameLabel, usernameField);
    formLayout->addRow(passwordLabel, passwordField);
    mainLayout->addLayout(formLayout);
    mainLayout->addWidget(loginButton);

    setLayout(mainLayout);

    connect(urlField, SIGNAL(editingFinished()), this, SLOT(urlEditingFinished()));
    connect(passwordField, SIGNAL(returnPressed()), this, SLOT(loginButtonClicked()));
    connect(usernameField, SIGNAL(returnPressed()), this, SLOT(loginButtonClicked()));
    connect(urlField, SIGNAL(returnPressed()), this, SLOT(loginButtonClicked()));
    connect(loginButton, SIGNAL(clicked()), this, SLOT(loginButtonClicked()));
}

void TaskLoginWidget::urlEditingFinished() {
    memset(state->taskState->host, '\0', sizeof(state->taskState->host));
    strcpy(state->taskState->host, urlField->text().toStdString().c_str());
    // cut off trailing slash
    if(state->taskState->host[strlen(state->taskState->host)-1] == '/') {
        state->taskState->host[strlen(state->taskState->host)-1] = '\0';
    }
}

void TaskLoginWidget::loginButtonClicked() {
    char url[1024];
    CURLcode code;
    long httpCode;
    struct httpResponse response;
    FILE *cookie;

    char username[512];
    char password[512];
    char postdata[1024];

    memset(username, '0', 512);
    memset(password, '\0', 512);
    strcpy(username, usernameField->text().toStdString().c_str());
    strcpy(password, passwordField->text().toStdString().c_str());
    sprintf(postdata, "<login><username>%s</username><password>%s</password></login>", username, password);

    // build url to send to
    memset(url, '\0', 1024);
    strcpy(url, state->taskState->host);
    strcat(url, "/knossos/session/");
    // prepare http response object
    response.length = 0;
    response.content = (char *)calloc(1, response.length+1);

    // remove contents of cookie file to fill it with new cookie
    cookie = fopen(state->taskState->cookieFile, "w");
    if(cookie) {
        fclose(cookie);
    }

    code = taskState::httpPOST(url, postdata, &response, &httpCode, state->taskState->cookieFile);
    if(code == CURLE_OK) {
        if(httpCode == 200) {

            QXmlStreamReader xml(response.content);
            if(xml.hasError()) {
                serverStatus->setText("<font color='red'>Error in transmission. Please try again.</font>");
                return;
            }
            xml.readNextStartElement();
            if(xml.isStartElement() == false) {
                serverStatus->setText("<font color='red'>Error in transmission. Please try again.</font>");
                return;
            }
            // transmission successful
            this->hide();
            if(xml.name() == "task") {
                QString attribute;
                QXmlStreamAttributes attributes = xml.attributes();
                attribute = attributes.value("taskname").toString();
                if(attribute.isNull() == false) {
                    taskManagementWidget->setTask(attribute);
                }
                attribute = attributes.value("taskfile").toString();
                if(attribute.isNull() == false) {
                    memset(state->taskState->taskFile, '\0', sizeof(state->taskState->taskFile));
                    strcpy(state->taskState->taskFile, attribute.toStdString().c_str());
                }
            }
            this->hide();
            taskManagementWidget->setResponse(QString("Hello %1!").arg(username));
            taskManagementWidget->setActiveUser(username);
            taskManagementWidget->show();
            return;
        }
        else if (httpCode == 400) {
            serverStatus->setText(QString("<font color='red'>%1</font>").arg(response.content));
        }
    }
    else { // !CURLE_OK
        serverStatus->setText("<font color='red'>Could not connect to host. Wrong URL?</font>");
    }
    free(response.content);
}

void TaskLoginWidget::setResponse(QString message) {
    serverStatus->setText(message);
}

void TaskLoginWidget::setTaskManagementWidget(TaskManagementWidget *w) {
    delete taskManagementWidget;
    taskManagementWidget = w;
}

void TaskLoginWidget::closeEvent(QCloseEvent *event) {
    this->hide();
}
