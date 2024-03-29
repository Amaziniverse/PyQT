#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QUdpSocket>   // ADD_Amazin
#include <QTextStream>  // ADD_Amazin

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void add_point_tmp(double x, double y);   // ADD_Amazin x, y 좌표
    void add_point_wet(double x, double y);   // ADD_Amazin x, y 좌표
    void clear_data_tmp();      // ADD_Amazin tmp 데이터 초기화
    void clear_data_wet();      // ADD_Amazin wet 데이터 초기와
    void plot_tmp();       // ADd_Amazin 표 그리기
    void plot_wet();       // ADD_Amazin 표 그리기
public slots:   // call back function pulic slots --> QT에서 정의된 기능
    void readyRead();

private slots:
    void on_pushButtonSend_clicked();

    void on_checkBoxLED1_stateChanged(int arg1);

    void on_checkBoxLED2_stateChanged(int arg1);

    void on_checkBoxLED3_stateChanged(int arg1);

    void on_dial_servo_valueChanged(int value);

    void on_dial_led_valueChanged(int value);

    void on_horizontalSlider_DHT11_tmp_valueChanged(int value);

    void on_horizontalSlider_DHT11_wet_valueChanged(int value);

    void on_pushButton_clearTmp_clicked();

    void on_pushButton_clearWet_clicked();

private:
    Ui::MainWindow *ui;
    QUdpSocket *socket = nullptr;     // ADD_Amazin 소켓을 할당받고 소켓의 주소를 저장하는 변수
    QVector <double> qv_x, qv_y;    // ADD_Amazin
    QVector <double> qw_x, qw_y;    // ADD_Amazin
    QString temp;   // ADD_Amazin
};

#endif // MAINWINDOW_H
