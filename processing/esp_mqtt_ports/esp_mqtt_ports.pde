import controlP5.*;
import processing.mqtt.*;

MQTTClient client;
ControlP5 cp5;
ArrayList<CheckBox> checkboxes = new ArrayList<CheckBox>();
int myColorBackground;







String espTopic = "/deviceId/portexpander";
String mqttBrokerUrl = "mqtt://demo:demo@192.168.0.2";





CheckBox addCheckbox(ControlP5 cp5, String name, PVector pos) {
  CheckBox rv = cp5.addCheckBox(name)
    .setPosition(pos)
    .setColorForeground(color(120))
    .setColorActive(color(255))
    .setColorLabel(color(255))
    .setSize(10, 10)
    .setItemsPerRow(16)
    .setSpacingColumn(7)
    .setSpacingRow(7)
    .addItem(name+"1", 1)
    .addItem(name+"2", 2)
    .addItem(name+"3", 3)
    .addItem(name+"4", 4)
    .addItem(name+"5", 5)
    .addItem(name+"6", 6)
    .addItem(name+"7", 7)
    .addItem(name+"8", 8)
    .addItem(name+"9", 9)
    .addItem(name+"10", 10)
    .addItem(name+"11", 11)
    .addItem(name+"12", 12)
    .addItem(name+"13", 13)
    .addItem(name+"14", 14)
    .addItem(name+"15", 15)
    .addItem(name+"16", 16)
    .hideLabels();
  return rv;
}

void setup() {
  size(375, 625);
  smooth();
  client = new MQTTClient(this);
  client.connect(mqttBrokerUrl, "my-client");
  
  cp5 = new ControlP5(this);
  for (int g=0;g<4;g++) {
    for (int a=0;a<8;a++) {
      checkboxes.add(addCheckbox(cp5, ""+g+a, new PVector(50,50+(g*9+a)*15)));
    }
  }
}

void draw() {
  background(0);
}

void controlEvent(ControlEvent theEvent) {
  
  for (int c=0;c<checkboxes.size();c++){
    CheckBox checkbox = checkboxes.get(c);
    if (theEvent.isFrom(checkbox)) {
      //print("got an event from "+checkbox.getName()+"\t\n");
      String msg = checkbox.getName();

      for (int i=0;i<checkbox.getArrayValue().length;i++) {
        msg += (int)checkbox.getArrayValue()[i];
      }
      println("publishing: " + msg + " to " + espTopic); 
      client.publish(espTopic, msg);   
    }
  }
}

