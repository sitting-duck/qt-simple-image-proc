#pragma once

#include <QObject>
#include <QNetworkAccessManager>
#include <QJsonArray>
#include <QString>

class PhotoSyncClient : public QObject
{
    Q_OBJECT

public:
    explicit PhotoSyncClient(QObject* parent = nullptr);

    void setBaseUrl(const QString& url);
    void fetchPhotos();

signals:
    void photosReceived(QJsonArray photos);
    void requestFailed(QString error);

private:
    QNetworkAccessManager m_net;
    QString m_baseUrl;
};