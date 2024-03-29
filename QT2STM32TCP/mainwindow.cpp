#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "qcustomplot.h"    // ADD_Amazin
#include <QDebug>   // ADD_Amazin similar as stdio.h
#include <QString>  // ADD_Amazin c++의 string을 진화시킨 것


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

// ================= Amazin start =================

    // 1. create socket
    socket = new QUdpSocket(this);
    // 2. Register IP, SW PORT Number
    bool result = socket->bind(QHostAddress::AnyIPv4, 9999);
    qDebug() << result;     // qDebug --> c++의 cout
    if (result) {
        qDebug() << "PASS";
    }
    else {
        qDebug() << "FAIL";
    }
    // SIGNAL : event 발생
    // SLOT : INT service routine(function)
    // connetc : =mapping(연결시키는 것)
    connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()));
    // = socket으로부터 읽을 데이터가 존재하면 나의(this) readyRead()함수를 호출해라

    ui->plot_tmp->setInteraction(QCP::iRangeDrag, true);
    ui->plot_tmp->setInteraction(QCP::iRangeZoom, true);
    ui->plot_tmp->addGraph();
    //ui->plot_tmp->xAxis->setLabel("time(s)");
    ui->plot_tmp->yAxis->setLabel("temp");
    ui->plot_tmp->yAxis->setRange(0.0, 40.0);
    ui->plot_tmp->graph(0)->setScatterStyle(QCPScatterStyle::ssCircle);     // 점찍는 스타일 결정.
    ui->plot_tmp->graph(0)->setLineStyle(QCPGraph::lsLine);                 // 라인 스타일 결정.
    connect(ui->plot_tmp, SIGNAL(mouseDoubleClickEvent(QMouseEvent*)), SLOT(QMouseEvent*));     // 주석 처리해도 상관 없는 부분

    ui->plot_wet->setInteraction(QCP::iRangeDrag, true);
    ui->plot_wet->setInteraction(QCP::iRangeZoom, true);
    ui->plot_wet->addGraph();
    //ui->plot_wet->xAxis->setLabel("time(s)");
    ui->plot_wet->yAxis->setLabel("temp");
    ui->plot_wet->yAxis->setRange(0.0, 40.0);
    ui->plot_wet->graph(0)->setScatterStyle(QCPScatterStyle::ssCircle);
    ui->plot_wet->graph(0)->setLineStyle(QCPGraph::lsLine);
    connect(ui->plot_wet, SIGNAL(mouseDoubleClickEvent(QMouseEvent*)), SLOT(QMouseEvent*));

// ================= Amazin end =================
}

void MainWindow::readyRead() {
    QByteArray Buffer;  // 그냥 버퍼
    QByteArray Buffer_wet;      // 습도용 버퍼
    double temp_tmp;  // 온도값
    double temp_wet;    // 습도값
    static int time_tmp = 0;
    static int time_wet = 0;

    QHostAddress sender;    // 송신자의 IP Address
    quint16 senderPort;     // 송신자의 SW PORT Num

    Buffer.resize(socket->pendingDatagramSize());   // 현재 대기중인 데이터를 resize

    // 3. Read data from socket
    socket->readDatagram(Buffer.data(), Buffer.size(), &sender, &senderPort);
    Buffer.chop(1);     // 배열에서 마지막에서부터 ()안에 입력된 숫자까지 제거한다.
    // ex) chop(1)이면 [Tmp]25\n 가 들어왔을 때 \n을 제거한다.
    ui->textEditRxData->append(Buffer);     // 창의 맨 끝에 글자를 붙여넣는다.
    ui->textEditRxData->show();     // 창에 데이터를 display

    // LCD 창에 습도 출력
    Buffer_wet = Buffer.right(2);
    temp_wet = Buffer_wet.toDouble();
    ui->lcdNumberWet->display(temp_wet);

    // LCD 창에 온도 출력
    Buffer.chop(9);     // 버퍼에 [Tmp]25만 남기기 위해 다시 chop()
    Buffer = Buffer.right(2);    // [Tmp]25 맨 오른쪽부터 2자리를 잘라 버퍼에 넣는다. 즉, 숫자만 버퍼에 집어넣는다.
    // Buffer에는 25가 들어있다. 25는 utf8 mode. 유니코드를 하드디스크에 저장하는 포맷을 설정한다.
    temp_tmp = Buffer.toDouble();     // 버퍼에 있는 내용을 double형으로 변환한다.
    ui->lcdNumberTmp->display(temp_tmp);     // LCD 창에 출력

    // 습도 Graph
    qDebug() <<"Message From :: " << sender.toString();
    qDebug() <<"Port From :: "<< senderPort;
    qDebug() <<"Buffer :: " << Buffer_wet;
    add_point_wet(time_wet, temp_wet);
    time_wet+=3;
    ui->plot_wet->xAxis->setRange(0, time_wet+3);
    plot_wet();

    // 온도 Graph
    qDebug() <<"Message From :: " << sender.toString();
    qDebug() <<"Port From :: "<< senderPort;
    qDebug() <<"Buffer :: " << Buffer;
    add_point_tmp(time_tmp, temp_tmp);
    time_tmp+=3;
    ui->plot_tmp->xAxis->setRange(0, time_tmp+3);
    plot_tmp();
}

