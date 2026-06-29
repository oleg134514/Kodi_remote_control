# Kodi_remote_control
IR remote control for api kodi

Description in English and description in Russian

================================================================================

FULL DESCRIPTION OF THE KODI CONTROL FIRMWARE

================================================================================

DEVICE PURPOSE:

----------------------

This firmware is designed for an ESP32-C3 microcontroller connected to a 
ST7735 TFT display (160x128 pixels) and an IR receiver. The device functions 
as a custom remote control and status monitor for the Kodi media center 
running on a remote server (Debian Linux).
The device connects to the local WiFi network, establishes an HTTP connection 
to the Kodi server (port 8080), authenticates, and then:
1) Receives commands from the IR remote and sends them to Kodi via the JSON-RPC API
2) Periodically polls Kodi to retrieve system information
3) Displays playback status, volume, CPU temperature, and other parameters on the screen
4) Visually indicates remote button presses
5) Switches to clock mode during prolonged inactivity or pause

================================================================================

FIRMWARE ARCHITECTURE

================================================================================

THE FIRMWARE IS DIVIDED INTO THE FOLLOWING FILES:

---------------------------------------

1. config.h

--------

Contains network settings and connection parameters:
- WIFI_SSID, WIFI_PASS — WiFi network name and password
- KODI_IP, KODI_PORT — IP address and port of the Kodi server
- KODI_USER, KODI_PASS — login and password for Kodi authentication
- Microcontroller pins for connecting the TFT display and IR receiver
- NTP_SERVER — NTP server address for time synchronization
- NTP_TIMEZONE — timezone as a number (e.g., 5 for UTC+5)

2. color_and_coordinates.h

------------------------

Contains ALL customizable interface parameters:
- Colors for all elements (statuses, text, digits, progress bar)
- Colors for clock mode (separate for NTP and internal clock)
- X and Y coordinates for each element on the screen
- Coordinates for hours, minutes, days, months, and separators
- Element sizes (screen width/height, icons, fonts)
- Padding between elements
- Threshold values for critical parameters (CPU, temperature)
- Critical warning colors
This file allows for complete interface customization without 
modifying the program logic.

3. timings.h

----------

Contains ALL timing parameters for the firmware:
- WiFi and server connection timeouts
- Intervals between reconnection attempts
- Delays for displaying statuses on the screen
- Kodi polling interval (default 1000 ms)
- Duration for showing the pressed button icon (2000 ms)
- Button auto-repeat parameters (intervals and acceleration phases)
- Clock mode timings:
* CLOCK_IDLE_TIMEOUT — inactivity time to activate (5 minutes)
* CLOCK_PAUSE_TIMEOUT — pause time to activate (30 minutes)
* CLOCK_UPDATE_INTERVAL — update interval (1 minute)
* NTP_SYNC_INTERVAL — NTP synchronization interval (1 hour)

4. display.h

----------

Display and font handling module:
- Binary search functions for glyphs in fonts (7x14 and 12x24)
- UTF-8 string decoding for Cyrillic support
- printText() function for outputting text with word wrap
- clearArea() function for clearing a screen area with the background
- getTextWidth() function accounting for character spacing

5. connection.h

-------------

Connection management module:
- performSequencedInitialization() — sequential check:
* WiFi connection (up to 15 attempts)
* Kodi server availability check (TCP connection)
* Authentication via HTTP Basic Auth (JSON-RPC Ping)
- Displays "OK" (green) or "error" (red) statuses on the screen
- On success — transitions to the main screen with the background
- On error — clears the screen and waits for a retry

6. ir_control.h

-------------

IR signal processing module:
- IrCommand struct — description of each button (code, icon, JSON command)
- irCommands[] table — all 28 remote buttons with their parameters
- RepeatType enum — auto-repeat types (NONE, FIXED, ACCELERATED)
- calculateRepeatInterval() function — calculates auto-repeat interval 
based on button hold time

7. kodi_api.h

-----------

HTTP interaction module with Kodi:
- sendKodiBatchRequest() — sends a batch JSON-RPC request
- sendKodiCommand() — sends a single control command
- parseTemperature() — parses the temperature string from the API response
- parseCpuLoad() — universal CPU load parser, supports:
* Integers (e.g., 50)
* Floats (e.g., 0.5 = 50%)
* Strings in the format "#0: 0.0% #1: 16% #2: 0.0% #3: 0.0%" 
(calculates the average load across all CPU cores)

8. progress_bar.h

---------------

Progress bar module:
- formatTime() — formats seconds into an "H:MM:SS" or "MM:SS" string
- drawProgressBar() — renders the progress bar with optimization:
* Draws only the changed parts (without full redraw)
* If totalSec=0, draws a red line without a slider
* If currentSec>totalSec, caps the progress at 1.0
* NOT drawn in clock mode

9. ui_elements.h

--------------

UI elements rendering module:
- drawPlaybackStatus() — playback status icon:
* color_play — during normal playback
* color_pause — during pause
* color_forward — during fast forward
* color_rewind — during rewind
- drawVolumeDisplay() — volume icon and digits (00-100)
- drawPressedIcon() — pressed button icon with a fade-out timer
- drawCpuAndTemp() — CPU load and temperature with critical colors
- ALL functions are protected against execution in clock mode

10. clock.h

-------

Clock mode module (activates during inactivity):
- ClockState — clock state struct
- initNTP() — initializes NTP synchronization
- updateClockTime() — fetches time from NTP or internal clock
- checkClockMode() — checks mode activation conditions
- drawClock() — renders date and time
- updateClock() — main clock update loop
- resetInteractionTimer() — resets the timer on button press
Clock mode activates in two cases:
1) During inactivity (nothing playing) for more than 5 minutes
2) During a pause for more than 30 minutes
Any button press disables clock mode and restores the normal UI.

11. symbol.h

--------

Custom symbols module:
- degreeSymbol[] — degree symbol ° (7x14 pixels)
- colonBitmap[] — colon for time (10x72 pixels)
Used between hours and minutes in clock mode
- slashBitmap[] — slash for date (24x48 pixels)
Used between days and months in clock mode
- Size macros: COLON_WIDTH, COLON_HEIGHT, SLASH_WIDTH, SLASH_HEIGHT

12. bitmaps.h + bitmaps_data.h

--------------------------

- bitmaps.h — external icon array declarations (extern)
- bitmaps_data.h — array definitions with pixel data
- Includes separate color_play and color_pause icons to 
distinguish between playback and pause statuses

13. font_7x14.h

-----------

Main text font (Cyrillic + Latin + digits + symbols)
Size: 7x14 pixels, 161 glyphs

14. font_12x24_digits.h

-------------------

Font for volume digits
Size: 12x24 pixels, 10 glyphs (0-9)
Struct type: GlyphEntryDigits12x24

15. font_24x48_digits.h

-------------------

Font for date in clock mode
Size: 24x48 pixels, 10 glyphs (0-9)
Struct type: GlyphEntryDigits24x48

16. font_36x72_digits.h

-------------------

Font for time in clock mode
Size: 36x72 pixels, 10 glyphs (0-9)
Struct type: GlyphEntryDigits36x72

17. background.h

------------

Screen background image (160x128 pixels, RGB565)

18. comments.h

---------

This file contains the complete firmware documentation

19. pr3.ino

--------

Main file with setup() and loop() functions, as well as:
- handleIRCommand() — handles button presses
- updateKodiData() — polls and updates data from Kodi
- Global state variables
- 
================================================================================

STEP-BY-STEP OPERATION LOGIC

================================================================================

STAGE 1: INITIALIZATION (setup function)

-------------------------------------

1. Serial port initialization (115200 baud)
2. IR receiver pin configured as INPUT
3. IrReceiver library started without LED indication
4. TFT display hardware reset via RES pin:
- HIGH → delay(50) → LOW → delay(100) → HIGH → delay(100)
5. SPI initialization at 20 MHz
6. ST7735 display initialization (INITR_BLACKTAB)
7. Screen orientation set (rotation = 3, landscape)
8. performSequencedInitialization() called
9. On successful initialization:
- Initial UI drawn
- initNTP() called for time synchronization
- clockState.lastInteractionTime set to millis()

STAGE 2: SEQUENTIAL CONNECTION CHECK

---------------------------------------------

The performSequencedInitialization() function executes three stages:

STAGE 2.1: WiFi Connection

----------------------------

- Clears the screen with black
- Displays "Connecting to WI-FI .........." in white
- Calls WiFi.begin() with SSID and password from config.h
- Wait loop for up to 15 attempts with a 400 ms delay between them
- On success: displays "OK" in green, waits 1000 ms
- On failure: displays "error" in red, waits 2000 ms

STAGE 2.2: Server Check (ONLY if WiFi is connected)

-------------------------------------------------------

- Displays "Connecting to server ........" in white
- Creates a WiFiClient and attempts to connect to KODI_IP:KODI_PORT
- On success: displays "OK" in green, closes connection
- On failure: displays "error" in red
- If WiFi is not connected — immediately displays "error" in red

STAGE 2.3: Authentication (ONLY if server is reachable)

---------------------------------------------------

- Displays "Authenticating ...." in white
- Creates an HTTPClient with Basic Auth (KODI_USER:KODI_PASS)
- Sends JSON-RPC request: {"method":"JSONRPC.Ping","id":1}
- If response contains "pong" — displays "OK" in green
- Otherwise — displays "error" in red
- If server is unreachable — immediately displays "error"

