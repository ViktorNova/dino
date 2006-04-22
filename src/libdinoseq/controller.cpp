#include <cassert>

#include "controller_numbers.hpp"
#include "controller.hpp"
#include "deleter.hpp"


namespace Dino {


  Controller::Controller(const std::string& name, unsigned int size, 
			 long param, int min, int max) 
    : m_name(name),
      m_events(size, 0),
      m_param(param),
      m_min(min),
      m_max(max) {
    assert(controller_is_set(m_param));
  }
  
  
  Controller::~Controller() {
    InterpolatedEvent* previous = 0;
    for (unsigned i = 0; i < m_events.size(); ++i) {
      if (m_events[i] != 0 && m_events[i] != previous)
	delete m_events[i];
      previous = m_events[i];
    }
  }

  
  const std::string& Controller::get_name() const {
    return m_name;
  }
  

  int Controller::get_min() const {
    return m_min;
  }
  
  
  int Controller::get_max() const {
    return m_max;
  }


  long Controller::get_param() const {
    return m_param;
  }
  
  
  unsigned int Controller::get_size() const {
    return m_events.size();
  }


  void Controller::set_name(const std::string& name) {
    m_name = name;
  }
  
  
  void Controller::add_point(unsigned int step, int value) {
    assert(step <= m_events.size());
    assert(value >= m_min);
    assert(value <= m_max);
    
    // change the last point in the controller if there is an event
    if (step == m_events.size() && m_events[step - 1]) {
      m_events[step - 1]->set_end(value);
      return;
    }
      
    // if an old event is going on at this step, handle it
    InterpolatedEvent* old_event = m_events[step];
    if (old_event != 0) {
      if (old_event->get_step() == step) {
	old_event->set_start(value);
	if (step > 0 && m_events[step - 1]) {
	  m_events[step - 1]->set_end(value);
	  m_events[step - 1]->set_length(step - m_events[step - 1]->get_step());
	}
      }
      else {
	unsigned int length = old_event->get_step() + 
	  old_event->get_length() - step;
	int end = old_event->get_end();
	old_event->set_length(step - old_event->get_step());
	old_event->set_end(value);
	m_events[step] = new InterpolatedEvent(value, end, step, length);
	for (unsigned i = step + 1; i < step + length; ++i)
	  m_events[i] = m_events[step];
      }
    }
    
    // if not, we are the first event
    else {
      unsigned i;
      for (i = step; i < m_events.size() && m_events[i] == 0; ++i);
      int end = value;
      if (i < m_events.size())
	end = m_events[i]->get_start();
      if (step > 0 && m_events[step - 1]) {
	m_events[step - 1]->set_end(value);
	m_events[step - 1]->set_length(step - m_events[step - 1]->get_step());
      }
      m_events[step] = new InterpolatedEvent(value, end, step, i - step);
      for (unsigned j = step + 1; j < i; ++j)
	m_events[j] = m_events[step];
    }
  }


  void Controller::remove_point(unsigned int step) {
    assert(step < m_events.size());
    
    InterpolatedEvent* event = m_events[step];
    if (!event || event->get_step() != step)
      return;
    
    for (unsigned i = step; i < step + event->get_length(); ++i)
      m_events[i] = 0;
    
    if (event && event->get_step() == step) {
      if (step > 0 && m_events[step - 1]) {
	m_events[step - 1]->set_end(event->get_end());
	m_events[step - 1]->set_length(step + event->get_length() - 
				       m_events[step - 1]->get_step());
	for (unsigned i = step; i < step + event->get_length(); ++i)
	  m_events[i] = m_events[step - 1];
      }
    }
    
    Deleter::queue(event);
  }


  const InterpolatedEvent* Controller::get_event(unsigned int step) const {
    assert(step < m_events.size());
    return m_events[step];
  }


}