void MainWindow::add_point_tmp(double x, double y){
    qv_x.append(x);
    qv_y.append(y);
}

void MainWindow::add_point_wet(double x, double y){
    qw_x.append(x);
    qw_y.append(y);
}

void MainWindow::clear_data_tmp(){
    qv_x.clear();
    qv_y.clear();
}

void MainWindow::clear_data_wet(){
    qw_x.clear();
    qw_y.clear();
}

void MainWindow::plot_tmp(){
    ui->plot_tmp->graph(0)->setData(qv_x, qv_y);
    ui->plot_tmp->replot();
    ui->plot_tmp->update();
}

void MainWindow::plot_wet(){
    ui->plot_wet->graph(0)->setData(qw_x, qw_y);
    ui->plot_wet->replot();
    ui->plot_wet->update();
}

MainWindow::~MainWindow()
{
    delete ui;
}

// 생성 경로 :send --> 마우스 우클릭 --> go to slot --> click
void MainWindow::on_pushButtonSend_clicked()
{
    QByteArray Data;

    Data = ui->lineEditSendData->text().toUtf8();   // 만약 ASCII 코드였다면 utf8로 변환시킬 필요는 없었을 것이다.
    socket->writeDatagram(Data, QHostAddress("10.10.15.79"), 9999);     // STM32 IP ADDRESS
}

void MainWindow::on_checkBoxLED1_stateChanged(int arg1)
{
    QString buffer;
    QByteArray SendData;

    buffer.sprintf("LED001");
    SendData = buffer.toUtf8();     // ASCII는 utf8과 값이 동일하여 변환이 굳이 필요하지 않다. 단, 한글은 꼭 필요하다.
    socket->writeDatagram(SendData, QHostAddress("10.10.15.79"), 9999);
}

void MainWindow::on_checkBoxLED2_stateChanged(int arg1)
{
    QString buffer;
    QByteArray SendData;

    buffer.sprintf("LED002");
    SendData = buffer.toUtf8();
    socket->writeDatagram(SendData, QHostAddress("10.10.15.79"), 9999);
}

void MainWindow::on_checkBoxLED3_stateChanged(int arg1)
{
    QString buffer;
    QByteArray SendData;

    buffer.sprintf("LED003");
    SendData = buffer.toUtf8();
    socket->writeDatagram(SendData, QHostAddress("10.10.15.79"), 9999);
}

// SERVO:999
void MainWindow::on_dial_servo_valueChanged(int value)
{
    QByteArray servo_data = "SERVO:";   // TCP/IP 통신에서는 \n이 필요없어 굳이 붙이지 않았다.

    servo_data.append(QString::number(ui->dial_servo->value()));     // append()는 붙이는 함수
    ui->lcdNumber_servo->display(ui->dial_servo->value());
    socket->writeDatagram(servo_data, QHostAddress("10.10.15.79"), 9999);

    qDebug() << "servo data: " << servo_data << endl;
}

void MainWindow::on_dial_led_valueChanged(int value)
{
    QByteArray led_data = "LED:";

    led_data.append(QString::number(ui->dial_led->value()));
    ui->lcdNumber_led->display(ui->dial_led->value());
    socket->writeDatagram(led_data, QHostAddress("10.10.15.79"), 9999);

    qDebug() << "LED data: " << led_data << endl;
}

void MainWindow::on_horizontalSlider_DHT11_tmp_valueChanged(int value)
{
    QByteArray interval_tmp_data = "DHT11 interval tmp:";

    interval_tmp_data.append(QString::number(ui->horizontalSlider_DHT11_tmp->value()));
    ui->lcdNumber_tmp->display(ui->horizontalSlider_DHT11_tmp->value());
    socket->writeDatagram(interval_tmp_data, QHostAddress("10.10.15.79"), 9999);

    qDebug() << "DHT11 interval tmp data: " << interval_tmp_data << endl;
}

void MainWindow::on_horizontalSlider_DHT11_wet_valueChanged(int value)
{
    QByteArray interval_wet_data = "DHT11 interval wet:";

    interval_wet_data.append(QString::number(ui->horizontalSlider_DHT11_wet->value()));
    ui->lcdNumber_wet->display(ui->horizontalSlider_DHT11_wet->value());
    socket->writeDatagram(interval_wet_data, QHostAddress("10.10.15.79"), 9999);

    qDebug() << "DHT11 interval wet data: " << interval_wet_data << endl;
}

void MainWindow::on_pushButton_clearTmp_clicked()
{
    clear_data_tmp();

    QByteArray clear_tmp_message = "Clear tmp graph ";      // 맨 뒤 한 글자가 꼭 잘려서 한 칸을 띄워준다.
    socket->writeDatagram(clear_tmp_message, QHostAddress("10.10.15.79"), 9999);

    qDebug() << clear_tmp_message << " " << endl;
}

void MainWindow::on_pushButton_clearWet_clicked()
{
    clear_data_wet();

    QByteArray clear_wet_message = "Clear wet graph ";
    socket->writeDatagram(clear_wet_message, QHostAddress("10.10.15.79"), 9999);

    qDebug() << clear_wet_message << endl;
}
