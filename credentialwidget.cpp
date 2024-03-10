#include "credentialwidget.h"
#include "ui_credentialwidget.h"

CredentialWidget::CredentialWidget(const QString &site
                                   , const int id
                                   , QWidget *parent)
    :QWidget(parent)
    , ui(new Ui::CredentialWidget)
    , m_id(id)
{
    //ui->setupUi(this);
    ui->lblSite->setText(site);
}

CredentialWidget::~CredentialWidget()
{
    delete ui;
}

void CredentialWidget::on_loginbutton_clicked()
{
    qDebug() << "*** Pressed" << m_id;
    emit decryptLogin(m_id);
}


void CredentialWidget::on_pushButton_clicked()
{
    qDebug() << "*** Pressed" << m_id;
    emit decryptPassword(m_id);
}
