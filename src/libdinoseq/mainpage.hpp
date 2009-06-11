/*****************************************************************************
    libdinoseq - a library for MIDI sequencing
    Copyright (C) 2009  Lars Luthman <lars.luthman@gmail.com>
    
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.
    
    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*****************************************************************************/

/** @file
    This file contains no code, just the Doxygen comments for the documentation
    main page. */

/** @mainpage libdinoseq API documentation
    This document describes the API that programmers should use to interface
    with the libdinoseq library in order to create, edit, store and sequence
    MIDI songs and related data. The API is in C++ and makes use of features
    in the C++1x revision so you need a compiler that supports those features.
    The latest version of GCC is usually a good choice.
    
    Most of the classes in libdinoseq deal with one of the following 4 
    subjects:
    
    @li MIDI data structures
    @li instrument abstractions
    @li a sequencer engine
    @li an undo-capable command system
    
    There are also some utility classes and functions that don't fit in either
    of those 4 categories, but most do.
    
    The MIDI data structures consist of an abstract base class, Sequencable, 
    that specifies the interface that the sequencer uses to play a chunk of
    MIDI data. This interface is then implemented by the subclasses of 
    Sequencable; Curve, Pattern and Track. There is also the Dino::Song 
    class which isn't strictly a Dino::Sequencable but acts as a container
    for a number of Tracks as well as Arrangements, which each have their 
    own TempoMap. 
    
    The instrument abstraction part consists of the InstrumentManager class,
    which, as the name suggests, manages the instruments, and the abstract
    base class Instrument that represents an instrument that is available to
    the sequencer. Instrument has the subclass JackMIDIInput which represents
    a MIDI input port that exists in the JACK graph.
    
    The sequencer engine is the class Sequencer which handles the actual 
    sequencing of MIDI data from Sequencable objects to Instrument objects.
    Sequencables can be added and removed to the sequencer and associated with
    zero or more Instrument objects. The sequencer is written to be realtime
    safe as long as the associated functions in all Sequencable and Instrument
    objects are, but can also be used for offline rendering of MIDI data. There
    is a subclass of Sequencer called SongSequencer that is used to sequence
    complete songs instead of just collections of Sequencable objects.
    
    The command system consists of the abstract base class Command and its
    subclasses that perform different tasks such as adding or removing MIDI
    data, and the class CommandProxy where a stack of executed commands are
    stored so they can be undone. This part is optional, if you want to you
    can access the other data structures directly and ignore the command system
    altogether.
    
    You can of course have your own classes inherit the abstract base classes
    in the library in order to add new undoable commands, new Sequencable types
    that for example generate MIDI data on the fly, or new Instrument types
    to handle instrument plugins or other types of MIDI-driven instruments.
 
*/
