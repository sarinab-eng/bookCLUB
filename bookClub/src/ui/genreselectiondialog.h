#ifndef GENRESELECTIONDIALOG_H
#define GENRESELECTIONDIALOG_H

#include <QDialog>
#include <QStringList>

namespace Ui {
class GenreSelectionDialog;
}

class GenreSelectionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GenreSelectionDialog(QWidget *parent = nullptr);
    ~GenreSelectionDialog();

    QStringList selectedGenres() const;

private slots:
    void on_saveButton_clicked();

private:
    Ui::GenreSelectionDialog *ui;
    QStringList m_selectedGenres;
};

#endif