#ifndef REGISTERPAGE_H
#define REGISTERPAGE_H

#include <QWidget>
#include <QVector>

namespace Ui { class RegisterPage; }

class RegisterPage : public QWidget {
    Q_OBJECT
public:
    explicit RegisterPage(QWidget *parent = nullptr);
    ~RegisterPage();

signals:
    void goToLoginRequested();
    // تعریف استاندارد ۶ پارامتری برای هماهنگی با MainWindow
    void registerRequested(const QString &username, const QString &password,
                           const QString &question, const QString &answer,
                           const QString &userType, const QVector<int> &genres);

private slots:
    void on_registerButton_clicked();
    void on_goToLoginButton_clicked();

private:
    Ui::RegisterPage *ui;
};
#endif