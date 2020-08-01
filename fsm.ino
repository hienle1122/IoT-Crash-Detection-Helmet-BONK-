void bonk_fsm(float reading){
  /*
   * Main Bonk State Machine.
   * Controls what actions are
   * completed.
   */
  switch(state){
    case IDLE:
                if (reading > threshold)  {
                  Serial.println("TRIGGERED");
                  Serial.println(reading);
                  state = CHECK_FALL;
                  check_timer = millis();
                }

                if (reading < sleep_threshold){
                  sleep_fsm(reading);
                } else{
                  sleep_state = 0;
                }
                
                break;

    case CHECK_FALL:  if (millis() - check_timer > 50)  {  // a long enough time has happened to get rid of "bouncing"
                        if (reading < threshold){  // if signal has fallen below threshold then thats a fall                      
                          Serial.println("Fall recorded");
                          fall_timer = millis();
                          Serial.println("Location request data");
                          memset(loc_data, 0, sizeof(loc_data));
                          loc = location.getGeoFromWiFi();
                          sprintf (loc_data, "%f, %f ", loc.lat, loc.lon);
                          Serial.println(loc_data);
                          state = WAIT;
                        } else {
                          // not a fall, signal took to long to come down. Return to IDLE
                          state = IDLE;
                          sleep_state = 0;
                          Serial.println("NOT A VALID FALL!");
                        }
                      }
                      break;

    case WAIT:  if (millis() - fall_timer > 5000) {   // time buffer for post-fall imu readings like bouncing around
                  if (reading > 1.5){  // 1 is a good threshold for slight movement
                    if(danger_sent){
                      state = CLEANUP;   // if there is movement after the danger message has been sent, go to CLEANUP. Our job is done.
                      break;
                    }else{
                      state = FUNNY;  // movement was sensed rather quickly; implies that accident is not serious. Funny tweet will be posted
                      break;
                    }   
                  }
                  
                  if (millis() - fall_timer > danger_time && (millis()- fall_timer < (danger_time + 50))) {
                    state = DANGER;   // A tweet that alerts people of a possible serious accident will be sent out.
                  }
                  
                  if (millis() - fall_timer > 60000) {  // no movement has been sensed in a minute, this is bad news
                    state = LONGTIME;   // python script will send a DM to an emergency contact
                  }
                }
                break;

      case FUNNY:   Serial.println("---------------------FUNNY----------------");
                    post_request("funny", loc_data);
                    memset(outcome, 0, sizeof(outcome));
                    state = CLEANUP;
                    break;

      case DANGER:  Serial.println("___________DANGER______________");
                    post_request("danger", loc_data);
                    memset(outcome, 0, sizeof(outcome));
                    state = WAIT;
                    danger_sent = true;
                    break;
        
      case LONGTIME:  Serial.println("--------------------------------LONGTIME----------------------------");
                      post_request("longtime", loc_data);
                      memset(outcome, 0, sizeof(outcome));
                      state = CLEANUP;
                      break;

      case CLEANUP:   Serial.println("------------ THANK YOU FOR PLAYING!-----------"); 
                      ESP.restart();
                      break;  // un-necessary, just used to explicitlty state that this is the end of case: CLEANUP

  }
}


void sleep_fsm(float input) {
  /*
   * Controls the sleep-cycle.
   * Will send ESP32 to sleep
   * after 30 seconds of no movement.
   */
  int wait_ms  = 30000; //how many millisec we want the machine to wait before it turns off 

  switch (sleep_state){

    case 0:   sleep_timer = millis();
              sleep_state = 1;
              break;
                     
    case 1:   //Serial.println(millis() - sleep_timer);
              if (input > sleep_threshold){
                sleep_state = 0;
              }
              if (millis() - sleep_timer >= wait_ms) {
                sleep_state = 2;  //if we're not moving for 30 seconds, go to state 2
                Serial.println("hehe sleepy time");
              } 
              break;

    case 2:   sleep_timer = millis();
              esp_deep_sleep_start();   // go into deep sleep
              break;
  }
}
