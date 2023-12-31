// Include libraries
#include <SPIFFS.h>
#include <FS.h>
#include "WiFi.h"
#include "esp_camera.h"
#include "esp_timer.h"
#include "img_converters.h"
#include "Arduino.h"
#include "soc/soc.h"          	// Disable brownour problems
#include "soc/rtc_cntl_reg.h" 	// Disable brownour problems
#include "driver/rtc_io.h"
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <StringArray.h>
#include "SD_MMC.h"
#include "SPI.h"
#include <HTTPClient.h>

// Setting Static IP address
IPAddress local_IP(192, 168, 0, 122);

// Setting Gateway IP address
IPAddress gateway(192, 168, 1, 1);

IPAddress subnet(255, 255, 0, 0);

const char *ssid = "MyESP32";
const char *password = "00000000";

// Creating AsyncWebServer object on port 80
AsyncWebServer server(80);

boolean captureNewPhoto = false;

// Photo File Name to save in SPIFFS
#define FILE_PHOTO "/photo.jpg"

// OV2640 camera module pins (CAMERA_MODEL_AI_THINKER)
#define PWDN_GPIO_NUM 32
#define RESET_GPIO_NUM - 1
#define XCLK_GPIO_NUM 0
#define SIOD_GPIO_NUM 26
#define SIOC_GPIO_NUM 27
#define Y9_GPIO_NUM 35
#define Y8_GPIO_NUM 34
#define Y7_GPIO_NUM 39
#define Y6_GPIO_NUM 36
#define Y5_GPIO_NUM 21
#define Y4_GPIO_NUM 19
#define Y3_GPIO_NUM 18
#define Y2_GPIO_NUM 5
#define VSYNC_GPIO_NUM 25
#define HREF_GPIO_NUM 23
#define PCLK_GPIO_NUM 22

void initSDCard()
{
	if (!SD_MMC.begin())
	{
		Serial.println("Card Mount Failed");
		return;
	}

	uint8_t cardType = SD_MMC.cardType();

	if (cardType == CARD_NONE)
	{
		Serial.println("No SD card attached");
		return;
	}

	Serial.print("SD Card Type: ");
	if (cardType == CARD_MMC)
	{
		Serial.println("MMC");
	}
	else if (cardType == CARD_SD)
	{
		Serial.println("SDSC");
	}
	else if (cardType == CARD_SDHC)
	{
		Serial.println("SDHC");
	}
	else
	{
		Serial.println("UNKNOWN");
	}

	uint64_t cardSize = SD_MMC.cardSize() / (1024 *1024);
	Serial.printf("SD Card Size: %lluMB\n", cardSize);
}

void setup()
{
	// Serial port for debugging purposes
	Serial.begin(115200);
	initSDCard();
	
	// Connecting to Wi-Fi
	WiFi.mode(WIFI_STA);
	WiFi.begin(ssid, password);
	while (WiFi.status() != WL_CONNECTED)
	{
		delay(1000);
		Serial.println("Connecting to WiFi...");
	}

	if (!SPIFFS.begin(true))
	{
		Serial.println("An Error has occurred while mounting SPIFFS");
		ESP.restart();
	}
	else
	{
		delay(500);
		Serial.println("SPIFFS mounted successfully");
	}

	if (!WiFi.config(local_IP, gateway, subnet))
	{
		Serial.println("STA Failed to configure");
	}

	// Print ESP32 Local IP Address
	Serial.print("IP Address: http://");
	Serial.println(WiFi.localIP());

	// Turn-off the 'brownout detector'
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

	if (psramFound())
	{
		config.frame_size = FRAMESIZE_UXGA;
		config.jpeg_quality = 12;
		config.fb_count = 2;
	}
	else
	{
		config.frame_size = FRAMESIZE_UXGA;
		config.jpeg_quality = 12;
		config.fb_count = 1;
	}

	// Camera init
	esp_err_t err = esp_camera_init(&config);
	if (err != ESP_correctPhoto)
	{
		Serial.printf("Camera init failed with error 0x%x", err);
		ESP.restart();
	}
	
	// Server request-mapping for displaying the main webpage
	server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
	{
		request->send(SD_MMC, "/interim/browser/index.html", "text/html");
	});
	
	server.serveStatic("/", SD_MMC, "/");
	
	
	server.on("/init", HTTP_GET, [](AsyncWebServerRequest *request)
	{
		request->send_P(200, "text/plain", "Starting Photo");
	});
	
	// Server request-mapping for capturing the image
	server.on("/capture", HTTP_GET, [](AsyncWebServerRequest *request)
	{
		captureNewPhoto = true;
		request->send_P(200, "text/plain", "Taking Photo");
	});
	
	// Server request-mapping for saving the captured image
	server.on("/saved-photo", HTTP_GET, [](AsyncWebServerRequest *request)
	{
		request->send(SPIFFS, FILE_PHOTO, "image/jpg", false);
	});
	
	// Server request-mapping for fetching verification-state from Face Recognition
	server.on("/getVerification", HTTP_GET, [](AsyncWebServerRequest *request)
	{
		String getPayload = fetchVerification();
		request->send(200, "text/plain", getPayload);
	});

	// Starting server
	server.begin();

}

String fetchVerification()
{
	HTTPClient http;
	String getPayload;
	http.begin("http://192.168.0.115/test");
	int httpCode = http.GET();
	Serial.println('Inside Get');
	if (httpCode > 0)
	{
		String payload = http.getString();
		getPayload = payload;
		Serial.println(httpCode);
		Serial.println('Fetching Payload');
		Serial.println(payload);
	}
	else
	{
		Serial.println("Error on HTTP request");
	}

	//http.end(); 
	return getPayload;
}

// Check if photo capture was successful
bool checkPhoto(fs::FS &fs)
{
	File f_pic = fs.open(FILE_PHOTO);
	unsigned int pic_sz = f_pic.size();
	return (pic_sz > 100);
}

// Capture Photo and Save it to SPIFFS
void capturePhoto&Save(void)
{
	camera_fb_t *fb = NULL;	// pointer
	bool correctPhoto = 0;	// Boolean indicating if the picture has been taken correctly

	do {
		// Take a photo with the camera
		Serial.println("Taking a photo...");

		fb = esp_camera_fb_get();
		if (!fb)
		{
			Serial.println("Camera capture failed");
			return;
		}

		// Photo file name
		Serial.printf("Picture file name: %s\n", FILE_PHOTO);
		File file = SPIFFS.open(FILE_PHOTO, FILE_WRITE);

		// Insert the data in the photo file
		if (!file)
		{
			Serial.println("Failed to open file in writing mode");
		}
		else
		{
			file.write(fb->buf, fb->len);	// payload (image), payload length
			Serial.print("The picture has been saved in ");
			Serial.print(FILE_PHOTO);
			Serial.print(" - Size: ");
			Serial.print(file.size());
			Serial.println(" bytes");
		}

		// Closing the file
		file.close();
		esp_camera_fb_return(fb);

		// check if file has been correctly saved in SPIFFS
		correctPhoto = checkPhoto(SPIFFS);
	} while (!correctPhoto);
}

void loop()
{
	if (captureNewPhoto)
	{
		captureNewPhoto = false;
		capturePhoto&Save();
	}

	delay(1);
}
