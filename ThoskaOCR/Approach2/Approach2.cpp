
#include "SD_MMC.h"
#include <SPIFFS.h>
#include <FS.h>
#include "WiFi.h"
#include "esp_camera.h"
#include "esp_timer.h"
#include "img_converters.h"
#include "Arduino.h"
#include "soc/soc.h"         
#include "soc/rtc_cntl_reg.h" 
#include "driver/rtc_io.h"
#include <ESPAsyncWebServer.h>
#include <StringArray.h>

// Setting the Static IP address - This helps to keep a static IP everytime the ESP is flashed
IPAddress local_IP(192, 168, 0, 122);
// Setting the Static Gateway IP address
IPAddress gateway(192, 168, 1, 1);

IPAddress subnet(255, 255, 0, 0);

// Connecting to wifi
// As we initiate a API call, we need a WiFi connection
const char* ssid = "1234"; //Username
const char* password = "1234"; // Password

// Creating AsyncWebServer object on port 80 - Only 1 server can be initiated for a single port number
AsyncWebServer server(80);

// This boolean value is set to counter image capturing
boolean capturePhoto = false;

// Setting the name of file to be saved in flash memory
#define FILE_PHOTO "/thoska-capture.jpg"

//CAMERA_MODEL_AI_THINKER - OV2640 camera module pins 
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27
#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

//This rawliteral contains the structure of the webpage 
//Entire Javascript functions are handled within
//Since, no external storage/SD-Card is used, to store external file load, scripts are embedded together
const char landing_page[] PROGMEM = R"rawliteral(<!DOCTYPE HTML>
<html>

