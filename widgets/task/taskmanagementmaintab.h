#ifndef TASKMANAGEMENTMAINTAB_H
#define TASKMANAGEMENTMAINTAB_H

#include <QWidget>

#include "taskloginwidget.h"

class QPushButton;
class QLabel;
class QCheckBox;
class QDialog;
class QLineEdit;
class TaskManagementMainTab : public QWidget
{
    Q_OBJECT
public:
    explicit TaskManagementMainTab(TaskLoginWidget *taskLoginWidget, QWidget *parent = 0);
    void setResponse(QString message);
    void setActiveUser(QString username);
    void setTask(QString task);

protected:
    QLabel *statusLabel;
    QLabel *loggedAsLabel;
    QLabel *currentTaskLabel;
    QPushButton *logoutButton;
    QPushButton *loadLastSubmitButton;
    QPushButton *startNewTaskButton;
    QPushButton *submitButton;
    TaskLoginWidget *taskLoginWidget;

    QDialog *submitDialog;
    QLineEdit *submitDialogCommentField;
    QCheckBox *submitDialogFinalCheckbox;
    QPushButton *submitDialogCancelButton;
    QPushButton *submitDialogOkButton;

    void resetSession(QString message);

signals:
    void hideSignal();
    void saveSkeletonSignal();
    bool loadSkeletonSignal(const QString &fileName);
    void setDescriptionSignal(QString description);
    void setCommentSignal(QString comment);

public slots:
    void submitButtonClicked();
    void submitDialogCanceled();
    void submitDialogOk();

    void startNewTaskButtonClicked();
    void loadLastSubmitButtonClicked();
    void logoutButtonClicked();
    
};

#endif // TASKMANAGEMENTMAINTAB_H