STAGE 2.4: Finalization

---------------------

- Waits 5000 ms so the user can see all statuses
- If ALL three checks passed successfully:
* Draws the background image fullscreen
* Returns true
- If at least one check failed:
* Clears the screen with black
* Returns false

STAGE 3: MAIN LOOP (loop function)

------------------------------------

The loop runs continuously and includes the following checks:

3.1. Re-initialization check

---------------------------------------------

- If systemReady == false AND more than INIT_RETRY_DELAY (1000 ms) 
has passed since the last attempt:
* Calls performSequencedInitialization()
* On success, sets systemReady = true and draws the initial UI
* On failure, leaves systemReady = false
* Updates lastInitAttempt

3.2. IR signal processing

--------------------------

- Calls IrReceiver.decode() to check for a signal
- If signal received and protocol is NEC:
* Extracts button code (decodedRawData)
* Determines if it's a first press or a hold (IRDATA_FLAGS_IS_REPEAT flag)
* If it's a repeat signal with code 0x0 — uses lastCode from repeatState
* Calls handleIRCommand(code, isRepeat)
- Calls IrReceiver.resume() to prepare for the next signal

3.3. Button press handling (handleIRCommand)

-----------------------------------------------

- Resets interaction timer (resetInteractionTimer)
- If clock mode is active — disables it and restores UI
- Searches for the command in irCommands[] table by code
- If command found:
* Calls drawPressedIcon(code) — draws the button icon
* Sets pressedIconTimer = millis()

--- SPECIAL PLAY/PAUSE HANDLING ---
If code == 0xFB04FD00 (Play/Pause):
* Determines command based on currentState.playbackStatus:
- STOP (0), PAUSE (2), FWD (3), REW (4) → sends "play"
- PLAY (1) → sends "pause"
* Uses Input.ExecuteAction instead of Player.PlayPause

--- STANDARD HANDLING FOR OTHER BUTTONS ---
* If button supports auto-repeat (repeatType != REPEAT_NONE):
- On first press: saves state, sends command
- On hold: calculates interval via calculateRepeatInterval():
* For REPEAT_FIXED: always 1000 ms (channels)
* For REPEAT_ACCELERATED (volume, navigation):
  · 0-5 sec hold: 1000 ms interval
  · 5-10 sec hold: 500 ms interval
  · Over 10 sec: 250 ms interval
* If button has NO auto-repeat:
- Sends command ONLY on the first press

3.4. Hiding the pressed button icon

----------------------------------

- If pressedIconVisible == true AND more than PRESSED_ICON_DURATION 
(2000 ms) has passed since the last press:
* If clock mode is NOT active — clears the icon area with the background
* Sets pressedIconVisible = false

3.5. Periodic Kodi polling

-----------------------------

- If systemReady == true AND more than QUERY_INTERVAL (1000 ms) 
has passed since the last poll:
* Updates lastQueryTime
* Calls updateKodiData()

3.6. Clock mode update

----------------------------

- Calls updateClock(currentState.playbackStatus)
- The function checks activation conditions and, if necessary:
* Updates time from NTP or internal clock
* Redraws clock and date

STAGE 4: KODI DATA UPDATE (updateKodiData)

-----------------------------------------------

The function sends a batch HTTP request to Kodi with three JSON-RPC methods:

4.1. Batch request formation

-----------------------------------

An array of three requests is sent:
- id=1: Application.GetProperties (version, volume, muted)
- id=2: XBMC.GetInfoLabels (System.CpuUsage, System.CPUTemperature)
- id=3: Player.GetActivePlayers

4.2. Response handling

---------------------

For each element in the responses array:

If id == 1 (application properties):
- Extracts volume (integer) → currentState.volume
- Extracts muted (boolean) → currentState.muted

If id == 2 (system labels):
- Extracts System.CpuUsage as JsonVariant
- Calls parseCpuLoad() for universal parsing:
* Supports integers, floats, and strings
* For the format "#0: 0.0% #1: 16% #2: 0.0% #3: 0.0%" 
extracts all numbers and calculates the average value
- Result → currentState.cpuLoad
- Extracts System.CPUTemperature → parseTemperature()
- Result → currentState.temperature

If id == 3 (active players):
- If players array is empty:
* currentState.playbackStatus = 0 (nothing playing)
* currentState.progressBarVisible = false
- If there is an active player:
* Extracts playerId
* Sends an ADDITIONAL Player.GetProperties request with this playerId
* Extracts speed, time (hours/minutes/seconds), totaltime
* Determines playbackStatus based on speed value:
- speed == 0 → playbackStatus = 2 (PAUSE)
- speed == 1 → playbackStatus = 1 (PLAYBACK)
- speed > 1 → playbackStatus = 3 (FAST FORWARD)
- speed < 0 → playbackStatus = 4 (REWIND)
* Calculates currentSec and totalSec in seconds
* Sets progressBarVisible = true

4.3. Conditional UI redraw

----------------------------

REDRAW OCCURS ONLY IF CLOCK MODE IS INACTIVE:
- If playbackStatus changed → drawPlaybackStatus()
- If volume or muted changed → drawVolumeDisplay()
- If cpuLoad or temperature changed → drawCpuAndTemp()
- If progressBarVisible, currentSec, or totalSec changed 
→ drawProgressBar()
The previousState variable is ALWAYS updated (even in clock mode) 
so that the UI is in the actual state upon exiting clock mode.

4.4. Network error handling

--------------------------

- If sendKodiBatchRequest() returns false (HTTP or JSON parsing error):
* Sets systemReady = false
* Re-initialization will be triggered on the next loop iteration
* 
================================================================================

CLOCK MODE (clock.h)

================================================================================

PURPOSE:

-----------

Clock mode is a separate device operating mode where only the current date 
and time are displayed on the screen. It activates automatically during 
prolonged user inactivity.

TIME SOURCE:

-----------------

1. NTP server (primary source):
- Address set in config.h (NTP_SERVER = "172.16.76.59")
- Timezone set in config.h (NTP_TIMEZONE = 5 for UTC+5)
- Synchronizes on startup and every NTP_SYNC_INTERVAL (1 hour)
- Time and date color — COLOR_CLOCK_TIME and COLOR_CLOCK_DATE

2. ESP32 internal clock (fallback source):
- Activates if the NTP server is unavailable
- Requires prior synchronization via NTP
- Time and date color — COLOR_CLOCK_TIME_INTERNAL and 
COLOR_CLOCK_DATE_INTERNAL (usually red/orange)

ACTIVATION CONDITIONS:

------------------

1. Inactivity when stopped:
- playbackStatus == 0 (nothing playing)
- More than CLOCK_IDLE_TIMEOUT (5 minutes) passed since last press
- Clock mode activates
2. Prolonged pause:
- playbackStatus == 2 (pause)
- Pause lasts longer than CLOCK_PAUSE_TIMEOUT (30 minutes)
- Clock mode activates
3. Exiting the mode:
- Any button press on the IR remote
- resetInteractionTimer() is called
- Clock mode disables, normal UI restores

DISPLAY FORMAT:

-------------------

Date (24x48 font, top):
Format: DD/MM (e.g., "28/06")
Coordinates:
- Days: (CLOCK_DAY_X, CLOCK_DAY_Y)
- Slash: (CLOCK_SLASH_X, CLOCK_SLASH_Y)
- Months: (CLOCK_MONTH_X, CLOCK_MONTH_Y)
Slash is a custom 24x48 pixel symbol from symbol.h

Time (36x72 font, bottom):
Format: HH:MM (24-hour format, always 2 digits)
Coordinates:
- Hours: (CLOCK_HOURS_X, CLOCK_HOURS_Y)
- Colon: (CLOCK_COLON_X, CLOCK_COLON_Y)
- Minutes: (CLOCK_MINUTES_X, CLOCK_MINUTES_Y)
Colon is a custom 10x72 pixel symbol from symbol.h

UPDATE:

-----------

- Time updates every CLOCK_UPDATE_INTERVAL (1 minute)
- NTP synchronization every NTP_SYNC_INTERVAL (1 hour)
- Activation conditions checked on every loop iteration

CONFLICT PROTECTION:

---------------------

In clock mode:
- UI drawing functions are NOT called (drawPlaybackStatus, drawVolumeDisplay, 
drawCpuAndTemp, drawProgressBar)
- Data from Kodi continues to update (to ensure accuracy upon exit)
- previousState is updated for correct synchronization upon exit
- Pressed button icon is not cleared over the clock
- 
================================================================================

UI ELEMENTS RENDERING

================================================================================

ELEMENT 1: PLAYBACK STATUS (drawPlaybackStatus)

------------------------------------------------------

Coordinates: (STATUS_X=4, STATUS_Y=4), size 48x48
Logic:
- Clears 48x48 area with background from tft_background
- If playbackStatus == 1 → draws color_play
- If playbackStatus == 2 → draws color_pause
- If playbackStatus == 3 → draws color_forward
- If playbackStatus == 4 → draws color_rewind
- If playbackStatus == 0 → draws nothing (background only)

ELEMENT 2: VOLUME (drawVolumeDisplay)

----------------------------------------

