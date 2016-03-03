#include "CDrumKit.h"

static CDrumKit *instance = 0;

CDrumKit * CDrumKit::getInstance(void) {
    if(instance == 0) {
        instance = new CDrumKit();
    }

    return instance;
}

void CDrumKit::init(int portNumber) {
    std::vector<unsigned char> message;

    if(midiOut->isPortOpen()) {
        midiOut->closePort();
    }
    midiOut->openPort(portNumber);

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
    message.push_back(note);
    message.push_back(127);
    midiOut->sendMessage(&message);
}

QStringList CDrumKit::getMidiPorts(void) {
    QStringList list;
    int portCount = midiOut->getPortCount();

    for(int i=0;i<portCount;i++) {
        list << QString::fromStdString(midiOut->getPortName(i));
    }

    return list;
}

CDrumKit::CDrumKit(void) {
    midiOut = new RtMidiOut();
}
