#include <iostream>

#include "patterneditor.hpp"


PatternEditor::PatternEditor() : rowHeight(8), colWidth(8), maxNote(128), 
				 pat(NULL) {
  colormap  = Colormap::get_system();
  bgColor.set_rgb(65535, 65535, 65535);
  bgColor2.set_rgb(60000, 60000, 65535);
  fgColor.set_rgb(0, 0, 65535);
  gridColor.set_rgb(40000, 40000, 40000);
  edgeColor.set_rgb(0, 0, 0);
  hlColor.set_rgb(65535, 0, 0);
  colormap->alloc_color(bgColor);
  colormap->alloc_color(bgColor2);
  colormap->alloc_color(fgColor);
  colormap->alloc_color(gridColor);
  colormap->alloc_color(edgeColor);
  colormap->alloc_color(hlColor);
  
  add_events(BUTTON_PRESS_MASK | BUTTON_RELEASE_MASK | BUTTON_MOTION_MASK);
  
  addedNote = make_pair(-1, -1);
  dragStep = -1;
}


void PatternEditor::setPattern(Pattern* pat) {
  if (this->pat != pat) {
    this->pat = pat;
    if (pat) {
      set_size_request(pat->getLength() * pat->getSteps() * colWidth + 1,
		       maxNote * rowHeight + 1);
    }
    else
      set_size_request();
  }
}


bool PatternEditor::on_button_press_event(GdkEventButton* event) {
  if (!pat)
    return false;
  
  if (event->x < pat->getLength() * pat->getSteps() * colWidth &&
      event->y < maxNote * rowHeight) {
    int note = maxNote - int(event->y) / rowHeight - 1;
    int step = int(event->x) / colWidth;
    if (event->button == 1) {
      addedNote = make_pair(step, note);
      pat->addNote(addedNote.first, addedNote.second, 1);
      update();
    }
    else if (event->button == 2) {
      int start = pat->deleteNote(step, note);
      if (start != -1) {
	addedNote = make_pair(start, note);
	pat->addNote(addedNote.first, addedNote.second, step - start + 1);
	update();
      }
    }
    else if (event->button == 3) {
      pat->deleteNote(int(event->x) / colWidth, 
		      maxNote - int(event->y) / rowHeight - 1);
      update();
    }
    dragStep = step;
    dragNote = note;
  }
  return true;
}


bool PatternEditor::on_button_release_event(GdkEventButton* event) {
  if (!pat)
    return false;

  if (addedNote.first != -1 && event->button == 1) {
    int step = int(event->x) / colWidth;
    if (step < addedNote.first)
      step = addedNote.first;
    else if (step >= pat->getLength() * pat->getSteps())
      step = pat->getLength() * pat->getSteps() - 1;
    pat->addNote(addedNote.first, addedNote.second, step - addedNote.first +1);
    addedNote = make_pair(-1, -1);
    update();
  }
  return true;
}


bool PatternEditor::on_motion_notify_event(GdkEventMotion* event) {
  if (!pat)
    return false;

  if (addedNote.first != -1 && 
      (event->state & (GDK_BUTTON1_MASK | GDK_BUTTON2_MASK))) {
    int step = int(event->x) / colWidth;
    int note = maxNote - int(event->y) / rowHeight - 1;
    if (step == dragStep && note == dragNote)
      return true;
    if (step < addedNote.first)
      step = addedNote.first;
    else if (step >= pat->getLength() * pat->getSteps())
      step = pat->getLength() * pat->getSteps() - 1;
    pat->addNote(addedNote.first, addedNote.second, step - addedNote.first +1);
    update();
    dragStep = step;
    dragNote = note;
  }
  else if (event->state & GDK_BUTTON3_MASK) {
    int step = int(event->x) / colWidth;
    int note = maxNote - int(event->y) / rowHeight - 1;
    if (step == dragStep && note == dragNote)
      return true;
    if (step >= 0 && step < pat->getLength() * pat->getSteps() &&
	note >= 0 && note < maxNote) {
      pat->deleteNote(step, note);
      update();
      dragStep = step;
      dragNote = note;
    }
  }

  return true;
}


void PatternEditor::on_realize() {
  DrawingArea::on_realize();
  RefPtr<Gdk::Window> win = get_window();
  gc = GC::create(win);
  gc->set_background(bgColor);
  gc->set_foreground(fgColor);
  win->clear();
}


bool PatternEditor::on_expose_event(GdkEventExpose* event) {
  RefPtr<Gdk::Window> win = get_window();
  win->clear();
  if (!pat)
    return true;
  
  int width = pat->getLength() * pat->getSteps() * colWidth;
  int height = maxNote * rowHeight;
  for (int b = 0; b < pat->getLength(); ++b) {
    if (b % 2 == 0)
      gc->set_foreground(bgColor);
    else
      gc->set_foreground(bgColor2);
    win->draw_rectangle(gc, true, b * pat->getSteps() * colWidth, 0, 
			pat->getSteps() * colWidth, height);
  }
  gc->set_foreground(gridColor);
  for (int r = -1; r < maxNote; ++r) {
    win->draw_line(gc, 0, (maxNote - r - 1) * rowHeight, 
		   width, (maxNote - r - 1) * rowHeight);
    if (r % 12 == 1 || r % 12 == 3 || 
	r % 12 == 6 || r % 12 == 8 || r % 12 == 10) {
	win->draw_rectangle(gc, true, 0, (maxNote - r - 1) * rowHeight,
			    width, rowHeight);
    }
  }
  for (int c = 0; c < pat->getSteps() * pat->getLength() + 1; ++c) {
    win->draw_line(gc, c * colWidth, 0, c * colWidth, height);
  }
  
  const Pattern::NoteMap& notes(pat->getNotes());
  Pattern::NoteMap::const_iterator iter;
  for (iter = notes.begin(); iter != notes.end(); ++iter) {
    gc->set_foreground(fgColor);
    win->draw_rectangle(gc, true, iter->first.first * colWidth + 1, 
			(maxNote - iter->first.second - 1) * rowHeight + 1, 
			iter->second->length * colWidth - 1, rowHeight - 1);
    gc->set_foreground(edgeColor);
    win->draw_rectangle(gc, false, iter->first.first * colWidth, 
			(maxNote - iter->first.second - 1) * rowHeight, 
			iter->second->length * colWidth, rowHeight);
  }
  
  /*
    gc->set_foreground(hlColor);
    win->draw_rectangle(gc, false, event->area.x, event->area.y,
    event->area.width - 1, event->area.height - 1);
  */
  
  return true;
}


void PatternEditor::update() {
  pat->getDirtyRect(&dMinStep, &dMinNote, &dMaxStep, &dMaxNote);
  RefPtr<Gdk::Window> win = get_window();
  int width = pat->getLength() * pat->getSteps() * colWidth+ 1;
  int height = maxNote * rowHeight + 1;
  win->invalidate_rect(Rectangle(dMinStep * colWidth, 
				 (maxNote - dMaxNote - 1) * rowHeight, 
				 (dMaxStep - dMinStep + 1) * colWidth + 1,
				 (dMaxNote - dMinNote + 1) * rowHeight + 1),
		       false);
  win->process_updates(false);
}
