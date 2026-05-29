#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <config.h>
#include <kame.h>
#include <gamepad.h>


Kame robot;
WebServer server(80);

int joy_x = 0;
int joy_y = 0;
int calibration[8] = {0, 0, 0, 0, 0, 0, 0, 0};
bool calibration_mode = false;

float progress = 0;
float period = 450;
float leg_spread = 20;
float body_height = 10;
float body_shift = 0;
float step_amplitude = 0;
float step_height = 20;

float phase_linear[] =  {90,  90,  270, 90,  270, 270, 90,  270};
float phase_angular[] = {90,  270, 270, 90,  270, 90,  90,  270};
float phase[] =         {0,   0,   0,   0,   0,   0,   0,   0};


void handleRoot() {
    calibration_mode = false;
    joy_x = 0;
    joy_y = 0;
    robot.home();
    server.send(200, "text/html", gamepad_html);
}

void copyRobotCalibration() {
    for (int i = 0; i < 8; i++) {
        calibration[i] = robot.calibration[i];
    }
}

void updateServo(int i){
    if (i >= 0 && i < 8){
        robot.setCalibration(calibration);
        robot.setServo(i, 90);
    }
}

String calibrationHtml() {
    String html = R"rawliteral(
    <!DOCTYPE html>
    <html lang="es">
    <head>
        <meta charset="utf-8" />
        <title>Calibration</title>
        <style>
        body {
            background-color: #121212;
            color: #f0f0f0;
            font-family: Arial, sans-serif;
            padding: 20px;
            text-align: center;
        }
        h1 {
            color: #ff3535;
        }
        .servo-grid {
            display: grid;
            grid-template-columns: repeat(4, 1fr);
            gap: 20px;
            margin: 30px 0;
        }
        .servo-box {
            background: #1e1e1e;
            padding: 20px;
            border-radius: 12px;
            box-shadow: 0 4px 12px rgba(0,0,0,0.5);
        }
        .servo-title {
            margin-bottom: 10px;
            font-size: 1.1em;
        }
        .value {
            font-size: 1.5em;
            margin: 10px 0;
        }
        .btn {
            font-size: 1.5em;
            text-decoration: none;
            padding: 8px 14px;
            margin: 5px;
            background: #ff3535;
            color: white;
            border-radius: 8px;
            transition: background 0.3s;
            display: inline-block;
        }
        .btn:hover {
            background: #ff5959;
        }
        pre {
            background: #1e1e1e;
            padding: 10px;
            border-radius: 8px;
            color: #f0f0f0;
            font-size: 1.1em;
        }
        </style>
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
    </head>
    <body>
        <h1>Calibration</h1>
        <a class="btn" href="/" style="font-size: 1em;">Gamepad</a>
        <div class="servo-grid">
    )rawliteral";

    for (int i = 0; i < 8; i++) {
        html += "<div class='servo-box'>";
        html += "<div class='servo-title'>Servo " + String(i) + "</div>";
        html += "<a class='btn' href='/calibration/increase?i=" + String(i) + "'>&#9650;</a>";
        html += "<div class='value'>" + String(calibration[i]) + "</div>";
        html += "<a class='btn' href='/calibration/decrease?i=" + String(i) + "'>&#9660;</a>";
        html += "</div>";
    }

    html += "</div>";
    html += "<a class='btn' href='/calibration/load' style='width: 180px; margin: 0 10px;'>Load</a>";
    html += "<a class='btn' href='/calibration/save' style='width: 180px; margin: 0 10px;'>Save</a>";

    html += R"rawliteral(
        <h3 style="margin-top: 50px;">Generated calibration array</h3>
        <pre>int servo_calibration[8] = {)rawliteral";

    for (int i = 0; i < 8; i++) {
        html += String(calibration[i]);
        if (i < 7) html += ", ";
    }

    html += R"rawliteral(};</pre>
        </body>
        </html>
        )rawliteral";

    return html;
}

void handleCalibration() {
    calibration_mode = true;
    joy_x = 0;
    joy_y = 0;
    server.send(200, "text/html", calibrationHtml());
}

void handleCalibrationLoad(){
    int* loaded = robot.loadCalibration();
    if (loaded != nullptr){
        copyRobotCalibration();
        for (int i = 0; i < 8; i++){
            updateServo(i);
        }
    }
    handleCalibration();
}

void handleCalibrationSave(){
    robot.saveCalibration(calibration);
    robot.setCalibration(calibration);
    handleCalibration();
}

void handleCalibrationIncrease(){
    int i = server.hasArg("i") ? server.arg("i").toInt() : -1;
    if (i >= 0 && i < 8){
        calibration[i]++;
        updateServo(i);
    }
    handleCalibration();
}

void handleCalibrationDecrease(){
    int i = server.hasArg("i") ? server.arg("i").toInt() : -1;
    if (i >= 0 && i < 8){
        calibration[i]--;
        updateServo(i);
    }
    handleCalibration();
}

void handleJoystick() {
    if (server.hasArg("x") && server.hasArg("y")) {
        calibration_mode = false;
        joy_x = server.arg("x").toInt();
        joy_y = server.arg("y").toInt();
        server.send(200, "text/plain", "OK");
    } else {
        server.send(400, "text/plain", "Missing x or y");
    }
}

