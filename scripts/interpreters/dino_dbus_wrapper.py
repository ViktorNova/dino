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
    
    def setTitle(self, title):
        self.SetTitle(title)
    
    def setAuthor(self, author):
        self.SetAuthor(author)
    
    def setInfo(self, info):
        self.SetInfo(info)
    
    def setLength(self, length):
        self.SetLength(length)
    
    def setLoopStart(self, beat):
        self.SetLoopStart(beat)
    
    def setLoopEnd(self, beat):
        self.SetLoopEnd(beat)
    
    def addTempoChange(self, beat, bpm):
        self.AddTempoChange(beat, bpm)
    
    def removeTempoChange(self, beat):
        self.RemoveTempoChange(beat)

    
    def setTrackName(self, track, name):
        self.SetTrackName(track, name)

    def addPattern(self, track, name, length, steps):
        self.AddPattern(track, name, length, steps)

    def duplicatePattern(self, track, pattern):
        self.DuplicatePattern(track, pattern)

    def removePattern(self, track, pattern):
        self.RemovePattern(track, pattern)

    def removeSequenceEntry(self, track, beat):
        self.RemoveSequenceEntry(track, beat)

    def setSequenceEntryLength(self, track, beat, length):
        self.SetSequenceEntryLength(track, beat, length)



def connect(busName):
    global bus, song, seq
    bus = dbus.SessionBus()
    dinoObject = bus.get_object(busName, '/')
    seq = SequencerInterface(dinoObject, 'org.nongnu.dino.Sequencer')
    song = SongInterface(dinoObject, 'org.nongnu.dino.Song')

