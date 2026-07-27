#include "cartpage.h"
#include "ui_cartpage.h"
#include <QTableWidgetItem>
#include <QMessageBox>

CartPage::CartPage(AuthManager *authManager, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::CartPage)
    , m_authManager(authManager)
{
    ui->setupUi(this);

    connect(m_authManager, &AuthManager::cartReceived, this, &CartPage::onCartReceived);
    connect(m_authManager, &AuthManager::cartSummaryReceived, this, &CartPage::onCartSummaryReceived);

    connect(m_authManager, QOverload<bool, const QString&, double, double, double>::of(&AuthManager::checkoutFinished),
            this, &CartPage::onCheckoutFinished);

    connect(m_authManager, &AuthManager::itemRemovedFromCart, this, [this](bool success, const QString &message){
        if (success) {
            requestCartRefresh();
        } else {
            QMessageBox::warning(this, "خطا", "حذف آیتم با خطا مواجه شد: " + message);
        }
    });
}


CartPage::~CartPage() { delete ui; }

void CartPage::requestCartRefresh()
{
    if (!m_username.isEmpty())
        m_authManager->requestCart(m_username);
}

void CartPage::setUsername(const QString &username)
{
    m_username = username;
}

void CartPage::onCartReceived(const QJsonArray &items)
{
    populateTable(items);
}

void CartPage::populateTable(const QJsonArray &items)
{
    ui->cartTable->clearContents();
    ui->cartTable->setRowCount(0);

    for (int i = 0; i < items.size(); ++i) {
        QJsonObject item = items[i].toObject();
        QString bookId = item["book_id"].toString();
        QString title = item["title"].toString();
        double price = item["price"].toDouble();
        int quantity = item["quantity"].toInt();
        double rowSubtotal = price * quantity;

        ui->cartTable->insertRow(i);

        QTableWidgetItem *titleItem = new QTableWidgetItem(title);
        titleItem->setData(Qt::UserRole, bookId);
        ui->cartTable->setItem(i, 0, titleItem);
        ui->cartTable->setItem(i, 1, new QTableWidgetItem(QString::number(price, 'f', 2)));
        ui->cartTable->setItem(i, 2, new QTableWidgetItem(QString::number(quantity)));
        ui->cartTable->setItem(i, 3, new QTableWidgetItem(QString::number(rowSubtotal, 'f', 2)));
    }
}

void CartPage::onCartSummaryReceived(int itemCount, double total, double discountAmount, double finalAmount)
{
    ui->itemCountLabel->setText(QString("تعداد اقلام سبد: %1").arg(itemCount));
    ui->totalLabel->setText(QString("مبلغ کل: %1 تومان").arg(total, 0, 'f', 0));
    ui->discountLabel->setText(discountAmount > 0
        ? QString("تخفیف: %1 تومان (مبلغ قابل پرداخت: %2 تومان)").arg(discountAmount, 0, 'f', 0).arg(finalAmount, 0, 'f', 0)
        : "تخفیف: 0 تومان");
}

void CartPage::onCheckoutFinished(bool success, const QString &message, double total, double discountAmount, double finalAmount)
{
    Q_UNUSED(total);

    if (success) {
        QMessageBox::information(this, "خرید موفق", "خرید شما با موفقیت ثبت شد!\n" + message);

        ui->discountLabel->setText(QString("تخفیف اعمال شده: %1 تومان").arg(discountAmount));
        ui->totalLabel->setText(QString("مبلغ پرداخت شده: %1 تومان").arg(finalAmount));
        ui->itemCountLabel->setText("تعداد اقلام سبد: 0");

        ui->cartTable->clearContents();
        ui->cartTable->setRowCount(0);

        emit checkoutSuccessful();

    } else {
        QMessageBox::critical(this, "خطا در خرید", "عملیات پرداخت ناموفق بود:\n" + message);
        requestCartRefresh();
    }
}

void CartPage::on_removeButton_clicked()
{
    int currentRow = ui->cartTable->currentRow();
    if (currentRow < 0) {
        QMessageBox::warning(this, "خطا", "لطفاً ابتدا کتاب مورد نظر را از جدول انتخاب کنید.");
        return;
    }

    QString bookId = ui->cartTable->item(currentRow, 0)->data(Qt::UserRole).toString();

    if (!m_username.isEmpty()) {
        m_authManager->removeFromCart(m_username, bookId);
    }
}


void CartPage::on_checkoutButton_clicked()
{
    if (!m_username.isEmpty())
        m_authManager->checkout(m_username);
}