Icon coordinates: (VOLUME_ICON_X, VOLUME_ICON_Y=4), size 48x48
Digit coordinates: (VOLUME_DIGITS_X, VOLUME_DIGITS_Y=16), size up to 36x24
Logic:
- If muted == true → draws color_mute
- If muted == false → draws color_volume
- Clears digit area with background
- Determines digit color:
* If muted → COLOR_VOLUME_MUTED (gray 0x8410)
* If not muted → COLOR_VOLUME_NORMAL (white)
- If volume == 100 → draws THREE digits "100"
- If volume < 100 → draws TWO digits with leading zero (00-99)
- Each digit is 12x24 pixels

ELEMENT 3: PRESSED BUTTON ICON (drawPressedIcon)

--------------------------------------------------

Coordinates: (PRESSED_ICON_X=4, PRESSED_ICON_Y=56), size 48x48
Logic:
- Clears 48x48 area with background
- Searches for icon in irCommands[] table by button code
- Draws the found icon
- Sets timer for 2000 ms
- 2000 ms after the LAST press, the area is cleared with background

ELEMENT 4: CPU AND TEMPERATURE (drawCpuAndTemp)

---------------------------------------------

CPU coordinates: (CPU_X=56, CPU_Y=56)
Temperature coordinates: (TEMP_X=56, TEMP_Y=CPU_Y+15)
Logic:
- Clears 100x30 area with background
- For CPU:
* Draws "CPU=" in COLOR_CPU_TEMP_LABEL (white)
* Draws value (00-100) in color:
- COLOR_CPU_TEMP_VALUE (white) if cpuLoad < CPU_CRITICAL_THRESHOLD (80%)
- COLOR_CPU_CRITICAL (red) if cpuLoad >= 80%
- "--" if data is unavailable (cpuLoad < 0)
* Draws "%" symbol in the same color as the value
- For temperature:
* Draws "T=" in COLOR_CPU_TEMP_LABEL (white)
* Draws value (00.0-99.9) in color:
- COLOR_CPU_TEMP_VALUE (white) if temperature < TEMP_CRITICAL_THRESHOLD (70°C)
- COLOR_TEMP_CRITICAL (red) if temperature >= 70°C
* Draws custom degree symbol ° (7x14 bitmap) in the same color
* Draws letter "C" in the same color

ELEMENT 5: PROGRESS BAR (drawProgressBar)

-----------------------------------------

Line coordinates: Y=119, width=160, height=3 pixels
Slider coordinates: height=8 pixels (3px line + 2px bottom + 3px top)
Time coordinates: Y=101 (119 - 4 - 14)
Logic:
- If progressBarVisible == false:
* Clears entire progress bar area with background
* previousSliderWidth = 0
* Returns
- Determines line color:
* If totalSec == 0 → COLOR_PROGRESS_ERROR (red)
* Otherwise → COLOR_PROGRESS_NORMAL (white)
- Calculates new slider width:
* progress = min(currentSec / totalSec, 1.0)
* newSliderWidth = progress * TFT_WIDTH (160)
- REDRAW OPTIMIZATION:
* If oldWidth > newSliderWidth (slider decreased):
- Clears ONLY the right part (from newSliderWidth to oldWidth)
* If oldWidth == 0 (first draw):
- Draws base line across full width
* If newSliderWidth > 0:
- Draws slider ONLY in the changed area
* Saves previousSliderWidth = newSliderWidth
- Formats time:
* If hours == 0 → "MM:SS" format (e.g., "05:30")
* If hours > 0 → "H:MM:SS" format (e.g., "1:05:30")
* Minutes and seconds ALWAYS have leading zeros (05 instead of 5)
- Clears time area with background
- Draws current time on the left with a 4-pixel indent
- Draws total time on the right with a 4-pixel indent from the right edge
- Text width is calculated accounting for character spacing

ELEMENT 6: CLOCK AND DATE (drawClock)

----------------------------------

Active only in clock mode (clockState.isClockModeActive == true)
Logic:
- Determines colors based on time source:
* If NTP is synced → COLOR_CLOCK_TIME / COLOR_CLOCK_DATE
* If internal clock → COLOR_CLOCK_TIME_INTERNAL / COLOR_CLOCK_DATE_INTERNAL
- Clears entire screen with background
- Draws date with 24x48 font:
* Two day digits (DD)
* Custom slash symbol (24x48)
* Two month digits (MM)
- Draws time with 36x72 font:
* Two hour digits (HH)
* Custom colon symbol (10x72)
* Two minute digits (MM)
* 
================================================================================

REACTION TO EXTERNAL EVENTS

================================================================================

EVENT 1: Loss of WiFi connection

---------------------------------

Detection: loop() checks WiFi.status() != WL_CONNECTED
Reaction:
- systemReady = false
- performSequencedInitialization() is called on the next loop iteration
- All three check statuses are displayed sequentially on the screen
- Upon connection restoration — returns to normal mode
- If restoration is impossible — retries every 1000 ms

EVENT 2: Kodi server unreachable

-------------------------------------

Detection: sendKodiBatchRequest() returns false
Reaction:
- systemReady = false
- Full re-initialization starts (WiFi → server → auth)

EVENT 3: Authentication error

-----------------------------

Detection: Response from JSONRPC.Ping does not contain "pong"
Reaction:
- "error" is displayed in red during initialization stage
- Transition to the main screen DOES NOT occur
- Retry attempt after 1000 ms

EVENT 4: IR remote button press

--------------------------------------

Detection: IrReceiver.decode() returns true
Reaction:
- Interaction timer is reset
- If clock mode is active — it disables, UI restores
- Visual: pressed button icon drawn for 2 seconds
- Network: corresponding JSON-RPC command sent to Kodi
- On button hold — auto-repeat with acceleration (for supported buttons)

EVENT 5: Kodi state change

-----------------------------------

Detection: During the next poll, data differs from previousState
Reaction:
- If clock mode is NOT active:
* ONLY the changed UI element is redrawn
* Other elements are not redrawn (optimization)
- If clock mode is active:
* Data is updated in memory, but UI is not redrawn
* previousState is synced for a correct mode exit

EVENT 6: Reaching critical values

------------------------------------------

Detection: cpuLoad >= 80% or temperature >= 70°C
Reaction:
- Value digits change color from white to red
- "CPU=" and "T=" labels remain white
- "%" and "°C" symbols also turn red

EVENT 7: Clock mode activation

---------------------------------

Detection: updateClock() checks conditions:
- playbackStatus == 0 and inactivity > 5 minutes
- playbackStatus == 2 and pause > 30 minutes
Reaction:
- clockState.isClockModeActive is set to true
- Screen is cleared
- Date and time displayed with correct colors
- UI elements stop redrawing

EVENT 8: Exiting clock mode

--------------------------------

Detection: Any button press on the IR remote
Reaction:
- Interaction timer is reset
- clockState.isClockModeActive = false
- Background image restores
- All UI elements redrawn with actual data

EVENT 9: Loss of NTP synchronization

-----------------------------------


Detection: getLocalTime() returns false in updateClockTime()
Reaction:
- clockState.isNtpSynced = false
- Time and date color changes to fallback (red/orange)
- ESP32 internal clock is used
- Periodic attempts to re-synchronize are made
- 
================================================================================

IR COMMANDS TABLE

================================================================================

CODE        BUTTON           AUTO-REPEAT        JSON-RPC METHOD

---------   --------------   ----------------   --------------------------------

0xEA15FD00  1                None               Input.SendText "1"
0xE916FD00  2                None               Input.SendText "2"
0xE817FD00  3                None               Input.SendText "3"
0xE619FD00  4                None               Input.SendText "4"
0xE51AFD00  5                None               Input.SendText "5"
0xE41BFD00  6                None               Input.SendText "6"
0xE21DFD00  7                None               Input.SendText "7"
0xE11EFD00  8                None               Input.SendText "8"
0xE01FFD00  9                None               Input.SendText "9"
0xBE41FD00  0                None               Input.SendText "0"
0xDF20FD00  VOL+             Accelerated        Application.SetVolume increment
0xDE21FD00  VOL-             Accelerated        Application.SetVolume decrement
0xDD22FD00  MUTE             None               Application.SetMute toggle
0xE718FD00  CH+              Fixed 1 sec        Input.ExecuteAction channelup
0xE31CFD00  CH-              Fixed 1 sec        Input.ExecuteAction channeldown
0xA55AFD00  REW              None               Input.ExecuteAction rewind
0xFB04FD00  PLAY/PAUSE       None               Input.ExecuteAction play/pause
                                               (depends on current state)
0xF10EFD00  FWD              None               Input.ExecuteAction fastforward
0xF40BFD00  UP               Accelerated        Input.Up
0xF00FFD00  DOWN             Accelerated        Input.Down
0xB649FD00  LEFT             Accelerated        Input.Left
0xB54AFD00  RIGHT            Accelerated        Input.Right
0xF20DFD00  OK               None               Input.Select
0xFD02FD00  BACK             None               Input.Back
0xF807FD00  MENU             None               Input.ExecuteAction home
0xA758FD00  CONTEXT MENU     None               Input.ContextMenu
0x9F60FD00  INFO             None               Input.Info
0xED12FD00  SCREEN TOGGLE    None               (local indication only)

================================================================================

PLAY/PAUSE BUTTON LOGIC

================================================================================

The Play/Pause button (0xFB04FD00) has a DUAL purpose and is handled 
specifically in the handleIRCommand() function:

Current Kodi State       Action on Press         Sent Command

