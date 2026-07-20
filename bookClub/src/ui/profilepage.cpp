#include "profilepage.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGridLayout>
#include <QFrame>
#include <QMessageBox>
#include <QJsonArray>

static const QStringList kGenres = {
    "رمان", "علمی", "تاریخ", "کودک و نوجوان", "شعر",
    "زندگی‌نامه", "فلسفه", "روان‌شناسی", "ماجراجویی", "علمی-تخیلی", "عاشقانه"
};

ProfilePage::ProfilePage(AuthManager *authManager, QWidget *parent)
    : QWidget(parent), m_authManager(authManager)
{
    setupUi();

    connect(m_authManager, &AuthManager::profileReceived, this, &ProfilePage::onProfileReceived);
    connect(m_authManager, &AuthManager::genresSaved, this, &ProfilePage::onGenresSaved);
    connect(m_authManager, &AuthManager::passwordChanged, this, &ProfilePage::onPasswordChanged);
    connect(m_saveGenresButton, &QPushButton::clicked, this, &ProfilePage::onSaveGenresClicked);
    connect(m_changePasswordButton, &QPushButton::clicked, this, &ProfilePage::onChangePasswordClicked);
}

void ProfilePage::setupUi()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // ---- اطلاعات حساب کاربری ----
    QLabel *infoTitle = new QLabel("اطلاعات حساب کاربری");
    infoTitle->setStyleSheet("font-weight: bold; font-size: 14px;");
    mainLayout->addWidget(infoTitle);

    QFormLayout *infoForm = new QFormLayout;
    m_usernameValueLabel = new QLabel;
    m_roleValueLabel = new QLabel;
    m_registrationDateValueLabel = new QLabel;
    infoForm->addRow("نام کاربری:", m_usernameValueLabel);
    infoForm->addRow("نقش:", m_roleValueLabel);
    infoForm->addRow("تاریخ عضویت:", m_registrationDateValueLabel);
    mainLayout->addLayout(infoForm);

    QFrame *sep1 = new QFrame;
    sep1->setFrameShape(QFrame::HLine);
    mainLayout->addWidget(sep1);

    // ---- ژانرهای مورد علاقه ----
    QLabel *genresTitle = new QLabel("ژانرهای مورد علاقه (۱ تا ۳ مورد)");
    genresTitle->setStyleSheet("font-weight: bold; font-size: 14px;");
    mainLayout->addWidget(genresTitle);

    QGridLayout *genresGrid = new QGridLayout;
    int col = 0, row = 0;
    for (const QString &genre : kGenres) {
        QCheckBox *cb = new QCheckBox(genre);
        genresGrid->addWidget(cb, row, col);
        m_genreCheckboxes.append(cb);
        col++;
        if (col >= 3) { col = 0; row++; }
    }
    mainLayout->addLayout(genresGrid);

    m_saveGenresButton = new QPushButton("ذخیره ژانرها");
    mainLayout->addWidget(m_saveGenresButton);

    QFrame *sep2 = new QFrame;
    sep2->setFrameShape(QFrame::HLine);
    mainLayout->addWidget(sep2);

    // ---- تغییر رمز عبور ----
    QLabel *passwordTitle = new QLabel("تغییر رمز عبور");
    passwordTitle->setStyleSheet("font-weight: bold; font-size: 14px;");
    mainLayout->addWidget(passwordTitle);

    QFormLayout *passwordForm = new QFormLayout;
    m_oldPasswordEdit = new QLineEdit;
    m_oldPasswordEdit->setEchoMode(QLineEdit::Password);
    m_newPasswordEdit = new QLineEdit;
    m_newPasswordEdit->setEchoMode(QLineEdit::Password);
    m_confirmPasswordEdit = new QLineEdit;
    m_confirmPasswordEdit->setEchoMode(QLineEdit::Password);
    passwordForm->addRow("رمز عبور فعلی:", m_oldPasswordEdit);
    passwordForm->addRow("رمز عبور جدید:", m_newPasswordEdit);
    passwordForm->addRow("تکرار رمز عبور جدید:", m_confirmPasswordEdit);
    mainLayout->addLayout(passwordForm);

    m_changePasswordButton = new QPushButton("تغییر رمز عبور");
    mainLayout->addWidget(m_changePasswordButton);

    mainLayout->addStretch();
}

void ProfilePage::setUsername(const QString &username)
{
    m_username = username;
    m_usernameValueLabel->setText(username);
}

void ProfilePage::requestProfileRefresh()
{
    if (!m_username.isEmpty() && m_authManager)
        m_authManager->requestProfile(m_username);
}

void ProfilePage::onProfileReceived(const QJsonObject &profile)
{
    if (profile.value("username").toString() != m_username) return;

    m_roleValueLabel->setText(profile.value("role").toString());
    m_registrationDateValueLabel->setText(profile.value("registration_date").toString());

    QJsonArray genres = profile.value("favoriteGenres").toArray();
    QStringList selected;
    for (const QJsonValue &v : genres) selected.append(v.toString());

    for (QCheckBox *cb : m_genreCheckboxes) {
        cb->setChecked(selected.contains(cb->text()));
    }
}

void ProfilePage::onSaveGenresClicked()
{
    QStringList selected;
    for (QCheckBox *cb : m_genreCheckboxes) {
        if (cb->isChecked()) selected.append(cb->text());
    }

    if (selected.size() < 1 || selected.size() > 3) {
        QMessageBox::warning(this, "خطا", "لطفاً بین ۱ تا ۳ ژانر انتخاب کنید.");
        return;
    }

    m_savingGenres = true;
    if (m_authManager) m_authManager->saveGenres(m_username, selected);
}

void ProfilePage::onGenresSaved(bool success)
{
    // اولین ورود (انتخاب ژانر تو GenreSelectionDialog) هم از همین سیگنال استفاده میکند
    // این صفحه فقط وقتی باید واکنش نشون دهد که خودش درخواست ذخیره رو زده باشد
    if (!m_savingGenres) return;
    m_savingGenres = false;

    if (success) {
        QMessageBox::information(this, "موفق", "ژانرهای مورد علاقه ذخیره شد.");
        // پروفایل رو دوباره می‌گیریم تا سیگنال profileReceived دوباره فرستاده شود
        // CustomerPage هم به همین سیگنال گوش می دهد و لیست پیشنهادی رو آپدیت میکند
        requestProfileRefresh();
    } else {
        QMessageBox::warning(this, "خطا", "ذخیره ژانرها با خطا مواجه شد.");
    }
}

void ProfilePage::onChangePasswordClicked()
{
    QString oldPass = m_oldPasswordEdit->text();
    QString newPass = m_newPasswordEdit->text();
    QString confirmPass = m_confirmPasswordEdit->text();

    if (oldPass.isEmpty() || newPass.isEmpty() || confirmPass.isEmpty()) {
        QMessageBox::warning(this, "خطا", "لطفاً تمام فیلدها را پر کنید.");
        return;
    }
    if (newPass != confirmPass) {
        QMessageBox::warning(this, "خطا", "رمز عبور جدید و تکرار آن یکسان نیستند.");
        return;
    }

    if (m_authManager) m_authManager->changePassword(m_username, oldPass, newPass);
}

void ProfilePage::onPasswordChanged(bool success, const QString &message)
{
    if (success) {
        QMessageBox::information(this, "موفق", message);
        m_oldPasswordEdit->clear();
        m_newPasswordEdit->clear();
        m_confirmPasswordEdit->clear();
    } else {
        QMessageBox::warning(this, "خطا", message);
    }
}
