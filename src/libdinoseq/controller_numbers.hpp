#ifndef CONTROLLER_NUMBERS_HPP
#define CONTROLLER_NUMBERS_HPP

// Most of these functions are adapted from the DSSI header

namespace Dino {
  
  static const long g_controller_cc_bits = 0x20000000;
  static const long g_controller_nrpn_bits = 0x40000000;
  
  static const long g_controller_none = -1;
  
  
  static inline bool controller_is_set(long id) {
    return (g_controller_none != id);
  }
  
  
  static inline long make_cc(long cc) {
    return g_controller_cc_bits | cc;
  }
  
  
  static inline bool is_cc(long id) {
    return g_controller_cc_bits & id;
  }
  
  
  static inline long cc_number(long id) {
    return id & 0x7f;
  }
  

  static inline long make_nrpn(long nrpn) {
    return g_controller_nrpn_bits | (nrpn << 8);
  }
  
  
  static inline bool is_nrpn(long id) {
    return g_controller_nrpn_bits & id;
  }

  
  static inline long nrpn_number(long id) {
    return (id >> 8) & 0x3fff;
  }
  
  
  static inline long make_pbend() {
    return 128;
  }
  
  
  static inline bool is_pbend(long id) {
    return (id == 128);
  }


}

#endif
