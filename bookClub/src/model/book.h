#ifndef BOOK_H
#define BOOK_H

#include <QObject>
#include <QString>
#include <QVector>
#include <QJsonObject>
#include <QJsonArray>

class Book : public QObject {
    Q_OBJECT

    // تعریف Propertyها برای استفاده راحت‌تر در QML یا بخش‌های دیگر Qt (اختیاری اما استاندارد)
    Q_PROPERTY(int id READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
    Q_PROPERTY(QString author READ author WRITE setAuthor NOTIFY authorChanged)
    Q_PROPERTY(QString publisher READ publisher WRITE setPublisher NOTIFY publisherChanged)
    Q_PROPERTY(double price READ price WRITE setPrice NOTIFY priceChanged)
    Q_PROPERTY(double discountPercent READ discountPercent WRITE setDiscountPercent NOTIFY discountPercentChanged)

public:
    explicit Book(QObject *parent = nullptr);
    Book(int id, const QString &title, const QString &author, const QString &publisher,
         double price, double discountPercent, const QString &pdfPath, const QString &coverPath,
         const QVector<int> &genres, QObject *parent = nullptr);

    // سازنده کپی (Copy Constructor) برای کپی راحت تر اشیاء
    Book(const Book &other);
    Book& operator=(const Book &other);

    // Getters
    int id() const { return m_id; }
    QString title() const { return m_title; }
    QString author() const { return m_author; }
    QString publisher() const { return m_publisher; }
    double price() const { return m_price; }
    double discountPercent() const { return m_discountPercent; }
    double finalPrice() const; // محاسبه قیمت پس از اعمال تخفیف
    QString pdfPath() const { return m_pdfPath; }
    QString coverPath() const { return m_coverPath; }
    QVector<int> genres() const { return m_genres; }
    double averageRating() const { return m_averageRating; }

    // Setters
    void setId(int id);
    void setTitle(const QString &title);
    void setAuthor(const QString &author);
    void setPublisher(const QString &publisher);
    void setPrice(double price);
    void setDiscountPercent(double discountPercent);
    void setPdfPath(const QString &pdfPath);
    void setCoverPath(const QString &coverPath);
    void setGenres(const QVector<int> &genres);
    void setAverageRating(double rating);

    // متدهای تبدیل داده برای لایه شبکه (JSON Serialization)
    QJsonObject toJson() const;
    static Book* fromJson(const QJsonObject &json, QObject *parent = nullptr);

signals:
    void idChanged();
    void titleChanged();
    void authorChanged();
    void publisherChanged();
    void priceChanged();
    void discountPercentChanged();
    void pdfPathChanged();
    void coverPathChanged();
    void genresChanged();
    void averageRatingChanged();

private:
    int m_id;
    QString m_title;
    QString m_author;
    QString m_publisher;
    double m_price;
    double m_discountPercent;
    QString m_pdfPath;     // مسیر فایل یا شناسه فایل روی سرور
    QString m_coverPath;   // مسیر تصویر جلد کتاب
    QVector<int> m_genres; // شناسه ژانرهای مرتبط با کتاب
    double m_averageRating; // میانگین امتیاز کتاب
};

#endif // BOOK_H