────────────────────────────────────────────────────────────────────────

STOP (0)               → PLAY (start)          Input.ExecuteAction "play"
PLAY (1)               → PAUSE (pause)         Input.ExecuteAction "pause"
PAUSE (2)              → PLAY (resume)         Input.ExecuteAction "play"
FAST FORWARD (3)       → PLAY (normal speed)   Input.ExecuteAction "play"
REWIND (4)             → PLAY (normal speed)   Input.ExecuteAction "play"

Implementation details:
- Uses Input.ExecuteAction instead of Player.PlayPause
- Does not require knowing the playerid (works at the Kodi interface level)
- Returns to normal playback speed when rewinding/fast-forwarding
- State is taken from currentState.playbackStatus (updated from API)
================================================================================
CPU LOAD PARSING
================================================================================
Kodi returns CPU load in the format:
"#0: 0.0% #1: 16% #2: 0.0% #3: 0.0%"
This is detailed information for each CPU core. The parseCpuLoad() 
function performs the following actions:
1. Extracts all numbers from the string (0.0, 16, 0.0, 0.0)
2. Calculates the average value: (0.0 + 16 + 0.0 + 0.0) / 4 = 4%
3. Returns the rounded integer value

Supported formats:
- Integers: "50" → 50
- Floats: "0.5" → 50 (multiplied by 100)
- Strings with percentages: "50%" → 50
- Multi-core format: "#0: 0.0% #1: 16% ..." → average value
- 
================================================================================

PERFORMANCE OPTIMIZATIONS

================================================================================

1. BINARY SEARCH FOR GLYPHS
Instead of a linear search through 161 characters, binary search is used.
Complexity: O(log N) ≈ 7-8 comparisons instead of 161.
Requirement: the glyphs array must be sorted by character code.

2. BATCH JSON-RPC REQUESTS
Three requests to Kodi are combined into a single HTTP request.
Savings: ~60% time on network operations (one TCP handshake instead of three).

3. CONDITIONAL UI REDRAW
Elements are redrawn ONLY when their state changes.
currentState is compared with previousState before each draw.

4. PARTIAL PROGRESS BAR REDRAW
When the slider position changes, ONLY the changed area is cleared and drawn.
The base line is drawn once on the first display.

5. STREAMING JSON PARSING
Using http.getStream() instead of getString() for ArduinoJson.
RAM savings: JSON is not loaded entirely into a string.

6. STORING DATA IN PROGMEM
All fonts, icons, and backgrounds are stored in Flash memory, not taking up RAM.

7. PROTECTION AGAINST UNNECESSARY REDRAWS IN CLOCK MODE
In clock mode, UI elements are not redrawn, which saves resources 
and prevents elements from overlapping the clock.

8. SEPARATION OF FONT STRUCT TYPES
Each font uses its own struct type (GlyphEntry, GlyphEntryDigits12x24, 
GlyphEntryDigits24x48, GlyphEntryDigits36x72) to avoid conflicts 
during compilation.

================================================================================

POSSIBLE PROBLEMS AND SOLUTIONS

================================================================================

PROBLEM 1: Device does not connect to WiFi
SOLUTION: Check WIFI_SSID and WIFI_PASS in config.h, ensure signal is present

PROBLEM 2: Authentication error
SOLUTION: Check KODI_USER and KODI_PASS, ensure web server is enabled in Kodi 
and remote connections are allowed

PROBLEM 3: IR remote does not respond
SOLUTION: Check IR_RECEIVER_PIN, ensure the remote uses the NEC protocol

PROBLEM 4: Screen flickering
SOLUTION: Conditional redraw is used — elements update only when data changes. 
If flickering persists — increase QUERY_INTERVAL

PROBLEM 5: Unstable operation on poor WiFi
SOLUTION: Increase SERVER_CONNECT_TIMEOUT and AUTH_TIMEOUT in timings.h, 
decrease QUERY_INTERVAL for faster problem detection

PROBLEM 6: Clock time is not synchronized
SOLUTION: Check NTP_SERVER availability, ensure NTP_TIMEZONE is correct. 
If NTP is unavailable, internal clock is used (red color).

PROBLEM 7: CPU shows 0%
SOLUTION: System.CpuUsage is used instead of System.CpuLoad. 
The parseCpuLoad() function automatically calculates the average across cores.

PROBLEM 8: Last time character is cut off
SOLUTION: The getTextWidth() function accounts for character spacing (spacingX=1). 
Formula: (length * FONT_WIDTH) + ((length-1) * 1)

================================================================================

CONCLUSION

================================================================================

The firmware is a complete solution for creating a custom remote control 
for the Kodi media center with a rich interface and feedback.
Key features:
- Modular architecture with a clear separation of responsibilities
- All customizable parameters are moved to separate files
- Optimized rendering for smooth interface operation
- Support for button auto-repeat with acceleration
- Visual indication of critical system states
- Resilience to connection loss with automatic recovery
- Clock mode with NTP synchronization and fallback time source
- Intelligent Play/Pause button handling based on current state
- Universal CPU load parsing with multi-core system support
To make changes to the interface, it is sufficient to edit the 
color_and_coordinates.h and timings.h files without touching the program logic.

================================================================================

================================================================================

                    ПОЛНОЕ ОПИСАНИЕ ПРОГРАММЫ УПРАВЛЕНИЯ KODI
                    
================================================================================


НАЗНАЧЕНИЕ УСТРОЙСТВА:

----------------------

Данная прошивка предназначена для микроконтроллера ESP32-C3 с подключенным 
TFT-дисплеем ST7735 (160x128 пикселей) и ИК-приемником. Устройство выполняет 
функции кастомного пульта дистанционного управления и монитора состояния для 
медиацентра Kodi, работающего на удаленном сервере (Debian Linux).

Устройство подключается к локальной сети WiFi, устанавливает соединение с 
сервером Kodi по протоколу HTTP (порт 8080), авторизуется и далее:
  1) Принимает команды с ИК-пульта и передает их в Kodi через JSON-RPC API
  2) Периодически опрашивает Kodi для получения системной информации
  3) Отображает на экране статус воспроизведения, громкость, температуру CPU 
     и другие параметры
  4) Визуально индицирует нажатия кнопок пульта
  5) Переходит в режим часов при длительном бездействии или паузе


================================================================================

                         АРХИТЕКТУРА ПРОГРАММЫ
                         
================================================================================


ПРОГРАММА РАЗДЕЛЕНА НА СЛЕДУЮЩИЕ ФАЙЛЫ:

---------------------------------------


1. config.h

   --------
   
   Содержит сетевые настройки и параметры подключения:
   - WIFI_SSID, WIFI_PASS — имя и пароль WiFi сети
   - KODI_IP, KODI_PORT — IP-адрес и порт сервера Kodi
   - KODI_USER, KODI_PASS — логин и пароль для авторизации в Kodi
   - Пины микроконтроллера для подключения TFT-дисплея и ИК-приемника
   - NTP_SERVER — адрес NTP-сервера для синхронизации времени
   - NTP_TIMEZONE — часовой пояс в виде числа (например, 5 для UTC+5)

3. color_and_coordinates.h

   ------------------------
   
   Содержит ВСЕ настраиваемые параметры интерфейса:
   - Цвета всех элементов (статусы, текст, цифры, прогресс-бар)
   - Цвета для режима часов (отдельно для NTP и внутренних часов)
   - Координаты X и Y для каждого элемента на экране
   - Координаты для часов, минут, дней, месяцев, разделителей
   - Размеры элементов (ширина/высота экрана, иконок, шрифтов)
   - Отступы между элементами
   - Пороговые значения для критических параметров (CPU, температура)
   - Цвета критических предупреждений
   
   Этот файл позволяет полностью настроить внешний вид интерфейса без 
   вмешательства в логику программы.

5. timings.h

   ----------
   
   Содержит ВСЕ временные параметры программы:
   - Таймауты подключения WiFi и сервера
   - Интервалы между попытками переподключения
   - Задержки отображения статусов на экране
   - Интервал опроса Kodi (по умолчанию 1000 мс)
   - Длительность показа иконки нажатой кнопки (2000 мс)
   - Параметры автоповтора кнопок (интервалы и фазы ускорения)
   - Тайминги режима часов:
     * CLOCK_IDLE_TIMEOUT — время бездействия для активации (5 минут)
     * CLOCK_PAUSE_TIMEOUT — время паузы для активации (30 минут)
     * CLOCK_UPDATE_INTERVAL — интервал обновления (1 минута)
     * NTP_SYNC_INTERVAL — интервал синхронизации с NTP (1 час)

7. display.h

   ----------
   
   Модуль работы с дисплеем и шрифтами:
   - Функции бинарного поиска глифов в шрифтах (7x14 и 12x24)
   - Декодирование UTF-8 строк для поддержки кириллицы
   - Функция printText() для вывода текста с переносом строк
   - Функция clearArea() для очистки области экрана фоном
   - Функция getTextWidth() с учётом отступов между символами

9. connection.h

   -------------
   
   Модуль управления подключениями:
   - performSequencedInitialization() — последовательная проверка:
     * Подключение к WiFi (до 15 попыток)
     * Проверка доступности сервера Kodi (TCP-соединение)
     * Авторизация через HTTP Basic Auth (JSON-RPC Ping)
   - Вывод статусов "OK" (зеленый) или "error" (красный) на экран
   - При успехе — переход на главный экран с фоном
   - При ошибке — очистка экрана и ожидание повторной попытки

