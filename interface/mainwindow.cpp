#include "mainwindow.h"
#include "ui_mainwindow.h"

#define ON_DURATION 2

double longueur_pondule = 0.0;
double obstacle = 0.0;


MainWindow::MainWindow(int updateRate, QWidget *parent):
    QMainWindow(parent)
{
    // Constructeur de la classe
    // Initialisation du UI
    ui = new Ui::MainWindow;
    ui->setupUi(this);
    this->setWindowTitle("Atelier Qt");

    // Modifié
    ui->graphA->setChart(&chartA_);
    chartA_.setTitle("Robot A");
    chartA_.addSeries(&seriesApos_);
    chartA_.addSeries(&seriesAangle_);
    QValueAxis* axisX = new QValueAxis();
    axisX->setTitleText("tmt");
    chartA_.addAxis(axisX, Qt::AlignBottom);
    seriesApos_.attachAxis(axisX);


    ui->graphB->setChart(&chartB_);
    chartB_.setTitle("Robot B");
    chartB_.legend()->hide();
    chartB_.addSeries(&seriesBpos_);
    chartB_.addSeries(&seriesBangle_);



    // Fonctions de connections events/slots
    connectTimers(updateRate);
    connectButtons();
    connectComboBox();

    // Recensement des ports
    portCensus();

    // initialisation du timer
    updateTimer_.start();
}

MainWindow::~MainWindow(){
    // Destructeur de la classe
    updateTimer_.stop();
    if(serialCom_!=nullptr){
      delete serialCom_;
    }
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event){
    // Fonction appelee lorsque la fenetre est detruite
    event->accept();
}

void MainWindow::receiveFromSerial(QString msg){
    // Fonction appelee lors de reception sur port serie
    // Accumulation des morceaux de message
    msgBuffer_ += msg;

    //Si un message est termine
    if(msgBuffer_.endsWith('\n')){
        // Passage ASCII vers structure Json
        QJsonDocument jsonResponse = QJsonDocument::fromJson(msgBuffer_.toUtf8());

        // Analyse du message Json
        if(~jsonResponse.isEmpty()){
            QJsonObject jsonObj = jsonResponse.object();

            //modifié
            //QString mess = jsonResponse.toJson(QJsonDocument::Indented);
            //ui->textBrowser->setText(mess.mid(2,buff.length()-4));

            if(jsonObj.contains("type") && jsonObj["type"] == "robot_state") {

                double time = jsonObj["time"].toDouble();
                double position = jsonObj["x"].toDouble();

                seriesApos_.append(time, position);
                chartA_.removeSeries(&seriesApos_);
                chartA_.addSeries(&seriesApos_);
                chartA_.createDefaultAxes();
                ui->totalTimeLabel->setText("Time: " + QString::number(time) + " sec");

                QString state = jsonObj["state"];
                ui->stateLabelA->setText("State: " + state);


                double angle = jsonObj["angle"].toDouble();
                seriesAangle_.append(time, angle);
                chartA_.removeSeries(&seriesAangle_);
                chartA_.addSeries(&seriesAangle_);
                chartA_.createDefaultAxes();


            }





            else {
                msgReceived_ = msgBuffer_;
                onMessageReceived(msgReceived_);
            }
        }
        // Reinitialisation du message tampon
        msgBuffer_ = "";
    }
}

void MainWindow::connectTimers(int updateRate){
    // Fonction de connection de timers
    connect(&updateTimer_, &QTimer::timeout, this, [this]{onPeriodicUpdate();});
    updateTimer_.start(updateRate);
}

void MainWindow::connectSerialPortRead(){
    // Fonction de connection au message de la classe (serialProtocol)
    connect(serialCom_, SIGNAL(newMessage(QString)), this, SLOT(receiveFromSerial(QString)));
}

