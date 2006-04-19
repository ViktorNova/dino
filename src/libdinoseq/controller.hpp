#ifndef CONTROLLER_HPP
#define CONTROLLER_HPP

#include <string>
#include <vector>

#include "interpolatedevent.hpp"


namespace Dino {

  
  /** This class represents a sequence of MIDI CC events used in a Pattern. */
  class Controller {
  public:
    
    /** Create a new controller with the given parameters. */
    Controller(const std::string& name, unsigned int size, 
	       long param, int min, int max);
    ~Controller();
    
    /// @name Accessors
    //@{
    /** Return the name of this Controller. */
    const std::string& get_name() const;
    /** Return the active event at the given step. */
    const InterpolatedEvent* get_event(unsigned int step) const;
    /** Return the minimum possible value for this controller. */
    int get_min() const;
    /** Return the maximum possible value for this controller. */
    int get_max() const;
    /** Return the parameter number for this controller (not the same as the
	CC or NRPN number, use the inline functions in controller_numbers.hpp
	to get those). */
    long get_param() const;
    /** Return the number of time steps in this controller. */
    unsigned int get_size() const;
    //@}

    /// @name Mutators
    //@{
    /** Set the name of this controller. */
    void set_name(const std::string& name);
    /** Add a control point to this controller. */
    void add_point(unsigned int step, int value);
    /** Remove a control point from this controller. */
    void remove_point(unsigned int step);
    //@}

  private:
    
    std::string m_name;
    std::vector<InterpolatedEvent*> m_events;
    long m_param;
    int m_min;
    int m_max;
    
  };

  
}


#endif