11. ir_control.h
   
   -------------
   
   Модуль обработки ИК-сигналов:
   - Структура IrCommand — описание каждой кнопки (код, иконка, JSON-команда)
   - Таблица irCommands[] — все 28 кнопок пульта с их параметрами
   - Enum RepeatType — типы автоповтора (NONE, FIXED, ACCELERATED)
   - Функция calculateRepeatInterval() — расчет интервала автоповтора 
     с учетом времени удержания кнопки

11. kodi_api.h
   
   -----------
   
   Модуль взаимодействия с Kodi через HTTP:
   - sendKodiBatchRequest() — отправка пакетного JSON-RPC запроса
   - sendKodiCommand() — отправка одиночной команды управления
   - parseTemperature() — парсинг строки температуры из ответа API
   - parseCpuLoad() — универсальный парсинг загрузки CPU, поддерживает:
     * Целые числа (например, 50)
     * Дробные числа (например, 0.5 = 50%)
     * Строки формата "#0: 0.0% #1: 16% #2: 0.0% #3: 0.0%"
       (вычисляет среднюю загрузку по всем ядрам процессора)

11. progress_bar.h
   
   ---------------
   
   Модуль прогресс-бара:
   - formatTime() — форматирование секунд в строку "Ч:ММ:СС" или "ММ:СС"
   - drawProgressBar() — отрисовка прогресс-бара с оптимизацией:
     * Рисует только изменившиеся части (без полного перерисовывания)
     * При totalSec=0 показывает красную линию без ползунка
     * При currentSec>totalSec ограничивает прогресс значением 1.0
     * НЕ отрисовывается в режиме часов

11. ui_elements.h
   
   --------------
   
   Модуль отрисовки UI-элементов:
   - drawPlaybackStatus() — иконка статуса воспроизведения:
     * color_play — при обычном воспроизведении
     * color_pause — при паузе
     * color_forward — при перемотке вперед
     * color_rewind — при перемотке назад
   - drawVolumeDisplay() — иконка громкости и цифры (00-100)
   - drawPressedIcon() — иконка нажатой кнопки с таймером исчезновения
   - drawCpuAndTemp() — загрузка CPU и температура с критическими цветами
   - ВСЕ функции защищены от вызова в режиме часов

11. clock.h

    -------
    
    Модуль режима часов (активируется при бездействии):
    - ClockState — структура состояния часов
    - initNTP() — инициализация NTP-синхронизации
    - updateClockTime() — получение времени из NTP или внутренних часов
    - checkClockMode() — проверка условий активации режима
    - drawClock() — отрисовка даты и времени
    - updateClock() — главный цикл обновления часов
    - resetInteractionTimer() — сброс таймера при нажатии кнопки
    
    Режим часов активируется в двух случаях:
    1) При бездействии (ничего не воспроизводится) более 5 минут
    2) При паузе более 30 минут
    
    При любом нажатии кнопки режим часов отключается и восстанавливается
    обычный UI.

13. symbol.h

    --------
    
    Модуль кастомных символов:
    - degreeSymbol[] — символ градуса ° (7x14 пикселей)
    - colonBitmap[] — двоеточие для времени (10x72 пикселя)
      Используется между часами и минутами в режиме часов
    - slashBitmap[] — слеш для даты (24x48 пикселей)
      Используется между днями и месяцами в режиме часов
    - Макросы размеров: COLON_WIDTH, COLON_HEIGHT, SLASH_WIDTH, SLASH_HEIGHT

15. bitmaps.h + bitmaps_data.h

    --------------------------
    
    - bitmaps.h — объявления внешних массивов иконок (extern)
    - bitmaps_data.h — определения массивов с пиксельными данными
    - Включают отдельные иконки color_play и color_pause для 
      различения статуса воспроизведения и паузы

17. font_7x14.h

    -----------
    
    Основной шрифт для текста (кириллица + латиница + цифры + символы)
    Размер: 7x14 пикселей, 161 глиф

19. font_12x24_digits.h

    -------------------
    
    Шрифт для цифр громкости
    Размер: 12x24 пикселя, 10 глифов (0-9)
    Тип структуры: GlyphEntryDigits12x24

21. font_24x48_digits.h
   
    -------------------
    
    Шрифт для даты в режиме часов
    Размер: 24x48 пикселя, 10 глифов (0-9)
    Тип структуры: GlyphEntryDigits24x48

23. font_36x72_digits.h

    -------------------
    
    Шрифт для времени в режиме часов
    Размер: 36x72 пикселя, 10 глифов (0-9)
    Тип структуры: GlyphEntryDigits36x72

25. background.h

     ------------
    
    Фоновое изображение экрана (160x128 пикселей, RGB565)

27. comments.h

     ---------
    
    Данный файл — полная документация программы

29. pr3.ino

    --------
    
    Главный файл с функциями setup() и loop(), а также:
    - handleIRCommand() — обработка нажатий кнопок
    - updateKodiData() — опрос и обновление данных с Kodi
    - Глобальные переменные состояния

================================================================================

                         ПОШАГОВАЯ ЛОГИКА РАБОТЫ
                         
================================================================================


ЭТАП 1: ИНИЦИАЛИЗАЦИЯ (функция setup)

-------------------------------------

1. Инициализация Serial-порта (115200 бод)
2. Настройка пина ИК-приемника как INPUT
3. Запуск библиотеки IrReceiver без LED-индикации
4. Аппаратный сброс TFT-дисплея через пин RES:
   - HIGH → delay(50) → LOW → delay(100) → HIGH → delay(100)
5. Инициализация SPI с частотой 20 МГц
6. Инициализация дисплея ST7735 (INITR_BLACKTAB)
7. Установка ориентации экрана (rotation = 3, альбомная)
8. Вызов performSequencedInitialization()
9. При успешной инициализации:
   - Рисование начального UI
   - Вызов initNTP() для синхронизации времени
   - Установка clockState.lastInteractionTime = millis()

ЭТАП 2: ПОСЛЕДОВАТЕЛЬНАЯ ПРОВЕРКА ПОДКЛЮЧЕНИЙ

---------------------------------------------

Функция performSequencedInitialization() выполняет три этапа:

  ЭТАП 2.1: Подключение к WiFi
  
  ----------------------------
  
  - Очищает экран черным цветом
  - Выводит текст "Подключение к WI-FI .........." белым цветом
  - Вызывает WiFi.begin() с SSID и паролем из config.h
  - Цикл ожидания до 15 попыток с задержкой 400 мс между ними
  - При успехе: выводит "OK" зеленым, ждет 1000 мс
  - При ошибке: выводит "error" красным, ждет 2000 мс

  ЭТАП 2.2: Проверка сервера (ТОЛЬКО если WiFi подключен)
  
  -------------------------------------------------------
  
  - Выводит текст "Подключение к серверу ........" белым
  - Создает WiFiClient и пытается подключиться к KODI_IP:KODI_PORT
  - При успехе: выводит "OK" зеленым, закрывает соединение
  - При ошибке: выводит "error" красным
  - Если WiFi не подключен — сразу выводит "error" красным

  ЭТАП 2.3: Авторизация (ТОЛЬКО если сервер доступен)
  
  ---------------------------------------------------
  
  - Выводит текст "Авторизация ...." белым
  - Создает HTTPClient с Basic Auth (KODI_USER:KODI_PASS)
  - Отправляет JSON-RPC запрос: {"method":"JSONRPC.Ping","id":1}
  - Если ответ содержит "pong" — выводит "OK" зеленым
  - Иначе — выводит "error" красным
  - Если сервер недоступен — сразу выводит "error"

  ЭТАП 2.4: Финализация
  
  ---------------------
  
  - Ждет 5000 мс, чтобы пользователь увидел все статусы
  - Если ВСЕ три проверки прошли успешно:
    * Рисует фоновое изображение на весь экран
    * Возвращает true
  - Если хотя бы одна проверка провалилась:
    * Очищает экран черным
    * Возвращает false

ЭТАП 3: ОСНОВНОЙ ЦИКЛ (функция loop)

