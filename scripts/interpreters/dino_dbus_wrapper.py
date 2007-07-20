import dbus


bus = None
song = None
seq = None


class SequencerInterface(dbus.Interface):
    def __init__(self, object, interface):
        dbus.Interface.__init__(self, object, interface)
    
    def play(self):
        self.Play()
    
    def stop(self):
        self.Stop()

    def goToBeat(self, beat):
        self.GoToBeat(beat)


class SongInterface(dbus.Interface):
    def __init__(self, object, interface):
        dbus.Interface.__init__(self, object, interface)

    def addTrack(self, name):
        self.AddTrack(name)

    def removeTrack(self, id):
        self.RemoveTrack(id)

    def setSongTitle(self, title):
        self.SetSongTitle(title)

    def setSongAuthor(self, author):
        self.SetSongAuthor(author)

    def setSongInfo(self, info):
        self.SetSongInfo(info)

    def setLoopStart(self, beat):
        self.SetLoopStart(beat)

    def setLoopEnd(self, beat):
        self.SetLoopEnd(beat)

    def addTempoChange(self, beat, bpm):
        self.AddTempoChange(beat, bpm)

    def removeTempoChange(self, beat):
        self.RemoveTempoChange(beat)


def connect(busName):
    global bus, song, seq
    bus = dbus.SessionBus()
    dinoObject = bus.get_object(busName, '/')
    seq = SequencerInterface(dinoObject, 'org.nongnu.dino.Sequencer')
    song = SongInterface(dinoObject, 'org.nongnu.dino.Song')

