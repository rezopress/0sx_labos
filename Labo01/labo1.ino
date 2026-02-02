// LABORATOIRE 01
// Nom: Lionel G.
// DA: 6357132


void setup() {


  pinMode(LED_BUILTIN, OUTPUT);  

  Serial.begin(9600);
}

void loop() {

  // =========================
  // ÉTAT : ALLUMÉ / ÉTEINT
  // =========================
  Serial.println("Etat : Allume – 6357132");

  digitalWrite(LED_BUILTIN, LOW);
  delay(300);

  digitalWrite(LED_BUILTIN, HIGH);
  delay(2000);

  digitalWrite(LED_BUILTIN, LOW);
  delay(1000);

  // =========================
  // ÉTAT : VARIATION D'INTENSITÉ
  // =========================
  Serial.println("Etat : Varie – 6357132");

  // Dernier chiffre paire donc 0 à 255
  for (int i = 0; i <= 255; i++) {
    analogWrite(LED_BUILTIN, i);
    delay(8);  // Durée total 2048ms = 2048 / 256 = 8ms
  }

  // =========================
  // ÉTAT : CLIGNOTEMENT
  // =========================
  Serial.println("Etat : Clignotement – 6357132");

  for (int i = 0; i < 2; i++) {   // 2 clignotements ( 3/2 = 1.5 -> plafond = 2 )

    digitalWrite(LED_BUILTIN, LOW);
    delay(350);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(350);
  }

}