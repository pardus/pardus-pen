#ifdef DBUS
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusReply>

#include <iostream>

#define SERVICE_NAME "org.eta.virtualkeyboard"

void showEtaKeyboard(){
    auto connection = QDBusConnection::sessionBus();

    if (!connection.isConnected()) {
        std::cout << "Failed to connect dbus session bus" << std::endl;
        return;
    }

    QDBusInterface iface(SERVICE_NAME, "/VirtualKeyboard", SERVICE_NAME);
    if (iface.isValid()) {
        iface.call("show", false);
    } else {
        std::cout << "Invalid dbus interface"  << std::endl;
    }
}
#else
void showEtaKeyboard(){}
#endif
