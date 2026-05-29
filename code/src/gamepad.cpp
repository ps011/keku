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
        <title>Kame32 Calibration</title>
        <style>
        :root {
            --font-sans: "DM Sans", ui-sans-serif, system-ui, -apple-system, BlinkMacSystemFont, "Segoe UI", sans-serif;
            --background: #eff6ff;
            --foreground: #000000;
            --main: #3b82f6;
            --main-foreground: #ffffff;
            --secondary-background: #ffffff;
            --border: #000000;
            --muted-foreground: #333333;
            --radius: 5px;
            --shadow: 4px 4px 0px 0px var(--border);
            --shadow-sm: 2px 2px 0px 0px var(--border);
        }

        * {
            box-sizing: border-box;
            -webkit-tap-highlight-color: transparent;
        }

        body {
            min-height: 100vh;
            margin: 0;
            color: var(--foreground);
            font-family: var(--font-sans);
            background:
                radial-gradient(circle at 1px 1px, rgba(0,0,0,0.14) 1px, transparent 0),
                var(--background);
            background-size: 22px 22px;
            padding: 18px;
        }

        .app {
            width: min(1120px, 100%);
            margin: 0 auto;
            display: flex;
            flex-direction: column;
            gap: 18px;
        }

        .topbar {
            display: flex;
            align-items: center;
            justify-content: space-between;
            gap: 12px;
            padding: 14px;
            border: 3px solid var(--border);
            border-radius: var(--radius);
            background: var(--secondary-background);
            box-shadow: var(--shadow);
        }

        .brand {
            display: flex;
            flex-direction: column;
            gap: 2px;
        }

        h1 {
            margin: 0;
            font-size: clamp(24px, 5vw, 42px);
            line-height: 0.95;
            font-weight: 800;
        }

        .subtitle {
            color: var(--muted-foreground);
            font-size: 13px;
            font-weight: 800;
            text-transform: uppercase;
        }

        .servo-grid {
            display: grid;
            grid-template-columns: repeat(4, minmax(0, 1fr));
            gap: 14px;
        }

        .servo-box {
            background: var(--secondary-background);
            padding: 14px;
            border: 3px solid var(--border);
            border-radius: var(--radius);
            box-shadow: var(--shadow);
            display: flex;
            flex-direction: column;
            gap: 12px;
        }

        .servo-title {
            display: flex;
            align-items: center;
            justify-content: space-between;
            gap: 10px;
            font-size: 18px;
            font-weight: 800;
        }

        .servo-title span {
            color: var(--muted-foreground);
            font-size: 11px;
            font-weight: 800;
            text-transform: uppercase;
        }

        .value {
            min-height: 58px;
            display: flex;
            align-items: center;
            justify-content: center;
            background: var(--background);
            border: 3px solid var(--border);
            border-radius: var(--radius);
            box-shadow: var(--shadow-sm);
            font-size: 32px;
            font-weight: 900;
        }

        .servo-actions {
            display: grid;
            grid-template-columns: 1fr 1fr;
            gap: 10px;
        }

        .btn,
        .nav-button {
            min-height: 44px;
            display: inline-flex;
            align-items: center;
            justify-content: center;
            gap: 8px;
            text-decoration: none;
            padding: 10px 14px;
            background: var(--main);
            color: var(--main-foreground);
            border: 3px solid var(--border);
            border-radius: var(--radius);
            box-shadow: var(--shadow);
            font-size: 15px;
            font-weight: 800;
            transition: transform 0.08s ease, box-shadow 0.08s ease;
        }

        .btn:active,
        .nav-button:active {
            transform: translate(4px, 4px);
            box-shadow: none;
        }

        .btn.secondary,
        .nav-button {
            background: var(--secondary-background);
            color: var(--foreground);
        }

        .btn strong,
        .nav-button strong {
            display: block;
            line-height: 1;
        }

        .btn span,
        .nav-button span {
            display: block;
            color: currentColor;
            font-size: 11px;
            font-weight: 900;
            line-height: 1;
            opacity: 0.78;
            text-transform: uppercase;
        }

        .page-actions {
            display: flex;
            flex-wrap: wrap;
            gap: 12px;
        }

        .code-panel {
            background: var(--secondary-background);
            border: 3px solid var(--border);
            border-radius: var(--radius);
            box-shadow: var(--shadow);
            padding: 14px;
        }

        .code-title {
            margin-bottom: 10px;
            font-size: 16px;
            font-weight: 800;
        }

        pre {
            margin: 0;
            padding: 12px;
            overflow-x: auto;
            background: var(--background);
            color: var(--foreground);
            border: 3px solid var(--border);
            border-radius: var(--radius);
            font-size: 14px;
            font-weight: 700;
        }

        @media (max-width: 860px) {
            .servo-grid {
                grid-template-columns: repeat(2, minmax(0, 1fr));
            }

            .topbar {
                align-items: flex-start;
                flex-direction: column;
            }
        }

        @media (max-width: 520px) {
            body {
                padding: 12px;
            }

            .servo-grid,
            .servo-actions {
                grid-template-columns: 1fr;
            }
        }
        </style>
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
    </head>
    <body>
        <main class="app">
        <header class="topbar">
            <div class="brand">
                <h1>Kame32</h1>
                <div class="subtitle">Servo calibration</div>
            </div>
            <a class="nav-button" href="/">
                <strong>Gamepad</strong>
                <span>Control</span>
            </a>
        </header>
        <div class="servo-grid">
    )rawliteral";

    for (int i = 0; i < 8; i++) {
        html += "<div class='servo-box'>";
        html += "<div class='servo-title'>Servo " + String(i) + "<span>Trim</span></div>";
        html += "<div class='value'>" + String(calibration[i]) + "</div>";
        html += "<div class='servo-actions'>";
        html += "<a class='btn secondary' href='/calibration/decrease?i=" + String(i) + "'><strong>-</strong><span>Decrease</span></a>";
        html += "<a class='btn' href='/calibration/increase?i=" + String(i) + "'><strong>+</strong><span>Increase</span></a>";
        html += "</div>";
        html += "</div>";
    }

    html += "</div>";
    html += "<div class='page-actions'>";
    html += "<a class='btn secondary' href='/calibration/load'><strong>Load</strong><span>Memory</span></a>";
    html += "<a class='btn' href='/calibration/save'><strong>Save</strong><span>Memory</span></a>";
    html += "</div>";

    html += R"rawliteral(
        <section class="code-panel">
        <div class="code-title">Generated calibration array</div>
        <pre>int servo_calibration[8] = {)rawliteral";

    for (int i = 0; i < 8; i++) {
        html += String(calibration[i]);
        if (i < 7) html += ", ";
    }

    html += R"rawliteral(};</pre>
        </section>
        </main>
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
