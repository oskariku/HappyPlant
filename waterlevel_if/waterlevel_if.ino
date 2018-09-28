   if (soilSensor()>=208) {
    char watersymbol[1];
     sprintf(watersymbol, "%c", 72);
      u8g2.setCursor(0,32);
      u8g2.print(watersymbol);
    u8g2.sendBuffer();
    } else if (soilSensor()<208 && soilSensor()>=156) {
      char watersymbol[1];
      sprintf(watersymbol, "%c", 72);
        u8g2.setCursor(0,32);
        u8g2.print(watersymbol);
          u8g2.setCursor(32,32);
          u8g2.print(watersymbol);
      u8g2.sendBuffer();
      } else if (soilSensor()<156 && soilSensor>=104) {
        char watersymbol[1];
        sprintf(watersymbol, "%c", 72);
          u8g2.setCursor(0,32);
          u8g2.print(watersymbol);
            u8g2.setCursor(32,32);
            u8g2.print(watersymbol);
              u8g2.setCursor(64,32);
              u8g2.print(watersymbol);
        u8g2.sendBuffer();
        } else if (soilSensor()<104){
          char watersymbol[1];
          sprintf(watersymbol, "%c", 72);
            u8g2.setCursor(0,32);
            u8g2.print(watersymbol);
              u8g2.setCursor(32,32);
              u8g2.print(watersymbol);
                u8g2.setCursor(64,32);
                u8g2.print(watersymbol);
                 u8g2.setCursor(98,32);
                  u8g2.print(watersymbol);
          u8g2.sendBuffer();
          }