------------------------------------
Цикл выполняется непрерывно и содержит следующие проверки:


  3.1. Проверка необходимости переинициализации
  
  ---------------------------------------------
  - Если systemReady == false И прошло больше INIT_RETRY_DELAY (1000 мс) 
    с последней попытки:
    * Вызывает performSequencedInitialization()
    * При успехе устанавливает systemReady = true и рисует начальный UI
    * При ошибке оставляет systemReady = false
    * Обновляет lastInitAttempt

  3.2. Обработка ИК-сигналов
  
  --------------------------
  
  - Вызывает IrReceiver.decode() для проверки наличия сигнала
  - Если сигнал принят и протокол NEC:
    * Извлекает код кнопки (decodedRawData)
    * Определяет, это первое нажатие или удержание (флаг IRDATA_FLAGS_IS_REPEAT)
    * Если это repeat-сигнал с кодом 0x0 — использует lastCode из repeatState
    * Вызывает handleIRCommand(code, isRepeat)
  - Вызывает IrReceiver.resume() для готовности к следующему сигналу

  3.3. Обработка нажатия кнопки (handleIRCommand)
  
  -----------------------------------------------
  
  - Сбрасывает таймер взаимодействия (resetInteractionTimer)
  - Если активен режим часов — отключает его и восстанавливает UI
  - Ищет команду в таблице irCommands[] по коду
  - Если команда найдена:
    * Вызывает drawPressedIcon(code) — рисует иконку кнопки
    * Устанавливает pressedIconTimer = millis()
    
    --- СПЕЦИАЛЬНАЯ ОБРАБОТКА PLAY/PAUSE ---
    Если код == 0xFB04FD00 (Play/Pause):
    * Определяет команду на основе currentState.playbackStatus:
      - STOP (0), PAUSE (2), FWD (3), REW (4) → отправляет "play"
      - PLAY (1) → отправляет "pause"
    * Использует Input.ExecuteAction вместо Player.PlayPause
    
    --- СТАНДАРТНАЯ ОБРАБОТКА ОСТАЛЬНЫХ КНОПОК ---
    * Если кнопка поддерживает автоповтор (repeatType != REPEAT_NONE):
      - При первом нажатии: сохраняет состояние, отправляет команду
      - При удержании: рассчитывает интервал через calculateRepeatInterval():
        * Для REPEAT_FIXED: всегда 1000 мс (каналы)
        * Для REPEAT_ACCELERATED (громкость, навигация):
          · 0-5 сек удержания: интервал 1000 мс
          · 5-10 сек удержания: интервал 500 мс
          · Более 10 сек: интервал 250 мс
    * Если кнопка БЕЗ автоповтора:
      - Отправляет команду ТОЛЬКО при первом нажатии

  3.4. Скрытие иконки нажатой кнопки
  
  ----------------------------------
  
  - Если pressedIconVisible == true И прошло больше PRESSED_ICON_DURATION 
    (2000 мс) с момента последнего нажатия:
    * Если режим часов НЕ активен — очищает область иконки фоном
    * Устанавливает pressedIconVisible = false

  3.5. Периодический опрос Kodi
  
  -----------------------------
  
  - Если systemReady == true И прошло больше QUERY_INTERVAL (1000 мс) 
    с последнего опроса:
    * Обновляет lastQueryTime
    * Вызывает updateKodiData()

  3.6. Обновление режима часов
  
  ----------------------------
  
  - Вызывает updateClock(currentState.playbackStatus)
  - Функция проверяет условия активации и при необходимости:
    * Обновляет время из NTP или внутренних часов
    * Перерисовывает часы и дату

ЭТАП 4: ОБНОВЛЕНИЕ ДАННЫХ KODI (updateKodiData)

-----------------------------------------------

Функция выполняет пакетный HTTP-запрос к Kodi с тремя JSON-RPC методами:

  4.1. Формирование пакетного запроса
  
  -----------------------------------
  
  Отправляется массив из трех запросов:
  - id=1: Application.GetProperties (version, volume, muted)
  - id=2: XBMC.GetInfoLabels (System.CpuUsage, System.CPUTemperature)
  - id=3: Player.GetActivePlayers

  4.2. Обработка ответа
  
  ---------------------
  
  Для каждого элемента массива responses:
  
  Если id == 1 (свойства приложения):
  - Извлекает volume (целое число) → currentState.volume
  - Извлекает muted (boolean) → currentState.muted
  
  Если id == 2 (системные метки):
  - Извлекает System.CpuUsage как JsonVariant
  - Вызывает parseCpuLoad() для универсального парсинга:
    * Поддерживает целые числа, дробные числа и строки
    * Для формата "#0: 0.0% #1: 16% #2: 0.0% #3: 0.0%"
      извлекает все числа и вычисляет среднее значение
  - Результат → currentState.cpuLoad
  - Извлекает System.CPUTemperature → parseTemperature()
  - Результат → currentState.temperature
  
  Если id == 3 (активные плееры):
  - Если массив players пуст:
    * currentState.playbackStatus = 0 (ничего не воспроизводится)
    * currentState.progressBarVisible = false
  - Если есть активный плеер:
    * Извлекает playerId
    * Отправляет ДОПОЛНИТЕЛЬНЫЙ запрос Player.GetProperties с этим playerId
    * Извлекает speed, time (hours/minutes/seconds), totaltime
    * Определяет playbackStatus по значению speed:
      - speed == 0 → playbackStatus = 2 (ПАУЗА)
      - speed == 1 → playbackStatus = 1 (ВОСПРОИЗВЕДЕНИЕ)
      - speed > 1 → playbackStatus = 3 (ПЕРЕМОТКА ВПЕРЕД)
      - speed < 0 → playbackStatus = 4 (ПЕРЕМОТКА НАЗАД)
    * Вычисляет currentSec и totalSec в секундах
    * Устанавливает progressBarVisible = true

  4.3. Условная перерисовка UI
  
  ----------------------------
  
  ПЕРЕРИСОВКА ПРОИСХОДИТ ТОЛЬКО ЕСЛИ РЕЖИМ ЧАСОВ НЕ АКТИВЕН:
  - Если playbackStatus изменился → drawPlaybackStatus()
  - Если volume или muted изменились → drawVolumeDisplay()
  - Если cpuLoad или temperature изменились → drawCpuAndTemp()
  - Если progressBarVisible, currentSec или totalSec изменились 
    → drawProgressBar()
  
  Переменная previousState обновляется ВСЕГДА (даже в режиме часов),
  чтобы при выходе из режима часов UI был в актуальном состоянии.

  4.4. Обработка ошибок сети
  
  --------------------------
  
  - Если sendKodiBatchRequest() вернул false (ошибка HTTP или парсинга JSON):
    * Устанавливает systemReady = false
    * На следующем витке loop() будет вызвана переинициализация


================================================================================

                    РЕЖИМ ЧАСОВ (clock.h)
                    
================================================================================


НАЗНАЧЕНИЕ:

-----------

Режим часов — это отдельный режим работы устройства, при котором на экране
отображаются только текущие дата и время. Режим активируется автоматически
при длительном бездействии пользователя.

ИСТОЧНИК ВРЕМЕНИ:

-----------------

1. NTP-сервер (приоритетный источник):
   - Адрес задаётся в config.h (NTP_SERVER = "172.16.76.59")
   - Часовой пояс задаётся в config.h (NTP_TIMEZONE = 5 для UTC+5)
   - Синхронизация при запуске и каждые NTP_SYNC_INTERVAL (1 час)
   - Цвет времени и даты — COLOR_CLOCK_TIME и COLOR_CLOCK_DATE

2. Внутренние часы ESP32 (резервный источник):
   - Активируются, если NTP-сервер недоступен
   - Требуют предварительной синхронизации через NTP
   - Цвет времени и даты — COLOR_CLOCK_TIME_INTERNAL и 
     COLOR_CLOCK_DATE_INTERNAL (обычно красный/оранжевый)

УСЛОВИЯ АКТИВАЦИИ:

------------------

1. Бездействие при остановке:
   - playbackStatus == 0 (ничего не воспроизводится)
   - Прошло больше CLOCK_IDLE_TIMEOUT (5 минут) с последнего нажатия
   - Активируется режим часов

2. Длительная пауза:
   - playbackStatus == 2 (пауза)
   - Пауза длится больше CLOCK_PAUSE_TIMEOUT (30 минут)
   - Активируется режим часов

3. Выход из режима:
   - Любое нажатие кнопки на ИК-пульте
   - Вызывается resetInteractionTimer()
   - Режим часов отключается, восстанавливается обычный UI

ФОРМАТ ОТОБРАЖЕНИЯ:

-------------------

Дата (шрифт 24x48, сверху):
  Формат: ДД/ММ (например, "28/06")
  Координаты:
  - Дни: (CLOCK_DAY_X, CLOCK_DAY_Y)
  - Слеш: (CLOCK_SLASH_X, CLOCK_SLASH_Y)
  - Месяцы: (CLOCK_MONTH_X, CLOCK_MONTH_Y)
  Слеш — кастомный символ 24x48 пикселя из symbol.h

Время (шрифт 36x72, снизу):
  Формат: ЧЧ:ММ (24-часовой формат, всегда 2 цифры)
  Координаты:
  - Часы: (CLOCK_HOURS_X, CLOCK_HOURS_Y)
  - Двоеточие: (CLOCK_COLON_X, CLOCK_COLON_Y)
  - Минуты: (CLOCK_MINUTES_X, CLOCK_MINUTES_Y)
  Двоеточие — кастомный символ 10x72 пикселя из symbol.h

ОБНОВЛЕНИЕ:

-----------

- Время обновляется каждые CLOCK_UPDATE_INTERVAL (1 минута)
- NTP-синхронизация каждые NTP_SYNC_INTERVAL (1 час)
- Проверка условий активации — в каждом витке loop()

ЗАЩИТА ОТ КОНФЛИКТОВ:

---------------------

В режиме часов:
- НЕ вызываются функции отрисовки UI (drawPlaybackStatus, drawVolumeDisplay,
  drawCpuAndTemp, drawProgressBar)
- Данные с Kodi продолжают обновляться (для актуальности при выходе)
- previousState обновляется для корректной синхронизации при выходе
- Иконка нажатой кнопки не очищается поверх часов

================================================================================

                         ОТРИСОВКА UI-ЭЛЕМЕНТОВ
                         
================================================================================


ЭЛЕМЕНТ 1: СТАТУС ВОСПРОИЗВЕДЕНИЯ (drawPlaybackStatus)

------------------------------------------------------

