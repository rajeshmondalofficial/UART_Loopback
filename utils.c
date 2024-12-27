char* receive_message = "+RCV=50,5,Hello,-99,40";

strtok_r(receive_message, "=", &receive_message);
printf("%s", receive_message);