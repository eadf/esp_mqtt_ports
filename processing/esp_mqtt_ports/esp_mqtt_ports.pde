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
    .setSpacingRow(7);
  for (int i=1;i<=16;i++) {  
    rv.addItem(name+i, i);
  }
  return rv.hideLabels();
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

