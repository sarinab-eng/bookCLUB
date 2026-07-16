#ifndef PUBLISHERDASHBOARD_H
#define PUBLISHERDASHBOARD_H

#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include "book.h"
#include "ClientNetworkManager.h"

class PublisherDashboard : public QWidget {
    Q_OBJECT
public:
    explicit PublisherDashboard(QWidget *parent = nullptr);

private slots:
    void onAddBookClicked();
    void onDeleteBookClicked();
    void updateBookTable(const QVector<Book> &books);

private:
    void setupUI();
    QTableWidget *bookTable;
    QPushButton *addBtn;
    QPushButton *refreshBtn;
    QPushButton *deleteBtn;
};

#endif // PUBLISHERDASHBOARD_H
