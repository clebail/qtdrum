#include "CDrumKit.h"

static CDrumKit *instance = nullptr;

CDrumKit * CDrumKit::getInstance(void) {
    if(instance == nullptr) {
        instance = new CDrumKit();
    }

    return instance;
}

void CDrumKit::init(int portNumber) {
    std::vector<unsigned char> message;

    if(midiOut->isPortOpen()) {
        midiOut->closePort();
    }
    midiOut->openPort(static_cast<unsigned>(portNumber));

    message.push_back(192);
    message.push_back(5);
    midiOut->sendMessage(&message);

    message[0] = 176;
    message[1] = 7;
    message.push_back(127);
    midiOut->sendMessage(&message);
}

void CDrumKit::playNote(char note) {
    std::vector<unsigned char> message;

    message.push_back(153);
    message.push_back(static_cast<unsigned char>(note));
    message.push_back(127);
    midiOut->sendMessage(&message);
}

QStringList CDrumKit::getMidiPorts(void) {
    QStringList list;
    int portCount = static_cast<int>(midiOut->getPortCount());

    for(int i=0;i<portCount;i++) {
        list << QString::fromStdString(midiOut->getPortName(static_cast<unsigned>(i)));
    }

    return list;
}

CDrumKit::CDrumKit(void) {
    midiOut = new RtMidiOut();
}