void MainWindow::connectButtons(){
    // modifié
    connect(ui->resetButton, SIGNAL(clicked(bool)), this, SLOT(resetButtonClicked()));
    connect(ui->startButton, SIGNAL(clicked(bool)), this, SLOT(startButtonClicked()));
    connect(ui->stopButton, SIGNAL(clicked(bool)), this, SLOT(stopButtonClicked()));
}

void MainWindow::resetButtonClicked() {
    // modifié
    qDebug().noquote() <<"Bouton reset";
    QJsonObject jsonObject{
        {"cmd", RESET}
    };
    QJsonDocument doc(jsonObject); // Formatage en document JSON
    QString strJson(doc.toJson(QJsonDocument::Compact));// Casting en type QString
    sendMessage(strJson);   // Envoi du message
}

void MainWindow::startButtonClicked() {
    // modifié
    qDebug().noquote() <<"Bouton start";
    QJsonObject jsonObject{
        {"cmd", START}
    };
    QJsonDocument doc(jsonObject); // Formatage en document JSON
    QString strJson(doc.toJson(QJsonDocument::Compact));// Casting en type QString
    sendMessage(strJson);   // Envoi du message
}

void MainWindow::stopButtonClicked() {
    // modifié
    qDebug().noquote() <<"Bouton stop";
    QJsonObject jsonObject
    {
        {"cmd", STOP}
    };
    QJsonDocument doc(jsonObject); // Formatage en document JSON
    QString strJson(doc.toJson(QJsonDocument::Compact));// Casting en type QString
    sendMessage(strJson);   // Envoi du message
}

void MainWindow::stateMachine(){
    double x_target = 0.0;
    if(receiveFromSerial.state == Idle){
        x_target =  0.0;

    }

    if(longueur_pondule && obstacle){
        next_state = MoveToX;
        x_target = 120.0;
        next_state = Stabilize;
    }

    if(receiveFromSerial.state == Stabilize){
        next_state = Drop;
        next_state = Idle;
    }





    QJsonObject jsonObject
    {
        {"cmd", next_state},
        {"x_target", target}
    };

    QJsonDocument doc(jsonObject); // Formatage en document JSON
    QString strJson(doc.toJson(QJsonDocument::Compact));// Casting en type QString
    sendMessage(strJson);





}

void MainWindow::connectComboBox(){
    // Fonction de connection des entrees deroulantes
    connect(ui->comboBoxPort, SIGNAL(activated(QString)), this, SLOT(startSerialCom(QString)));
}

void MainWindow::portCensus(){
    // Fonction pour recenser les ports disponibles
    ui->comboBoxPort->clear();
    Q_FOREACH(QSerialPortInfo port, QSerialPortInfo::availablePorts()) {
        ui->comboBoxPort->addItem(port.portName());
    }
}

void MainWindow::startSerialCom(QString portName){
    // Fonction SLOT pour demarrer la communication serielle
    qDebug().noquote() << "Connection au port"<< portName;
    if(serialCom_!=nullptr){
        delete serialCom_;
    }
    serialCom_ = new SerialProtocol(portName, BAUD_RATE);
    connectSerialPortRead();
}


void MainWindow::sendMessage(QString msg){
    // Fonction SLOT d'ecriture sur le port serie
    if(serialCom_==nullptr){
        qDebug().noquote() <<"Erreur aucun port serie !!!";
        return;
    }
    serialCom_->sendMessage(msg);
    qDebug().noquote() <<"Message du RPI: "  <<msg;
}

void MainWindow::setUpdateRate(int rateMs){
    // Fonction d'initialisation du chronometre
    updateTimer_.start(rateMs);
}

void MainWindow::onMessageReceived(QString msg){
    // Fonction appelee lors de reception de message
    // Decommenter la ligne suivante pour deverminage
    qDebug().noquote() << "Message du Arduino: " << msg;
}

void MainWindow::onPeriodicUpdate(){
    // Fonction SLOT appelee a intervalle definie dans le constructeur
    qDebug().noquote() << "*";
}
