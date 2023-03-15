#include <mbed.h>

#define ADDRESSE_I2C_PAR_DEFAUT 0x51 //1010001

#if !DEVICE_I2CSLAVE
#error [NOT_SUPPORTED] I2C Slave is not supported
#endif

//static BufferedSerial serial_port(USBTX, USBRX);

// Utiliser la classe I2CSlave pour créer un objet slave.
// Consulter la documentation pour voir les méthodes disponibles.
I2CSlave slave(D14, D15);

///////////////////////////////////////////
// Créer un objet debug_led à partir de la classe DigitalOut pour vous aider dans le debuggage
///////////////////////////////////////////
//DigitalOut led(LED2);

///////////////////////////////////////////
// Créer un objet moteur à partir de la classe PwmOut
///////////////////////////////////////////
//PwmOut moteur(D3);


///////////////////////////////////////////
// Créer une variable pour la machine à état qui gére le moteur (OFF ou Activé)
///////////////////////////////////////////
//enum{On, Off};

//int etat = Off;

int main() {
  //serial_port.set_baud(9600);

  char buffer[10];
  

  slave.address(ADDRESSE_I2C_PAR_DEFAUT << 1);

  //double pourcentage = 0.075; //milieu
  //moteur.period(0.02);

  while (1) {


        // Attendre une requête du master
        int read = slave.receive();

        // Traiter la requête
        switch (read) {

            // Si le master envoie une requête de lecture
            case I2CSlave::ReadAddressed:
                ///////////////////////////////////////////
                // Retourner l'état du moteur (sa position ou OFF sous forme d'une chaine de caractères)
                ///////////////////////////////////////////

                slave.write(buffer, strlen(buffer) + 1); // Includes null char
                break;

            // Si le master envoie une requête d'écriture qui nous est adressée
            case I2CSlave::WriteAddressed:
                slave.read(buffer, 10);
                //printf("Read A: %s\n\r", buffer);

                

                ///////////////////////////////////////////
                // Modifier l'état du moteur en fonction de la commande reçue
                ///////////////////////////////////////////
                /*int8_t commande_recue = buffer[0];
                switch (etat){
                    case Off:
                        moteur.suspend();

                        if(commande_recue == 126){
                            moteur.resume();
                            moteur.write(0.075); //retourne au milieu
                            etat = On;
                        }
                        break;

                    case On:
                        if(commande_recue <= 90 && commande_recue >= -90){
                            pourcentage = ((double)commande_recue * (2/45) + 7.5)/100;
                            moteur.write(pourcentage); 
                        }
                        else if(commande_recue == 127){
                            etat = Off;
                        }
                        break;
                }*/
                break;
        }
        
        // Vider le buffer de lecture
        unsigned int i = 0;
        for (i = 0; i < sizeof(buffer); i++) {
            buffer[i] = 0;
        }
        
    }
}