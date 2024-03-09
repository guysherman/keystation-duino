// Copyright Guy Sherman 2024

#define COUNTER_TYPE unsigned int
const byte NUM_GROUPS = 8;
const byte WRAP_MASK = NUM_GROUPS-1;
const byte GROUP_PINS[NUM_GROUPS] = {2, 3, 4, 5, 6, 7, 22}; // {2, 3}; // 
const COUNTER_TYPE PIN_MASK = 0x1;

const byte NUM_COLLECTORS = 7;
const byte NUM_SWITCHES = 2;
const byte SW0_COLLECTORS[NUM_COLLECTORS] = {8, 24, 26, 28, 30, 32, 34};  // {8};
const byte SW1_COLLECTORS[NUM_COLLECTORS] = {9, 25, 27, 29, 21, 33, 35}; //{9}; 

COUNTER_TYPE COUNTERS[NUM_GROUPS * NUM_COLLECTORS * NUM_SWITCHES];
COUNTER_TYPE DELTAS[NUM_GROUPS * NUM_COLLECTORS];
COUNTER_TYPE SWITCHES[NUM_GROUPS * NUM_COLLECTORS * NUM_SWITCHES];

COUNTER_TYPE MASKS[2] = { 0x0, 0xFFFF };

void setup() {
  pinMode(12, OUTPUT);
  Serial.begin(9600);

  for (byte i = 0; i < NUM_GROUPS; i++) {
    pinMode(GROUP_PINS[i], OUTPUT);    
    digitalWrite(GROUP_PINS[i], LOW);
  }

  for (byte i = 0; i < NUM_COLLECTORS; i++) {
    pinMode(SW0_COLLECTORS[i], INPUT);
    pinMode(SW1_COLLECTORS[i], INPUT);
  }

  for (byte i = 0; i < NUM_GROUPS * NUM_COLLECTORS * 2; i++) {
    COUNTERS[i] = 0;
  }

  Serial.println("Hello");
}

byte curr_group = 0;
byte prior_group = NUM_GROUPS-1;
COUNTER_TYPE c = 0L;

const char buf[128];

void loop() {
  digitalWrite(12, c & PIN_MASK);
  prior_group = curr_group;
  curr_group = (curr_group + 1) & WRAP_MASK;


  digitalWrite(GROUP_PINS[curr_group], HIGH);
  digitalWrite(GROUP_PINS[prior_group], LOW);


  for (byte i = 0; i < NUM_COLLECTORS; i++) {
    byte sw0_idx = counter_index(curr_group, i, 0);
    byte sw1_idx = counter_index(curr_group, i, 1);
    byte d_idx = delta_index(curr_group, i);
    
    COUNTER_TYPE sw0 = MASKS[digitalRead(SW0_COLLECTORS[i])];
    COUNTER_TYPE sw1 = MASKS[digitalRead(SW1_COLLECTORS[i])];
    
    COUNTER_TYPE edge0 = sw0 ^ SWITCHES[sw0_idx];
    COUNTER_TYPE edge1 = sw1 ^ SWITCHES[sw1_idx];


    
    /*
    COUNTER_TYPE c0 = 0;
    if (sw0 == 0x0) { 
      c0 = 0;
    } else if (edge0 == 0xFFFF) {
      c0 = c;
    } else {
      c0 = COUNTERS[sw0_idx];
    }

    COUNTER_TYPE c1 = 0;
    if (sw1 == 0x0) {
      c1 = c0;
    } else if (edge1 == 0xFFFF) {
      c1 = c;
    } else {
      c1 = COUNTERS[sw1_idx];
    }
    */

    /**/
    COUNTER_TYPE c0 = (COUNTERS[sw0_idx] & ~edge0) // prior if not at edge else 0       
                          | (c & edge0 & sw0);  // curr at rising edge else 0
    
    COUNTER_TYPE c1 = (COUNTERS[sw1_idx] & ~edge1 & sw1) // prior if not at edge and high else 0       
                          | (c & edge1 & sw1) // curr at rising edge else 0
                          | (c0 & ~sw1); //  counter for sw0 if low
                          /**/

    COUNTER_TYPE delta = c1 - c0;

    /*
    if (sw0 == 0xFFFF) {
      print_event(curr_group, i, delta, key_num(curr_group, i), "S0", c0, COUNTERS[sw0_idx], c1, COUNTERS[sw1_idx]);

    }
    */

    if (delta > DELTAS[d_idx]) {
      // note on
      print_event(curr_group, i, delta, key_num(curr_group, i), "ON", c0, COUNTERS[sw0_idx], c1, COUNTERS[sw1_idx]);
      
    } else if (delta < DELTAS[d_idx]) {
      // note off
      //int chars = printf(&buf[0], "OF:%d:%d:%d", curr_group, i, delta);
      //Serial.println(buf);
      print_event(curr_group, i, delta, key_num(curr_group, i), "OF", c0, COUNTERS[sw0_idx], c1, COUNTERS[sw1_idx]);
    }

    DELTAS[d_idx] = delta;
    SWITCHES[sw0_idx] = sw0;
    SWITCHES[sw1_idx] = sw1;
    COUNTERS[sw0_idx] = c0;
    COUNTERS[sw1_idx] = c1;

    //int chars = printf(buf, "%d:%d:%d", curr_group, i, delta);
    
  }






  c++;
}

byte counter_index(byte group, byte collector, byte sw) {
  //return (group * NUM_COLLECTORS * NUM_SWITCHES) + (collector * NUM_SWITCHES) + sw;
  return ((group << 4) - (group << 1)) + (collector << 1) + sw;
}

byte delta_index(byte group, byte collector) {
  //return (group * NUM_COLLECTORS) + collector;
  return ((group << 3) - group) + collector;
}

void print_event(byte group, byte collector, COUNTER_TYPE delta, byte key, const char* event, COUNTER_TYPE c0, COUNTER_TYPE prior_c0, COUNTER_TYPE c1, COUNTER_TYPE prior_c1) {
    Serial.print(event); Serial.print(":");
    Serial.print(curr_group); Serial.print(":");
    Serial.print(collector); Serial.print(":");
    Serial.print(key); Serial.print(":");
    Serial.print(delta); Serial.print(":");
    Serial.print(c0); Serial.print(":");
    Serial.print(prior_c0); Serial.print(":");
    Serial.print(c1); Serial.print(":");
    Serial.print(prior_c1); Serial.print(":");

    Serial.println("");
}

byte key_num(byte group, byte collector) {
  //return (collector * NUM_GROUPS) + group;
  return (collector << 3) + group;
}