void handleButton() {
    if (server.hasArg("label")) {
        calibration_mode = false;
        String label = server.arg("label");
        Serial.println(label);
        if (label == "A") {
            robot.hello();
        } else if (label == "B") {
            robot.jump();
        } else if (label == "C") {
            robot.pushUp(4, 1000);
        } else if (label == "X") {
            robot.dance(2, 1000);
        } else if (label == "Y") {
            robot.moonwalkL(2, 2000);
        } else if (label == "Z") {
            robot.frontBack(2, 1000);
        } else if (label == "Start") {
            robot.arm();
        } else if (label == "Stop") {
            robot.disarm();
        } else {
            Serial.println("Unknown button: " + label);
        }
        server.send(200, "text/plain", "Button OK");
    } else {
        server.send(400, "text/plain", "Missing label");
    }
}


void setup() {
    Serial.begin(115200);

    WiFi.mode(WIFI_AP);
    WiFi.softAP(SSID, PASSWORD);
    // WiFi.mode(WIFI_STA);
    // WiFi.begin(SSID, PASSWORD);
    MDNS.begin(HOSTNAME);
    
    robot.init();
    copyRobotCalibration();
    robot.home();

    server.on("/", handleRoot);
    server.on("/calibration", handleCalibration);
    server.on("/calibration/", handleCalibration);
    server.on("/calibration/increase", handleCalibrationIncrease);
    server.on("/calibration/decrease", handleCalibrationDecrease);
    server.on("/calibration/load", handleCalibrationLoad);
    server.on("/calibration/save", handleCalibrationSave);
    server.on("/joystick", handleJoystick);
    server.on("/button", handleButton);
    server.begin();
}

void loop() {
    server.handleClient();

    if (calibration_mode) {
        return;
    }

    progress += robot.oscillator[0].getPhaseProgress();
    while (progress > 360)
        progress -= 360;

    if (abs(joy_x) > 0.0 || abs(joy_y) > 0.0) {
        if (abs(joy_y) >= abs(joy_x)) {
            // Linear movement
            step_amplitude = joy_y * 0.25;
            body_shift = step_amplitude * 0.8;

            phase[0] = phase_linear[0] + progress;
            phase[1] = phase_linear[1] + progress;
            phase[2] = phase_linear[2] + 2 * progress;
            phase[3] = phase_linear[3] + 2 * progress;
            phase[4] = phase_linear[4] + progress;
            phase[5] = phase_linear[5] + progress;
            phase[6] = phase_linear[6] + 2 * progress;
            phase[7] = phase_linear[7] + 2 * progress;
        } else {
            // Angular movement
            step_amplitude = joy_x * 0.25;
            body_shift = 0.0;

            phase[0] = phase_angular[0] + progress;
            phase[1] = phase_angular[1] + progress;
            phase[2] = phase_angular[2] + 2 * progress;
            phase[3] = phase_angular[3] + 2 * progress;
            phase[4] = phase_angular[4] + progress;
            phase[5] = phase_angular[5] + progress;
            phase[6] = phase_angular[6] + 2 * progress;
            phase[7] = phase_angular[7] + 2 * progress;
        }

        for (int i = 0; i < 8; i++) {
            robot.oscillator[i].setPhase(phase[i]);
            robot.oscillator[i].reset();
        }

        robot.oscillator[0].setAmplitude(step_amplitude);
        robot.oscillator[1].setAmplitude(step_amplitude);
        robot.oscillator[4].setAmplitude(step_amplitude);
        robot.oscillator[5].setAmplitude(step_amplitude);

        robot.oscillator[2].setAmplitude(step_height);
        robot.oscillator[3].setAmplitude(step_height);
        robot.oscillator[6].setAmplitude(step_height);
        robot.oscillator[7].setAmplitude(step_height);

        robot.oscillator[0].setOffset(90 + leg_spread - body_shift);
        robot.oscillator[1].setOffset(90 - leg_spread + body_shift);
        robot.oscillator[4].setOffset(90 - leg_spread - body_shift);
        robot.oscillator[5].setOffset(90 + leg_spread + body_shift);

        robot.oscillator[2].setOffset(90 - body_height);
        robot.oscillator[3].setOffset(90 + body_height);
        robot.oscillator[6].setOffset(90 + body_height);
        robot.oscillator[7].setOffset(90 - body_height);

        robot.oscillator[0].setPeriod(period);
        robot.oscillator[1].setPeriod(period);
        robot.oscillator[4].setPeriod(period);
        robot.oscillator[5].setPeriod(period);

        robot.oscillator[2].setPeriod(period / 2);
        robot.oscillator[3].setPeriod(period / 2);
        robot.oscillator[6].setPeriod(period / 2);
        robot.oscillator[7].setPeriod(period / 2);

        robot.setServo(0, robot.oscillator[0].refresh());
        robot.setServo(1, robot.oscillator[1].refresh());
        robot.setServo(4, robot.oscillator[4].refresh());
        robot.setServo(5, robot.oscillator[5].refresh());

        if (progress < 180) {
            robot.setServo(3, robot.oscillator[3].refresh());
            robot.setServo(6, robot.oscillator[6].refresh());
        } else {
            robot.setServo(2, robot.oscillator[2].refresh());
            robot.setServo(7, robot.oscillator[7].refresh());
        }
    }
    else{
        robot.home();
    }
}