Координаты: (STATUS_X=4, STATUS_Y=4), размер 48x48
Логика:
  - Очищает область 48x48 фоном из tft_background
  - Если playbackStatus == 1 → рисует color_play
  - Если playbackStatus == 2 → рисует color_pause
  - Если playbackStatus == 3 → рисует color_forward
  - Если playbackStatus == 4 → рисует color_rewind
  - Если playbackStatus == 0 → ничего не рисует (только фон)

ЭЛЕМЕНТ 2: ГРОМКОСТЬ (drawVolumeDisplay)

----------------------------------------

Координаты иконки: (VOLUME_ICON_X, VOLUME_ICON_Y=4), размер 48x48
Координаты цифр: (VOLUME_DIGITS_X, VOLUME_DIGITS_Y=16), размер до 36x24
Логика:
  - Если muted == true → рисует color_mute
  - Если muted == false → рисует color_volume
  - Очищает область цифр фоном
  - Определяет цвет цифр:
    * Если muted → COLOR_VOLUME_MUTED (серый 0x8410)
    * Если не muted → COLOR_VOLUME_NORMAL (белый)
  - Если volume == 100 → рисует ТРИ цифры "100"
  - Если volume < 100 → рисует ДВЕ цифры с ведущим нулем (00-99)
  - Каждая цифра имеет размер 12x24 пикселя

ЭЛЕМЕНТ 3: ИКОНКА НАЖАТОЙ КНОПКИ (drawPressedIcon)

--------------------------------------------------

Координаты: (PRESSED_ICON_X=4, PRESSED_ICON_Y=56), размер 48x48
Логика:
  - Очищает область 48x48 фоном
  - Ищет иконку в таблице irCommands[] по коду кнопки
  - Рисует найденную иконку
  - Устанавливает таймер на 2000 мс
  - Через 2000 мс после ПОСЛЕДНЕГО нажатия область очищается фоном

ЭЛЕМЕНТ 4: CPU И ТЕМПЕРАТУРА (drawCpuAndTemp)

---------------------------------------------

Координаты CPU: (CPU_X=56, CPU_Y=56)
Координаты температуры: (TEMP_X=56, TEMP_Y=CPU_Y+15)
Логика:
  - Очищает область 100x30 фоном
  - Для CPU:
    * Рисуется "CPU=" цветом COLOR_CPU_TEMP_LABEL (белый)
    * Рисуется значение (00-100) цветом:
      - COLOR_CPU_TEMP_VALUE (белый), если cpuLoad < CPU_CRITICAL_THRESHOLD (80%)
      - COLOR_CPU_CRITICAL (красный), если cpuLoad >= 80%
      - "--" если данные недоступны (cpuLoad < 0)
    * Рисуется символ "%" тем же цветом, что и значение
  - Для температуры:
    * Рисуется "T=" цветом COLOR_CPU_TEMP_LABEL (белый)
    * Рисуется значение (00.0-99.9) цветом:
      - COLOR_CPU_TEMP_VALUE (белый), если temperature < TEMP_CRITICAL_THRESHOLD (70°C)
      - COLOR_TEMP_CRITICAL (красный), если temperature >= 70°C
    * Рисуется кастомный символ градуса ° (битмап 7x14) тем же цветом
    * Рисуется буква "C" тем же цветом

ЭЛЕМЕНТ 5: ПРОГРЕСС-БАР (drawProgressBar)

-----------------------------------------

Координаты линии: Y=119, ширина=160, высота=3 пикселя
Координаты ползунка: высота=8 пикселей (3 пикселя линии + 2 снизу + 3 сверху)
Координаты времени: Y=101 (119 - 4 - 14)
Логика:
  - Если progressBarVisible == false:
    * Очищает всю область прогресс-бара фоном
    * previousSliderWidth = 0
    * Возврат
  
  - Определяет цвет линии:
    * Если totalSec == 0 → COLOR_PROGRESS_ERROR (красный)
    * Иначе → COLOR_PROGRESS_NORMAL (белый)
  
  - Вычисляет новую ширину ползунка:
    * progress = min(currentSec / totalSec, 1.0)
    * newSliderWidth = progress * TFT_WIDTH (160)
  
  - ОПТИМИЗАЦИЯ ПЕРЕРИСОВКИ:
    * Если oldWidth > newSliderWidth (ползунок уменьшился):
      - Очищает ТОЛЬКО правую часть (от newSliderWidth до oldWidth)
    * Если oldWidth == 0 (первая отрисовка):
      - Рисует базовую линию на всю ширину
    * Если newSliderWidth > 0:
      - Рисует ползунок ТОЛЬКО в изменившейся области
    * Сохраняет previousSliderWidth = newSliderWidth
  
  - Форматирует время:
    * Если hours == 0 → формат "ММ:СС" (например, "05:30")
    * Если hours > 0 → формат "Ч:ММ:СС" (например, "1:05:30")
    * Минуты и секунды ВСЕГДА с ведущим нулем (05 вместо 5)
  
  - Очищает область времени фоном
  - Рисует текущее время слева с отступом 4 пикселя
  - Рисует общее время справа с отступом 4 пикселя от правого края
  - Ширина текста вычисляется с учётом отступов между символами

ЭЛЕМЕНТ 6: ЧАСЫ И ДАТА (drawClock)

----------------------------------

Активен только в режиме часов (clockState.isClockModeActive == true)
Логика:
  - Определяет цвета в зависимости от источника времени:
    * Если NTP синхронизирован → COLOR_CLOCK_TIME / COLOR_CLOCK_DATE
    * Если внутренние часы → COLOR_CLOCK_TIME_INTERNAL / COLOR_CLOCK_DATE_INTERNAL
  - Очищает весь экран фоном
  - Рисует дату шрифтом 24x48:
    * Две цифры дня (ДД)
    * Кастомный символ слеша (24x48)
    * Две цифры месяца (ММ)
  - Рисует время шрифтом 36x72:
    * Две цифры часов (ЧЧ)
    * Кастомный символ двоеточия (10x72)
    * Две цифры минут (ММ)

================================================================================

                         РЕАКЦИЯ НА ВНЕШНИЕ СОБЫТИЯ
                         
================================================================================


СОБЫТИЕ 1: Потеря WiFi соединения

---------------------------------

Обнаружение: В loop() проверяется WiFi.status() != WL_CONNECTED
Реакция:
  - systemReady = false
  - На следующем витке loop() вызывается performSequencedInitialization()
  - На экране последовательно отображаются все три статуса проверки
  - При восстановлении связи — возврат к нормальному режиму
  - При невозможности восстановления — повтор через 1000 мс

СОБЫТИЕ 2: Недоступность сервера Kodi

-------------------------------------

Обнаружение: sendKodiBatchRequest() возвращает false
Реакция:
  - systemReady = false
  - Запускается полная переинициализация (WiFi → сервер → авторизация)

СОБЫТИЕ 3: Ошибка авторизации

-----------------------------

Обнаружение: Ответ от JSONRPC.Ping не содержит "pong"
Реакция:
  - На этапе инициализации отображается "error" красным
  - Переход на главный экран НЕ происходит
  - Повторная попытка через 1000 мс

СОБЫТИЕ 4: Нажатие кнопки на ИК-пульте

--------------------------------------

Обнаружение: IrReceiver.decode() вернул true
Реакция:
  - Сбрасывается таймер взаимодействия
  - Если активен режим часов — он отключается, восстанавливается UI
  - Визуальная: рисуется иконка нажатой кнопки на 2 секунды
  - Сетевая: отправляется соответствующая JSON-RPC команда в Kodi
  - При удержании кнопки — автоповтор с ускорением (для поддерживаемых кнопок)

СОБЫТИЕ 5: Изменение состояния Kodi

-----------------------------------

Обнаружение: При очередном опросе данные отличаются от previousState
Реакция:
  - Если режим часов НЕ активен:
    * Перерисовывается ТОЛЬКО изменившийся элемент UI
    * Остальные элементы не перерисовываются (оптимизация)
  - Если режим часов активен:
    * Данные обновляются в памяти, но UI не перерисовывается
    * previousState синхронизируется для корректного выхода из режима

СОБЫТИЕ 6: Достижение критических значений

------------------------------------------

Обнаружение: cpuLoad >= 80% или temperature >= 70°C
Реакция:
  - Цифры значения меняют цвет с белого на красный
  - Надписи "CPU=" и "T=" остаются белыми
  - Символы "%" и "°C" тоже становятся красными

СОБЫТИЕ 7: Активация режима часов

---------------------------------

Обнаружение: В updateClock() проверяются условия:
  - playbackStatus == 0 и бездействие > 5 минут
  - playbackStatus == 2 и пауза > 30 минут
Реакция:
  - Устанавливается clockState.isClockModeActive = true
  - Очищается экран
  - Отображаются дата и время с правильными цветами
  - UI-элементы перестают перерисовываться

СОБЫТИЕ 8: Выход из режима часов

--------------------------------

Обнаружение: Любое нажатие кнопки на ИК-пульте
Реакция:
  - Сбрасывается таймер взаимодействия
  - clockState.isClockModeActive = false
  - Восстанавливается фоновое изображение
  - Перерисовываются все UI-элементы с актуальными данными

СОБЫТИЕ 9: Потеря NTP-синхронизации

-----------------------------------

