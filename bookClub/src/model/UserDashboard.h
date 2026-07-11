#ifndef USERDASHBOARD_H
#define USERDASHBOARD_H

#include <QComboBox>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QTabWidget>
#include <QWidget>
#include "Book.h"
#include "PersonalLibrary.h"

class UserDashboard : public QWidget
{
    Q_OBJECT
public:
    explicit UserDashboard(QWidget *parent = nullptr);
    void refreshBooks(const QVector<Book> &allBooks);

signals:
    void bookSelected(const Book &book);

private slots:
    void onSearchTextChanged(const QString &text);
    void onFilterChanged(int index);
    void onGenrePreferenceSelected();

private:
    void setupUi();
    void displayBooks(const QVector<Book> &booksToDisplay);

    QLineEdit *searchLineEdit;
    QComboBox *filterComboBox;
    QTabWidget *tabWidget;
    QGridLayout *booksLayout;
    QWidget *booksScrollWidget;

    QVector<Book> m_allBooks;
    QStringList m_preferredGenres;
    PersonalLibrary m_library;
};

#endif // USERDASHBOARD_H
