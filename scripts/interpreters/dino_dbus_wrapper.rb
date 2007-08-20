require 'dbus';

bus = DBus::SessionBus.instance;
dino_service = bus.service("org.nongnu.dino");
dino_object = dino_service.object("/");
dino_object.introspect;
song = dino_object["org.nongnu.dino.Song"];
seq = dino_object["org.nongnu.dino.Sequencer"];
