#include <cassert>
#include <cmath>
#include <iostream>

#include "cceditor.hpp"
#include "pattern.hpp"


CCEditor::CCEditor() : height(64 + 3), colWidth(8), pat(NULL), lineStep(-1),
		       vScale((height - 3) / 128.0) {
  colormap  = Colormap::get_system();
  bgColor.set_rgb(65535, 65535, 65535);
  bgColor2.set_rgb(60000, 60000, 65535);
  edgeColor.set_rgb(0, 0, 0);
  fgColor.set_rgb(0, 40000, 0);
  colormap->alloc_color(bgColor);
  colormap->alloc_color(bgColor2);
  colormap->alloc_color(fgColor);
  colormap->alloc_color(edgeColor);
  
  add_events(BUTTON_PRESS_MASK | BUTTON_RELEASE_MASK | BUTTON_MOTION_MASK);
  setHeight(64);
}


void CCEditor::setPattern(Pattern* pat) {
  if (this->pat != pat) {
    this->pat = pat;
    if (pat) {
      width = colWidth * pat->getLength() * pat->getSteps();
      set_size_request(pat->getLength() * pat->getSteps() * colWidth + 1,
		       height);
    }
    else
      set_size_request(-1, height);
    update();
  }
}


void CCEditor::setCCNumber(int ccNumber) {
  assert(ccNumber >= 0 && ccNumber < 128);
  this->ccNumber = ccNumber;
  update();
}


void CCEditor::setHeight(int height) {
  assert(height > 0);
  this->height = height + 3;
  vScale = height / 128.0;
  if (pat)
    set_size_request(pat->getLength() * pat->getSteps() * colWidth + 1,
		     this->height);
  else
    set_size_request(-1, this->height);
  
  update();
}


bool CCEditor::on_button_press_event(GdkEventButton* event) {
  if (!pat)
    return false;
  
  if (event->button == 1) {
    int step = int(event->x) / colWidth;
    int value = int(127 - event->y / vScale);
    value = value > 0 ? value : 0;
    dragStep = step;
    if (step >= 0 && step < pat->getLength() * pat->getCCSteps()) {
      pat->addCC(ccNumber, step, value);
      update(step);
    }
  }
  else if (event->button == 2) {
    int step = int(event->x) / colWidth;
    int value = int(127 - event->y / vScale);
    value = value > 0 ? value : 0;
    if (step >= 0 && step < pat->getLength() * pat->getSteps()) {
      lineStep = step;
      lineValue = value;
    }
  }
  else if (event->button == 3) {
    int step = int(event->x) / colWidth;
    dragStep = step;
    if (step >= 0 && step < pat->getLength() * pat->getCCSteps()) {
      pat->deleteCC(ccNumber, step);
      update(step);
    }
  }
  return true;
}


bool CCEditor::on_button_release_event(GdkEventButton* event) {
  if (!pat)
    return false;
  
  if (event->button == 2 && lineStep != -1) {
    int step = int(event->x) / colWidth;
    int value = int(127 - event->y / vScale);
    step = step >= 0 ? step : 0;
    int totalSteps = pat->getLength() * pat->getSteps();
    step = step < totalSteps ? step : totalSteps - 1;
    value = value > 0 ? value : 0;
    value = value < 128 ? value : 127;
    if (step == lineStep) {
      pat->addCC(ccNumber, step, value);
      update();
      lineStep = -1;
      return true;
    }
    else if (step < lineStep) {
      int tmp = lineStep;
      lineStep = step;
      step = tmp;
      tmp = lineValue;
      lineValue = value;
      value = tmp;
    }
    for (int i = 0; i <= step - lineStep; ++i) {
      int iValue;
      double id = double(i) / (step - lineStep + 1);
      if (event->state & GDK_SHIFT_MASK)
	iValue = lineValue + int((value - lineValue) * pow(id, 3));
      else if (event->state & GDK_CONTROL_MASK)
	iValue = lineValue - int((value - lineValue) * (pow(1 - id, 3) - 1));
      else if (event->state & GDK_MOD1_MASK)
	iValue = lineValue + 
	  int((value - lineValue) * (-cos(id * 3.141592)/2 + 0.5));
      else
	iValue = lineValue + int(i*double(value-lineValue)/(step-lineStep+1));
      pat->addCC(ccNumber, lineStep + i, iValue);
    }
    update();
    lineStep = -1;
  }
  return true;
}


bool CCEditor::on_motion_notify_event(GdkEventMotion* event) {
  if (!pat)
    return false;

  if (event->state & GDK_BUTTON1_MASK) {
    int step = int(event->x) / colWidth;
    if (step >= 0 && step < pat->getLength() * pat->getCCSteps()) {
      int value = int(127 - event->y / vScale);
      value = value > 127 ? 127 : value;
      value = value < 0 ? 0 : value;
      pat->addCC(ccNumber, step, value);
      update(step);
      dragStep = step;
    }
  }
  else if (event->state & GDK_BUTTON3_MASK) {
    int step = int(event->x) / colWidth;
    if (step == dragStep)
      return true;
    if (step >= 0 && step < pat->getLength() * pat->getCCSteps()) {
      pat->deleteCC(ccNumber, step);
      update(step);
      dragStep = step;
    }
  }

  return true;
}


void CCEditor::on_realize() {
  DrawingArea::on_realize();
  RefPtr<Gdk::Window> win = get_window();
  gc = GC::create(win);
  gc->set_background(bgColor);
  gc->set_foreground(fgColor);
  win->clear();
}


bool CCEditor::on_expose_event(GdkEventExpose* event) {
  RefPtr<Gdk::Window> win = get_window();
  win->clear();
  if (!pat)
    return true;
  
  int width = pat->getLength() * pat->getSteps() * colWidth;
  int height = 128;
  const Pattern::CCData& data = pat->getCC(ccNumber);
  std::map<int, Pattern::CCEvent*>::const_iterator iter;
  iter = data.changes.upper_bound(event->area.x / colWidth - 2);
  for ( ; iter != data.changes.end() && 
	  iter->first <= (event->area.x + event->area.width) / colWidth + 1;
	++iter) {
    gc->set_foreground(fgColor);
    win->draw_rectangle(gc, true, iter->first * colWidth, 
			(128 - iter->second->value - 1) * vScale, colWidth, 3);
    gc->set_foreground(edgeColor);
    win->draw_rectangle(gc, false, iter->first * colWidth, 
			(128 - iter->second->value - 1) * vScale, colWidth, 3);
  }
  
  return true;
}


void CCEditor::update(int step) {
  RefPtr<Gdk::Window> win = get_window();
  if (win) {
    if (step == -1) {
      win->invalidate_rect(Rectangle(0, 0, width, height), false);
      win->process_updates(false);
    }
    else {
      win->invalidate_rect(Rectangle(step * colWidth, 0, colWidth + 1, height),
			   false);
      win->process_updates(false);
    }
  }
}
