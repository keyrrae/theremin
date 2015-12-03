unsigned int tuningWord[32] = {206620225, 184102226, 173810798, 154811237, 137922738, 
                        122872623, 115976486, 103327704, 92051113, 86885168, 
                        77405618, 68960489, 61437191, 57989122, 51662093,
                        46025556, 43442144, 38702809, 34480684, 30718595, 
                        28994561, 25830606, 23013218, 21721072, 19351404, 
                        17240342, 15358858, 14496840, 12915303, 11506169, 
                        10860536, 9675702};

unsigned char pitchnum;
unsigned char volume;
unsigned int distance0;
unsigned int distance1;

int tmp;
unsigned int tmp_distance0;
unsigned int tmp_distance1;



while(1){
tmp = (distance0 - 300) >> 4;

if (distance > 31)
    pitchnum = 31;
else
    pitchnum = distance0;

if (distance1 - 300 > 255)
    volume = 0;
else
    volume = 255 - (distance1 - 300);

XGpio_DiscreteWrite(&pwm, PWM_CHANNEL, tuningWord[pitchnum]);
XGpio_DiscreteWrite(&scale, SCALE_CHANNEL, volume);

tmp_distance0 = XGpio_DiscreteRead(&width0, WIDTH0_CHANNEL);
tmp_distance1 = XGpio_DiscreteRead(&width1, WIDTH1_CHANNEL);



}



handler

distance0 = tmp_distance0;
distance0 = tmp_distance1;
