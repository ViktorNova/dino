#!/usr/bin/python

import sys
import pygtk
pygtk.require('2.0')
import gtk
import dino
import gc
from weakref import ref


class ToggleInfo:
    def __init__(self, row, col):
        self.row = row
        self.col = col


class ToySequencer:

    # this is called when the user hits "Play"
    def on_play(self, arg):
        self.sequencer.play()

    # this is called when the user hits "Stop"
    def on_stop(self, arg):
        self.sequencer.stop()

    # this is called when the user hits "Back"
    def on_back(self, arg):
        self.sequencer.go_to_beat(0)

    # this is called when the user changes the BPM
    def on_bpm_change(self, arg):
        self.song.add_tempo_change(0, arg.get_value())

    # this is called when the user selects instrument
    def on_select_instrument(self, arg):
        self.sequencer.set_instrument(1, self.instcombo.get_active_text())
        
    # this is called when the user toggles one of the buttons
    # remove or add notes here
    def on_toggle(self, info):
        button = self.toggles[info.row][info.col]
        key = int(self.spinbuttons[info.row].get_value())
        color1 = gtk.gdk.Color(50000, 30000, 30000)
        if ((info.col / self.steps) % 2 == 0):
            color2 = gtk.gdk.Color(60000, 60000, 65000)
        else:
            color2 = gtk.gdk.Color(60000, 65000, 60000)
        if button.get_active():
            self.pattern.add_note(info.col, key, 64, 1)
            self.toggles[info.row][info.col].modify_bg(gtk.STATE_NORMAL, color1)
        else:
            self.pattern.delete_note(info.col, key)
            self.toggles[info.row][info.col].modify_bg(gtk.STATE_NORMAL, color2)

    # this is called when the user changes one of the spinbuttons
    # it removed all notes and re-adds them with the correct keys
    def on_change_key(self, row):
        for n in self.pattern.notes:
            self.pattern.delete_note(n.step, n.key)
        for row in range(0, self.rows):
            key = int(self.spinbuttons[row].get_value())
            for col in range(0, self.beats * self.steps):
                if self.toggles[row][col].get_active():
                    self.pattern.add_note(col, key, 64, 1)

    # the constructor
    def __init__(self, rows, beats, steps = None):

        # initialise the song structures
        if steps == None:
            self.rows = rows
            self.song = dino.Song()
            self.song.load_file(beats)
            track = self.song.find_track(1)
            self.pattern = track.find_pattern(1)
            self.beats = self.pattern.length
            self.steps = self.pattern.steps
        else:
            self.rows = rows
            self.beats = beats
            self.steps = steps
            self.song = dino.Song()
            self.song.length = beats
            self.song.add_tempo_change(0, 100)
            track = self.song.add_track("Single track")
            pat = self.pattern = track.add_pattern("Single pattern",
                                                   beats, steps)
            track.set_sequence_entry(0, pat.id, beats)

        # initialise the sequencer
        self.sequencer = dino.Sequencer("Toy sequencer", self.song)
        # XXX without ref() this creates a reference cycle that the GC doesn't
        # detect - bad!
        self.sequencer.signal_instruments_changed(ref(self.update_combo))

        # create the window
        self.window = gtk.Window(gtk.WINDOW_TOPLEVEL)
        self.window.set_title("Toy sequencer")
        self.window.connect("destroy", gtk.main_quit)
        self.window.set_border_width(3)

        # add the controls
        vbox = gtk.VBox()
        vbox.set_spacing(10)
        hbox = gtk.HBox()
        hbox.set_spacing(5)
        playbtn = gtk.Button("Play", gtk.STOCK_MEDIA_PLAY)
        stopbtn = gtk.Button("Stop", gtk.STOCK_MEDIA_STOP)
        backbtn = gtk.Button("Back", gtk.STOCK_MEDIA_PREVIOUS)
        playbtn.connect("clicked", self.on_play)
        stopbtn.connect("clicked", self.on_stop)
        backbtn.connect("clicked", self.on_back)
        for b in self.song.tempochanges:
            bpm = b.bpm
            break
        bpmctrl = gtk.SpinButton(gtk.Adjustment(bpm, 0, 200, 1))
        bpmctrl.connect("value_changed", self.on_bpm_change)
        self.instcombo = gtk.combo_box_new_text()
        self.update_combo()
        self.instcombo.set_active(0)
        self.instcombo.connect("changed", self.on_select_instrument)
        hbox.pack_start(playbtn, False)
        hbox.pack_start(stopbtn, False)
        hbox.pack_start(backbtn, False)
        hbox.pack_end(bpmctrl, False)
        hbox.pack_end(gtk.Label('BPM:'), False)
        hbox.pack_end(self.instcombo, False)
        vbox.pack_start(hbox, False)
        
        # create the toggle buttons and key selectors
        table = gtk.Table(rows, self.beats * self.steps + 2)
        table.set_col_spacing(0, 0)
        self.toggles = []
        for row in range(0, rows):
            coltoggles = []
            for col in range(0, self.beats * self.steps):
                button = gtk.ToggleButton("")
                coltoggles += [button];
                button.set_size_request(20, 20)
                button.connect_object("toggled", self.on_toggle,
                                      ToggleInfo(row, col))
                if ((col / self.steps) % 2 == 0):
                    button.modify_bg(gtk.STATE_NORMAL,
                                     gtk.gdk.Color(60000, 60000, 65000))
                else:
                    button.modify_bg(gtk.STATE_NORMAL,
                                     gtk.gdk.Color(60000, 65000, 60000))
                table.attach(button, col + 2, col + 3, row, row + 1)
            self.toggles += [coltoggles]
        self.spinbuttons = []
        for row in range(0, self.rows):
            table.attach(gtk.Label("Key: "), 0, 1, row, row + 1, gtk.FILL)
            spinbutton = gtk.SpinButton(gtk.Adjustment(60 + row, 0, 127, 1))
            spinbutton.connect_object("value_changed", self.on_change_key, row)
            table.attach(spinbutton, 1, 2, row, row + 1, gtk.FILL)
            self.spinbuttons += [spinbutton]
        for n in self.pattern.notes:
            self.toggles[n.key - 60][n.step].set_active(True)

        # show everything
        vbox.pack_start(table)
        self.window.add(vbox)
        self.window.show_all()

    # this is called when a new writable JACK MIDI input port appears
    def update_combo(self):
        self.instcombo.get_model().clear()
        self.instcombo.append_text("No instrument")
        for ii in self.sequencer.instruments:
            self.instcombo.append_text(ii.name)

    # save state to file
    def save(self, filename):
        self.song.write_file(filename)

    # run!
    def main(self):
        gtk.main()


def main():
    if len(sys.argv) > 1:
        toyseq = ToySequencer(4, sys.argv[1])
    else:
        toyseq = ToySequencer(4, 8, 4)
    toyseq.main()
    if len(sys.argv) > 1:
        toyseq.save(sys.argv[1])
    else:
        toyseq.save("toyseq.song")

if __name__ == "__main__":
    main()
    gc.collect()
    
