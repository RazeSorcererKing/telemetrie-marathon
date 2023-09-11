#include "maclasse.h"
#include "./ui_maclasse.h"

MaClasse::MaClasse(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MaClasse)
{
    ui->setupUi(this);
}

MaClasse::~MaClasse()
{
    delete ui;
}


void MaClasse::on_pushButton_clicked()
{
    // Récupération du teste saisi
    QString texte = ui->lineEdit->text();

    //Affichage label
    ui->label->setText(texte);
}
