#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    // Initialisation de l'interface graphique
    ui->setupUi(this);

    // Instanciation de la socket
    tcpSocket = new QTcpSocket(this);

    // Attachement d'un slot qui sera appelé à chaque fois que des données arrivent (mode asynchrone)
    connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(gerer_donnees()));

    // Idem pour les erreurs
    connect(tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(afficher_erreur(QAbstractSocket::SocketError)));
    //Instantier le timer
    pTimer = new QTimer();
    // Association du "tick" du timer à l'appel d'une méthode SLOT
        connect(pTimer, SIGNAL(timeout()), this, SLOT(mettre_a_jour_ihm()));
    // Lancement du timer avec un tick toutes les 1000 ms
    pTimer->start(1000);
}

MainWindow::~MainWindow()
{
    // Destruction de la socket
    tcpSocket->abort();
    delete tcpSocket;
    // Arrêt du timer
    pTimer->stop();
    // Destruction du timer
    delete pTimer;

    // Destruction de l'interface graphique
    delete ui;

}

void MainWindow::on_connexionButton_clicked()
{
    // Récupération des paramètres
    QString adresse_ip = ui->lineEdit_ip->text();
    unsigned short port_tcp = ui->lineEdit_port->text().toInt();

    // Connexion au serveur
    tcpSocket->connectToHost(adresse_ip, port_tcp);
}

void MainWindow::on_deconnexionButton_clicked()
{
    // Déconnexion du serveur
    tcpSocket->close();
}

void MainWindow::on_envoiButton_clicked()
{
    // Préparation de la requête
    QByteArray requete;
    requete = "RETR\r\n";
    // Envoi de la requête
    tcpSocket->write(requete);
}

void MainWindow::gerer_donnees()
{

    // Réception des données
    QByteArray reponse = tcpSocket->readAll();

    // Affichage
    ui->lineEdit_reponse->setText(QString(reponse));
    qDebug() << QString(reponse);
    QString trame = QString(reponse);
    //Décodage
    QStringList liste = trame.split(",");
    qDebug() << liste[1];
    QString lat = liste[2];
    QString N_or_S = liste[3];
    QString lon = liste[4];
    QString W_or_E = liste[5];
    QString postype = liste[6];
    QString nb_satellite = liste[7];
    QString precision_horizontale = liste[8];
    QString altitude = liste[9];
    QString unite_altitude = liste[10];
    QString hauteur_geo = liste[11];
    QString unite_hauteur = liste[12];
    QString tps_last_maj = liste[13];
    QString frequence_cardiaque = liste[14];
    //Date
    int heures = liste[1].mid(0,2).toInt();
    int minutes = liste[1].mid(2,2).toInt();
    int secondes = liste[1].mid(4,2).toInt();
    int timestamp = (heures * 3600)+(minutes * 60) + (secondes);
    qDebug() << "timestamp : " << timestamp;
    qDebug() << "Heures : " << heures;
    qDebug() << "minutes : " << minutes;
    qDebug() << "secondes : " << secondes;
    QString timestampQString = QString ("%1").arg(timestamp);
    QString heuresQString = QString ("%1").arg(heures);
    QString minutesQString = QString ("%1").arg(minutes);
    QString secondesQString = QString ("%1").arg(secondes);

    // Latitude
    double latitude = 0.0;
    double degres_lat = lat.mid(0,2).toDouble();
    double minutes_lat = lat.mid(2,7).toDouble();
    if( N_or_S == "S"){
        latitude = (degres_lat + (minutes_lat / 60))*(-1);
    }else if(N_or_S == "N"){
        latitude = degres_lat + (minutes_lat / 60);

    }else{
        latitude =(degres_lat + (minutes_lat / 60));
    }
    QString latitude_string = QString("%1").arg(latitude);
    ui->lineEdit_lat->setText(latitude_string);

    // Longitude
    double longitude = 0.0;
    double degres_long = lon.mid(0,2).toDouble();
    double minutes_long = lon.mid(2,7).toDouble();
    if( W_or_E == "W"){
        longitude = (degres_long + (minutes_long / 60))*(-1);
    }else if(W_or_E == "E"){
        longitude = degres_long + (minutes_long / 60);

    }else{
        longitude =(degres_long + (minutes_long / 60));
    }
    QString longitude_string = QString("%1").arg(longitude);
    ui->lineEdit_long->setText(longitude_string);

    //Fréquence cardiaque
    int freq = frequence_cardiaque.mid(1,3).toInt();
    QString freq_string = QString("%1").arg(freq);
    ui->lineEdit_bpm->setText(freq_string);

    //age
    int age = ui->spinBox->value();

    //frequence max
    float fCmax = 207-(0.7 * age);
    QString fcmax_string = QString("%1").arg(fCmax);
    ui->lineEdit_fcmax->setText(fcmax_string);

    //intensité
    int intensite = (freq / fCmax) * 100;
    ui->progressBar->setValue(intensite);
}
void MainWindow::mettre_a_jour_ihm()
{
    // Préparation de la requête
    QByteArray requete;
    requete = "RETR\r\n";

    // Envoi de la requête
    tcpSocket->write(requete);
}
void MainWindow::afficher_erreur(QAbstractSocket::SocketError socketError)
{
    switch (socketError)
    {
        case QAbstractSocket::RemoteHostClosedError:
            break;
        case QAbstractSocket::HostNotFoundError:
            QMessageBox::information(this, tr("Client TCP"),
                                     tr("Hôte introuvable"));
            break;
        case QAbstractSocket::ConnectionRefusedError:
            QMessageBox::information(this, tr("Client TCP"),
                                     tr("Connexion refusée"));
            break;
        default:
            QMessageBox::information(this, tr("Client TCP"),
                                     tr("Erreur : %1.")
                                     .arg(tcpSocket->errorString()));
    }
}
