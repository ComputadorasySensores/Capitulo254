// A2DPSink example - Released to the public domain in 2024 by Earle F. Philhower, III

// Traducido y ampliado para uso de pantalla OLED por Computadoras y Sensores
// https://www.youtube.com/@ComputadorasySensores

#include <BluetoothAudio.h>
#include <PWMAudio.h>

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define PANTALLA_ANCHO 128
#define PANTALLA_ALTO 64

Adafruit_SSD1306 display(PANTALLA_ANCHO, PANTALLA_ALTO, &Wire, -1);

PWMAudio pwm;
A2DPSink a2dp;

volatile A2DPSink::PlaybackStatus status = A2DPSink::STOPPED;

void volumeCB(void *param, int pct) {
  (void) param;
  Serial.printf("Volumen cambiado a %d%%\n", pct);
}

void connectCB(void *param, bool connected) {
  (void) param;
  if (connected) {
    Serial.printf("A2DP conexión iniciada con %s\n", bd_addr_to_str(a2dp.getSourceAddress()));
  } else {
    Serial.printf("A2DP conexión detenida\n");
    display.clearDisplay();
    display.display();
  }
}

void playbackCB(void *param, A2DPSink::PlaybackStatus state) {
  (void) param;
  status = state;
}

void setup() {
  Serial.begin(115200);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 fallo de inicialización"));
    for(;;);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(18, 0);
  display.println("Pico W Bluetooth");
  display.display(); 

  delay(3000);
  Serial.printf("Iniciando, conéctate a la PicoW por Bluetooth\n");
  Serial.printf("Presiona BOOTSEL para pausar/resumir\n");
  a2dp.setName("PicoW Boom 00:00:00:00:00:00");
  a2dp.setConsumer(new BluetoothAudioConsumerPWM(pwm));
  a2dp.onVolume(volumeCB);
  a2dp.onConnect(connectCB);
  a2dp.onPlaybackStatus(playbackCB);
  a2dp.begin();
}

char *nowPlaying = nullptr;

void loop() {
  if (BOOTSEL) {
    if (status == A2DPSink::PAUSED) {
      a2dp.play();
      Serial.printf("Resumiendo\n");
    } else if (status == A2DPSink::PLAYING) {
      a2dp.pause();
      Serial.printf("Pausando\n");
    }
    while (BOOTSEL);
  }
  if (!nowPlaying || strcmp(nowPlaying, a2dp.trackTitle())) {
    free(nowPlaying);
    nowPlaying = strdup(a2dp.trackTitle());
    Serial.printf("Reproduciendo: %s\n", nowPlaying);
    display.clearDisplay();
    display.setCursor(0, 20);
    display.setTextColor(WHITE,BLACK);
    display.println("Reproduciendo: ");
    display.setCursor(0, 40);
    display.println(nowPlaying);
    display.display();
  }
}
