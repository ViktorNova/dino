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

    def addSequenceEntry(self, track, beat, pattern, length):
        self.AddSequenceEntry(track, beat, pattern, length)

    def removeSequenceEntry(self, track, beat):
        self.RemoveSequenceEntry(track, beat)

    def setSequenceEntryLength(self, track, beat, length):
        self.SetSequenceEntryLength(track, beat, length)
    
    def setTrackMidiChannel(self, track, channel):
        self.SetTrackMidiChannel(track, channel)

    def addController(self, track, number, name, min, max, default, isGlobal):
        self.AddController(track, number, name, min, max, default, isGlobal)
    
    def removeController(self, track, number):
        self.RemoveController(track, number)
    
    def setControllerName(self, track, number, name):
        self.SetControllerName(track, number, name)
    
    def setControllerMin(self, track, number, min):
        self.SetControllerMin(track, number, min)
    
    def setControllerMax(self, track, number, max):
        self.SetControllerMax(track, number, max)
    
    def setControllerDefault(self, track, number, default):
        self.SetControllerDefault(track, number, default)
    
    def setControllerNumber(self, track, number, newNumber):
        self.SetControllerNumber(track, number, newNumber)
    
    def setControllerGlobal(self, track, number, isGlobal):
        self.SetControllerGlobal(track, number, isGlobal)
    
    def setPatternName(self, track, pattern, name):
        self.SetPatternName(track, pattern, name)
    
    def setPatternLength(self, track, pattern, beats):
        self.SetSatternLength(iii)
    
    def setPatternSteps(self, track, pattern, steps):
        self.SetPatternSteps(iii)
    
    def addNote(self, track, pattern, step, key, velocity, length):
        self.AddNote(track, pattern, step, key, velocity, length)
    
    def setNoteVelocity(self, track, pattern, step, key, velocity):
        self.SetNoteVelocity(track, pattern, step, key, velocity)
    
    def setNoteSize(self, track, pattern, step, key, length):
        self.SetNoteSize(track, pattern, step, key, length)
    
    def deleteNote(self, track, pattern, step, key):
        self.DeleteNote(track, pattern, step, key)
    
    def addCurvePoint(self, track, pattern, number, step, value):
        self.AddCurvePoint(track, pattern, number, step, value)
    
    def removeCurvePoint(self, track, pattern, number, step):
        self.RemoveCurvePoint(track, pattern, number, step)
    



def connect(busName):
    global bus, song, seq
    bus = dbus.SessionBus()
    dinoObject = bus.get_object(busName, '/')
    seq = SequencerInterface(dinoObject, 'org.nongnu.dino.Sequencer')
    song = SongInterface(dinoObject, 'org.nongnu.dino.Song')

