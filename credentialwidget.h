#ifndef CREDENTIALWIDGET_H
#define CREDENTIALWIDGET_H

#include <QWidget>

namespace Ui {
class CredentialWidget;
}

class CredentialWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CredentialWidget(const QString&site,
                              const int id,
                              QWidget *parent = nullptr);
    ~CredentialWidget();

private slots:
    void on_loginbutton_clicked();

    void on_pushButton_clicked();

signals:
    void decryptLogin(int id);
    void decryptPassword(int id);

private:
    Ui::CredentialWidget *ui;
    int m_id = -1;
};

#endif // CREDENTIALWIDGET_H
