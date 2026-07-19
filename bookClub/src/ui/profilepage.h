#ifndef PROFILEPAGE_H
#define PROFILEPAGE_H

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QCheckBox>
#include <QJsonObject>
#include <QVector>
#include "Authmanager.h"

class ProfilePage : public QWidget
{
    Q_OBJECT

public:
    explicit ProfilePage(AuthManager *authManager, QWidget *parent = nullptr);

    void setUsername(const QString &username);
    void requestProfileRefresh();

private slots:
    void onProfileReceived(const QJsonObject &profile);
    void onGenresSaved(bool success);
    void onPasswordChanged(bool success, const QString &message);
    void onSaveGenresClicked();
    void onChangePasswordClicked();

private:
    void setupUi();

    AuthManager *m_authManager;
    QString m_username;

    QLabel *m_usernameValueLabel;
    QLabel *m_roleValueLabel;
    QLabel *m_registrationDateValueLabel;

    QVector<QCheckBox*> m_genreCheckboxes;
    QPushButton *m_saveGenresButton;

    QLineEdit *m_oldPasswordEdit;
    QLineEdit *m_newPasswordEdit;
    QLineEdit *m_confirmPasswordEdit;
    QPushButton *m_changePasswordButton;
};

#endif // PROFILEPAGE_H
