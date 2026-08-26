# DCPQ Monitor

Firmware and cloud pipeline for the **DCPQ Monitor**, a hybrid edge-cloud IoT
system for real-time temperature, humidity, and dew-point psychrometric
monitoring against ASHRAE TC 9.9 thermal guidelines, deployed as a tropical
server-room case study in Jakarta, Indonesia.

This repository accompanies the manuscript:

> Real Time Temperature, Humidity, and Dew Point Psychrometric Monitoring for
> ASHRAE TC 9.9 Compliance: A Hybrid Edge Cloud IoT Case Study in a Tropical
> Server Room
> Syah Alam,Diyanda Armawan Chairul Gagarin Irianto, Lydia Sari, Yuli
> Kurnia Ningsih, and Gagas Setyawahyuaji
> Department of Electrical Engineering, Universitas Trisakti, Jakarta,
> Indonesia
> Submitted to the *International Journal of System Assurance Engineering
> and Management* (IJSA).

It is provided to support reproducibility, as requested during peer review.
It documents the Tier-1 (edge) and Tier-2 (cloud) implementation described in
Section 4 of the manuscript. The psychrometric derivation (saturation vapor
pressure, humidity ratio, dew point, wet-bulb temperature, specific
enthalpy — Equations 1-6) is computed on demand by the dashboard `doGet()`
handler from the raw (temperature, humidity, timestamp, device_id) records
stored by `doPost()`; that dashboard/visualization layer is not included
here (see "Scope" below).

## Repository structure

```
dcpq-monitor/
├── firmware/          ESP32 Arduino sketch (Tier-1, edge device)
│   ├── ESP32SHT401.ino   Setup/loop, SHT40 sampling, HTTPS POST, offline queue
│   ├── fn.h              JSON payload builder, HTTP POST, LittleFS offline queue, Magnus dew-point formula
│   ├── display_set.h     TFT LCD UI (status icons, live T/RH/DP readout)
│   ├── logo.h            Boot-screen logo (PNG byte array)
│   ├── FreeFont.h        Adafruit GFX free-font reference table (third-party, see Third-party code)
│   └── User_Setup.h      TFT_eSPI display driver configuration (third-party, see Third-party code)
├── cloud/
│   └── code.gs           Google Apps Script Web App: doPost() ingestion endpoint (Tier-2)
├── LICENSE
└── README.md
```

## Scope of this release

This repository documents the **data acquisition and ingestion pipeline**
actually used for the deployment reported in the manuscript (device sampling,
HTTPS transport, offline queueing, and the `doPost()` ingestion endpoint). It
is released as-is to support reproducibility, and has not been generalized
into a polished, plug-and-play package.

Not included:

- The `doGet()` dashboard handler that computes the six psychrometric
  parameters (Equations 1-6) on demand and serves the live visualization
  described in Section 4.2 and Section 5 of the manuscript. The Magnus-based
  dew-point formula used on the device side for the local TFT display is
  included (`fn.h`, `hitungDewPoint()`); the full psychrometric suite
  (pressure altitude correction, saturation vapor pressure, humidity ratio,
  wet-bulb, enthalpy, and ASHRAE TC 9.9 zone classification) is described
  analytically in Section 3 of the manuscript rather than reproduced here as
  code.
- The Google Sites dashboard front-end. A live instance, reporting real-time
  readings from the deployment described in the manuscript, is viewable at
  the link in "Live dashboard" below; its front-end source is not included
  in this repository.
- The raw deployment dataset (9,334 transmitted records, 13-16 April 2026).
  Contact the corresponding author for data-sharing requests.

## Live dashboard

A live instance of the interactive dashboard described in Section 4.2 and
Section 5 of the manuscript — showing real-time temperature, humidity, dew
point, and ASHRAE TC 9.9 zone classification at the deployment site — is
viewable at:

https://sites.google.com/std.trisakti.ac.id/room/home

This is provided for illustration of the system described in the paper. For
questions about adapting this system to a different site or use case,
please get in touch via the corresponding author's contact details below.

## Security note before you deploy your own copy

The original firmware and script used a hardcoded shared-secret token and a
live Apps Script deployment URL. Both have been **replaced with placeholders**
in this repository (`YOUR_SECRET_TOKEN_HERE` in `firmware/fn.h` and
`cloud/code.gs`; `YOUR_DEPLOYMENT_ID` in `firmware/ESP32SHT401.ino`). Before
building your own deployment:

1. Generate your own random token and set it identically in both
   `firmware/fn.h` (`TOKEN_SECRET`) and `cloud/code.gs` (`secret`).
2. Deploy `cloud/code.gs` as your own Apps Script Web App
   (Deploy > New deployment > Web app) and use *your* resulting exec URL in
   `firmware/ESP32SHT401.ino` (`serverName`).
3. Do not commit your real token or deployment URL to a public repository.

## Hardware and required libraries

- ESP32-2432S028R (ESP32 + 2.4" TFT LCD, ST7789 driver, SPI)
- Sensirion SHT40 temperature/humidity sensor (I2C, factory accuracy
  ±0.2 °C / ±1.8 %RH)

Arduino libraries (install via Library Manager unless noted):

1. `PNGdec` by Larry Bank
2. `TFT_eSPI` by Bodmer
3. `Adafruit SHT4x` by Adafruit (plus `Adafruit Unified Sensor`)
4. `ArduinoJson` by Benoit Blanchon
5. `WiFiManager` by tzapu
6. `Preferences`, `LittleFS`, `WiFi`, `HTTPClient`, `Wire`, `SPI` (bundled with the ESP32 Arduino core)

### TFT_eSPI configuration

After installing `TFT_eSPI`, back up the library's own `User_Setup.h` and
replace it with `firmware/User_Setup.h` from this repository (ST7789 driver,
240x320, pin mapping for the ESP32-2432S028R).

## Build and flash

1. Open `firmware/ESP32SHT401.ino` in the Arduino IDE (or PlatformIO) with
   the ESP32 board package installed.
2. Install the libraries above and apply the `User_Setup.h` replacement.
3. Set your own `TOKEN_SECRET` (`fn.h`) and `serverName` (`ESP32SHT401.ino`)
   as described in "Security note" above.
4. Flash to the ESP32-2432S028R. On first boot the device opens a WiFi
   provisioning access point (`Minilog-Pro32` / `minilog32`); connect and
   enter your WiFi credentials.
5. Readings are sampled continuously and POSTed at the top of every 00 and
   30 second mark; failed POSTs are queued to flash (`LittleFS`) and
   retried automatically once connectivity returns (see `fn.h`,
   `flushOneQueuedReading()`).

## Third-party code

`firmware/FreeFont.h` and `firmware/User_Setup.h` are configuration/reference
files from the `TFT_eSPI` (Bodmer) and Adafruit GFX font libraries, included
here only to document the exact configuration used, not as original
contributions of this work. Please consult those projects' own repositories
for their current license terms.

## Citation

If you use this code, please cite the manuscript above (full citation to be
updated with volume/issue/DOI upon publication).

## License

Released under the MIT License — see [LICENSE](LICENSE). This applies to the
original firmware and Apps Script code in this repository; it does not
relicense the third-party files noted above.

## Contact

Corresponding author: Syah Alam, Department of Electrical Engineering,
Universitas Trisakti, Jakarta, Indonesia — syah.alam@trisakti.ac.id
