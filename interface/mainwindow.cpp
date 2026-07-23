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
    seriesApos_.attachAxis(axisX);




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

                time = jsonObj["time"].toDouble() / 1000.0; // le temps est est en ms
                position = jsonObj["x"].toDouble();
                angle = jsonObj["angle"].toDouble();
                state = jsonObj.value("state").toString();

                ui->totalTimeLabel->setText("Time: " + QString::number(getTime(),'f', 2) + " sec");
                ui->stateLabelA->setText("State: " + getState());

                seriesApos_.append(getTime(), getPosition());
                seriesAangle_.append(getTime(), getAngle());

                qDebug()
                        << "Position" << getPosition()
                        << "Angle"    << getAngle()
                        << "State"    << getState();
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


QString MainWindow::getState() const
{
     return state;
}
double MainWindow::getPosition() const
{
    return position;
}
double MainWindow::getAngle() const
{
    return angle;
}

double MainWindow::getTime () const
{
    return time;
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
        {"cmd", "START"}
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
        {"cmd", "STOP"}
    };
    QJsonDocument doc(jsonObject); // Formatage en document JSON
    QString strJson(doc.toJson(QJsonDocument::Compact));// Casting en type QString
    sendMessage(strJson);   // Envoi du message
}

void MainWindow::stateMachine(){

    QString nextState;
    double x_target = getPosition();

    if(getState() == "Idle"){
        qDebug() << "Le robot est en attente";

    }

    else if(getState() == "MoveToX"){
        qDebug() << "Position actuelle"
                 << getPosition();

    }

    else if(getState() == "Stabilize"){
        qDebug() << "Angle actuel"
                 << getAngle();

        if (qAbs(getAngle() < 1.0)
                 qDebug() << "Le pendule est presque stable";

    }

    else if (getState() == "Drop"){
         qDebug() << "Le robot depose la charge";
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