<head>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
        @import url('https://fonts.googleapis.com/css2?family=Montserrat&display=swap');

        :root {
            /* COLORS */
            --white: #e9e9e9;
            --gray: #333;
            --blue: #0367a6;
            --lightblue: #008997;

            /* RADII */
            --button-radius: 0.7rem;

            /* SIZES */
            --max-width: 758px;
            --max-height: 420px;

            font-size: 16px;
            font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, Oxygen,
                Ubuntu, Cantarell, "Open Sans", "Helvetica Neue", sans-serif;
        }

        *,
        *:before,
        *:after {
            margin: 0;
            padding: 0;
            border: 0;
            font-size: 100%;
            font: inherit;
            vertical-align: baseline;
        }

        html {
            box-sizing: border-box;
        }

        body {
            align-items: center;
            background-color: var(--white);
            background-attachment: fixed;
            background-position: center;
            background-repeat: no-repeat;
            background-size: cover;
            display: grid;
            height: 100vh;
            place-items: center;
        }

        .container {
            height: auto;
            display: flex;
            flex-direction: row;
            margin: 0 auto;
            margin-top: 3%;
            background-color: var(--white);
            border-radius: var(--button-radius);
            box-shadow: 0 0.9rem 1.7rem rgba(0, 0, 0, 0.25),
                0 0.7rem 0.7rem rgba(0, 0, 0, 0.22);
            max-width: var(--max-width);
            overflow: hidden;
            position: relative;
            width: 100%;
        }

        .feed {
            flex-basis: 65%;
            text-align: center;
        }

        .options {
            flex-basis: 35%;
        }

        .feed {
            border-right: 1px solid;
        }

        .feed-preview {
            margin: 65px 0;
            width: auto;
            padding: 10px;
            max-width: 100%;
            display: inline-flex;
            border-radius: 5px;
            justify-content: center;
            border: 1px solid;
            box-shadow: rgb(0 0 0 / 24%) 0px 3px 8px;
        }

        .to-capture {
            bottom: -3px;
            z-index: 30;
            position: absolute;
        }

        .options,
        .buttons {
            display: flex;
            flex-direction: column;
        }

        .fetch {
            position: absolute;
            right: 0;
            BOTTOM: -2px;
            transform: translateX(-15px);
        }

        .type,
        .owner {
            display: inline-flex;
        }

        .option-one,
        .option-two {
            padding: 8px;
            box-shadow: rgba(0, 0, 0, 0.1) 0px 1px 3px 0px, rgba(0, 0, 0, 0.06) 0px 1px 2px 0px;
        }

        .option-one {
            border-top-left-radius: 5px;
            border-bottom-left-radius: 5px;
        }

        .option-two {
            border-top-right-radius: 5px;
            border-bottom-right-radius: 5px;
        }

        .option-one.highlight {
            background-color: #d2dbc1;
        }

        .option-two.highlight {
            background-color: teal;
        }

        .alert-danger {
            display: none;
        }

        .options {
            text-align: center;
            margin-top: 40px;
        }

        .options p {
            font-weight: 600;
        }

        .progress-overlay {
            text-align: center;
            font-weight: 600;
            color: white;
        }

        .result-row {
            margin: 10px;
        }

        .matriculation,
        .expire {
            border-bottom: 1px solid;
            box-shadow: rgb(0 0 0 / 9%) 0px 2px 1px, rgb(0 0 0 / 9%) 0px 4px 2px, rgb(0 0 0 / 9%) 0px 8px 4px, rgb(0 0 0 / 9%) 0px 16px 8px, rgb(0 0 0 / 9%) 0px 32px 16px;
        }

        #overlay {
            padding: 20%;
            position: fixed;
            display: none;
            width: 100%;
            height: 100%;
            top: 0;
            left: 0;
            right: 0;
            bottom: 0;
            background-color: rgba(0, 0, 0, 0.9);
            z-index: 100;
            cursor: pointer;
        }

        @media only screen and (max-width: 768px) {
            body {
                margin: 0 20px !important;
            }

            .fetch {
                transform: none;
            }

            .container {
                display: flex;
                flex-direction: column;
            }

            .feed-preview {
                margin: 20px 0;
            }

            .options {
                border-top: 1px solid;
            }

            .feed {
                border: none;
            }

            .to-capture,
            .fetch {
                text-align: center;
                position: inherit;
                top: 10px;
                margin: 20px 0px;
            }
        }
    </style>

    <link href='https://cdnjs.cloudflare.com/ajax/libs/twitter-bootstrap/4.3.1/css/bootstrap.min.css' rel='stylesheet'>
    <script type='text/javascript' src='https://cdnjs.cloudflare.com/ajax/libs/jquery/3.4.1/jquery.min.js'></script>
    <script type='text/javascript' src='https://unpkg.com/tesseract.js@v2.0.0-beta.1/dist/tesseract.min.js'></script>
  <script>
    var deg = 0;

    //This function initiates a GET request which is handled by the ESP32 Webserver
    function captureThoska() {
        var xhr = new XMLHttpRequest();
        xhr.open('GET', "/capture", true);
        xhr.send();
    }
    function isOdd(n) { return Math.abs(n % 2) == 1; }
    
    //Validation function to check attributes of Thoska
    function checkIfImageLethal(text) {
        var line = '';
        var lines = text.split("\n");
        var msg = '';
        let isMatri, isOwner, isType, isExpiry = false;
        for (var i = 0; i < lines.length; i++) {
            if (lines[i].startsWith(" ")) {
                line += " " + lines[i].trim();
            } else {
                line += "," + lines[i].trim();

            }
        }
        if (line.includes("inr")) {
            isMatri = true;
        }
        else {
            $('.alert-danger').fadeIn();
            $('.alert-danger').text("Image not good for matriculation verification ! Please try again :(");
        }
            isExpiry = true;
        
        if (line.includes("VMT") || line.includes("ticket") || line.includes("Sem")) {
            isOwner = true;
        }
        else {
            $('.alert-danger').fadeIn();
            $('.alert-danger').text("Image not good for owner verification ! Please try again :(");
        }
        if (isMatri) {
            return line;
            $('.alert-danger').fadeOut();
        }
		  if (!isMatri && !isOwner && !isType && !isExpiry) {
            $('.alert-danger').fadeIn();
            $('.alert-danger').text("No Thoska detected OR Bad Capture!!!");
        }
    }

    //Function to initiate OCR on the thoska image
    //Here the Tesseract API is initialized and then OCR is performed on the captured image
    function scan(img) {
        if (img == "") {
            console.log('image not fetched')
        }

        Tesseract.recognize(img, "deu", { logger: m => showStates(m) })
            .then(({ data: { text } }) => {
              console.log(text);
                showStates({ progress: 1, status: "Completed" });
                $('#overlay').fadeOut();
               
                let processedText = checkIfImageLethal(text);
                 console.log(processedText);
                processedText != "" ? extractMat(processedText) : false;
            })
    }

    //This function extracts the necessary value from the image captured
    function extractMat(text) {

        var typeOfCard = '';
        var matri = '';
        var expiry = '';
        //alert("Matriculation" + text.replace(/[^0-9]/gi, '').substring(0,6));
        if (text.includes("inr")) {
            matri = text.substring(1).trim().split('inr')[1].match(/\d+/)[0];
            $('.matriculation').text(matri);
        }
        if (text.includes("erendenaus") || text.includes("ausweis") || text.includes("Semesterticket")) {
            typeOfCard = "Student";
            $('.type .option-one').addClass('highlight');
        }
        else {
            $('.type .option-two').addClass('highlight');
        }
        if (text.includes("VMT") && (text.includes("erendenaus") || text.includes("ausweis") || text.includes("Semesterticket"))) {
            $('.owner .option-one').addClass('highlight');
        }
        else {
            $('.owner .option-two').addClass('highlight');
        }
        if (text.includes("bis")) {
            expiry = text.substring(1).trim().split('bis')[1].replace(/[^0-9\.]+/g, "");
            $('.expire').text(expiry);
        }

    }

    //Function to show the progress of OCR
    function showStates(data) {
        $('#overlay').fadeIn();
        $(".progress-bar")
            .attr("valuenow", data.status)
            .width(data.progress * 100 + "%")
            .parents()
            .eq(1)
            .fadeIn();
        $(".status").text(data.status);
    }
</script>
</head>

