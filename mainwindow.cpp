#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "credentialwidget.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>

#include <openssl/evp.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->lineEdit, &QLineEdit::textChanged, this, &MainWindow::onFilterTextChanged);

    // for(int i = 0; i < m_jsonarray.size(); i++){
    //     qDebug() << "*** m_jsonarray{[" << i << "] = " << m_jsonarray[i];

    //     QListWidgetItem * newItem = new QListWidgetItem();
    //     // if(m_jsonarray[i].isObject()){ // есть ли поле site
    //     //     if(m_jsonarray[i].toObject().contains("site")){
    //     //         if(m_jsonarray[i].toObject()["site"].isString()) {
    //     //             qDebug() << m_jsonarray[i];
    //     //         }
    //     //     }
    //     // }
    //     CredentialWidget * itemWidget =
    //         new CredentialWidget(m_jsonarray[i].toObject()["site"].toString(), i);
    //     newItem->setSizeHint(itemWidget->sizeHint());
    //     // newItem->setSizeHint(itemWidget->sizeHint());
    //     ui->listWidget->addItem(newItem);
    //     ui->listWidget->setItemWidget(newItem, itemWidget);
    // }
}

void MainWindow::onFilterTextChanged(const QString &text)
{
    // Очистка QListWidget перед применением фильтра
    ui->listWidget->clear();

    // фильтр к m_jsonarray и добавление отфильтрованных элементы в QListWidget
    for (int i = 0; i < m_jsonarray.size(); i++) {
        QString site = m_jsonarray[i].toObject()["site"].toString().toLower();
        qDebug() << "\n" << 1 << "\n";
        if (site.contains(text.toLower())) {
            qDebug() << "\n" << 2 << site << "\n";
            QListWidgetItem *newItem = new QListWidgetItem();
            qDebug() << "\n" << 3 << site << "\n";
            // поломка в следующей строчке
            CredentialWidget *itemWidget = new CredentialWidget(site, i);
            //
            qDebug() << "\n" << 4 << site << "\n";
            newItem->setSizeHint(itemWidget->sizeHint());
            ui->listWidget->addItem(newItem);
            ui->listWidget->setItemWidget(newItem, itemWidget);
            qDebug() << "\n" << 5 << "\n";
        }
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

// функция считывает учетные записи из файла JSON в структуру данных QList
bool MainWindow::readJSON(const QByteArray &aes256_key)
{
    QFile jsonFile("credentials.json");
    jsonFile.open(QFile::ReadOnly);
    if (!jsonFile.isOpen())
        return false;

    QByteArray hexEncryptedBytes = jsonFile.readAll();
    qDebug() << "*** hexEncryptedilytes" << hexEncryptedBytes;
    QByteArray encryptedBytes = QByteArray::fromHex(hexEncryptedBytes);
    qDebug() << "*** encryptedBytes" << encryptedBytes;
    QByteArray decryptedBytes;

    decryptFile(aes256_key, encryptedBytes, decryptedBytes);
    qDebug() << "*** decryptFile(), decryptedBytes = " << decryptedBytes;

    QJsonParseError jsonErr;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(decryptedBytes, &jsonErr);
    if(jsonErr.error != QJsonParseError::NoError)
        return false;
    QJsonObject rootObject = jsonDoc.object();

    m_jsonarray = rootObject["credentials"].toArray();
    jsonFile.close();
}

int MainWindow::decryptFile(
    const QByteArray & aes256_key,
    const QByteArray & encryptedBytes,
    QByteArray & decryptedBytes)
{
    // HEX(iv) = 'f3724fed6a617d5f80493368d4477983'
    // HEX(key) = '7b5e5de52aa9fad69f834a1fa4b65dcf96f609579ef701181c22ee47bbc5b294'
    // QByteArray key_qba = QByteArray::fromHex("7b5e5de52aa9fad69f834a1fa4b65dcf96f609579ef701181c22ee47bbc5b294");
    // нужно поставить aes_256 наверх
    qDebug() << "---" << aes256_key.toHex();
    QByteArray key_qba = QByteArray::fromHex(aes256_key.toHex());
    QByteArray iv_qba = QByteArray::fromHex("f3724fed6a617d5f80493368d4477983");
    unsigned char key[32], iv[16];
    const int bufferLen = 256;
    unsigned char encryptedBuffer[bufferLen] = {0}, decryptedBuffer[bufferLen] = {0};
    int encryptedLen, decryptedLen;
    QDataStream encryptedStream(encryptedBytes);
    QDataStream decryptedStream(&decryptedBytes, QIODevice::ReadWrite);

    memcpy(key, key_qba.data(), 32);
    memcpy(iv, iv_qba.data(), 16);

    EVP_CIPHER_CTX *ctx;
    ctx = EVP_CIPHER_CTX_new();
    if (!EVP_DecryptInit_ex2(ctx, EVP_aes_256_cbc(), key, iv, NULL)) {
        EVP_CIPHER_CTX_free(ctx);
        key_qba = 0;
        iv_qba = 0;
        return 1;
    }
    do {
        encryptedLen = encryptedStream.readRawData(reinterpret_cast<char*>(encryptedBuffer), bufferLen);
        if (!EVP_DecryptUpdate(ctx, decryptedBuffer, &decryptedLen,
                               encryptedBuffer, encryptedLen)) {
            EVP_CIPHER_CTX_free(ctx);
            key_qba = 0;
            iv_qba = 0;
            return 2;
        }
        decryptedStream.writeRawData(reinterpret_cast<char*>(decryptedBuffer), decryptedLen);
    } while (encryptedLen > 0);

    if (!EVP_DecryptFinal_ex(ctx, decryptedBuffer, &decryptedLen)) {
        EVP_CIPHER_CTX_free(ctx);
        key_qba = 0;
        iv_qba = 0;
        return 3;
    }
    decryptedStream.writeRawData(reinterpret_cast<char*>(decryptedBuffer), decryptedLen);
    EVP_CIPHER_CTX_free(ctx);
    key_qba = 0;
    iv_qba = 0;
    return 0;
}

void MainWindow::on_edtPin_returnPressed()
{
    QByteArray hash = QCryptographicHash::hash(
        ui->edtPin->text().toUtf8(),
        QCryptographicHash::Sha256);
    qDebug() << "*** Sha256 = " << hash.toHex();
    if(m_isStartup){
        if(readJSON(hash)) {
            ui->stackedWidget->setCurrentIndex(1);
            onFilterTextChanged("");
            m_isStartup = 0;
        }else {
            ui->lblLogin->setText("Неверный пин");
            ui->lblLogin->setStyleSheet("color:red;");
        }
    }
    else {
        // QByteArray encrypted_creds = QByteArray::fromHex(
        //     m_jsonarray[m_current_id].toObject()["logpass"].toString().toUtf8()
        //     );
        // QByteArray decrypted_creds;
        qDebug() << "Приложение уже запущено";
        // decryptFile(hash, encrypted_creds, decrypted_creds);
        // QGuiApplication::clipboard() -> setText(QString::fromUtf8(decrypted_creds));
        // ui->stackedWidget->setCurrentIndex(1);
    }


    ui->edtPin->setText(QString().fill('*', ui->edtPin->text().size()));
    ui->edtPin->clear();

    hash.setRawData(const_cast<const char *>( QByteArray().fill('*', 32).data()), 32);
    hash.clear();
}
