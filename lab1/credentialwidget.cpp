#include "credentialwidget.h"
#include "ui_credentialwidget.h"

CredentialWidget::CredentialWidget(const QString &site
                                   , const int id
                                   , QWidget *parent)
    :QWidget(parent)
    , ui(new Ui::CredentialWidget)
    , m_id(id)
{
    ui->setupUi(this);
    ui->lblSite->setText(site);
}

CredentialWidget::~CredentialWidget()
{
    delete ui;
}

void CredentialWidget::on_loginbutton_clicked()
{
    qDebug() << "*** Pressed login" << m_id;
    isPass = 0;
    emit decryptLoginPassword(m_id, isPass);
}


void CredentialWidget::on_pushButton_clicked()
{
    qDebug() << "*** Pressed password" << m_id;
    isPass = 1;
    emit decryptLoginPassword(m_id, isPass);
}
