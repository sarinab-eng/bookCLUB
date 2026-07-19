#include "genreselectiondialog.h"
#include "ui_genreselectiondialog.h"

#include <QListWidgetItem>
#include <QMessageBox>

GenreSelectionDialog::GenreSelectionDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::GenreSelectionDialog)
{
    ui->setupUi(this);
}

GenreSelectionDialog::~GenreSelectionDialog()
{
    delete ui;
}

QStringList GenreSelectionDialog::selectedGenres() const
{
    return m_selectedGenres;
}

void GenreSelectionDialog::on_saveButton_clicked()
{
    m_selectedGenres.clear();

    QList<QListWidgetItem*> items = ui->genreListWidget->selectedItems();

    if(items.size()<1 || items.size()>3)
    {
        QMessageBox::warning(this,"Warning","Select between 1 and 3 genres.");
        return;
    }

    for(auto item:items)
        m_selectedGenres.append(item->text());

    accept();
}