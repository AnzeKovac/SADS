
//Required Libraries
#include <ESP8266HTTPClient.h> //Inbuilt ESP8266 Library
#include <ESP8266WiFi.h> //Inbuilt ESP8266 Library
#include <ArduinoJson.h> //From https://arduinojson.org/

//Wifi Settings
const char* ssid = "SSID";
const char* password = "Pass";

//Salesforce User Settings
const char* sfUsername = "anze@sads.fri";
const char* sfPassword = "testtest123";
const char* sfToken = "613PRvzEicUSlpFUX4NN3K5g";

//Client ID and Secret from Conected app
const char* sfClientId = "3MVG9T46ZAw5GTfVgwTaXnYsDtJU4QCOnLQeeh4cSb9up7Ug0LThwxBlU4IIXXECjl4tPjh0ecC5T6f1Odsc.";
const char* sfClientKey = "1345CFDC1D820C628E20AA890E4DD2AF0D92E3B885D5BFEB98715CC23A395C40"; //Client Secret

//SF login && instance cert footprints (pre-generated since you can't do that on ESP8266)
const char*  sfLoginFingerprint = "0B:33:19:AC:6D:9E:C1:5F:08:AB:93:17:2A:FE:F9:E0:90:69:C7:9A";
const char*  sfInstanceFingerprint = "BA:69:22:06:CA:B4:1D:B9:0D:01:AB:1F:98:BB:1F:53:37:64:95:98x";

//Variables to store SF auth token and SF instance url
String sfAuthToken = "";
String sfInstanceURL = "";

// Login method, which returns a JSON object with auth token and instance URL you can use to perform requests
void doLogin(String username, String password, String token, String clientId, String clientKey, String fingerprint) {
        
    HTTPClient http;
    //you can change this to test.salesforce.com if you need to
    http.begin("https://login.salesforce.com/services/oauth2/token", fingerprint);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    
    String postBody = "?&grant_type=password";
    postBody += "&client_id=" + clientId;
    postBody += "&client_secret=" + clientKey;
    postBody += "&username=" + username;
    postBody += "&password=" + password + token;
    
    int httpCode = http.POST(postBody);
    Serial.print("http result:");
    Serial.println(httpCode);
    
    String payload = http.getString();
    
    http.end();

    StaticJsonBuffer<1024> jsonBuffer;
    
    JsonObject& auth = jsonBuffer.parseObject(payload);
    Serial.println("Response: ");
    Serial.println(payload);
    
    if(httpCode == 200) {    
      Serial.println("Successfully logged in!");
      String token = auth["access_token"];
      String url = auth["instance_url"];
      sfAuthToken = token;
      sfInstanceURL = url;
    }
    
    else {
      Serial.println("An error occured, not logged in!");
    }

}

// Method to insert Platform Event
bool insertSObject(String objectName, JsonObject& object) {
  
  bool insertSuccess = false;
  String reqURL = (String)sfInstanceURL + "/services/data/v45.0/sobjects/" + (String)objectName;

  Serial.println("Instance URL: " + (String)sfInstanceURL);
  Serial.println("Auth Token: " + (String)sfAuthToken);
  Serial.println("Request URL: " + reqURL);
  Serial.println("JSON Sent: ");
  object.printTo(Serial);
  Serial.println();

  HTTPClient http;
  http.begin(reqURL, sfInstanceFingerprint);
  http.addHeader("Authorization", "Bearer " + (String)sfAuthToken);
  http.addHeader("Content-Type", "application/JSON");
  
  String objectToInsert;
  object.printTo(objectToInsert);
    
  int httpCode = http.POST(objectToInsert);
  String payload = http.getString();
  
  http.end();

  if(httpCode == 201) {
    insertSuccess = true;
  }

  Serial.println("HTTP Code:");
  Serial.println(httpCode);
  Serial.println("HTTP Response:");
  Serial.println(payload);

  return insertSuccess;
}

// Return JSON object of Salesforce Platform Event
JsonObject& buildPlatformEvent(float lightAmount, String groupName, String comment, String members) {

  StaticJsonBuffer<1024> jsonBuffer;

  JsonObject& root = jsonBuffer.createObject();

  //Define values for each published event
  root["Device_Id__c"] = ESP.getChipId();
  root["Group_Name__c"] = groupName;
  root["Sensor_Value__c"] = lightAmount;
  root["Additional_Comment__c"] = comment;
  root["Members__c"] = members;
  
  return root;

}

//Sets up Serial monitor, connects to WiFi and logins to Salesforce
void setup() {
  
  Serial.begin(115200);
  WiFi.begin(ssid,password);
  
  Serial.println("Waiting for connection");
  
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  
  Serial.println("WiFi Connected!");
  
  //Login to Salesforce via OAuth
  doLogin(sfUsername, sfPassword, sfToken, sfClientId, sfClientKey, sfLoginFingerprint);

}

//Loop the loop
//Do your magic here, currenlty sensor value is sent to Salesforce every 5 seconds. And yes it's a random value :see-no-evil:
void loop() {
  //Sensor reading (it should be from 0 to 100)

  //0 is max brightnes, 1024 is darknes (hello my old friend)
  //value should be normalized to scale of 0-100 (can be shown in a frontend graph)
  float lightAmount = analogRead(A0);
  
  //Group name should match your group name
  String groupName = "Group Name";

  //Additional comment for the waiter
  String additionalComment = "";

  //All the members (their names)
  String members = "Jaka,Janez,France,Edgar,Luka,Blaž";
  
  JsonObject& event = buildPlatformEvent(lightAmount, groupName, additionalComment, members);

  bool insertSuccess = insertSObject("Empty_Glass__e", event);
  if(insertSuccess) {
    Serial.println("Record Inserted Successfully!");
  }
  else {
      Serial.println("Record Insert Failed!");
  }
  
  //3000 == 3s
  delay(3000);

}
