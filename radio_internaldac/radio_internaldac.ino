#include <Wire.h>
#include <AudioTools.h>
#include <Adafruit_Si4713.h>
#include <BluetoothA2DPSink.h>
#include <config.h>

#define RESETPIN 27
#define FMSTATION 10800      // 10800 == 108.00 MHz

Adafruit_Si4713 radio = Adafruit_Si4713(RESETPIN);
AnalogAudioStream out;
BluetoothA2DPSink a2dp_sink(out);

void avrc_metadata_callback(uint8_t data1, const uint8_t *data2) {
  Serial.printf("Broadcasting '%s' as the current song on RDS\n", data2);
  radio.setRDSbuffer((const char *)data2);
}

void setup() {
  //The FM transmitter fucker won't start unless I do this
  pinMode(RESETPIN, OUTPUT);
  digitalWrite(RESETPIN, LOW);
  delay(10);
  digitalWrite(RESETPIN, HIGH);
  delay(10);

  Serial.begin(9600);
  Serial.println("Adafruit Radio - Si4713 Test");
  
  a2dp_sink.set_avrc_metadata_attribute_mask(ESP_AVRC_MD_ATTR_TITLE);
  a2dp_sink.set_avrc_metadata_callback(avrc_metadata_callback);
  a2dp_sink.start("Nakii Golf test");

  if (!radio.begin()) {
    Serial.println("Couldn't find radio?");
    while (1);
  }

  //Just to be safe add 2 seconds of delay upon starting
  delay(2000);

  Serial.print("\nSet TX power");
  radio.setTXpower(115);  // dBuV, 88-115 max

  Serial.print("\nTuning into ");
  Serial.print(FMSTATION / 100);
  Serial.print('.');
  Serial.println(FMSTATION % 100);
  radio.tuneFM(FMSTATION);

  radio.readTuneStatus();
  Serial.print("\tCurr freq: ");
  Serial.println(radio.currFreq);
  Serial.print("\tCurr freqdBuV:");
  Serial.println(radio.currdBuV);
  Serial.print("\tCurr ANTcap:");
  Serial.println(radio.currAntCap);

  // Start RDS transmission
  radio.beginRDS();
  radio.setRDSstation("NakiiStation");
  radio.setRDSbuffer("Please connect via BT!");

  Serial.println("RDS on!");
}

void loop() {
  radio.readASQ();
  Serial.print("\tCurr ASQ: 0x");
  Serial.println(radio.currASQ, HEX);
  Serial.print("\tCurr InLevel:");
  Serial.println(radio.currInLevel);
  delay(500);
}
