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

    //Préparer les séries
    seriesApos_.setName("Position (cm)");
    seriesAangle_.setName("Angle (deg)");

    // Modifié le graph

    ui->graphA->setChart(&chartA_);
    chartA_.setTitle("Robot A");
    chartA_.legend()->show();

    chartA_.addSeries(&seriesApos_);
    chartA_.addSeries(&seriesAangle_);

    axisX_ = new QValueAxis();
    axisY_ = new QValueAxis();

    axisX_->setTitleText("Temps");
    axisY_->setTitleText("Valeur");

    axisX_->setRange(0, 10);
    axisY_->setRange(-150, 150);

    chartA_.addAxis(axisX_, Qt::AlignBottom);
    chartA_.addAxis(axisY_, Qt::AlignLeft);

    seriesApos_.attachAxis(axisX_);
    seriesApos_.attachAxis(axisY_);

    seriesAangle_.attachAxis(axisX_);
    seriesAangle_.attachAxis(axisY_);

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
    const QString cleanMsg = msg.trimmed();

    if(cleanMsg.isEmpty())
    {
        qDebug() << "Ligne série vide ignorée";
        return;
    }

    // On prend le message recu
    QJsonParseError parseError;
    QJsonDocument jsonResponse = QJsonDocument::fromJson(cleanMsg.toUtf8(), &parseError);

    // Y a t il une error
    if (parseError.error != QJsonParseError::NoError)
    {
        qDebug().noquote()
                << "JSON invalid : "
                << parseError.errorString()
                << "\nMessage recu : "
                << msg;
        return;
    }

    // Est ce qu'on a bien recu un objet
    if(!jsonResponse.isObject())
    {
        qDebug() << "JSON n'est pas un objet";
        return;
    }

    const QJsonObject jsonObj = jsonResponse.object();
    const QString messageType = jsonObj.value("type").toString();

    // C'est quoi le type de message
    if(messageType != "robot_state")
    {
        onMessageReceived(msg);
        return;
    }

    // Parse le message
    time         = jsonObj.value("time").toDouble() / 1000.0; // le temps est est en ms
    position     = jsonObj.value("position").toDouble();
    speed        = jsonObj.value("speed").toDouble();
    angle        = jsonObj.value("angle").toDouble();
    angularSpeed = jsonObj.value("angular_speed").toDouble();
    state        = jsonObj.value("state").toString();
    power        = jsonObj.value("power").toDouble();
    hasExtended  = jsonObj.value("extended").toBool();
    numTree      = jsonObj.value("nb_tree").toInt();

    // On veut que le time comment a zéro sur le premier bouton start
    if(firstStart)
    {
        startTime_ = time;
    }

    qDebug()
            << "State"     << state
            << "Position"  << position
            << "Vitesse"   << speed
            << "Angle"     << angle
            << "AngVel"    << angularSpeed
            << "Extended"  << hasExtended;

    // Update les labels
    ui->totalTimeLabel->setText("Time: " + QString::number(time - startTime_,'f', 2) + " sec");
    ui->stateLabelA->setText("State: " + state);
    ui->maxPowerLabel->setText("Maximum power: " + QString::number(power,'f', 2));
    ui->numTreeLabel->setText("Tree dropped: " + QString::number(numTree, 'f', 0));


    // Update les données du graph
    seriesApos_.append(time, position*100);
    seriesAangle_.append(time, angle*180/StateMachine::PI);
    axisX_->setRange(time - 10, time);

    /*
    // Mise a jour state machine
    stateMachine_.update(state, position, speed, angle, angularSpeed, hasExtended);
    QString nextState = stateMachine_.getState();
    double x_target = stateMachine_.getTargetX();

    // Formatte et envoie message
    QJsonObject jsonObject
    {
        {"cmd", nextState},
        {"x_target", x_target}
    };

    QJsonDocument doc(jsonObject); // Formattage en document JSON
    QString strJson(doc.toJson(QJsonDocument::Compact));// Casting en type QString
    sendMessage(strJson);
    */
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
    //qDebug().noquote() <<"Bouton reset";
    firstStart = true;

    QJsonObject jsonObject
    {
        {"cmd", "STOP"}
    };
    QJsonDocument doc(jsonObject); // Formatage en document JSON
    QString strJson(doc.toJson(QJsonDocument::Compact));// Casting en type QString
    stateMachine_.sendButtonCommand(StateMachine::ButtonType::RESET);
    sendMessage(strJson);   // Envoi du message
}

void MainWindow::startButtonClicked() {
    // modifié
    //qDebug().noquote() <<"Bouton start";
    firstStart = false;
    QJsonObject jsonObject
    {
        {"cmd", "STABILIZE"},
        {"x_target", 0}
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
