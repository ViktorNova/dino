#!/usr/bin/python

import pygtk
pygtk.require('2.0')
import gtk
import gobject
import dino


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
    
    
    def __init__(self, rows, beats, steps):

        # initialise the sequencer and song structures
        self.rows = rows
        self.beats = beats
        self.steps = steps
        self.song = dino.Song()
        self.song.length = beats
        self.song.add_tempo_change(0, 100)
        track = self.song.add_track("Single track")
        pat = self.pattern = track.add_pattern("Single pattern", beats, steps)
        track.set_sequence_entry(0, pat.id, beats)
        
        # create the window
        self.window = gtk.Window(gtk.WINDOW_TOPLEVEL)
        self.window.set_title("Toy sequencer")
        self.window.connect("destroy", gtk.main_quit)
        self.window.set_border_width(3)

        # add the transport controls and the BPM control
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
        bpmctrl = gtk.SpinButton(gtk.Adjustment(100, 0, 200, 1))
        bpmctrl.connect("value_changed", self.on_bpm_change)
        self.instcombo = gtk.combo_box_new_text()
        self.instcombo.connect("changed", self.on_select_instrument)
        
        hbox.pack_start(playbtn, False)
        hbox.pack_start(stopbtn, False)
        hbox.pack_start(backbtn, False)
        hbox.pack_end(bpmctrl, False)
        hbox.pack_end(gtk.Label('BPM:'), False)
        hbox.pack_end(self.instcombo, False)
        vbox.pack_start(hbox, False)
        
        # create the toggle buttons and key selectors
        table = gtk.Table(rows, beats * steps + 2)
        table.set_col_spacing(0, 0)
        self.toggles = []
        for row in range(0, rows):
            coltoggles = []
            for col in range(0, beats * steps):
                button = gtk.ToggleButton("")
                coltoggles += [button];
                button.set_size_request(20, 20)
                button.connect_object("toggled", self.on_toggle,
                                      ToggleInfo(row, col))
                if ((col / steps) % 2 == 0):
                    button.modify_bg(gtk.STATE_NORMAL,
                                     gtk.gdk.Color(60000, 60000, 65000))
                else:
                    button.modify_bg(gtk.STATE_NORMAL,
                                     gtk.gdk.Color(60000, 65000, 60000))
                table.attach(button, col + 2, col + 3, row, row + 1)
            self.toggles += [coltoggles]
        self.spinbuttons = []
        for row in range(0, rows):
            table.attach(gtk.Label("Key: "), 0, 1, row, row + 1, gtk.FILL)
            spinbutton = gtk.SpinButton(gtk.Adjustment(60 + row, 0, 127, 1))
            spinbutton.connect_object("value_changed", self.on_change_key, row)
            table.attach(spinbutton, 1, 2, row, row + 1, gtk.FILL)
            self.spinbuttons += [spinbutton]

        # show everything
        vbox.pack_start(table)
        self.window.add(vbox)
        self.window.show_all()

    def update_combo(self):
        self.instcombo.get_model().clear()
        self.instcombo.append_text("No instruments")
        for ii in self.sequencer.instruments:
            self.instcombo.append_text(ii.name)
        
    def load(self, filename):
        self.song.load_file(filename)
        track = self.song.find_track(1)
        self.pattern = track.find_pattern(1)
        for n in self.pattern.notes:
            self.toggles[n.key - 60][n.step].set_active(True)
        self.sequencer = dino.Sequencer("Toy sequencer", self.song)
        self.sequencer.signal_instruments_changed(self.update_combo)
        self.update_combo()
        
    def save(self, filename):
        self.song.write_file(filename)

    def main(self):
        gtk.main()


if __name__ == "__main__":
    toyseq = ToySequencer(4, 8, 4)
    toyseq.load("toyseq.song")
    toyseq.main()
    toyseq.save("toyseq.song")
