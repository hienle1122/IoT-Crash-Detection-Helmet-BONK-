float measure_fall(){
  /*   
   *    Used to calculate an average of the IMU
   *    readings. Helps reduce noise.
   */
  imu.readAccelData(imu.accelCount);
  x = imu.accelCount[0] * imu.aRes;
  y = imu.accelCount[1] * imu.aRes;
  z = imu.accelCount[2] * imu.aRes;
  float acc_mag = sqrt(x * x + y * y + z * z);
  //      Serial.println(acc_mag);
  float avg_acc_mag = (acc_mag + old_acc + older_acc) / 3;
  //      Serial.println(avg_acc_mag);
  older_acc = old_acc;
  old_acc = avg_acc_mag;
  return avg_acc_mag;
}


void setup_user(){
  /*
   * Sets up the user and their emergency 
   * contact on the Bonk database. Posts to
   * add_EC.py on the server.
   */
  char vars[200]; // form variables
  char request[500];
  sprintf(vars, "handle=%s&ec=%s", twitter_user, emergency_contact);
  Serial.println(vars);
  sprintf(request, "POST /sandbox/sc/team078/twitter/add_EC.py HTTP/1.1\r\n");
  sprintf(request + strlen(request), "Host: %s\r\n", host);
  strcat(request, "Content-Type: application/x-www-form-urlencoded\r\n");
  sprintf(request + strlen(request), "Content-Length: %d\r\n\r\n", strlen(vars));
  strcat(request, vars);
  //do_http_request(host, request, response, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT, true);
  Serial.println(response);
}


void post_request(char* outcome, char* coords){
  /*
   * Creates an appropriate POST request for
   * bonk_twitter.py on the server.
   */
  char vars[200]; // form variables
  char request[500];
  sprintf(vars, "user=%s&loc=%s&status=%s", twitter_user, coords, outcome);
//  sprintf(vars + strlen(vars), "&loc=%s",coords);
//  sprintf(vars + strlen(vars), "&status=%s",outcome); 
  sprintf(request, "POST /sandbox/sc/team078/twitter/bonk_twitter.py HTTP/1.1\r\n");
  sprintf(request + strlen(request), "Host: %s\r\n", host);
  strcat(request, "Content-Type: application/x-www-form-urlencoded\r\n");
  sprintf(request + strlen(request), "Content-Length: %d\r\n\r\n", strlen(vars));
  strcat(request, vars);
  //Serial.println(request);
  do_http_request(host, request, response, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT, true);
  Serial.println(response);
}


void do_http_request(char* host, char* request, char* response, uint16_t response_size, uint16_t response_timeout, uint8_t serial) {
  WiFiClient client; //instantiate a client object
  if (client.connect(host, 80)) { //try to connect to host on port 80
    if (serial) //Serial.print(request);//Can do one-line if statements in C without curly braces
    client.print(request);
    memset(response, 0, response_size); //Null out (0 is the value of the null terminator '\0') entire buffer
    uint32_t count = millis();
    while (client.connected()) { //while we remain connected read out data coming back
      client.readBytesUntil('\n', response, response_size);
      if (serial) //Serial.println(response);
      if (strcmp(response, "\r") == 0) { //found a blank line!
        break;
      }
      memset(response, 0, response_size);
      if (millis() - count > response_timeout) break;
    }
    memset(response, 0, response_size);
    count = millis();
    while (client.available()) { //read out remaining text (body of response)
      char_append(response, client.read(), OUT_BUFFER_SIZE);
    }
    //if (serial) Serial.println(response);
    client.stop();
    if (serial) Serial.println("-----------");
  } else {
    if (serial) Serial.println("connection failed :/");
    if (serial) Serial.println("wait 0.5 sec...");
    client.stop();
  }
}

//
///*----------------------------------
// * char_append Function:
// * Arguments:
// *    char* buff: pointer to character array which we will append a
// *    char c: 
// *    uint16_t buff_size: size of buffer buff
// *    
// * Return value: 
// *    boolean: True if character appended, False if not appended (indicating buffer full)
// */
uint8_t char_append(char* buff, char c, uint16_t buff_size) {
        int len = strlen(buff);
        if (len>buff_size) return false;
        buff[len] = c;
        buff[len+1] = '\0';
        return true;
}
//
///*----------------------------------
// * do_http_request Function:
// * Arguments:
// *    char* host: null-terminated char-array containing host to connect to
// *    char* request: null-terminated char-arry containing properly formatted HTTP request
// *    char* response: char-array used as output for function to contain response
// *    uint16_t response_size: size of response buffer (in bytes)
// *    uint16_t response_timeout: duration we'll wait (in ms) for a response from server
// *    uint8_t serial: used for printing debug information to terminal (true prints, false doesn't)
// * Return value:
// *    void (none)
// */
//void do_http_request(char* host, char* request, char* response, uint16_t response_size, uint16_t response_timeout, uint8_t serial){
//  WiFiClient client; //instantiate a client object
//  if (client.connect(host, 80)) { //try to connect to host on port 80
//    if (serial) Serial.print(request);//Can do one-line if statements in C without curly braces
//    client.print(request);
//    memset(response, 0, response_size); //Null out (0 is the value of the null terminator '\0') entire buffer
//    uint32_t count = millis();
//    while (client.connected()) { //while we remain connected read out data coming back
//      client.readBytesUntil('\n',response,response_size);
//      if (serial) Serial.println(response);
//      if (strcmp(response,"\r")==0) { //found a blank line!
//        break;
//      }
//      memset(response, 0, response_size);
//      if (millis()-count>response_timeout) break;
//    }
//    memset(response, 0, response_size);  
//    count = millis();
//    while (client.available()) { //read out remaining text (body of response)
//      char_append(response,client.read(),OUT_BUFFER_SIZE);
//    }
//    if (serial) Serial.println(response);
//    client.stop();
//    if (serial) Serial.println("-----------");  
//  }else{
//    if (serial) Serial.println("connection failed :/");
//    if (serial) Serial.println("wait 0.5 sec...");
//    client.stop();
//  }
//}        
