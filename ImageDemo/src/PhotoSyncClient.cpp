#include "PhotoSyncClient.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>

PhotoSyncClient::PhotoSyncClient(QObject* parent)
    : QObject(parent)
{
}

void PhotoSyncClient::setBaseUrl(const QString& url)
{
    m_baseUrl = url;
}

void PhotoSyncClient::fetchPhotos()
{
    const QUrl url(m_baseUrl + "/photos");
    QNetworkRequest request(url);

    QNetworkReply* reply = m_net.get(request);

    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        if (reply->error() != QNetworkReply::NoError) {
            emit requestFailed(reply->errorString());
            reply->deleteLater();
            return;
        }

        const QByteArray data = reply->readAll();
        reply->deleteLater();

        QJsonParseError parseError;
        const QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);

        if (parseError.error != QJsonParseError::NoError) {
            emit requestFailed(QString("JSON parse error: %1").arg(parseError.errorString()));
            return;
        }

        if (doc.isArray()) {
            emit photosReceived(doc.array());
            return;
        }

        if (doc.isObject()) {
            const QJsonObject obj = doc.object();

            if (obj.contains("items") && obj.value("items").isArray()) {
                emit photosReceived(obj.value("items").toArray());
                return;
            }

            if (obj.contains("photos") && obj.value("photos").isArray()) {
                emit photosReceived(obj.value("photos").toArray());
                return;
            }
        }

        emit requestFailed("Response JSON was not an array or recognized photo container.");
    });
}