<body>
    <div>
        <h2>ESP32-CAM Thoska OCR</h2>
    </div>
    <div class="container">
        <div class="feed">
            <div class="alert alert-danger" style="position:absolute; margin-top: 5px;" role="alert">
                Image not good for text verification ! Please try again :(
            </div>
            <div class="feed-preview" onclick="location.reload();"><img src="http://192.168.0.122/saved-photo/photo.jpg" id="photo" width="100%"></div>
            <div class="to-capture"><button class="btn btn-outline-secondary capture"
                    onclick="captureThoska();">CAPTURE PHOTO</button></div>
        </div>
        <div class="options">
            <div class="buttons">
                <button class="btn btn-outline-success fetch" onclick="scan('http://192.168.0.122/saved-photo/photo.jpg');">FETCH
                    DETAILS</button>
            </div>
            <div class="result">
                <div class="result-row">
                    <p>Type Of Card</p>
                </div>
                <div class="type result-row">
                    <div class="option-one">Thoska</div>
                    <div class="option-two">Not a Thoska</div>
                </div>
                <div class="result-row">
                    <p>Ownership Of Card</p>
                </div>
                <div class="owner result-row">
                    <div class="option-one">Student</div>
                    <div class="option-two">Other</div>
                </div>
            </div>
            <div class="result-row">
                <p>Matriculation Number</p>
            </div>
            <div>
                <div class="matriculation result-row"></div>
            </div>
            <div class="result-row">
                <p>Expire Date</p>
            </div>
            <div class="result-row">
                <div class="expire"></div>
            </div>
            <p id="res"></p>
        </div>
    </div>

    </div>
    <div id="overlay">
        <div class="progress-overlay">
            <div class="progress mt-2 mb-2">
                <div class="progress-bar progress-bar-striped progress-bar-animated" role="progressbar"
                    aria-valuenow="75" aria-valuemin="0" aria-valuemax="100" style="width: 0%"></div>
            </div>
            <p class="status"></p>

        </div>


    </div>

</body>


</html>)rawliteral";


void setup() {
  
  Serial.begin(115200);

  // Connecting to WiFi
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to the WiFi...");
  }

  //Checking allocation of SPIFFS
  if (!SPIFFS.begin(true)) {
    Serial.println("Could not mount SPIFFS");
    ESP.restart();
  }
  else {
    delay(500);
    Serial.println("SPIFFS mounted");
  }

 if (!WiFi.config(local_IP, gateway, subnet)) {
  Serial.println("STA Failed to configure");
}
  
  Serial.print("IP Address: http://");
  Serial.println(WiFi.localIP());

  // Turning-off the 'brownout detector' for optimization
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);

  // OV2640 camera module
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;

  //Setting high value of resolution for OCR
  if (psramFound()) {
    config.frame_size = FRAMESIZE_UXGA;
    config.jpeg_quality = 12;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }
  // Initiating the Camera
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    ESP.restart();
  }

  // Landing Page Preview
  // The server hosts all requests from here
  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/html", landing_page);
  });
  
  // Capture request mapping to initiate picture capturing of thoska
  // The boolean value is checked in the loop function 
  server.on("/capture", HTTP_GET, [](AsyncWebServerRequest * request) {
    capturePhoto = true;
    request->send_P(200, "text/plain", "Taking Photo");
  });

  // This request gives the preview of captured image
  server.on("/saved-photo", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, FILE_PHOTO, "image/jpg", false);
  });

  // Starting the  server
  server.begin();

}

// Checking if the capture is completed
bool checkPhoto( fs::FS &fs ) {
  File f_pic = fs.open( FILE_PHOTO );
  unsigned int pic_sz = f_pic.size();
  return ( pic_sz > 100 );
}

// Capturing the photo and saving it to SPIFFS
void capturePhotoSaveSpiffs( void ) {
  camera_fb_t * fb = NULL; 
  bool imgCap = 0; // Boolean Value to check the captured pic

  do {
    // Taking the photo 
    Serial.println("Capturing Thoska...");

    fb = esp_camera_fb_get();
    if (!fb) {
      Serial.println("Capturing Thoska failed");
      return;
    }

    File file = SPIFFS.open(FILE_PHOTO, FILE_WRITE);

    // Writing captured data to image
    if (!file) {
      Serial.println("Failed to open file in writing mode");
    }
    else {
      file.write(fb->buf, fb->len);
      Serial.print("Thoska image saved as ");
      Serial.print(FILE_PHOTO);
      Serial.print(" - Size: ");
      Serial.print(file.size());
      Serial.println(" bytes");
    }
    // Closing the file object
    file.close();
    // Reverting Camera
    esp_camera_fb_return(fb);

    // Function to check if the image is captured
    imgCap = checkPhoto(SPIFFS);
  } while ( !imgCap );
}

void loop() {
  // Condition is checked in loop
  // This condition is changed on the button click on the webpage
  // After the capture button is clicked on webpage, the image is saved in SPIFFS
  // From SPIFFS, the image is fetched, and then OCR is performed
  if (capturePhoto) {
    capturePhoto = false;
    capturePhotoSaveSpiffs();
  }
  delay(1);
}
