#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QJsonArray>
#include <QMainWindow>
#include <QCryptographicHash>

class Cridential {
public:
    QString hostname;
    QString login;
    QString password;

};

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    bool readJSON(const QByteArray &aes256_key);
    void onFilterTextChanged(const QString &text);

private:
    Ui::MainWindow *ui;
    int decryptFile(const QByteArray &aes256_key, const QByteArray & encryptedBytes, QByteArray & deccryptedBytes);
    QJsonArray m_jsonarray; //структура данных, содержащая учетные записи
    void on_edtPin_returnPressed();
};


#endif // MAINWINDOW_H
