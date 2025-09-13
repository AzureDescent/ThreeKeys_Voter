#include <WiFi.h>
#include <Wire.h>

const uint8_t LED_PINS[] = {12, 13, 14}; // ?×√
const uint8_t BUTTON_PINS[] = {5, 2, 4}; //?×√
const char *AnswerLabels[] = {"❓", "❌", "✅"};
enum AnswerTypes
{
    UNCERTAIN,
    WRONG,
    CORRECT,
    COUNT
};
int AnswerStates[COUNT] = {0};
bool LastButtonStates[COUNT] = {HIGH};

const char *DeviceID = "Device_01";
#define WIFI_SSID "Wokwi_GUEST"
#define WIFI_PASSWORD ""

WiFiServer server(80);

void setup()
{
    Serial.begin(115200);

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("Connecting to WIFI:");
    Serial.print(WIFI_SSID);

    unsigned int StartTime = millis();

    while (WiFi.status() != WL_CONNECTED && millis() - StartTime < 30000)
    {
        delay(100);
        Serial.print(".");
    }

    if (WiFi.status() != WL_CONNECTED)
    {
        Serial.println("WIFI CONNECTION FAIL!");
    }
    else
    {
        Serial.println("\nConnected!");
        Serial.print("IP Address:");
        Serial.println(WiFi.localIP());
    }

    for (byte i = 0; i < COUNT; i++)
    {
        pinMode(LED_PINS[i], OUTPUT);
        pinMode(BUTTON_PINS[i], INPUT_PULLUP);
    }
    Serial.println("Initialization Completion!");
}

void print_results(int AnsTypes, int Delay)
{
    Serial.print("[");
    Serial.print(double(millis()) / 1000);
    Serial.print("s] ");

    Serial.print(DeviceID);

    Serial.print("|Added：");
    Serial.print(AnswerLabels[AnsTypes]);

    Serial.print("\nTotal：");
    for (byte i = 0; i < COUNT; i++)
    {
        Serial.print(AnswerLabels[i]);
        Serial.print(":");
        Serial.print(AnswerStates[i]);
        if (i < COUNT - 1)
        {
            Serial.print(",");
        }
        else
        {
            Serial.print(".  ");
        }
    }

    Serial.print("RandomDelayTime:");
    Serial.print(double(Delay) / 1000);
    Serial.println(".");
}

void loop()
{
    WiFiClient client = server.available();
    String Data = "✅:" + String(AnswerStates[CORRECT]) + " ❓:" + String(AnswerStates[UNCERTAIN]) + " ❌:" + String(AnswerStates[WRONG]) + " |Time:" + String(double(millis()) / 1000);
    if (client)
    {
        client.println(Data);
        Serial.println("Sent:" + Data);
        delay(5000);
    }

    for (byte i = 0; i < COUNT; i++)
    {
        bool CurrentButtonState = digitalRead(BUTTON_PINS[i]);

        if (CurrentButtonState == LOW && LastButtonStates[i] == HIGH)
        {
            AnswerStates[i]++;
            digitalWrite(LED_PINS[i], HIGH);

            int DelayTime = random(1000, 3000);
            delay(DelayTime);

            print_results(i, DelayTime);

            delay(500);
            digitalWrite(LED_PINS[i], LOW);
        }
        LastButtonStates[i] = CurrentButtonState;
    }
    delay(10);
}
