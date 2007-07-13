/****************************************************************************
   Dino - A simple pattern based MIDI sequencer
   
   Copyright (C) 2006  Lars Luthman <lars.luthman@gmail.com>
   
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation, 
   Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
****************************************************************************/

#include <cstdlib>
#include <iostream>

#include <glib-object.h>

#include "dbusinterface.hpp"
#include "dino-glue.h"
#include "plugininterface.hpp"


using namespace Dino;
using namespace std;


namespace {
  PluginInterface* m_plif = 0;
}

  
struct _DBUSInterfacePrivate {
  gboolean dispose_has_run;
};


static GObjectClass* parent_class = 0;


static void dbusinterface_dispose(GObject *obj) {
  DBUSInterface *self = (DBUSInterface *)obj;

  if (self->priv->dispose_has_run) {
   /* If dispose did already run, return. */
    return;
  }
  /* Make sure dispose does not run twice. */
  self->priv->dispose_has_run = TRUE;

  /* 
   * In dispose, you are supposed to free all types referenced from this
   * object which might themselves hold a reference to self. Generally,
   * the most simple solution is to unref all members on which you own a 
   * reference.
   */
  
  /* Chain up to the parent class */
  G_OBJECT_CLASS (parent_class)->dispose (obj);
}


static void dbusinterface_finalize(GObject *obj) {
  DBUSInterface *self = (DBUSInterface *)obj;

  /* Chain up to the parent class */
  G_OBJECT_CLASS (parent_class)->finalize (obj);
}


static void dbusinterface_class_init (DBUSInterfaceClass *klass) {
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

  gobject_class->dispose = dbusinterface_dispose;
  gobject_class->finalize = dbusinterface_finalize;

  parent_class = (GObjectClass*)g_type_class_peek_parent(klass);
  g_type_class_add_private(klass, sizeof(DBUSInterfacePrivate));
}


static void dbusinterface_init (GTypeInstance *instance, gpointer g_class) {
  DBUSInterface *self = (DBUSInterface *)instance;
  //self->priv = G_TYPE_INSTANCE_GET_PRIVATE(self, DBUSINTERFACE_TYPE, 
  //					   DBUSInterfacePrivate);
  //self->priv->dispose_has_run = FALSE;
}


GType dbusinterface_get_type() {
  static GType type = 0;
  if (type == 0) {
    static const GTypeInfo info = {
      sizeof (DBUSInterfaceClass),
      NULL,   /* base_init */
      NULL,   /* base_finalize */
      NULL,   /* class_init */
      NULL,   /* class_finalize */
      NULL,   /* class_data */
      sizeof (DBUSInterface),
      0,      /* n_preallocs */
      dbusinterface_init    /* instance_init */
    };
    type = g_type_register_static(G_TYPE_OBJECT, "DBUSInterfaceType", &info, (GTypeFlags)0);
  }
  return type;
}


void dbusinterface_hello(DBUSInterface *self) {

}



  


extern "C" {
  string dino_get_name() { 
    return "DBUS interface"; 
  }
  
  void dino_load_plugin(PluginInterface& plif) {
    m_plif = &plif; 
    
  }
  
  void dino_unload_plugin() {

  }
}


void my_object_many_args() {

}

