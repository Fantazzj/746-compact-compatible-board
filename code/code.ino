/*
	764 compatible board
*/

//#include <eeprom.h>

//Configurazione
	const unsigned long int TC = 60000;		//Tempo chiusura automatica
	const unsigned long int TF = 500;		//Tempo del freno
	const unsigned long int TS1 = 1000;		//Tempo sicurezza per blocco inizio motore apertura/chiusura
	const unsigned long int TS2 = 18000;	//Tempo sicurezza per blocco motore apertura/chiusura
	const unsigned long int TS3 = 1000;		//Tempo sicurezza per rimbalzo pulsanti
	const unsigned long int TS4 = 200;		//Tempo per relay

//Ingressi
	const int FCA_PIN = A1;		//Finecorsa cancello aperto
	const int FCC_PIN = A2;		//Finecorsa cancello chiuso
	const int FTC_PIN = A3;		//Fotocellula
	const int CANC_PIN = A5;	//Finecorsa cancellino
	const int PSTART_PIN = A4;	//Pulsante di start (radicomando / chiave / pulsante)
	const int J2PIN = 2;		//Jumper chiusura automatica (logica negata)
	const int J3PIN = 3;		//Jumper taratura automatica 
	const int J4PIN = 4;		//Jumper ausiliario (non configurato)
	const int J5PIN = 5;		//Jumper ausiliario (non configurato)

//Uscite
	const int KMA_PIN = 7;		//Relay apertura
	const int KMC_PIN = 8;		//Relay chiusura
	const int KFR_PIN = 9;		//Relay freno

//Variabili
	bool fca = 0;
	bool fcc = 0;
	bool fccPulse = 0;
	bool fccNew = 0;
	bool fccOld = 0;
	bool ftc = 0;
	bool canc = 0;
	bool pStartPulse = 0;
	bool pStartNew = 0;
	bool pStartOld = 0;
	bool j2 = 1;
	bool j3 = 1;
	bool j4 = 1;
	bool j5 = 1;
	bool kma = 0;
	bool kmc = 0;
	bool kfr = 0;
	bool safe = true;
	int step = 0;
	int fccCounter = 0;
	unsigned long int previousMillis = 0;
	unsigned long int elapsedTime = 0;

void setup()					//Setup di arduino
{
	pinMode(J2PIN, INPUT_PULLUP);
	pinMode(J3PIN, INPUT_PULLUP);
	pinMode(J4PIN, INPUT_PULLUP);
	pinMode(J5PIN, INPUT_PULLUP);
	pinMode(KMA_PIN, OUTPUT);
	pinMode(KMC_PIN, OUTPUT);
	pinMode(KFR_PIN, OUTPUT);
	Serial.begin(9600);

	calibration(digitalRead(J3PIN));
}

void loop()						//Loop di arduino
{
	//Lettura ingressi
	inputRead();

	if (safe || (step != 100 && step != 108 && step != 200)){

		//Temporizzazione
		elapsedTime = millis() - previousMillis;
		
		//Ciclo automatico
		autoCycle();

		//Analisi uscite
		outputAnalysis();
		
		//Attivazione uscite
		outputWrite();

	}

	//Sicurezza fotocellula
	safetyFtc();

	//Debug
	debug(false);
}

void inputRead() 				//Funzione che esegue tutte le letture degli ingressi
{
	
	fca = digitalRead(FCA_PIN);
	fcc = digitalRead(FCC_PIN);
	ftc = digitalRead(FTC_PIN);
	canc = digitalRead(CANC_PIN);
	pStartNew = digitalRead(PSTART_PIN);
	j2 = digitalRead(J2PIN);
	j3 = digitalRead(J3PIN);
	j4 = digitalRead(J4PIN);
	j5 = digitalRead(J5PIN);

	if (pStartNew > pStartOld) pStartPulse = 1;
	else pStartPulse = 0;
	pStartOld = pStartNew;

}

void outputWrite() 				//Funzione che attiva le uscite
{
	digitalWrite(KMA_PIN, kma);
	digitalWrite(KMC_PIN, kmc);
	digitalWrite(KFR_PIN, kfr);
}