Обнаружение: getLocalTime() возвращает false в updateClockTime()
Реакция:
  - clockState.isNtpSynced = false
  - Цвет времени и даты меняется на резервный (красный/оранжевый)
  - Используются внутренние часы ESP32
  - Периодически предпринимаются попытки повторной синхронизации

================================================================================

                         ТАБЛИЦА ИК-КОМАНД
                         
================================================================================


КОД         КНОПКА           АВТОПОВТОР         JSON-RPC МЕТОД

---------   --------------   ----------------   --------------------------------

0xEA15FD00  1                Нет                Input.SendText "1"
0xE916FD00  2                Нет                Input.SendText "2"
0xE817FD00  3                Нет                Input.SendText "3"
0xE619FD00  4                Нет                Input.SendText "4"
0xE51AFD00  5                Нет                Input.SendText "5"
0xE41BFD00  6                Нет                Input.SendText "6"
0xE21DFD00  7                Нет                Input.SendText "7"
0xE11EFD00  8                Нет                Input.SendText "8"
0xE01FFD00  9                Нет                Input.SendText "9"
0xBE41FD00  0                Нет                Input.SendText "0"
0xDF20FD00  VOL+             С ускорением       Application.SetVolume increment
0xDE21FD00  VOL-             С ускорением       Application.SetVolume decrement
0xDD22FD00  MUTE             Нет                Application.SetMute toggle
0xE718FD00  CH+              Фикс. 1 сек        Input.ExecuteAction channelup
0xE31CFD00  CH-              Фикс. 1 сек        Input.ExecuteAction channeldown
0xA55AFD00  REW              Нет                Input.ExecuteAction rewind
0xFB04FD00  PLAY/PAUSE       Нет                Input.ExecuteAction play/pause
                                               (зависит от текущего состояния)
0xF10EFD00  FWD              Нет                Input.ExecuteAction fastforward
0xF40BFD00  UP               С ускорением       Input.Up
0xF00FFD00  DOWN             С ускорением       Input.Down
0xB649FD00  LEFT             С ускорением       Input.Left
0xB54AFD00  RIGHT            С ускорением       Input.Right
0xF20DFD00  OK               Нет                Input.Select
0xFD02FD00  BACK             Нет                Input.Back
0xF807FD00  MENU             Нет                Input.ExecuteAction home
0xA758FD00  CONTEXT MENU     Нет                Input.ContextMenu
0x9F60FD00  INFO             Нет                Input.Info
0xED12FD00  SCREEN TOGGLE    Нет                (только локальная индикация)

================================================================================

                    ЛОГИКА КНОПКИ PLAY/PAUSE
                    
================================================================================


Кнопка Play/Pause (0xFB04FD00) имеет ДВОЙНОЕ назначение и обрабатывается
особым образом в функции handleIRCommand():

Текущее состояние Kodi    Действие при нажатии    Отправляемая команда

────────────────────────────────────────────────────────────────────────

STOP (0)               →  PLAY (начать)          Input.ExecuteAction "play"
PLAY (1)               →  PAUSE (приостановить)  Input.ExecuteAction "pause"
PAUSE (2)              →  PLAY (возобновить)     Input.ExecuteAction "play"
FAST FORWARD (3)       →  PLAY (норм. скорость)  Input.ExecuteAction "play"
REWIND (4)             →  PLAY (норм. скорость)  Input.ExecuteAction "play"

Особенности реализации:
- Используется Input.ExecuteAction вместо Player.PlayPause
- Не требует знания playerid (работает на уровне интерфейса Kodi)
- При перемотке возвращает к нормальной скорости воспроизведения
- Состояние берётся из currentState.playbackStatus (обновляется из API)

================================================================================

                    ПАРСИНГ ЗАГРУЗКИ CPU
                    
================================================================================


Kodi возвращает загрузку CPU в формате:
"#0: 0.0% #1: 16% #2: 0.0% #3: 0.0%"

Это детальная информация по каждому ядру процессора. Функция parseCpuLoad()
выполняет следующие действия:

1. Извлекает все числа из строки (0.0, 16, 0.0, 0.0)
2. Вычисляет среднее значение: (0.0 + 16 + 0.0 + 0.0) / 4 = 4%
3. Возвращает округлённое целое значение

Поддерживаемые форматы:
- Целые числа: "50" → 50
- Дробные числа: "0.5" → 50 (умножение на 100)
- Строки с процентами: "50%" → 50
- Многоядерный формат: "#0: 0.0% #1: 16% ..." → среднее значение

================================================================================

                         ОПТИМИЗАЦИИ ПРОИЗВОДИТЕЛЬНОСТИ
                         
================================================================================


1. БИНАРНЫЙ ПОИСК ГЛИФОВ
   Вместо линейного перебора 161 символа используется бинарный поиск.
   Сложность: O(log N) ≈ 7-8 сравнений вместо 161.
   Требование: массив glyphs должен быть отсортирован по коду символа.

2. ПАКЕТНЫЕ JSON-RPC ЗАПРОСЫ
   Три запроса к Kodi объединяются в один HTTP-запрос.
   Экономия: ~60% времени на сетевые операции (один TCP handshake вместо трех).

3. УСЛОВНАЯ ПЕРЕРИСОВКА UI
   Элементы перерисовываются ТОЛЬКО при изменении их состояния.
   Сравнение currentState с previousState перед каждой отрисовкой.

4. ЧАСТИЧНАЯ ПЕРЕРИСОВКА ПРОГРЕСС-БАРА
   При изменении позиции ползунка очищается и рисуется ТОЛЬКО изменившаяся область.
   Базовая линия рисуется один раз при первом отображении.

5. ПОТОКОВЫЙ ПАРСИНГ JSON
   Использование http.getStream() вместо getString() для ArduinoJson.
   Экономия RAM: JSON не загружается целиком в строку.

6. ХРАНЕНИЕ ДАННЫХ В PROGMEM
   Все шрифты, иконки и фон хранятся во Flash-памяти, не занимая RAM.

7. ЗАЩИТА ОТ ЛИШНИХ ПЕРЕРИСОВОК В РЕЖИМЕ ЧАСОВ
   В режиме часов UI-элементы не перерисовываются, что экономит ресурсы
   и предотвращает наложение элементов на часы.

8. РАЗДЕЛЕНИЕ ТИПОВ СТРУКТУР ШРИФТОВ
   Каждый шрифт использует свой тип структуры (GlyphEntry, GlyphEntryDigits12x24,
   GlyphEntryDigits24x48, GlyphEntryDigits36x72) для избежания конфликтов
   при компиляции.

================================================================================

                         ВОЗМОЖНЫЕ ПРОБЛЕМЫ И РЕШЕНИЯ
                         
================================================================================


ПРОБЛЕМА 1: Устройство не подключается к WiFi
РЕШЕНИЕ: Проверить WIFI_SSID и WIFI_PASS в config.h, убедиться в наличии сигнала

ПРОБЛЕМА 2: Ошибка авторизации
РЕШЕНИЕ: Проверить KODI_USER и KODI_PASS, убедиться что в Kodi включен 
         веб-сервер и разрешены подключения извне

ПРОБЛЕМА 3: ИК-пульт не реагирует
РЕШЕНИЕ: Проверить IR_RECEIVER_PIN, убедиться что пульт использует протокол NEC

ПРОБЛЕМА 4: Мерцание экрана
РЕШЕНИЕ: Используется условная перерисовка — элементы обновляются только при 
         изменении данных. Если мерцание сохраняется — увеличить QUERY_INTERVAL

ПРОБЛЕМА 5: Нестабильная работа при плохом WiFi
РЕШЕНИЕ: Увеличить SERVER_CONNECT_TIMEOUT и AUTH_TIMEOUT в timings.h, 
         уменьшить QUERY_INTERVAL для более быстрого обнаружения проблем

ПРОБЛЕМА 6: Время на часах не синхронизировано
РЕШЕНИЕ: Проверить доступность NTP_SERVER, убедиться в корректности NTP_TIMEZONE.
         При недоступности NTP используются внутренние часы (красный цвет).

ПРОБЛЕМА 7: CPU показывает 0%
РЕШЕНИЕ: Используется System.CpuUsage вместо System.CpuLoad.
         Функция parseCpuLoad() автоматически вычисляет среднее по ядрам.

ПРОБЛЕМА 8: Последний символ времени обрезается
РЕШЕНИЕ: Функция getTextWidth() учитывает отступы между символами (spacingX=1).
         Формула: (длина * FONT_WIDTH) + ((длина-1) * 1)

================================================================================

                         ЗАКЛЮЧЕНИЕ
                         
================================================================================


Программа представляет собой законченное решение для создания кастомного пульта 
управления медиацентром Kodi с богатым интерфейсом и обратной связью. 

Ключевые особенности:
- Модульная архитектура с четким разделением ответственности
- Все настраиваемые параметры вынесены в отдельные файлы
- Оптимизированная отрисовка для плавной работы интерфейса
- Поддержка автоповтора кнопок с ускорением
- Визуальная индикация критических состояний системы
- Устойчивость к потере связи с автоматическим восстановлением
- Режим часов с NTP-синхронизацией и резервным источником времени
- Интеллектуальная обработка кнопки Play/Pause с учётом состояния
- Универсальный парсинг загрузки CPU с поддержкой многоядерных систем

Для внесения изменений в интерфейс достаточно отредактировать файлы 
color_and_coordinates.h и timings.h, не затрагивая логику программы.

================================================================================
