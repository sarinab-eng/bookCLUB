#ifndef PUBLISHER_H
#define PUBLISHER_H

#include <QString>
#include <QJsonObject>

class Publisher {
public:
    Publisher() : m_id(-1) {}
    Publisher(int id, const QString &name, const QString &email)
        : m_id(id), m_name(name), m_email(email) {}

    int getId() const { return m_id; }
    QString getName() const { return m_name; }
    QString getEmail() const { return m_email; }

    static Publisher fromJson(const QJsonObject &json) {
        return Publisher(json["id"].toInt(), json["name"].toString(), json["email"].toString());
    }

private:
    int m_id;
    QString m_name;
    QString m_email;
};

#endif