void debug(bool activate) 		//Funzione che stampa sulla seriale tutti i valori delle variabili
{
	if(activate){
		Serial.print("Step:");
		Serial.print(step);
		Serial.print("  ");
		Serial.print("FCA:");
		Serial.print(fca);
		Serial.print("  ");
		Serial.print("FCC:");
		Serial.print(fcc);
		Serial.print("  ");
		Serial.print("FTC:");
		Serial.print(ftc);
		Serial.print("  ");
		Serial.print("Canc:");
		Serial.print(canc);
		Serial.print("  ");
		Serial.print("Pstart:");
		Serial.print(pStartPulse);
		Serial.print("  ");
		Serial.print("kma:");
		Serial.print(kma);
		Serial.print("  ");
		Serial.print("kfr:");
		Serial.print(kfr);
		Serial.print("  ");
		Serial.print("kmc:");
		Serial.print(kmc);
		Serial.print("  ");
		Serial.print("timePassed:");
		Serial.print(elapsedTime);
		Serial.print("  ");
		Serial.print("previousMillis:");
		Serial.print(previousMillis);
		Serial.print("  ");
		Serial.print("millis:");
		Serial.print(millis());
		Serial.print("  ");
		Serial.print("fccCounter:");
		Serial.print(fccCounter);
		Serial.println("  ");
	}
}

void autoCycle() 				//Funzione che esegue il ciclo automatico
{

	if (step == 0 && !fcc) changeStep(100);
		if (step == 100 && ((pStartPulse && elapsedTime >= TS3) || (elapsedTime >= TC && !j2))) changeStep(102);
			if (step == 102 && ftc && elapsedTime >= TS1) changeStep(105);
				if (step == 105 && elapsedTime >= TS4) changeStep(103);
					if (step == 103 && elapsedTime >= TF) changeStep(112);
						if (step == 112 && elapsedTime >= TS4) changeStep(202);
			if (step == 102 && ftc && elapsedTime <= TS1) changeStep(100);
			if (step == 102 && fcc) changeStep(107);
				if (step == 107 && elapsedTime >= TS4) changeStep(104);
					if (step == 104 && elapsedTime >= TF) changeStep(200);
			if (step == 102 && pStartPulse && elapsedTime >= TS3) changeStep(110);
				if (step == 110 && elapsedTime >= TS4) changeStep(106);
					if (step == 106 && elapsedTime >= TF) changeStep(113);
						if (step == 113 && elapsedTime >= TS4) changeStep(202);
			if (step == 102 && fca && elapsedTime >= TS1) changeStep(108);
				if (step == 108 && pStartPulse && elapsedTime >= TS3) changeStep(102);
				if (step == 108 && fcc && !fca) changeStep(200);
			if (step == 102 && elapsedTime >= TS2) changeStep(108);
			if (step == 102 && canc && elapsedTime >= TS1) changeStep(111);
				if (step == 111 && elapsedTime >= TS4) changeStep(109);
					if (step == 109 && elapsedTime >= TF) changeStep(108);
			if (step == 102 && canc && elapsedTime <= TS1) changeStep(108);
		if (step == 100 && fcc && !fca) changeStep(200);
		if (step == 100 && ftc) changeStep(100);

	if (step == 0 && fcc) changeStep(200);
		if (step == 200 && pStartPulse && elapsedTime >= TS3) changeStep(202);
			if (step == 202 && fca) changeStep(207);
				if (step==207 && elapsedTime >= TS4) changeStep(203);
					if (step == 203 && elapsedTime >= TF) changeStep(100);
			if (step == 202 && pStartPulse && elapsedTime >= TS3) changeStep(208);
				if (step == 208 && elapsedTime >= TS4) changeStep(205);
					if (step == 205 && elapsedTime >= TF) changeStep(206);
						if (step == 206 && pStartPulse && elapsedTime >= TS3) changeStep(102);
			if (step == 202 && fcc && elapsedTime >= TS1) changeStep(200);
			if (step == 202 && elapsedTime >= TS2) changeStep(200);
			if (step == 202 && canc && elapsedTime >= TS1) changeStep(209);
				if (step == 209 && elapsedTime >= TS4) changeStep(210);
					if (step == 210 && elapsedTime >= TF) changeStep(200);
			if (step == 202 && canc && elapsedTime <= TS1) changeStep(200);
		if (step == 200 && !fcc) changeStep(108);

}

void outputAnalysis() 			//Funzione che verifica quali uscite sono da attivare
{

	if (step == 103 || step == 104 || step == 106 ||  step == 203 || step == 205 || step == 210) kfr = 1;
	else kfr = 0;

	if (step == 102) kmc = 1;
	else kmc = 0;

	if (step == 202) kma = 1;
	else kma = 0;

}

void changeStep(int newStep) 	//Funzione che cambia lo stato
{
	step = newStep;
	previousMillis = millis();
	elapsedTime = 0;
}

void calibration(bool cal)		//Funzione che esegue la taratura del tempo
{
	if(cal)
	{
		
	}
}

void safetyFtc()				//Funzione che analizza il funzionamento della fotocellula
{
	fccNew = fcc;

	if (fccNew != fccOld && fccNew == 1) fccPulse = 1;
	else fccPulse = 0;

	if (ftc) fccCounter = 0;
	else if(fccPulse) fccCounter++;

	if (fccCounter >= 4) safe = false;
	else safe = true;

	fccOld = fccNew;
}
