#include "mainwindow.h"
#include "ui_mainwindow.h"

#define ON_DURATION 2

//-------------------------------------------
// Constructeur
//-------------------------------------------
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
    chartA_.legend()->hide();
    chartA_.addSeries(&seriesApos_);
    chartA_.addSeries(&seriesAangle_);

    stateMachine_ = StateMachine();

    // Fonctions de connections events/slots
    connectTimers(updateRate);
    connectButtons();
    connectComboBox();

    // Recensement des ports
    portCensus();

    // initialisation du timer
    updateTimer_.start();
}

//-------------------------------------------
// Destructeur
//-------------------------------------------
MainWindow::~MainWindow(){
    // Destructeur de la classe
    updateTimer_.stop();
    if(serialCom_!=nullptr){
      delete serialCom_;
    }
    delete ui;
}

//-------------------------------------------
// Fonctions d'events
//-------------------------------------------
void MainWindow::closeEvent(QCloseEvent *event){
    // Fonction appelee lorsque la fenetre est detruite
    event->accept();
}

void MainWindow::receiveFromSerial(QString msg){
    // Fonction appelee lors de reception sur port serie
    // Accumulation des morceaux de message
    msgBuffer_ += msg;

    //qDebug() << "MSG : " << jsonResponse.isEmpty();

    //Si un message est termine
    if(msgBuffer_.endsWith('\n')){
        // Passage ASCII vers structure Json
        QJsonDocument jsonResponse = QJsonDocument::fromJson(msgBuffer_.toUtf8());

        // Analyse du message Json
        if(!jsonResponse.isEmpty()){
            QJsonObject jsonObj = jsonResponse.object();

            //modifié
            //QString mess = jsonResponse.toJson(QJsonDocument::Indented);
            //ui->textBrowser->setText(mess.mid(2,buff.length()-4));

            if(jsonObj.contains("type") && jsonObj["type"] == "robot_state") {

                time = jsonObj["time"].toDouble() / 1000.0; // le temps est est en ms
                position = jsonObj["position"].toDouble();
                speed = jsonObj["speed"].toDouble();
                angle = jsonObj["angle"].toDouble();
                angularSpeed = jsonObj["angular_speed"].toDouble();
                state = jsonObj.["state"].toString();

                ui->totalTimeLabel->setText("Time: " + QString::number(time,'f', 2) + " sec");
                ui->stateLabelA->setText("State: " + state);

                seriesApos_.append(time, position);
                chartA_.removeSeries(&seriesApos_);
                chartA_.addSeries(&seriesApos_);
                chartA_.createDefaultAxes();


                seriesAangle_.append(time, angle);
                chartA_.removeSeries(&seriesAangle_);
                chartA_.addSeries(&seriesAangle_);
                chartA_.createDefaultAxes();

                qDebug()
                        << "Position" << position
                        << "Vitesse"    << speed
                        << "Angle"    << angle
                        << "AngVel"    << angularSpeed
                        << "State"    << state;
            }
            else {
                msgReceived_ = msgBuffer_;
                onMessageReceived(msgReceived_);
            }
        }
        // Reinitialisation du message tampon
        msgBuffer_ = "";

        // Mise a jour state machine
        stateMachine_.update(state, position, speed, angle, angularSpeed);
        QString nextState = stateMachine_.getState();
        double x_target = stateMachine_.getTargetX();

        // Formatte et envoie message
        QJsonObject jsonObject
        {
            {"cmd", nextState},
            {"x_target", x_target}
        };

        QJsonDocument doc(jsonObject); // Formatage en document JSON
        QString strJson(doc.toJson(QJsonDocument::Compact));// Casting en type QString
        sendMessage(strJson);
    }
}

void MainWindow::onMessageReceived(QString msg){
    // Fonction appelee lors de reception de message
    // Decommenter la ligne suivante pour deverminage
    qDebug().noquote() << "Message du Arduino: " << msg;
}

void MainWindow::onPeriodicUpdate(){
    // Fonction SLOT appelee a intervalle definie dans le constructeur
    // qDebug().noquote() << "*";
}

//-------------------------------------------
// Fonctions de connections
//-------------------------------------------
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

void MainWindow::connectComboBox(){
    // Fonction de connection des entrees deroulantes
    connect(ui->comboBoxPort, SIGNAL(activated(QString)), this, SLOT(startSerialCom(QString)));
}

//-------------------------------------------
// Functions de boutons
//-------------------------------------------
void MainWindow::resetButtonClicked() {
    // modifié
    qDebug().noquote() <<"Bouton reset";
    QJsonObject jsonObject
    {
        {"cmd", "RESET"}
    };
    QJsonDocument doc(jsonObject); // Formatage en document JSON
    QString strJson(doc.toJson(QJsonDocument::Compact));// Casting en type QString
    stateMachine_.sendButtonCommand(StateMachine::ButtonType::RESET);
    sendMessage(strJson);   // Envoi du message
}

void MainWindow::startButtonClicked() {
    // modifié
    qDebug().noquote() <<"Bouton start";
    QJsonObject jsonObject
    {
        {"cmd", "START"}
    };
    QJsonDocument doc(jsonObject); // Formatage en document JSON
    QString strJson(doc.toJson(QJsonDocument::Compact));// Casting en type QString
    stateMachine_.sendButtonCommand(StateMachine::ButtonType::START);
    sendMessage(strJson);   // Envoi du message
}

void MainWindow::stopButtonClicked() {
    // modifié
    qDebug().noquote() <<"Bouton stop";
    QJsonObject jsonObject
    {
        {"cmd", "STOP"}
    };
    QJsonDocument doc(jsonObject); // Formatage en document JSON
    QString strJson(doc.toJson(QJsonDocument::Compact));// Casting en type QString
    stateMachine_.sendButtonCommand(StateMachine::ButtonType::STOP);
    sendMessage(strJson);   // Envoi du message
}



//-------------------------------------------
// Fonctions utilitaires
//-------------------------------------------
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
