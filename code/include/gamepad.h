#ifndef GAMEPAD_H
#define GAMEPAD_H

#include <Arduino.h>

String gamepad_html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <meta charset="utf-8" />
    <title>Kame32 Gamepad</title>
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
                linear-gradient(rgba(0,0,0,0.06) 1px, transparent 1px),
                linear-gradient(90deg, rgba(0,0,0,0.06) 1px, transparent 1px),
                var(--background);
            background-size: 24px 24px;
            padding: 18px;
            user-select: none;
        }

        .app {
            width: min(1040px, 100%);
            min-height: calc(100vh - 36px);
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

        .title {
            margin: 0;
            font-size: clamp(24px, 5vw, 42px);
            line-height: 0.95;
            font-weight: 800;
        }

        .subtitle {
            color: var(--muted-foreground);
            font-size: 13px;
            font-weight: 700;
            text-transform: uppercase;
        }

        .stage {
            flex: 1;
            display: grid;
            grid-template-columns: minmax(260px, 1fr) minmax(320px, 1.1fr);
            gap: 18px;
            align-items: stretch;
        }

        .panel {
            border: 3px solid var(--border);
            border-radius: var(--radius);
            background: var(--secondary-background);
            box-shadow: var(--shadow);
            padding: 18px;
        }

        .panel-title {
            display: flex;
            align-items: baseline;
            justify-content: space-between;
            gap: 10px;
            margin-bottom: 16px;
            font-size: 18px;
            font-weight: 800;
        }

        .panel-title span {
            color: var(--muted-foreground);
            font-size: 12px;
            font-weight: 800;
            text-transform: uppercase;
        }

        .joystick-panel {
            display: flex;
            flex-direction: column;
            align-items: center;
        }

        #stick {
            width: min(60vw, 280px);
            height: min(60vw, 280px);
            max-width: 280px;
            max-height: 280px;
            min-width: 220px;
            min-height: 220px;
            background: #ffffff;
            border: 3px solid var(--border);
            border-radius: 50%;
            box-shadow: inset 0 0 0 12px #dbeafe, var(--shadow);
            position: relative;
            touch-action: none;
        }

        #stick::before,
        #stick::after {
            content: "";
            position: absolute;
            background: var(--border);
            opacity: 0.22;
            pointer-events: none;
        }

        #stick::before {
            width: 3px;
            height: 74%;
            left: calc(50% - 1.5px);
            top: 13%;
        }

        #stick::after {
            height: 3px;
            width: 74%;
            left: 13%;
            top: calc(50% - 1.5px);
        }

        #dot {
            width: 100px;
            height: 100px;
            background: var(--main);
            border: 3px solid var(--border);
            box-shadow: var(--shadow-sm);
            border-radius: 50%;
            position: absolute;
            top: calc(50% - 50px);
            left: calc(50% - 50px);
            display: flex;
            align-items: center;
            justify-content: center;
            color: var(--main-foreground);
            font-weight: 900;
            letter-spacing: 0;
        }

        .buttons-grid {
            display: grid;
            grid-template-columns: repeat(3, minmax(88px, 1fr));
            gap: 12px;
        }

        button,
        .link-button {
            min-height: 44px;
            border: 3px solid var(--border);
            border-radius: var(--radius);
            background: var(--main);
            color: var(--main-foreground);
            box-shadow: var(--shadow);
            font: inherit;
            font-weight: 800;
            cursor: pointer;
            transition: transform 0.08s ease, box-shadow 0.08s ease;
            text-decoration: none;
        }

        button:active,
        .link-button:active {
            transform: translate(4px, 4px);
            box-shadow: none;
        }

        .move-button {
            min-height: 84px;
            padding: 10px;
            display: flex;
            flex-direction: column;
            align-items: flex-start;
            justify-content: space-between;
            gap: 6px;
            color: #000000;
        }

        .key {
            display: inline-flex;
            width: 28px;
            height: 28px;
            align-items: center;
            justify-content: center;
            border: 3px solid var(--border);
            border-radius: 50%;
            background: #ffffff;
            box-shadow: var(--shadow-sm);
            font-size: 15px;
            line-height: 1;
        }

        .action {
            font-size: 15px;
            line-height: 1.05;
            text-align: left;
        }

        .red    { background: #ef4444; }
        .yellow { background: #facc15; }
        .blue   { background: #60a5fa; }
        .orange { background: #fb923c; }
        .purple { background: #c084fc; }
        .green  { background: #4ade80; }

        .actions {
            display: grid;
            grid-template-columns: repeat(3, minmax(0, 1fr));
            gap: 12px;
            margin-top: 18px;
        }

        .action-button,
        .link-button {
            display: flex;
            align-items: center;
            justify-content: center;
            gap: 8px;
            padding: 10px 12px;
            background: var(--secondary-background);
            color: var(--foreground);
        }

        .action-button strong,
        .link-button strong {
            display: block;
            font-size: 13px;
            line-height: 1;
        }

        .action-button span,
        .link-button span {
            display: block;
            color: var(--muted-foreground);
            font-size: 11px;
            font-weight: 800;
            line-height: 1;
            text-transform: uppercase;
        }

        @media (max-width: 820px) {
            body {
                padding: 12px;
            }

            .app {
                min-height: calc(100vh - 24px);
            }

            .stage {
                grid-template-columns: 1fr;
            }

            .topbar {
                align-items: flex-start;
                flex-direction: column;
            }
        }

        @media (max-width: 520px) {
            .panel {
                padding: 14px;
            }

            .buttons-grid {
                grid-template-columns: repeat(2, minmax(0, 1fr));
            }

            .actions {
                grid-template-columns: 1fr;
            }
        }
    </style>
    <meta name="viewport" content="initial-scale=1.0, maximum-scale=1.0, user-scalable=no">
</head>
<body>
    <main class="app">
        <header class="topbar">
            <div class="brand">
                <h1 class="title">Kame32</h1>
                <div class="subtitle">Gamepad</div>
            </div>
            <a class="link-button" href="/calibration">
                <strong>Calibration</strong>
                <span>Servos</span>
            </a>
        </header>

        <section class="stage">
            <div class="panel joystick-panel">
                <div class="panel-title">Joystick <span>X/Y axis</span></div>
                <div id="stick">
                    <div id="dot">K32</div>
                </div>
            </div>

            <div class="panel">
                <div class="panel-title">Moves <span>A/B/C/X/Y/Z</span></div>
                <div class="buttons-grid">
                    <button class="move-button red" onclick="handleButton('A')">
                        <span class="key">A</span>
                        <span class="action">Hello</span>
                    </button>
                    <button class="move-button yellow" onclick="handleButton('B')">
                        <span class="key">B</span>
                        <span class="action">Jump</span>
                    </button>
                    <button class="move-button blue" onclick="handleButton('C')">
                        <span class="key">C</span>
                        <span class="action">Push-up</span>
                    </button>
                    <button class="move-button orange" onclick="handleButton('X')">
                        <span class="key">X</span>
                        <span class="action">Dance</span>
                    </button>
                    <button class="move-button purple" onclick="handleButton('Y')">
                        <span class="key">Y</span>
                        <span class="action">Moonwalk</span>
                    </button>
                    <button class="move-button green" onclick="handleButton('Z')">
                        <span class="key">Z</span>
                        <span class="action">Front/back</span>
                    </button>
                </div>

                <div class="actions">
                    <button class="action-button" onclick="handleButton('Start')">
                        <strong>Start</strong>
                        <span>Arm</span>
                    </button>
                    <button class="action-button" onclick="handleButton('Stop')">
                        <strong>Stop</strong>
                        <span>Disarm</span>
                    </button>
                    <a class="link-button" href="/calibration">
                        <strong>Calibration</strong>
                        <span>Trim</span>
                    </a>
                </div>
            </div>
        </section>
    </main>

    <script>
        const stick = document.getElementById("stick");
        const dot = document.getElementById("dot");

        let lastSent = 0;
        let throttleTime = 100; // milliseconds
        let touchActive = false;

        function sendJoystick(x, y) {
            const now = Date.now();
            if (now - lastSent >= throttleTime) {
                const xhr = new XMLHttpRequest();
                xhr.open("GET", `/joystick?x=${x}&y=${y}`);
                xhr.send();
                lastSent = now;
            }
        }

        function handleButton(label) {
            const xhr = new XMLHttpRequest();
            xhr.open("GET", `/button?label=${label}`);
            xhr.send();
        }

        function moveDot(x, y) {
            const rect = stick.getBoundingClientRect();
            const dotSize = dot.offsetWidth;
            const radius = rect.width / 2;
            const min = dotSize / 2;
            const max = rect.width - dotSize / 2;
            const travel = Math.max(1, radius - dotSize / 2);

            x = Math.max(min, Math.min(max, x));
            y = Math.max(min, Math.min(max, y));

            dot.style.left = (x - dotSize / 2) + "px";
            dot.style.top = (y - dotSize / 2) + "px";

            let xVal = Math.round((x - radius) / travel * 100);
            let yVal = Math.round((y - radius) / travel * -100);

            sendJoystick(xVal, yVal);
        }

        stick.addEventListener("touchstart", function () {
            touchActive = true;
        });

        stick.addEventListener("touchmove", function (e) {
            e.preventDefault();
            touchActive = true;
            const touch = e.touches[0];
            const rect = stick.getBoundingClientRect();
            moveDot(touch.clientX - rect.left, touch.clientY - rect.top);
        }, { passive: false });


        let isMouseDown = false;

        stick.addEventListener("mousedown", function (e) {
            isMouseDown = true;
            touchActive = true;
            moveDotWithMouse(e);
        });

        document.addEventListener("mousemove", function (e) {
            if (!isMouseDown) return;
            moveDotWithMouse(e);
        });

        document.addEventListener("mouseup", function () {
            if (isMouseDown) {
                isMouseDown = false;
                resetJoystick();
            }
        });

        function moveDotWithMouse(e) {
            const rect = stick.getBoundingClientRect();
            moveDot(e.clientX - rect.left, e.clientY - rect.top);
        }

        function resetJoystick() {
            dot.style.left = "calc(50% - 50px)";
            dot.style.top = "calc(50% - 50px)";
            sendJoystick(0, 0);
            touchActive = false;
        }

        document.addEventListener("touchend", resetJoystick);
        document.addEventListener("touchcancel", resetJoystick);
        document.addEventListener("touchleave", resetJoystick);

        setInterval(() => {
            if (!touchActive) {
                sendJoystick(0, 0);
            }
        }, 200); // every 200ms
    </script>
</body>
</html>
)rawliteral";

#endif // GAMEPAD_H
