#include "devices.h"
#include "ui_devices.h"

Devices::Devices(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Devices)
{
    ui->setupUi(this);
    activityLog = new ActivityLog(this);
    scanThread = new QThread;
    ScanWorker *worker = new ScanWorker();
    worker->moveToThread(scanThread);

    connect(scanThread, SIGNAL(started()), worker, SLOT(start()));
    connect(worker, SIGNAL(newDeviceFound(QString,QString)), this, SLOT(addToList(QString,QString)));

    connect(ui->devices, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(setMac1(QListWidgetItem*)));
    connect(this, SIGNAL(log(QString)), activityLog, SLOT(addActivity(QString)));

    connect(ui->connectButton, SIGNAL(clicked(bool)), this, SLOT(connectButtonClicked(bool)));

    activityLog->show();
    scanThread->start();
}

void Devices::addToList(const QString &addr, const QString &name){
    ui->devices->addItem(addr + " : " + name);
    std::cout << addr.toStdString() << " : " << name.toStdString() << std::endl;
}

void Devices::setMac1(QListWidgetItem *addr){
    ui->mac1->setText(addr->text().split(" ")[0]);
    emit log(QString("mac1 set to: " + addr->text()));
}

void Devices::connectButtonClicked(bool checked){
    bl1 = new RFComm();
    bl2 = new RFComm();
    connect(bl1, SIGNAL(connected()), this, SLOT(bl1Connected()));
    connect(bl1, SIGNAL(cleaned(QString)), this, SLOT(bl1Cleaned(QString)));
    connect(bl1, SIGNAL(failed(int)), this, SLOT(bl1Failed(int)));
    connect(this, SIGNAL(bl1Connect(QString, QString)), bl1, SLOT(conn(QString,QString)));
    emit bl1Connect("./rf1", ui->mac1->text());
}

void Devices::bl1Connected(){
    std::cout << "bl1 connected" << std::endl;
    emit log("bl1 connected");
}

void Devices::bl1Failed(int code){
    std::cout << "bl1 failed: " << code << std::endl;
    emit log("bl1 failed: " + code);
}

void Devices::bl1Cleaned(const QString &out){
    std::cout << "bl1 cleaned: " << out.toStdString() << std::endl;
    emit log("bl1 cleaned");
}

Devices::~Devices()
{
    delete ui;
}
