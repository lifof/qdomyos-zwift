#ifndef KINGSMITHR2TREADMILL_H
#define KINGSMITHR2TREADMILL_H

#include <QBluetoothDeviceDiscoveryAgent>
#include <QtBluetooth/qlowenergyadvertisingdata.h>
#include <QtBluetooth/qlowenergyadvertisingparameters.h>
#include <QtBluetooth/qlowenergycharacteristic.h>
#include <QtBluetooth/qlowenergycharacteristicdata.h>

#include <QtBluetooth/qlowenergycontroller.h>
#include <QtBluetooth/qlowenergydescriptordata.h>
#include <QtBluetooth/qlowenergyservice.h>
#include <QtBluetooth/qlowenergyservicedata.h>

#include <QtCore/qbytearray.h>

#ifndef Q_OS_ANDROID
#include <QtCore/qcoreapplication.h>
#else
#include <QtGui/qguiapplication.h>
#endif
#include <QtCore/qlist.h>
#include <QtCore/qmutex.h>
#include <QtCore/qscopedpointer.h>
#include <QtCore/qtimer.h>

#include <QDateTime>
#include <QObject>
#include <QMap>

#include "treadmill.h"
#include "virtualbike.h"
#include "virtualtreadmill.h"

#ifdef Q_OS_IOS
#include "ios/lockscreen.h"
#endif

class kingsmithr2treadmill : public treadmill {

    Q_OBJECT
  public:
    kingsmithr2treadmill(uint32_t poolDeviceTime = 200, bool noConsole = false, bool noHeartService = false,
                            double forceInitSpeed = 0.0, double forceInitInclination = 0.0);
    bool connected();
    bool changeFanSpeed(uint8_t speed);
    double odometer();

    void setLastSpeed(double speed);
    void setLastInclination(double inclination);

    void *VirtualTreadMill();
    void *VirtualDevice();

  private:
    const QByteArray PLAINTEXT_TABLE = QStringLiteral("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=").toUtf8();
    const QByteArray ENCRYPT_TABLE =   QStringLiteral("SaCw4FGHIJqLhN+P9RVTU/WcY6ObDdefgEijklmnopQrsBuvMxXz1yA2t5078KZ3=").toUtf8();

    bool sendChangeFanSpeed(uint8_t speed);
    double GetInclinationFromPacket(const QByteArray &packet);
    double GetKcalFromPacket(const QByteArray &packet);
    double GetDistanceFromPacket(const QByteArray &packet);
    void forceSpeedOrIncline(double requestSpeed, double requestIncline);
    void updateDisplay(uint16_t elapsed);
    void btinit(bool startTape);
    void writeCharacteristic(const QString &data, const QString &info, bool disable_log = false,
                             bool wait_for_response = false);
    void startDiscover();
    double DistanceCalculated = 0;
    bool noConsole = false;
    bool noHeartService = false;
    uint32_t pollDeviceTime = 200;
    bool searchStopped = false;
    double lastSpeed = 0.0;
    double lastInclination = 0;
    uint8_t sec1Update = 0;
    uint8_t firstInit = 0;
    QMap<QString, double> props;
    QByteArray buffer;
    QByteArray lastValue;
    QDateTime lastTimeCharacteristicChanged;
    bool firstCharacteristicChanged = true;

    QTimer *refresh;
    virtualtreadmill *virtualTreadMill = nullptr;
    virtualbike *virtualBike = 0;

    QLowEnergyService *gattCommunicationChannelService = nullptr;
    QLowEnergyCharacteristic gattWriteCharacteristic;
    QLowEnergyCharacteristic gattNotifyCharacteristic;

    bool initDone = false;
    bool initRequest = false;

#ifdef Q_OS_IOS
    lockscreen *h = 0;
#endif

  Q_SIGNALS:
    void disconnected();
    void debug(QString string);
    void speedChanged(double speed);
    void packetReceived();

  public slots:
    void deviceDiscovered(const QBluetoothDeviceInfo &device);
    void searchingStop();

  private slots:

    void characteristicChanged(const QLowEnergyCharacteristic &characteristic, const QByteArray &newValue);
    void characteristicWritten(const QLowEnergyCharacteristic &characteristic, const QByteArray &newValue);
    void descriptorWritten(const QLowEnergyDescriptor &descriptor, const QByteArray &newValue);
    void stateChanged(QLowEnergyService::ServiceState state);
    void controllerStateChanged(QLowEnergyController::ControllerState state);
    void changeInclinationRequested(double grade, double percentage);

    void serviceDiscovered(const QBluetoothUuid &gatt);
    void serviceScanDone(void);
    void update();
    void error(QLowEnergyController::Error err);
    void errorService(QLowEnergyService::ServiceError);
};

#endif // KINGSMITHR2TREADMILL_H
