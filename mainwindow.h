#ifndef MAINWINDOW_H
#define MAINWINDOW_H
// Inclusion de la classe QTimer
#include <QTimer>
#include <QMainWindow>
#include <QtNetwork>
#include <QMessageBox>
#include <QDebug>
#include <QPainter>
#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_connexionButton_clicked();

    void on_deconnexionButton_clicked();

    void on_envoiButton_clicked();

    void on_pushButton_plan_clicked();

    void on_pushButton_satellite_clicked();

    void gerer_donnees();

    void afficher_erreur(QAbstractSocket::SocketError);

    void mettre_a_jour_ihm();

private:
    Ui::MainWindow *ui;
    QTcpSocket *tcpSocket;
    QTimer *pTimer;
    QImage *pCarte;
    QImage *pSatellite;
    QImage *pPhoto_vide;
    QImage *pPhoto_coureur;


    double latitude;
    double longitude;
    double px;
    double py;
    double lastpx;
    double lastpy;
    double distAB;
    double R;
    double lastlat_rad;
    double lastlong_rad;
    double lat_rad;
    double long_rad;
    double lastdistance;
    double distance;
    double altitude;
    int last_timestamp;
    int timestamp;
    double calorie;
    double compteur;
    QSqlDatabase bdd;
};

#endif // MAINWINDOW_H
