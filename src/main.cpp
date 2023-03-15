#include <mbed.h>

#define ADDRESSE_I2C_PAR_DEFAUT 0x51 //1010001

#if !DEVICE_I2CSLAVE
#error [NOT_SUPPORTED] I2C Slave is not supported
#endif

static BufferedSerial serial_port(USBTX, USBRX);

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
PwmOut moteur(PB_6);


int8_t position_moteur = -127;

int main() {
  serial_port.set_baud(9600);

  char buffer[10]; 

  slave.address(ADDRESSE_I2C_PAR_DEFAUT << 1);  

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
                int8_t commande_recue = buffer[0];
                printf("commande recu : %d\r\n", commande_recue);

                if(commande_recue == 126){
                    moteur.resume();
                    moteur.write(0.075); //retourne au milieu
                }else if(commande_recue == 127){
                    moteur.suspend();
                }else if (commande_recue >= -90 && commande_recue <= 90) {
                    position_moteur = commande_recue;
                    uint16_t pulse_width = 1500 + commande_recue * 6.666;
                    moteur.period_ms(20);
                    moteur.pulsewidth_us(pulse_width);
                }
                
                break;
        }
        
        // Vider le buffer de lecture
        unsigned int i = 0;
        for (i = 0; i < sizeof(buffer); i++) {
            buffer[i] = 0;
        }
        
    }
}