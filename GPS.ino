#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ESP_Mail_Client.h>
#define SMTP_HOST "smtp.office365.com"
#define SMTP_PORT 587
#define AUTHOR_EMAIL "elyamanma@students.rhu.edu.lb"
#define AUTHOR_PASSWORD "**********"
#define RECIPIENT_EMAIL "elyamanma@students.rhu.edu.lb"

SMTPSession smtp;
TinyGPSPlus gps;
SoftwareSerial SerialGPS(4, 5); 
WiFiClientSecure client;

void smtpCallback(SMTP_Status status);

const char* ssid = "D-Link-ppK2";
const char* wifi_password = "4rzyk9u4";

float Latitude , Longitude;
int year , month , date, hour , minute , second;
String DateString , TimeString , LatitudeString , LongitudeString, ForceString;


WiFiServer server(80);
void setup() {
  Serial.begin(9600);
  SerialGPS.begin(9600);
  Serial.println();
  Serial.print("Connecting");
  WiFi.begin(ssid, wifi_password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  server.begin();
  Serial.println("Server started");
  Serial.println(WiFi.localIP());

  client.setInsecure();
  
  smtp.debug(1);
  smtp.callback(smtpCallback);
  // ESP_Mail_Session session;

  // session.server.host_name = SMTP_HOST;
  // session.server.port = SMTP_PORT;
  // session.login.email = AUTHOR_EMAIL;
  // session.login.password = AUTHOR_PASSWORD;
  // session.login.user_domain = "";

//  SMTP_Message message;

//   message.sender.name = "ESP";
//   message.sender.email = AUTHOR_EMAIL;
//   message.subject = "Patient Status";
//   message.addRecipient("Mustapha", RECIPIENT_EMAIL);

//   String htmlMsg = "<html><head><title>Arduino HTML Table</title></head><body>";
//   htmlMsg += "<table border='1'>";
//   htmlMsg += "<tr><th>Latitude</th><td>";
//   htmlMsg += LatitudeString;
//   htmlMsg += "</td></tr>";
//   htmlMsg += "<tr><th>Longitude</th><td>";
//   htmlMsg += LongitudeString;
//   htmlMsg += "</td></tr>";
//   htmlMsg += "<tr><th>Date</th><td>";
//   htmlMsg += DateString;
//   htmlMsg += "</td></tr>";
//   htmlMsg += "<tr><th>Time</th><td>";
//   htmlMsg += TimeString;
//   htmlMsg += "</td></tr>";
//   htmlMsg += "</table></body></html>";

//   message.html.content = htmlMsg.c_str();
//   message.html.content = htmlMsg.c_str();
//   message.text.charSet = "us-ascii";
//   message.html.transfer_encoding = Content_Transfer_Encoding::enc_7bit;

  // if (!smtp.connect(&session))
  //   return;

  // /* Start sending Email and close the session */
  // if (!MailClient.sendMail(&smtp, &message))
  //   Serial.println("Error sending Email, " + smtp.errorReason());

}

void loop() {

  while (SerialGPS.available() > 0)
    if (gps.encode(SerialGPS.read()))
    {
      if (gps.location.isValid())
      {
        Latitude = gps.location.lat();
        LatitudeString = String(Latitude , 6);
        Longitude = gps.location.lng();
        LongitudeString = String(Longitude , 6);
      }

      if (gps.date.isValid())
      {
        DateString = "";
        date = gps.date.day();
        month = gps.date.month();
        year = gps.date.year();

        if (date < 10)
        DateString = '0';
        DateString += String(date);

        DateString += " / ";

        if (month < 10)
        DateString += '0';
        DateString += String(month);
        DateString += " / ";

        if (year < 10)
        DateString += '0';
        DateString += String(year);
      }

      if (gps.time.isValid())
      {
        TimeString = "";
        hour = gps.time.hour()+ 3; //adjust UTC
        minute = gps.time.minute();
        second = gps.time.second();
    
        if (hour < 10)
        TimeString = '0';
        TimeString += String(hour);
        TimeString += " : ";

        if (minute < 10)
        TimeString += '0';
        TimeString += String(minute);
        TimeString += " : ";

        if (second < 10)
        TimeString += '0';
        TimeString += String(second);
      }

    }

  WiFiClient client = server.available();

  if (!client)
  {
    return;
  }

  String s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n <!DOCTYPE html> <html> <head> <script src='https://smtpjs.com/v3/smtp.js'></script> <title>NEO-6M GPS Readings</title> <style>";
  s += "table, th, td {border: 1px solid blue;} </style> </head> <body> <h1  style=";
  s += "font-size:300%;";
  s += " ALIGN=CENTER>NEO-6M GPS Readings</h1>";
  s += "<p ALIGN=CENTER style=""font-size:150%;""";
  s += "> <b>Location Details</b></p> <table ALIGN=CENTER style=";
  s += "width:50%";
  s += "> <tr> <th>Latitude</th>";
  s += "<td ALIGN=CENTER >";
  s += LatitudeString;
  s += "</td> </tr> <tr> <th>Longitude</th> <td ALIGN=CENTER >";
  s += LongitudeString;
  s += "</td> </tr> <tr>  <th>Date</th> <td ALIGN=CENTER >";
  s += DateString;
  s += "</td></tr> <tr> <th>Time</th> <td ALIGN=CENTER >";
  s += TimeString;
  s += "</td></tr>";

if (gps.location.isValid()) {
    s += "<p align=center><a style=""color:RED;font-size:125%;"" href=""http://maps.google.com/maps?&z=15&mrt=yp&t=k&q=";
    s += LatitudeString;
    s += "+";
    s += LongitudeString;
    s += """ target=""_blank"">Click here</a> to open the location in Google Maps.</p>";
}

  ESP_Mail_Session session;
  session.server.host_name = SMTP_HOST;
  session.server.port = SMTP_PORT;
  session.login.email = AUTHOR_EMAIL;
  session.login.password = AUTHOR_PASSWORD;
  session.login.user_domain = "";

  SMTP_Message message;

  message.sender.name = "ESP";
  message.sender.email = AUTHOR_EMAIL;
  message.subject = "Patient Status";
  message.addRecipient("Mustapha", RECIPIENT_EMAIL);

  String htmlMsg = "<html><head><title>Arduino HTML Table</title></head><body>";
  htmlMsg += "<table border='1'>";
  htmlMsg += "<tr><th>Latitude</th><td>";
  htmlMsg += LatitudeString;
  htmlMsg += "</td></tr>";
  htmlMsg += "<tr><th>Longitude</th><td>";
  htmlMsg += LongitudeString;
  htmlMsg += "</td></tr>";
  htmlMsg += "<tr><th>Date</th><td>";
  htmlMsg += DateString;
  htmlMsg += "</td></tr>";
  htmlMsg += "<tr><th>Time</th><td>";
  htmlMsg += TimeString;
  htmlMsg += "</td></tr>";
  htmlMsg += "</table></body></html>";

  message.html.content = htmlMsg.c_str();
  message.html.content = htmlMsg.c_str();
  message.text.charSet = "us-ascii";
  message.html.transfer_encoding = Content_Transfer_Encoding::enc_7bit;

  if (!smtp.connect(&session))
    return;

  /* Start sending Email and close the session */
  if (!MailClient.sendMail(&smtp, &message))
    Serial.println("Error sending Email, " + smtp.errorReason());
  client.print(s);
  delay(100);
}

void smtpCallback(SMTP_Status status){
  /* Print the current status */
  Serial.println(status.info());

  /* Print the sending result */
  if (status.success()){
    Serial.println("----------------");
    ESP_MAIL_PRINTF("Message sent success: %d\n", status.completedCount());
    ESP_MAIL_PRINTF("Message sent failled: %d\n", status.failedCount());
    Serial.println("----------------\n");
    struct tm dt;

    for (size_t i = 0; i < smtp.sendingResult.size(); i++){
      /* Get the result item */
      SMTP_Result result = smtp.sendingResult.getItem(i);
      time_t ts = (time_t)result.timestamp;
      localtime_r(&ts, &dt);

      ESP_MAIL_PRINTF("Message No: %d\n", i + 1);
      ESP_MAIL_PRINTF("Status: %s\n", result.completed ? "success" : "failed");
      ESP_MAIL_PRINTF("Date/Time: %d/%d/%d %d:%d:%d\n", dt.tm_year + 1900, dt.tm_mon + 1, dt.tm_mday, dt.tm_hour, dt.tm_min, dt.tm_sec);
      ESP_MAIL_PRINTF("Recipient: %s\n", result.recipients.c_str());
      ESP_MAIL_PRINTF("Subject: %s\n", result.subject.c_str());
    }
    Serial.println("----------------\n");
  }
}