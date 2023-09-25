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
    pCarte = new QImage();
    pCarte->load(":/carte_la_rochelle.png");
    pSatellite = new QImage();
    pSatellite->load(":/carte_la_rochelle_satellite.png");
    pPhoto_vide = new QImage();
    pPhoto_vide->load(":/photo_vide");
    px = 0.0;
    py = 0.0;
    lastpx = 0.0;
    lastpy = 0.0;
    lastlat_rad = 0.0;
    lastlong_rad = 0.0;
    lat_rad = 0.0;
    long_rad = 0.0;
    lastdistance = 0.0;
    distAB = 0.0;
    distance = 0.0;
    ui->label_carte->setPixmap(QPixmap::fromImage(*pCarte));
    timestamp = 0.0;
    calorie = 0.0;
    compteur = 0.0;
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
    delete pCarte;
    delete pSatellite;
    delete pPhoto_vide;

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
double degToRad(double degrees) {
    return degrees * M_PI / 180.0;
}
void MainWindow :: on_pushButton_plan_clicked()
{
    ui->label_carte->setPixmap(QPixmap::fromImage(*pCarte));
}

void MainWindow :: on_pushButton_satellite_clicked()
{
    ui->label_carte->setPixmap(QPixmap::fromImage(*pSatellite));
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
    int satellite = nb_satellite.toInt();
    if (satellite > 3){
        qDebug()<<"satellite"<<satellite;
    //temps écoulé
    int heures = liste[1].mid(0,2).toInt();
    int minutes = liste[1].mid(2,2).toInt();
    int secondes = liste[1].mid(4,2).toInt();
    timestamp = (heures * 3600)+(minutes * 60) + (secondes);
    qDebug() << "timestamp : " << timestamp;
    qDebug() << "Heures : " << heures;
    qDebug() << "minutes : " << minutes;
    qDebug() << "secondes : " << secondes;
    QString heuresQString = QString ("%1").arg(heures);
    QString minutesQString = QString ("%1").arg(minutes);
    QString secondesQString = QString ("%1").arg(secondes);
    int premier_relevé = 28957;
        QString timestampQString = QString("%1").arg(timestamp-premier_relevé);
          ui->lineEdit_temps->setText(timestampQString);


    // Latitude
    double degres_lat = lat.mid(0,2).toDouble();
    double minutes_lat = lat.mid(2,7).toDouble();
    if( N_or_S == "S"){
        latitude = (degres_lat + (minutes_lat / 60.0))*(-1.0);
    }else if(N_or_S == "N"){
        latitude = degres_lat + (minutes_lat / 60.0);

    }else{
        latitude =(degres_lat + (minutes_lat / 60.0));
    }
    QString latitude_string = QString("%1").arg(latitude);
    ui->lineEdit_lat->setText(latitude_string);

    // Longitude
    double degres_long = lon.mid(0,3).toDouble();
    double minutes_long = lon.mid(3,7).toDouble();
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

    //dessin sur la carte
    // Préparation du contexte de dessin sur une image existante
    const double lat_hg = 46.173311;
    const double long_hg = -1.195703;
    const double lat_bd = 46.135451;
    const double long_bd = -1.136125;
    const double largeur_carte = 694.0;
    const double hauteur_carte = 638.0;
    px = largeur_carte * ( (longitude - long_hg ) / (long_bd - long_hg) );
    py = hauteur_carte * ( 1.0 - (latitude - lat_bd) / (lat_hg - lat_bd) );
    QPainter p(pPhoto_vide);
    // Choix de la couleur
    if ((lastpx != 0.0) && (lastpy != 0.0)){
        p.setPen(Qt::red);
        // Dessin d'une ligne
        p.drawLine(lastpx, lastpy, px, py);
        p.end();
        ui->label_photo_vide->setPixmap(QPixmap::fromImage(*pPhoto_vide));
    }
    else {
    }
    lastpx = px;
    lastpy = py;
    long_rad = degToRad(longitude);
    lat_rad = degToRad(latitude);

    //distance
    if(lastlat_rad != 0 && lastlong_rad != 0){
        distAB = 6378 * acos(sin(lastlat_rad)*sin(lat_rad) + cos(lastlat_rad) * cos(lat_rad)* cos(lastlong_rad - long_rad));
        distance = distAB + lastdistance;
        QString distAB_string = QString("%1").arg(distance);
        ui->lineEdit_distance->setText(distAB_string);
    }else{

    }
    //taille
    int taille = ui->spinBox_taille->value();

    //Calories dépensé
    double poids = ui->spinBox_poids->value();
    double calorie = distance * poids * 1.036;
    QString calorie_string = QString("%1").arg(calorie);
    ui->lineEdit_calorie->setText(calorie_string);
    //altitude
    ui->lineEdit_altitude->setText(altitude);

    //vitesse
    double vitesse;
    double diff_tps = timestamp - last_timestamp;
    vitesse = distAB/ (diff_tps/3600.0);
    QString vitesseString = QString("%1").arg(vitesse);
    ui->lineEdit_vitesse->setText(vitesseString);

    // courbe fréquence
    QPainter painter(pPhoto_vide);

    painter.setPen(QPen(Qt::transparent, 1));
    painter.drawLine(compteur, 200, compteur,200);
    painter.setPen(QPen(Qt::red, 1));
    painter.drawLine(compteur, 500, compteur,600 - freq);
    compteur += 1;
    if (compteur >= ui->label_courbe_cardiaque->width()) {
        pPhoto_vide->fill(Qt::transparent);
        compteur = 0;
    }
    //courbe altitude
    int altitudeDouble = altitude.toDouble();
    painter.setPen(QPen(Qt::black, 1));
    painter.drawLine(compteur, 600, compteur,550 - altitudeDouble);
    ui->label_courbe_cardiaque->width();
    painter.end();

    ui->lineEdit_satellite->setText(nb_satellite);
    }
    else{
    ui->lineEdit_lat->setText("error");
    ui->lineEdit_long->setText("error");
    ui->lineEdit_altitude->setText("error");
    ui->lineEdit_vitesse->setText("error");
    ui->lineEdit_distance->setText("error");
    ui->lineEdit_satellite->setText("Pas assez de satellite");
    }
}
void MainWindow::mettre_a_jour_ihm()
{
    // Préparation de la requête
    QByteArray requete;
    requete = "RETR\r\n";

    // Envoi de la requête
    tcpSocket->write(requete);
    lastlat_rad = lat_rad;
    lastlong_rad = long_rad;
    lastdistance = distance;
    last_timestamp = timestamp;

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
