#ifndef CDRUMKIT_H
#define CDRUMKIT_H

#include <QStringList>
#include <RtMidi.h>

class CDrumKit {
public:
    static CDrumKit * getInstance(void);

    void init(int portNumber);
    void playNote(char note);
    QStringList getMidiPorts(void);
private:
    RtMidiOut *midiOut;

    CDrumKit(void);


};

#endif // CDRUMKIT_H
